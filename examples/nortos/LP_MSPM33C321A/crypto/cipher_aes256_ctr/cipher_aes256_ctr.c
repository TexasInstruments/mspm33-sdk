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
 *  @file       cipher_aes256_ctr.c
 *  @brief      Cipher module AES-256 counter (CTR) basic example
 * 
 *  This code example demonstrates how to use the AES counter (CTR) 
 *  function to encrypt and decrypt a sequence of 128-bit blocks using the 
 *  AES CTR stream cipher mode and a nonce.
 *
 *  The NIST CAVP AES CTR 1.0 test ID #65 is used to demonstrate encryption.
 *  Note that input / output buffers must be 32-bit word aligned.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Force 32-bit alignment of 8-bit arrays */
#define CRYPTO_ALIGN32 __attribute__((aligned(4)))

/* Length of nonce in bytes */
#define NONCE_LEN (12U)

/* Initial counter value */
#define COUNTER_INIT (1U)

/* Length of key in bytes */
#define KEY_LEN (32U)

/* Length of plain text in bytes */
#define MSG_LEN (80U)

/* The NIST test key vector */
static const uint8_t cavp_aesctr_id65_key[KEY_LEN] CRYPTO_ALIGN32 = {
    0x72, 0x88, 0x5f, 0xdb, 0x6e, 0x72, 0xd7, 0x3f, 0xeb, 0xff, 0xd0, 0xa2, 
    0x7c, 0xfa, 0x28, 0xf9, 0xe2, 0xcd, 0xf, 0x6b, 0xf1, 0xc7, 0xf7, 0x1f, 
    0x55, 0x86, 0x18, 0xe2, 0xb2, 0xe7, 0x20, 0xfb, 
};

/* The NIST test plain text vector */
static const uint8_t cavp_aesctr_id65_pt[MSG_LEN] CRYPTO_ALIGN32 = {
    0x60, 0xba, 0x97, 0x4b, 0xfc, 0xcb, 0xcc, 0x8b, 0x8e, 0x6f, 0x9c, 0x33, 
    0x39, 0xa0, 0x78, 0xfe, 0x3d, 0x97, 0x50, 0xa, 0x36, 0x52, 0x5f, 0x9, 
    0xd4, 0x5c, 0xa6, 0x3a, 0xfd, 0x65, 0xaf, 0x87, 0x33, 0x9, 0x42, 0xf4, 
    0xf8, 0x66, 0x91, 0x33, 0xd8, 0x45, 0xbd, 0x50, 0xd1, 0xcc, 0x7b, 0xfc, 
    0xc1, 0x15, 0x48, 0x61, 0x6f, 0x53, 0x4, 0xa3, 0xae, 0x8f, 0xb2, 0x8d, 
    0xff, 0xa4, 0xd0, 0x4c, 0xc0, 0x6d, 0xff, 0xb5, 0x9c, 0x39, 0x14, 0x86, 
    0xf8, 0xc0, 0x5e, 0x3b, 0xf5, 0xd, 0x99, 0x86, 
};

/* The NIST test nonce value */
static const uint8_t cavp_aesctr_id65_nonce[NONCE_LEN] CRYPTO_ALIGN32 = {
    0x3b, 0x1a, 0x4b, 0xf1, 0xe5, 0x5b, 0xe5, 0x69, 0xea, 0x3a, 0xf1, 0xa4, 
};

/* The NIST test cipher text vector */
static const uint8_t cavp_aesctr_id65_ct[MSG_LEN] CRYPTO_ALIGN32 = {
    0xd, 0xad, 0x34, 0x34, 0x6f, 0xe7, 0x62, 0x35, 0x8d, 0x2, 0xc3, 0x26, 
    0x60, 0x13, 0xeb, 0xbb, 0xb9, 0x1d, 0xdd, 0x8a, 0x87, 0x18, 0x86, 0xbf, 
    0x47, 0x91, 0xf1, 0xd4, 0x1, 0xa6, 0xd1, 0xb2, 0xae, 0x6f, 0x2c, 0xf4, 
    0x5a, 0x78, 0x92, 0x5a, 0x35, 0x98, 0x3a, 0x89, 0xa, 0x62, 0x78, 0x47, 
    0x91, 0x92, 0xc7, 0xad, 0xeb, 0x98, 0xcc, 0xc2, 0xe0, 0x1f, 0x5c, 0x6a, 
    0x98, 0xa0, 0x7c, 0x28, 0xbd, 0xbb, 0x77, 0x31, 0x30, 0xb9, 0x8e, 0xd0, 
    0x7e, 0xfd, 0xeb, 0x3e, 0xe6, 0xfc, 0x3d, 0xd9, 
};

/* SRAM buffers to store computed plaintext and ciphertext values */
static uint8_t ct[MSG_LEN] CRYPTO_ALIGN32;
static uint8_t pt[MSG_LEN] CRYPTO_ALIGN32;

int main(void)
{
    crypto_aes_stat_t stat;
    uint32_t i;

    SYSCFG_DL_init();

    /* Activate the AES module for subsequent operations */
    Crypto_AES_init();

    /* Load the 256-bit AES session key */
    stat = Crypto_AES_setSessionKey((uint32_t*)(&cavp_aesctr_id65_key[0]), \
                                    CRYPTO_AES_KEYTYPE_256);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Run the forward encryption operation */
    stat = Crypto_AES_CTR_encrypt((uint32_t*)(&cavp_aesctr_id65_pt[0]), \
                                  (uint32_t*)(&ct[0]), \
                                  MSG_LEN,
                                  (uint32_t*)(&cavp_aesctr_id65_nonce[0]), \
                                  COUNTER_INIT,
                                  CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed cipher text against expected cipher text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MSG_LEN; i++) {
        if (ct[i] != cavp_aesctr_id65_ct[i]) {
            goto err;
        }
    }

    /* Run the decryption operation to get the plaintext back */
    stat = Crypto_AES_CTR_decrypt((uint32_t*)(&ct[0]), \
                                  (uint32_t*)(&pt[0]), \
                                  MSG_LEN,
                                  (uint32_t*)(&cavp_aesctr_id65_nonce[0]), \
                                  COUNTER_INIT,
                                  CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed plain text against expected plain text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MSG_LEN; i++) {
        if (pt[i] != cavp_aesctr_id65_pt[i]) {
            goto err;
        }
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
