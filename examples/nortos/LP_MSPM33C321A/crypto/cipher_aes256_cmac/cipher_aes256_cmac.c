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
 *  @file       cipher_aes256_cmac.c
 *  @brief      Cipher module AES-256 CMAC generate/verify basic example
 * 
 *  This code example demonstrates how to use the AES CMAC
 *  function to generate and verify a CMAC tag for a given message.
 *
 *  The NIST CAVP AES CMAC test vector #52 is used to demonstrate generation.
 *  Note that input buffer must be 32-bit word aligned, but the length
 *  does not need to be 32-bit or 128-bit aligned (it can be an arbitrary
 *  size, and the library will pad it as per NIST SP 800-38B).
 *
 *  NOTE: This example uses a MAC of length 5 to demonstrate that the MAC
 *        can be smaller than 1 128-bit block.  However, TI recommends choosing
 *        the largest MAC size (16 bytes / 128 bits) when possible.
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
#define MSG_LEN (10U)

/* Length of MAC output in bytes */
#define MAC_LEN (5U)

/* The NIST test message vector */
static const uint8_t cavp_aescmac_id52_msg[MSG_LEN] CRYPTO_ALIGN32 = {
    0xd3, 0xb2, 0x73, 0x43, 0x11, 0x9c, 0xa, 0xc0, 0xa, 0x98
};

/* The NIST test key vector */
static const uint8_t cavp_aescmac_id52_key[KEY_LEN] CRYPTO_ALIGN32 = {
    0x8d, 0xe3, 0xb, 0x7, 0xe8, 0x35, 0x2f, 0x91, 0xac, 0x6, 0xaa, 0x96, 
    0x7c, 0x3a, 0x62, 0xb4, 0x8d, 0x3, 0x50, 0x53, 0xb9, 0xa6, 0x31, 0x88, 
    0xcf, 0x83, 0x7e, 0x3a, 0x2e, 0x50, 0x9f, 0x83
};

/* The NIST test MAC vector */
static const uint8_t cavp_aescmac_id52_mac[MAC_LEN] = {
    0x71, 0x53, 0x4, 0x0, 0xab
};

/* SRAM buffers to store computed MAC value */
static uint8_t mac[MAC_LEN];

int main(void)
{
    crypto_aes_stat_t stat;
    uint32_t i;

    SYSCFG_DL_init();

    /* Activate the AES module for subsequent operations */
    Crypto_AES_init();

    /* Load the 256-bit AES session key */
    stat = Crypto_AES_setSessionKey((uint32_t*)(&cavp_aescmac_id52_key[0]), \
                                    CRYPTO_AES_KEYTYPE_256);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }                                

    /* Generate MAC */
    stat = Crypto_AES_CMAC_genTag((uint32_t*)(&cavp_aescmac_id52_msg[0]), \
                                  &mac[0], \
                                  MSG_LEN, \
                                  MAC_LEN, \
                                  CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK) {
        goto err;
    }

    /* Check computed cipher text against expected cipher text and
     * jump to error trap if not equal 
     */
    for (i=0; i<MAC_LEN; i++) {
        if (mac[i] != cavp_aescmac_id52_mac[i]) {
            goto err;
        }
    }

    /* Verify MAC */
    stat = Crypto_AES_CMAC_verTag((uint32_t*)(&cavp_aescmac_id52_msg[0]), \
                                  &mac[0], \
                                  MSG_LEN, \
                                  MAC_LEN, \
                                  CRYPTO_AES_KEYSEL_SESSION);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_AES_OK_TAG_VALID) {
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
