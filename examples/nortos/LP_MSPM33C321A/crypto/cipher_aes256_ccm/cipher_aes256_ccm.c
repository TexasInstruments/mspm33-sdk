 /*
 * Copyright (c) 2025, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       cipher_aes256_ccm.c
 *  @brief      Cipher module AES-256 counter with CBC-MAC (CCM) AEAD example
 * 
 *  This code example demonstrates how to use the AES CCM AEAD (authenticated
 *  encryption with additional data).
 *
 *  function to encrypt/generate and decrypt/verify a payload with associated
 *  data.  The decrypt/verify steps are done twice- once with valid data (to
 *  show correct tag verification) and then the cipher text is modified and
 *  the decrypt/verify is again executed to show invalid tag error detection.
 *
 *  The NIST CAVP AES CCM 1.0 test ID #89 is used to demonstrate CCM.
 *  Here, the input / output buffers do not need to be 32-bit word aligned.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Force 32-bit alignment of 8-bit arrays */
#define CRYPTO_ALIGN32 __attribute__((aligned(4)))

/* Length of key in bytes */
#define KEY_LEN (32U)

/* Length of nonce in bytes */
#define NONCE_LEN (13U)

/* Initial counter value */
#define AAD_LEN (20U)

/* Length of payload in bytes */
#define PAYLOAD_LEN (32U)

/* Length of authentication tag in bytes */
#define TAG_LEN (16U)

/* The NIST test key vector */
static const uint8_t cavp_aesccm_id89_key[KEY_LEN] CRYPTO_ALIGN32 = {
    0x49, 0x75, 0xd, 0x13, 0xe, 0x6c, 0xe2, 0xcd, 0x25, 0x39, 0xbb, 0xde, 
    0x89, 0xe6, 0x1c, 0x8c, 0x30, 0x50, 0x1d, 0xda, 0x36, 0x1d, 0x32, 0xa8, 
    0xbb, 0x71, 0xe0, 0x97, 0xcc, 0x7, 0xe4, 0x80
};

/* The NIST test nonce vector */
static const uint8_t cavp_aesccm_id89_nonce[NONCE_LEN] = {
    0xfc, 0x9c, 0xe0, 0xdf, 0x0, 0x38, 0xc2, 0x6f, 0x8d, 0xac, 0x96, 0x3c, 
    0x48
};

/* The NIST additional associated data (AAD) vector */
static const uint8_t cavp_aesccm_id89_aad[AAD_LEN] = {
    0xbc, 0x9a, 0xa8, 0x63, 0x74, 0xb5, 0xff, 0xe2, 0xab, 0xc5, 0xe, 0xb4, 
    0x94, 0x32, 0xa2, 0x21, 0x37, 0x3c, 0xe3, 0x2e
};

/* The NIST test plain text vector */
static const uint8_t cavp_aesccm_id89_pt[PAYLOAD_LEN] = {
    0xed, 0x3d, 0x7b, 0x4e, 0x2f, 0x52, 0x87, 0x5a, 0xbd, 0x64, 0x19, 0xce, 
    0x89, 0x9b, 0x72, 0x19, 0x5a, 0x3c, 0x94, 0x90, 0x16, 0x29, 0x85, 0xc4, 
    0xc4, 0x3d, 0x65, 0x96, 0xc6, 0x9f, 0x32, 0x3e, 
};

/* The NIST test cipher text vector */
static const uint8_t cavp_aesccm_id89_ct[PAYLOAD_LEN] = {
    0xae, 0xf0, 0xbf, 0xbd, 0x5b, 0xfa, 0x66, 0xcd, 0xdb, 0x7, 0xa6, 0x23, 
    0xdd, 0x1b, 0x5f, 0x12, 0x36, 0x0, 0xfa, 0xd5, 0xea, 0x8d, 0x65, 0xa7, 
    0x91, 0x36, 0x3c, 0xf, 0xac, 0x21, 0x2, 0x15, 
};

/* The NIST test authentication tag vector */
static const uint8_t cavp_aesccm_id89_tag[TAG_LEN] = {
    0x2c, 0xff, 0xe3, 0xef, 0x81, 0x5d, 0xd2, 0xd1, 0xbf, 0xa, 0xe2, 0x4f, 
    0xcb, 0xcf, 0x7c, 0x61, 
};

/* SRAM buffers to store computed plaintext and ciphertext values */
static uint8_t ct[PAYLOAD_LEN];
static uint8_t pt[PAYLOAD_LEN];
static uint8_t tag[TAG_LEN];

int main(void)
{
    crypto_aes_stat_t stat;
    uint32_t i;

    SYSCFG_DL_init();

    /* Activate the AES module for subsequent operations */
    Crypto_AES_init();

    /* Load the 256-bit AES session key */
    stat = Crypto_AES_setSessionKey((uint32_t*)(&cavp_aesccm_id89_key[0]), \
                                    CRYPTO_AES_KEYTYPE_256);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Run the forward encryption operation */
    stat = Crypto_AES_CCM_encryptAndGen(
        &cavp_aesccm_id89_nonce[0], NONCE_LEN,
        cavp_aesccm_id89_aad, AAD_LEN,
        &cavp_aesccm_id89_pt[0], &ct[0], PAYLOAD_LEN,
        &tag[0], TAG_LEN,
        CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed cipher text against expected cipher text and
     * jump to error trap if not equal 
     */
    for (i=0; i<PAYLOAD_LEN; i++) {
        if (ct[i] != cavp_aesccm_id89_ct[i]) {
            goto err;
        }
    }

    /* Check computed tag against expected tag and
     * jump to error trap if not equal 
     */
     for (i=0; i<TAG_LEN; i++) {
        if (tag[i] != cavp_aesccm_id89_tag[i]) {
            goto err;
        }
    }

    /* Run the decryption operation to get the plaintext back */
    stat = Crypto_AES_CCM_decryptAndVer(
        &cavp_aesccm_id89_nonce[0], NONCE_LEN,
        cavp_aesccm_id89_aad, AAD_LEN,
        &ct[0], &pt[0], PAYLOAD_LEN,
        &tag[0], TAG_LEN,
        CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK_TAG_VALID) {
        goto err;
    }

    /* Check computed plain text against expected plain text and
     * jump to error trap if not equal 
     */
    for (i=0; i<PAYLOAD_LEN; i++) {
        if (pt[i] != cavp_aesccm_id89_pt[i]) {
            goto err;
        }
    }

    /* Force a change to the cipher text to break the authentication*/
    ct[0]++;

    /* Run the decryption operation to get the plaintext back */
    stat = Crypto_AES_CCM_decryptAndVer(
        &cavp_aesccm_id89_nonce[0], NONCE_LEN,
        cavp_aesccm_id89_aad, AAD_LEN,
        &ct[0], &pt[0], PAYLOAD_LEN,
        &tag[0], TAG_LEN,
        CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not flagged as invalid tag, jump to error trap */
    if (stat != CRYPTO_AES_TAG_INVALID_ERR) {
        goto err;
    }

    /* Deactivate the AES module after operations are completed */
    Crypto_AES_deinit();

    /* Trap here on a pass condition */
    while (1) { 
        __NOP(); 
    }

err:
    /* Trap here on an error condition */
    while (1) { 
        __NOP(); 
    }
}
