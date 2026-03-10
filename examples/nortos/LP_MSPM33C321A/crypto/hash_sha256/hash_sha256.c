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
 *  @file       hash_sha256.c
 *  @brief      Hash module SHA-256 basic example
 * 
 *  This code example demonstrates how to use the one-shot SHA-256 function
 *  to compute the SHA-256 hash of a message.
 *
 *  The NIST CAVP short message test vector of bit length 128b is used
 *  to demonstate the hash computation.  The hash routine is expected
 *  to return a success status and the actual (computed) hash is expected
 *  to match the expected (stored) hash value.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Length of the test message, in 8-bit bytes */
#define TEST_MSG_LEN_BYTES (16U)

/* Length of the test message SHA256 digest, in 32-bit words */
#define TEST_MD_LEN_WORDS (8U)

/* Length of the test message SHA256 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* The NIST CAVP SHA256 ShortMsg LEN=128b test vector message (msg) */
static const uint8_t cavp_sha256shortmsg_128_msg[TEST_MSG_LEN_BYTES] = {
    0xa, 0x27, 0x84, 0x7c, 0xdc, 0x98, 0xbd, 0x6f, 
    0x62, 0x22, 0xb, 0x4, 0x6e, 0xdd, 0x76, 0x2b, 
};

/* The NIST CAVP SHA256 ShortMsg LEN=128b test vector message digest (MD) */
static const uint8_t cavp_sha256shortmsg_128_md_exp[TEST_MD_LEN_BYTES] = {
    0x80, 0xc2, 0x5e, 0xc1, 0x60, 0x5, 0x87, 0xe7, 
    0xf2, 0x8b, 0x18, 0xb1, 0xb1, 0x8e, 0x3c, 0xdc, 
    0x89, 0x92, 0x8e, 0x39, 0xca, 0xb3, 0xbc, 0x25, 
    0xe4, 0xd4, 0xa4, 0xc1, 0x39, 0xbc, 0xed, 0xc4, 
};

/* SRAM buffer to store computed SHA-256 hash value */
static uint32_t nist_cavp_sha256shortmsg_128_md_act[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the hash of the message */
    stat = Crypto_SHA256(&cavp_sha256shortmsg_128_msg[0], 
                         TEST_MSG_LEN_BYTES,
                         &nist_cavp_sha256shortmsg_128_md_act[0]);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_HASH_STAT_OK) {
        goto err;
    }

    /* Check computed hash against expected hash and
     * jump to error trap if not equal 
     */
    act = (uint8_t*)&nist_cavp_sha256shortmsg_128_md_act[0];
    for (i=0; i<TEST_MD_LEN_BYTES; i++) {
        if (act[i] != cavp_sha256shortmsg_128_md_exp[i]) {
            goto err;
        }
    }

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
