/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2017-2019 Linaro LTD
 * Copyright (c) 2016-2019 JUUL Labs
 * Copyright (c) 2019-2020 Arm Limited
 *
 * Original license:
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include <flash_map_backend/flash_map_backend.h>

#include "bootutil/image.h"
#include "bootutil/crypto/sha256.h"
#include "bootutil/sign_key.h"
#include "bootutil/security_cnt.h"
#include "bootutil/fault_injection_hardening.h"

#include "mcuboot_config/mcuboot_config.h"
#include "ti_msp_dl_config.h"

#ifdef MCUBOOT_ENC_IMAGES
#include "bootutil/enc_key.h"
#endif
#if defined(MCUBOOT_SIGN_RSA)
#include "mbedtls/rsa.h"
#endif
#if defined(MCUBOOT_SIGN_EC) || defined(MCUBOOT_SIGN_EC256)
#include "mbedtls/ecdsa.h"
#endif
#if defined(MCUBOOT_ENC_IMAGES) || defined(MCUBOOT_SIGN_RSA) || \
    defined(MCUBOOT_SIGN_EC) || defined(MCUBOOT_SIGN_EC256)
#include "mbedtls/asn1.h"
#endif
#ifdef TI_CUSTOM_TLV_DYNAMIC_KEY_MANAGEMENT
#include "keystore.h"
#endif

#include "bootutil_priv.h"

/* Define signature types for dual authentication */
enum {
    SIG_TYPE_NONE,
    SIG_TYPE_ECDSA,
    SIG_TYPE_MLDSA
};

extern bool dmaDone;
extern bool digestReady;

/* Function prototype for MLDSA signature verification */
fih_int bootutil_verify_mldsa_sig(uint8_t *hash, uint32_t hlen, uint8_t *sig, size_t slen,
                                uint8_t key_id);

/*
 * Compute SHA256 over the image.
 */
static int
bootutil_img_hash(struct enc_key_data *enc_state, int image_index,
                  struct image_header *hdr, const struct flash_area *fap,
                  uint8_t *tmp_buf, uint32_t tmp_buf_sz, uint8_t *hash_result,
                  uint8_t *seed, int seed_len)
{
    bootutil_sha256_context sha256_ctx;
    uint32_t blk_sz;
    uint32_t size;
    uint16_t hdr_size;
    uint32_t off;
    int rc;
    uint32_t blk_off;
    uint32_t tlv_off;

#if (BOOT_IMAGE_NUMBER == 1) || !defined(MCUBOOT_ENC_IMAGES) || \
    defined(MCUBOOT_RAM_LOAD)
    (void)enc_state;
    (void)image_index;
    (void)hdr_size;
    (void)blk_off;
    (void)tlv_off;
#ifdef MCUBOOT_RAM_LOAD
    (void)blk_sz;
    (void)off;
    (void)rc;
#endif
#endif

#ifdef MCUBOOT_ENC_IMAGES
    /* Encrypted images only exist in the secondary slot */
    if (MUST_DECRYPT(fap, image_index, hdr) &&
            !boot_enc_valid(enc_state, image_index, fap)) {
        return -1;
    }
#endif

    #ifndef CSC_ENABLE_HW_SHA
    bootutil_sha256_init(&sha256_ctx);
    #endif

    /* in some cases (split image) the hash is seeded with data from
     * the loader image */
    if (seed && (seed_len > 0)) {
        bootutil_sha256_update(&sha256_ctx, seed, seed_len);
    }

    /* Hash is computed over image header and image itself. */
    size = hdr_size = hdr->ih_hdr_size;
    size += hdr->ih_img_size;
    tlv_off = size;

    /* If protected TLVs are present they are also hashed. */
    size += hdr->ih_protect_tlv_size;

#ifdef MCUBOOT_RAM_LOAD
    bootutil_sha256_update(&sha256_ctx,(void*)(hdr->ih_load_addr), size);
#else

#ifdef CSC_ENABLE_HW_SHA

    /* Initialize SHA hardware */
    NVIC_EnableIRQ(SHAW_INT_IRQn);
    NVIC_EnableIRQ(DMA0_INT_IRQn);
    
    /* Configure SHA hardware */
    DL_SHAW_setModeConfig(SHAW_INST, 
        DL_SHAW_ALGORITHM_SHA_256,
        DL_SHAW_MODE_HASH);
    DL_SHAW_enableOutputReadyInterrupt(SHAW_INST);
    DL_SHAW_enableDMATriggerInterrupt(SHAW_INST);
    
    /* Set data length for the entire hash operation */
    DL_SHAW_setDataLength(SHAW_INST, size, DL_SHAW_MODE_HASH);
    
    /* Signal mode and length available */
    DL_SHAW_signalModeAndLengthAvailable(SHAW_INST);
    
    /* Configure DMA to read directly from flash memory */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, fap->fa_off);
    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, DL_SHAW_DATA_IN_ADDR(SHAW_INST));
    
    /* Calculate number of words to transfer for the entire image (header + image + protected TLVs) */
    uint32_t words_to_transfer = (size + 3) / 4;
    DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, words_to_transfer);
    
    /* Configure DMA transfer properties */
    DL_DMA_setDestWidth(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_WIDTH_WORD);
    DL_DMA_setSrcWidth(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_WIDTH_WORD);
    DL_DMA_setSrcIncrement(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_ADDR_INCREMENT);
    DL_DMA_setDestIncrement(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_ADDR_UNCHANGED);
    
    /* Set up DMA trigger and enable channel */
    DL_DMA_setTrigger(DMA0, DMA0_CH0_CHAN_ID, SHAW_DMA_TRIGGER, DL_DMA_TRIGGER_TYPE_EXTERNAL);
    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);
    
    /* Start the DMA transfer */
    DL_SHAW_startDMA(SHAW_INST);
    
    /* Wait for DMA completion */
    while(!dmaDone) {
        __WFE();  // Wait for interrupt
    }
    
    /* Wait for the digest to be ready */
    while (!digestReady) {
        __WFE();  // Wait for interrupt
    }
    
    /* Read the digest */
    DL_SHAW_readDigest(SHAW_INST, (uint32_t *)hash_result, DL_SHAW_DIGEST_SIZE_256_BYTES);
    
    /* Release the digest */
    DL_SHAW_releaseDigest(SHAW_INST);

