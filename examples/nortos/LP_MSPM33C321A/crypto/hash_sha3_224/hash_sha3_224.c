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
 *  @file       hash_sha3_224.c
 *  @brief      Hash module SHA3-224 basic example
 *
 *  This code example demonstrates how to use the SHA3-224 hash function
 *  to compute the SHA3-224 hash of a message.
 *
 *  The example uses a NIST test vector to demonstrate the hash computation.
 *  The hash routine is expected to return a success status and the actual
 *  (computed) hash is expected to match the expected (stored) hash value.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Length of the test message, in 8-bit bytes
 * NIST CAVP SHA3-224 ShortMsg test vector: Len = 128 bits = 16 bytes
 */
#define TEST_MSG_LEN_BYTES (16U)

/* Length of the SHA3-224 digest, in 32-bit words (28 bytes / 4) */
#define TEST_MD_LEN_WORDS (7U)

/* Length of the SHA3-224 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* Test message from NIST CAVP SHA3-224 test vector
 * Len = 128 bits (16 bytes)
 * Msg = 2bbb42b920b7feb4e3962a1552cc390f
 */
static const uint8_t test_msg[TEST_MSG_LEN_BYTES] = {
    0x2b, 0xbb, 0x42, 0xb9, 0x20, 0xb7, 0xfe, 0xb4,
    0xe3, 0x96, 0x2a, 0x15, 0x52, 0xcc, 0x39, 0x0f,
};

/* Expected SHA3-224 digest for the test message
 * MD = 0dfa61f6b439bf8e3a6f378fe30a4134e8b2dfb652997a2a76c2789f
 */
static const uint8_t test_md_expected[TEST_MD_LEN_BYTES] = {
    0x0d, 0xfa, 0x61, 0xf6, 0xb4, 0x39, 0xbf, 0x8e,
    0x3a, 0x6f, 0x37, 0x8f, 0xe3, 0x0a, 0x41, 0x34,
    0xe8, 0xb2, 0xdf, 0xb6, 0x52, 0x99, 0x7a, 0x2a,
    0x76, 0xc2, 0x78, 0x9f,
};

/* SRAM buffer to store computed SHA3-224 hash value */
static uint32_t test_md_actual[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the SHA3-224 hash of the message */
    stat = Crypto_SHA3_224(&test_msg[0],
                           TEST_MSG_LEN_BYTES,
                           &test_md_actual[0]);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_HASH_STAT_OK) {
        goto err;
    }

    /* Check computed hash against expected hash and
     * jump to error trap if not equal
     */
    act = (uint8_t*)&test_md_actual[0];
    for (i = 0; i < TEST_MD_LEN_BYTES; i++) {
        if (act[i] != test_md_expected[i]) {
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
