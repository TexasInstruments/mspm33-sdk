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
 *  @file       cipher_aes256_cbc.c
 *  @brief      Cipher module AES-256 cipher block chaining (CBC) basic example
 * 
 *  This code example demonstrates how to use the AES cipher block chain 
 *  function to encrypt and decrypt a sequence of 128-bit blocks using the 
 *  AES CBC block cipher mode and an initialization vector.
 *
 *  The NIST CAVP AES CBC 1.0 test ID #2108 is used to demonstrate encryption.
 *  Note that input / output buffers must be 32-bit word aligned.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Force 32-bit alignment of 8-bit arrays */
#define CRYPTO_ALIGN32 __attribute__((aligned(4)))

/* Length of initialization vector in bytes */
#define IV_LEN (16U)

/* Length of key in bytes */
#define KEY_LEN (32U)

/* Length of plain text in bytes */
#define MSG_LEN (160U)

/* The NIST test plain text vector */
static const uint8_t cavp_aescbc_id2108_pt[MSG_LEN] CRYPTO_ALIGN32 = {
    0x2A, 0x45, 0x72, 0x7E, 0x60, 0xFD, 0x03, 0x77, 0x20, 0xE4, 0xB1, 0x52, 
    0xAE, 0x9C, 0xE0, 0x55, 0x7D, 0xF9, 0xBB, 0xC4, 0xA2, 0xFC, 0xDB, 0xA4, 
    0xD7, 0x61, 0x12, 0x07, 0x2F, 0x76, 0x0B, 0x6A, 0x06, 0x69, 0xE8, 0xAD, 
    0x52, 0x1C, 0x3B, 0x9C, 0x8B, 0xC3, 0x86, 0x7E, 0x93, 0xD3, 0x6B, 0xDD, 
    0xFB, 0x94, 0x63, 0xB3, 0x25, 0x0D, 0x95, 0x12, 0xF3, 0xC5, 0x9A, 0x9C, 
    0x9A, 0x8E, 0x3A, 0xA6, 0x62, 0x44, 0x25, 0x19, 0x79, 0xDB, 0xE9, 0xE0, 
    0xAD, 0xE0, 0xD1, 0x38, 0x0E, 0xCE, 0x69, 0x71, 0xFD, 0xAC, 0x96, 0x6A, 
    0x1D, 0x6D, 0xAF, 0x2C, 0x83, 0x62, 0x88, 0x3C, 0xFD, 0xE9, 0x7C, 0x5C, 
    0xFB, 0x95, 0x4C, 0xFD, 0x45, 0x68, 0x89, 0x92, 0xFE, 0x81, 0x21, 0xD4, 
    0x39, 0x18, 0x74, 0x7C, 0x01, 0x97, 0x85, 0x16, 0xEB, 0x36, 0x3C, 0xAF, 
    0x42, 0x6A, 0x82, 0x03, 0x6D, 0xE8, 0x62, 0xCE, 0xB5, 0x0B, 0x51, 0x83, 
    0x4A, 0x6A, 0x09, 0x83, 0xC3, 0x82, 0xA0, 0x59, 0x1D, 0xF1, 0x97, 0x05, 
    0x65, 0xF6, 0x24, 0x5F, 0xB8, 0x32, 0x67, 0x4C, 0x18, 0x97, 0x67, 0x4E,
    0xEB, 0x4D, 0x14, 0x9F
};

/* The NIST test key vector */
static const uint8_t cavp_aescbc_id2108_key[KEY_LEN] CRYPTO_ALIGN32 = {
    0x31, 0x59, 0x00, 0x14, 0x04, 0xB8, 0x56, 0x27, 0xB9, 0xB4, 0xBB, 0x4B,
    0xCD, 0xFD, 0x9F, 0x42, 0xE0, 0xC7, 0x20, 0x8A, 0xA3, 0xEF, 0xFC, 0x0A,
    0xCA, 0xF3, 0x64, 0xEC, 0xA8, 0xBB, 0x06, 0x2C
};