#else
    for (off = 0; off < size; off += blk_sz) {
        blk_sz = size - off;
        if (blk_sz > tmp_buf_sz) {
            blk_sz = tmp_buf_sz;
        }
#ifdef MCUBOOT_ENC_IMAGES
        /* The only data that is encrypted in an image is the payload;
         * both header and TLVs (when protected) are not.
         */
        if ((off < hdr_size) && ((off + blk_sz) > hdr_size)) {
            /* read only the header */
            blk_sz = hdr_size - off;
        }
        if ((off < tlv_off) && ((off + blk_sz) > tlv_off)) {
            /* read only up to the end of the image payload */
            blk_sz = tlv_off - off;
        }
#endif
        rc = flash_area_read(fap, off, tmp_buf, blk_sz);
        if (rc) {
            bootutil_sha256_drop(&sha256_ctx);
            return rc;
        }
#ifdef MCUBOOT_ENC_IMAGES
        if (MUST_DECRYPT(fap, image_index, hdr)) {
            /* Only payload is encrypted (area between header and TLVs) */
            if (off >= hdr_size && off < tlv_off) {
                blk_off = (off - hdr_size) & 0xf;
                boot_encrypt(enc_state, image_index, fap, off - hdr_size,
                        blk_sz, blk_off, tmp_buf);
            }
        }
#endif
        bootutil_sha256_update(&sha256_ctx, tmp_buf, blk_sz);
    }


    bootutil_sha256_finish(&sha256_ctx, hash_result);
    bootutil_sha256_drop(&sha256_ctx);
#endif /* MCUBOOT_RAM_LOAD */

#endif 

    return 0;
}

/*
 * Currently, we only support being able to verify one type of
 * signature, because there is a single verification function that we
 * call.  List the type of TLV we are expecting.  If we aren't
 * configured for any signature, don't define this macro.
 */
#if (defined(MCUBOOT_SIGN_RSA)      + \
     defined(MCUBOOT_SIGN_EC)       + \
     defined(MCUBOOT_SIGN_EC256)    + \
     defined(MCUBOOT_SIGN_ED25519)) > 1
