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
 *  @file       cipher_aes256.c
 *  @brief      Cipher module AES-256 block cipher basic example
 * 
 *  This code example demonstrates how to use the AES single block function
 *  to encrypt and decrypt a single block.
 *
 *  The NIST CAVP AES ECB 1.0 test ID #635 is used to demonstrate encryption.
 *  Note that input / output buffers must be 32-bit word aligned.
 *
 *  Note that this basic form of encryption is not secure on its own. The
 *  same plaintext will always yield the same ciphertext.  This single block
 *  ECB encryption may be used as a building block for more robust schemes
 *  but should not be used on its own in most cases.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Force 32-bit alignment of 8-bit arrays */
#define CRYPTO_ALIGN32 __attribute__((aligned(4)))

/* Length of key in bytes */
#define KEY_LEN (32U)

/* Length of plain text in bytes */
#define MSG_LEN (16U)

/* The NIST test plain text vector */
static const uint8_t cavp_aesecb_id635_pt[MSG_LEN] CRYPTO_ALIGN32 = {
    0x01, 0x47, 0x30, 0xF8, 0x0A, 0xC6, 0x25, 0xFE, 0x84, 0xF0, 0x26, 0xC6, 
    0x0B, 0xFD, 0x54, 0x7D
};

/* The NIST test key vector */
static const uint8_t cavp_aesecb_id635_key[KEY_LEN] CRYPTO_ALIGN32 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* The NIST test cipher text vector */
static const uint8_t cavp_aesecb_id635_ct[MSG_LEN] CRYPTO_ALIGN32 = {
    0x5C, 0x9D, 0x84, 0x4E, 0xD4, 0x6F, 0x98, 0x85, 0x08, 0x5E, 0x5D, 0x6A, 
    0x4F, 0x94, 0xC7, 0xD7
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
    stat = Crypto_AES_setSessionKey((uint32_t*)(&cavp_aesecb_id635_key[0]), \
                                    CRYPTO_AES_KEYTYPE_256);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }                                

    /* Run the forward encryption operation */
    stat = Crypto_AES_encrypt((uint32_t*)(&cavp_aesecb_id635_pt[0]), \
                              (uint32_t*)(&ct[0]), \
                              CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed cipher text against expected cipher text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MSG_LEN; i++) {
        if (ct[i] != cavp_aesecb_id635_ct[i]) {
            goto err;
        }
    }

    /* Run the decryption operation to get the plaintext back */
    stat = Crypto_AES_decrypt((uint32_t*)(&ct[0]), \
                              (uint32_t*)(&pt[0]), \
                              CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed plain text against expected plain text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MSG_LEN; i++) {
        if (pt[i] != cavp_aesecb_id635_pt[i]) {
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
