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
 *  @file       hash_sha3_256.c
 *  @brief      Hash module SHA3-256 basic example
 *
 *  This code example demonstrates how to use the SHA3-256 hash function
 *  to compute the SHA3-256 hash of a message.
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
 * NIST CAVP SHA3-256 ShortMsg test vector: Len = 128 bits = 16 bytes
 */
#define TEST_MSG_LEN_BYTES (16U)

/* Length of the SHA3-256 digest, in 32-bit words (32 bytes / 4) */
#define TEST_MD_LEN_WORDS (8U)

/* Length of the SHA3-256 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* Test message from NIST CAVP SHA3-256 test vector
 * Len = 128 bits (16 bytes)
 * Msg = d83c721ee51b060c5a41438a8221e040
 */
static const uint8_t test_msg[TEST_MSG_LEN_BYTES] = {
    0xd8, 0x3c, 0x72, 0x1e, 0xe5, 0x1b, 0x06, 0x0c,
    0x5a, 0x41, 0x43, 0x8a, 0x82, 0x21, 0xe0, 0x40,
};

/* Expected SHA3-256 digest for the test message
 * MD = b87d9e4722edd3918729ded9a6d03af8256998ee088a1ae662ef4bcaff142a96
 */
static const uint8_t test_md_expected[TEST_MD_LEN_BYTES] = {
    0xb8, 0x7d, 0x9e, 0x47, 0x22, 0xed, 0xd3, 0x91,
    0x87, 0x29, 0xde, 0xd9, 0xa6, 0xd0, 0x3a, 0xf8,
    0x25, 0x69, 0x98, 0xee, 0x08, 0x8a, 0x1a, 0xe6,
    0x62, 0xef, 0x4b, 0xca, 0xff, 0x14, 0x2a, 0x96,
};

/* SRAM buffer to store computed SHA3-256 hash value */
static uint32_t test_md_actual[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the SHA3-256 hash of the message */
    stat = Crypto_SHA3_256(&test_msg[0],
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