#error "Only a single signature type is supported!"
#endif

#if defined(MCUBOOT_VERIFY_HYBRID)
/* For hybrid verification, define both signature types */
#    define EXPECTED_SIG_TLV IMAGE_TLV_ECDSA256 /* Primary signature type */
#    define EXPECTED_MLDSA_TLV IMAGE_TLV_MLDSA87 /* Secondary signature type */
#    define SIG_BUF_SIZE 5000 /* Use the larger of the two buffer sizes */
#    define EXPECTED_SIG_LEN(x) (1) /* For ECDSA, ASN.1 will validate */
#    define EXPECTED_MLDSA_LEN(x) ((x) >= 2000 && (x) <= SIG_BUF_SIZE) /* For MLDSA */
#elif defined(MCUBOOT_SIGN_RSA)
#    if MCUBOOT_SIGN_RSA_LEN == 2048
#        define EXPECTED_SIG_TLV IMAGE_TLV_RSA2048_PSS
#    elif MCUBOOT_SIGN_RSA_LEN == 3072
#        define EXPECTED_SIG_TLV IMAGE_TLV_RSA3072_PSS
#    else
#        error "Unsupported RSA signature length"
#    endif
#    define SIG_BUF_SIZE (MCUBOOT_SIGN_RSA_LEN / 8)
#    define EXPECTED_SIG_LEN(x) ((x) == SIG_BUF_SIZE) /* 2048 bits */
#elif defined(MCUBOOT_SIGN_EC)
#    define EXPECTED_SIG_TLV IMAGE_TLV_ECDSA224
#    define SIG_BUF_SIZE 128
#    define EXPECTED_SIG_LEN(x)  (1) /* always true, ASN.1 will validate */
#elif defined(MCUBOOT_SIGN_EC256)
#    define EXPECTED_SIG_TLV IMAGE_TLV_ECDSA256
#    define SIG_BUF_SIZE 128
#    define EXPECTED_SIG_LEN(x)  (1) /* always true, ASN.1 will validate */
#elif defined(MCUBOOT_SIGN_MLDSA87_STANDARD) || defined(MCUBOOT_SIGN_MLDSA87_PREHASH)
#    define EXPECTED_SIG_TLV  IMAGE_TLV_MLDSA87
#    define SIG_BUF_SIZE 5000
#    define EXPECTED_SIG_LEN(x) ((x) >= 2000 && (x) <= SIG_BUF_SIZE) /* always true, ASN.1 will validate */
#elif defined(MCUBOOT_SIGN_ED25519)
#    define EXPECTED_SIG_TLV IMAGE_TLV_ED25519
#    define SIG_BUF_SIZE 64
#    define EXPECTED_SIG_LEN(x) ((x) == SIG_BUF_SIZE)
#else
#    define SIG_BUF_SIZE 32 /* no signing, sha256 digest only */
#endif

