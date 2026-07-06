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
 *  @file       hash_sha512.c
 *  @brief      Hash module SHA-512 basic example
 *
 *  This code example demonstrates how to use the one-shot SHA-512 function
 *  to compute the SHA-512 hash of a message.
 *
 *  The NIST CAVP short message test vector of bit length 128b is used
 *  to demonstrate the hash computation.  The hash routine is expected
 *  to return a success status and the actual (computed) hash is expected
 *  to match the expected (stored) hash value.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Length of the test message, in 8-bit bytes */
#define TEST_MSG_LEN_BYTES (16U)

/* Length of the test message SHA512 digest, in 32-bit words */
#define TEST_MD_LEN_WORDS (16U)

/* Length of the test message SHA512 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* The NIST CAVP SHA512 ShortMsg LEN=128b test vector message (msg) */
static const uint8_t cavp_sha512shortmsg_128_msg[TEST_MSG_LEN_BYTES] = {
    0xcd, 0x67, 0xbd, 0x40, 0x54, 0xaa, 0xa3, 0xba,
    0xa0, 0xdb, 0x17, 0x8c, 0xe2, 0x32, 0xfd, 0x5a,
};

/* The NIST CAVP SHA512 ShortMsg LEN=128b test vector message digest (MD) */
static const uint8_t cavp_sha512shortmsg_128_md_exp[TEST_MD_LEN_BYTES] = {
    0x0d, 0x85, 0x21, 0xf8, 0xf2, 0xf3, 0x90, 0x03,
    0x32, 0xd1, 0xa1, 0xa5, 0x5c, 0x60, 0xba, 0x81,
    0xd0, 0x4d, 0x28, 0xdf, 0xe8, 0xc5, 0x04, 0xb6,
    0x32, 0x8a, 0xe7, 0x87, 0x92, 0x5f, 0xe0, 0x18,
    0x8f, 0x2b, 0xa9, 0x1c, 0x3a, 0x9f, 0x0c, 0x16,
    0x53, 0xc4, 0xbf, 0x0a, 0xda, 0x35, 0x64, 0x55,
    0xea, 0x36, 0xfd, 0x31, 0xf8, 0xe7, 0x3e, 0x39,
    0x51, 0xca, 0xd4, 0xeb, 0xba, 0x8c, 0x6e, 0x04,
};

/* SRAM buffer to store computed SHA-512 hash value */
static uint32_t nist_cavp_sha512shortmsg_128_md_act[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the hash of the message */
    stat = Crypto_SHA512(&cavp_sha512shortmsg_128_msg[0],
                         TEST_MSG_LEN_BYTES,
                         &nist_cavp_sha512shortmsg_128_md_act[0]);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_HASH_STAT_OK) {
        goto err;
    }

    /* Check computed hash against expected hash and
     * jump to error trap if not equal
     */
    act = (uint8_t*)&nist_cavp_sha512shortmsg_128_md_act[0];
    for (i=0; i<TEST_MD_LEN_BYTES; i++) {
        if (act[i] != cavp_sha512shortmsg_128_md_exp[i]) {
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