/* The NIST test initialization vector */
static const uint8_t cavp_aescbc_id2108_iv[IV_LEN] CRYPTO_ALIGN32 = {                                                                                                                    
    0x56, 0x95, 0x40, 0xC1, 0x55, 0xA4, 0xD1, 0xEC, 0xF0, 0x90, 0x51, 0x00, 
    0xBD, 0x76, 0xD1, 0x95
};

/* The NIST test cipher text vector */
static const uint8_t cavp_aescbc_id2108_ct[MSG_LEN] CRYPTO_ALIGN32 = {
    0x76, 0x4C, 0xAE, 0x30, 0x87, 0x4B, 0xF6, 0x1E, 0xD5, 0x8C, 0x17, 0x16,
    0x78, 0xDF, 0xF2, 0x60, 0x26, 0xA6, 0x90, 0x42, 0x3F, 0x56, 0xFC, 0x46,
    0xE1, 0xEC, 0x46, 0xE6, 0xD4, 0x39, 0x6E, 0x60, 0x63, 0x7A, 0x3B, 0x07,
    0x29, 0x01, 0x4A, 0x22, 0xBF, 0x46, 0x89, 0x82, 0x6C, 0x87, 0xC0, 0x2E,
    0xF4, 0x79, 0x00, 0xFF, 0x36, 0x88, 0x89, 0x57, 0x6B, 0x14, 0xC1, 0x97,
    0xA1, 0xB0, 0xED, 0xBB, 0x93, 0x3D, 0x39, 0x84, 0x2C, 0xB8, 0xCD, 0xC0,
    0x3D, 0xDE, 0x1F, 0x1F, 0x65, 0x84, 0x61, 0x9E, 0x17, 0x55, 0x94, 0xCE,
    0x14, 0xC5, 0xA4, 0xA3, 0xF6, 0x26, 0x3E, 0xDF, 0xD2, 0x53, 0x0B, 0x7B,
    0xD4, 0x84, 0x7D, 0x7A, 0x9B, 0xFB, 0x8E, 0x60, 0xD7, 0xBC, 0x80, 0x72,
    0x60, 0xAF, 0xE2, 0x31, 0x51, 0x00, 0x78, 0x2B, 0x1C, 0xCF, 0x9E, 0x67,
    0xD7, 0x27, 0x4F, 0xC4, 0xFB, 0x9A, 0x94, 0x3A, 0xC3, 0x62, 0x26, 0x83,
    0x0F, 0xA8, 0x46, 0x49, 0xF6, 0x50, 0x73, 0x68, 0x09, 0xC3, 0xFD, 0xA0,
    0x0F, 0x8F, 0xC9, 0x9C, 0xE7, 0x82, 0x44, 0xCA, 0x60, 0xC9, 0xE4, 0x80,
    0xA6, 0x00, 0x96, 0x6D
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
    stat = Crypto_AES_setSessionKey((uint32_t*)(&cavp_aescbc_id2108_key[0]), \
                                    CRYPTO_AES_KEYTYPE_256);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }                                

    /* Run the forward encryption operation */
    stat = Crypto_AES_CBC_encrypt((uint32_t*)(&cavp_aescbc_id2108_pt[0]), \
                                  (uint32_t*)(&ct[0]), \
                                  MSG_LEN,
                                  (uint32_t*)(&cavp_aescbc_id2108_iv[0]), \
                                  CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed cipher text against expected cipher text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MSG_LEN; i++) {
        if (ct[i] != cavp_aescbc_id2108_ct[i]) {
            goto err;
        }
    }

    /* Run the decryption operation to get the plaintext back */
    stat = Crypto_AES_CBC_decrypt((uint32_t*)(&ct[0]), \
                                  (uint32_t*)(&pt[0]), \
                                  MSG_LEN,
                                  (uint32_t*)(&cavp_aescbc_id2108_iv[0]), \
                                  CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed plain text against expected plain text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MSG_LEN; i++) {
        if (pt[i] != cavp_aescbc_id2108_pt[i]) {
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