#ifdef EXPECTED_SIG_TLV
#if !defined(MCUBOOT_HW_KEY)
static int
bootutil_find_key(uint8_t *keyhash, uint8_t keyhash_len, int current_sig_type)
{
    bootutil_sha256_context sha256_ctx;
    int i;
    const struct bootutil_key *key;
    uint8_t hash[32];
    int start_idx = 0;
    int end_idx = bootutil_key_cnt;

    if (keyhash_len > 32) {
        return -1;
    }

    /* For hybrid verification, select key range based on signature type */
    #ifdef MCUBOOT_VERIFY_HYBRID
    if (current_sig_type == SIG_TYPE_ECDSA) {
        /* Search only ECDSA keys (assuming they're first in the array) */
        start_idx = 0;
        end_idx = 1;  /* Change this if you have multiple ECDSA keys */
    } else if (current_sig_type == SIG_TYPE_MLDSA) {
        /* Search only MLDSA keys (assuming they're after ECDSA keys) */
        start_idx = 1;  /* Change this if you have multiple ECDSA keys */
        end_idx = bootutil_key_cnt;
    }
    #endif

    for (i = start_idx; i < end_idx; i++) {
        key = &bootutil_keys[i];

        //need to add key length here
        #ifdef CSC_ENABLE_HW_SHA

        NVIC_EnableIRQ(SHAW_INT_IRQn);
        NVIC_EnableIRQ(DMA0_INT_IRQn);

        dmaDone = false;
        digestReady = false;
        DL_SHAW_enableOutputReadyInterrupt(SHAW_INST);
        DL_SHAW_enableDMATriggerInterrupt(SHAW_INST);

        /* Set data length for this chunk - use proper dereferencing */
        DL_SHAW_setDataLength(SHAW_INST, (uint32_t)*(key->len), DL_SHAW_MODE_HASH);
        
        /* Signal mode and length available */
        DL_SHAW_signalModeAndLengthAvailable(SHAW_INST);
        uint32_t *aligned_buffer = NULL;
        // Check for word alignment of the key data
        if ((uint32_t)(key->key) % 4 != 0) {
            // Key data isn't word-aligned, create aligned copy
            uint32_t aligned_size = (*(key->len) + 3) / 4; // Size in words, rounded up
            aligned_buffer = (uint32_t *)malloc(aligned_size * sizeof(uint32_t));
            if (aligned_buffer == NULL) {
                // Handle allocation failure
                bootutil_sha256_drop(&sha256_ctx);
                return -1;
            }
            memcpy(aligned_buffer, key->key, *(key->len));
            
            DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)aligned_buffer);
        } else {
            // Key data is already word-aligned, use directly
            DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(key->key));
        }

        DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, DL_SHAW_DATA_IN_ADDR(SHAW_INST));

        uint32_t words_to_transfer = (*(key->len) + DL_SHAW_WORD_SIZE_BYTES - 1) / DL_SHAW_WORD_SIZE_BYTES;
        DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, words_to_transfer);

        // Configure DMA for word transfers
        DL_DMA_setDestWidth(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_WIDTH_WORD);
        DL_DMA_setSrcWidth(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_WIDTH_WORD);

        // Set increment modes explicitly
        DL_DMA_setSrcIncrement(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_ADDR_INCREMENT);
        DL_DMA_setDestIncrement(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_ADDR_UNCHANGED);

        DL_DMA_setTrigger(DMA0, DMA0_CH0_CHAN_ID, SHAW_DMA_TRIGGER, DL_DMA_TRIGGER_TYPE_EXTERNAL);

        DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

        /* Start DMA trigger from SHA */
        DL_SHAW_startDMA(SHAW_INST);

        while(!dmaDone){
            __WFE();  // Wait for interrupt
        }

        while (!digestReady)
        {
            __WFE();  // Wait for interrupt
        }
        /* Read the digest */
        delay_cycles(10000);
        DL_SHAW_readDigest(SHAW_INST, (uint32_t *)hash,
                                    DL_SHAW_DIGEST_SIZE_256_BYTES);

        /* Release the digest */
        DL_SHAW_releaseDigest(SHAW_INST);
        if (!memcmp(hash, keyhash, keyhash_len)) {
            bootutil_sha256_drop(&sha256_ctx);
            int result = i;  
            free(aligned_buffer);  
            return result;
        }
        
        if (aligned_buffer != NULL) {
            free(aligned_buffer);
        }

        #else
        bootutil_sha256_init(&sha256_ctx);
        bootutil_sha256_update(&sha256_ctx, key->key, *key->len);
        bootutil_sha256_finish(&sha256_ctx, hash);
        if (!memcmp(hash, keyhash, keyhash_len)) {
            bootutil_sha256_drop(&sha256_ctx);
            return i;
        }
        #endif
    }
    bootutil_sha256_drop(&sha256_ctx);
    return -1;
}
#else
extern unsigned int pub_key_len;
static int
bootutil_find_key(uint8_t image_index, uint8_t *key, uint16_t key_len, int current_sig_type)
{
    bootutil_sha256_context sha256_ctx;
    uint8_t hash[32];
    uint8_t key_hash[32];
    size_t key_hash_size = sizeof(key_hash);
    int rc;
    fih_int fih_rc;

    bootutil_sha256_init(&sha256_ctx);
    bootutil_sha256_update(&sha256_ctx, key, key_len);
    bootutil_sha256_finish(&sha256_ctx, hash);
    bootutil_sha256_drop(&sha256_ctx);

    rc = boot_retrieve_public_key_hash(image_index, key_hash, &key_hash_size);
    if (rc) {
        return rc;
    }

    /* Adding hardening to avoid this potential attack:
     *  - Image is signed with an arbitrary key and the corresponding public
     *    key is added as a TLV field.
     * - During public key validation (comparing against key-hash read from
     *   HW) a fault is injected to accept the public key as valid one.
     */
    FIH_CALL(boot_fih_memequal, fih_rc, hash, key_hash, key_hash_size);
    if (fih_eq(fih_rc, FIH_SUCCESS)) {
        bootutil_keys[0].key = key;
        pub_key_len = key_len;
        return 0;
    }

    return -1;
}
#endif /* !MCUBOOT_HW_KEY */
#endif

#ifdef MCUBOOT_HW_ROLLBACK_PROT
/**
 * Reads the value of an image's security counter.
 *
 * @param hdr           Pointer to the image header structure.
 * @param fap           Pointer to a description structure of the image's
 *                      flash area.
 * @param security_cnt  Pointer to store the security counter value.
 *
 * @return              0 on success; nonzero on failure.
 */
int32_t
bootutil_get_img_security_cnt(struct image_header *hdr,
                              const struct flash_area *fap,
                              uint32_t *img_security_cnt)
{
    struct image_tlv_iter it;
    uint32_t off;
    uint16_t len;
    int32_t rc;

    if ((hdr == NULL) ||
        (fap == NULL) ||
        (img_security_cnt == NULL)) {
        /* Invalid parameter. */
        return BOOT_EBADARGS;
    }

    /* The security counter TLV is in the protected part of the TLV area. */
    if (hdr->ih_protect_tlv_size == 0) {
        return BOOT_EBADIMAGE;
    }

    rc = bootutil_tlv_iter_begin(&it, hdr, fap, IMAGE_TLV_SEC_CNT, true);
    if (rc) {
        return rc;
    }

    /* Traverse through the protected TLV area to find
     * the security counter TLV.
     */

    rc = bootutil_tlv_iter_next(&it, &off, &len, NULL);
    if (rc != 0) {
        /* Security counter TLV has not been found. */
        return -1;
    }

    if (len != sizeof(*img_security_cnt)) {
        /* Security counter is not valid. */
        return BOOT_EBADIMAGE;
    }

    rc = LOAD_IMAGE_DATA(hdr, fap, off, img_security_cnt, len);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    return 0;
}
#endif /* MCUBOOT_HW_ROLLBACK_PROT */

/*
 * Verify the integrity of the image.
 * Return non-zero if image could not be validated/does not validate.
 */
fih_int
bootutil_img_validate(struct enc_key_data *enc_state, int image_index,
                      struct image_header *hdr, const struct flash_area *fap,
                      uint8_t *tmp_buf, uint32_t tmp_buf_sz, uint8_t *seed,
                      int seed_len, uint8_t *out_hash)
{
    uint32_t off;
    uint16_t len;
    uint16_t type;
    int sha256_valid = 0;
#ifdef EXPECTED_SIG_TLV
    fih_int valid_signature = FIH_FAILURE;
    int key_id = -1;
#ifdef MCUBOOT_HW_KEY
    /* Few extra bytes for encoding and for public exponent. */
    uint8_t key_buf[SIG_BUF_SIZE + 24];
#endif
#if defined(MCUBOOT_VERIFY_HYBRID)
    int valid_ecdsa_signatures = 0;
    int valid_mldsa_signatures = 0;
#endif
    int current_sig_type = SIG_TYPE_NONE;
#endif
    struct image_tlv_iter it;
    uint8_t buf[SIG_BUF_SIZE];
#ifdef MCUBOOT_SIGN_MLDSA87_STANDARD
    uint8_t buf_msg[SIG_BUF_SIZE];
    uint32_t size;
#endif
    uint8_t hash[32];
    int rc = 0;
    fih_int fih_rc = FIH_FAILURE;
#ifdef MCUBOOT_HW_ROLLBACK_PROT
    fih_int security_cnt = fih_int_encode(INT_MAX);
    uint32_t img_security_cnt = 0;
    fih_int security_counter_valid = FIH_FAILURE;
#endif

    rc = bootutil_img_hash(enc_state, image_index, hdr, fap, tmp_buf,
            tmp_buf_sz, hash, seed, seed_len);
    if (rc) {
        goto out;
    }

    if (out_hash) {
        memcpy(out_hash, hash, 32);
    }

    #ifdef MCUBOOT_SIGN_MLDSA87_STANDARD
    size = hdr->ih_hdr_size + hdr->ih_img_size;

    flash_area_read(fap, 0, buf_msg, size);
    #endif

    rc = bootutil_tlv_iter_begin(&it, hdr, fap, IMAGE_TLV_ANY, false);
    if (rc) {
        goto out;
    }

    /*
     * Traverse through all of the TLVs, performing any checks we know
     * and are able to do.
     */
    while (true) {
        rc = bootutil_tlv_iter_next(&it, &off, &len, &type);
        if (rc < 0) {
            goto out;
        } else if (rc > 0) {
            break;
        }

        if (type == IMAGE_TLV_SHA256) {
            /*
             * Verify the SHA256 image hash.  This must always be
             * present.
             */
            if (len != sizeof(hash)) {
                rc = -1;
                goto out;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, sizeof(hash));
            if (rc) {
                goto out;
            }

            FIH_CALL(boot_fih_memequal, fih_rc, hash, buf, sizeof(hash));
            if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
                goto out;
            }

            sha256_valid = 1;
#ifdef EXPECTED_SIG_TLV
#ifndef MCUBOOT_HW_KEY
        } else if (type == IMAGE_TLV_KEYHASH) {
            /*
             * Determine which key we should be checking.
             */
            if (len > 32) {
                rc = -1;
                goto out;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, len);
            if (rc) {
                goto out;
            }
            
            #ifdef MCUBOOT_VERIFY_HYBRID
            /* For KEYHASH TLV, determine signature type */
            if (current_sig_type == SIG_TYPE_NONE) {
                /* First key hash is for ECDSA */
                current_sig_type = SIG_TYPE_ECDSA;
            } else if (valid_ecdsa_signatures > 0) {
                /* If we've already verified ECDSA, next key hash is for MLDSA */
                current_sig_type = SIG_TYPE_MLDSA;
            } else if (current_sig_type == SIG_TYPE_MLDSA && valid_ecdsa_signatures == 0){
                fih_rc = FIH_FAILURE;
            }
            #endif
            
            key_id = bootutil_find_key(buf, len, current_sig_type);
#else
        } else if (type == IMAGE_TLV_PUBKEY) {
            /*
             * Determine which key we should be checking.
             */
            if (len > sizeof(key_buf)) {
                rc = -1;
                goto out;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, key_buf, len);
            if (rc) {
                goto out;
            }
            
            #ifdef MCUBOOT_VERIFY_HYBRID
            /* For KEYHASH TLV, determine signature type */
            if (current_sig_type == SIG_TYPE_NONE) {
                /* First key hash is for ECDSA */
                current_sig_type = SIG_TYPE_ECDSA;
            } else if (valid_ecdsa_signatures > 0) {
                /* If we've already verified ECDSA, next key hash is for MLDSA */
                current_sig_type = SIG_TYPE_MLDSA;
            } else if (current_sig_type == SIG_TYPE_MLDSA && valid_ecdsa_signatures == 0){
                fih_rc = FIH_FAILURE;
            }
            #endif
            
            key_id = bootutil_find_key(image_index, key_buf, len, current_sig_type);
#endif /* !MCUBOOT_HW_KEY */
        } else if (type == EXPECTED_SIG_TLV) {
            /* Ignore this signature if it is out of bounds. */
            if (key_id < 0 || key_id >= bootutil_key_cnt) {
                key_id = -1;
                continue;
            }
            if (!EXPECTED_SIG_LEN(len) || len > sizeof(buf)) {
                rc = -1;
                goto out;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, len);
            if (rc) {
                goto out;
            }

            #if defined(MCUBOOT_SIGN_MLDSA87_STANDARD) && !defined(MCUBOOT_VERIFY_HYBRID)
            FIH_CALL(bootutil_verify_sig, valid_signature, buf_msg, size,
                                                           buf, len, key_id);

            #else
            FIH_CALL(bootutil_verify_sig, valid_signature, hash, sizeof(hash),
                                                
                                                           buf, len, key_id);

            #endif

            #if defined(MCUBOOT_VERIFY_HYBRID)
            
            current_sig_type = SIG_TYPE_ECDSA;
            
            if (fih_eq(valid_signature, FIH_SUCCESS)) {
                valid_ecdsa_signatures++;
            }

            #endif
            
            key_id = -1;
#endif /* EXPECTED_SIG_TLV */
#ifdef MCUBOOT_HW_ROLLBACK_PROT
        } else if (type == IMAGE_TLV_SEC_CNT) {
            /*
             * Verify the image's security counter.
             * This must always be present.
             */
            if (len != sizeof(img_security_cnt)) {
                /* Security counter is not valid. */
                rc = -1;
                goto out;
            }

            rc = LOAD_IMAGE_DATA(hdr, fap, off, &img_security_cnt, len);
            if (rc) {
                goto out;
            }

            FIH_CALL(boot_nv_security_counter_get, fih_rc, image_index,
                                                           &security_cnt);
            if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
                goto out;
            }

            /* Compare the new image's security counter value against the
             * stored security counter value.
             */
            fih_rc = fih_int_encode_zero_equality(img_security_cnt <
                                   fih_int_decode(security_cnt));
            if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
                goto out;
            }

            /* The image's security counter has been successfully verified. */
            security_counter_valid = fih_rc;
#endif /* MCUBOOT_HW_ROLLBACK_PROT */
#ifdef TI_CUSTOM_TLV_DYNAMIC_KEY_MANAGEMENT
        } else if (type == IMAGE_TLV_CUSTOM_TI_ADD_KEY){
            /* Check if Length is correct, and contains a full 256-bit hash */
            if(len != 0x20){
                // not a complete hash
                rc = -1;
                goto out;
            }
            /* The address of the hash pointer will be the current flash area
             * plus the provided offset. */
            FIH_CALL(Keystore_newKeyRequest, fih_rc, (fap->fa_off + off));
        } else if (type == IMAGE_TLV_CUSTOM_TI_REVOKE_KEY){
            if(len != 0x20){
                // not a complete hash
                rc = -1;
                goto out;
            }
            FIH_CALL(Keystore_revokeKeyRequest, fih_rc, (fap->fa_off + off));
#endif

#ifdef MCUBOOT_VERIFY_HYBRID
        } else if (type == EXPECTED_MLDSA_TLV) {
            /* Ignore this signature if it is out of bounds. */
            if (key_id < 0 || key_id >= bootutil_key_cnt) {
                key_id = -1;
                continue;
            }
            if (!EXPECTED_MLDSA_LEN(len) || len > sizeof(buf)) {
                rc = -1;
                goto out;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, len);
            if (rc) {
                goto out;
            }
            valid_signature = FIH_FAILURE;
            
            FIH_CALL(bootutil_verify_mldsa_sig, valid_signature, buf_msg, size,
                                                           buf, len, key_id);
            
            current_sig_type = SIG_TYPE_MLDSA;
            
            if (fih_eq(valid_signature, FIH_SUCCESS)) {
                valid_mldsa_signatures++;
            }
            
            key_id = -1;
#endif
        }
    }

    rc = !sha256_valid;
    if (rc) {
        goto out;
    }

#ifdef EXPECTED_SIG_TLV
    fih_rc = fih_int_encode_zero_equality(fih_not_eq(valid_signature,
                                                     FIH_SUCCESS));
#endif

#if defined(MCUBOOT_VERIFY_HYBRID)
    if (valid_ecdsa_signatures == 1 && 
        valid_mldsa_signatures == 1) {
        fih_rc = FIH_SUCCESS;
    } else {
        fih_rc = FIH_FAILURE;
    }

#endif

#ifdef MCUBOOT_HW_ROLLBACK_PROT
    if (fih_not_eq(security_counter_valid, FIH_SUCCESS)) {
        rc = -1;
        goto out;
    }
#endif

out:
    if (rc) {
        fih_rc = fih_int_encode(rc);
    }

    FIH_RET(fih_rc);
}