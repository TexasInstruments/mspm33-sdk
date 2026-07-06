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
 *  @file       hash_sha3_384.c
 *  @brief      Hash module SHA3-384 basic example
 *
 *  This code example demonstrates how to use the SHA3-384 hash function
 *  to compute the SHA3-384 hash of a message.
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
 * NIST CAVP SHA3-384 ShortMsg test vector: Len = 128 bits = 16 bytes
 */
#define TEST_MSG_LEN_BYTES (16U)

/* Length of the SHA3-384 digest, in 32-bit words (48 bytes / 4) */
#define TEST_MD_LEN_WORDS (12U)

/* Length of the SHA3-384 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* Test message from NIST CAVP SHA3-384 test vector
 * Len = 128 bits (16 bytes)
 * Msg = 65b27f6c5578a4d5d9f6519c554c3097
 */
static const uint8_t test_msg[TEST_MSG_LEN_BYTES] = {
    0x65, 0xb2, 0x7f, 0x6c, 0x55, 0x78, 0xa4, 0xd5,
    0xd9, 0xf6, 0x51, 0x9c, 0x55, 0x4c, 0x30, 0x97,
};

/* Expected SHA3-384 digest for the test message
 * MD = dd734f4987fe1a71455cf9fb1ee8986882c82448827a7880fc90d2043c33b5cbc0ed58b8529e4c6bc3a7288829e0a40d
 */
static const uint8_t test_md_expected[TEST_MD_LEN_BYTES] = {
    0xdd, 0x73, 0x4f, 0x49, 0x87, 0xfe, 0x1a, 0x71,
    0x45, 0x5c, 0xf9, 0xfb, 0x1e, 0xe8, 0x98, 0x68,
    0x82, 0xc8, 0x24, 0x48, 0x82, 0x7a, 0x78, 0x80,
    0xfc, 0x90, 0xd2, 0x04, 0x3c, 0x33, 0xb5, 0xcb,
    0xc0, 0xed, 0x58, 0xb8, 0x52, 0x9e, 0x4c, 0x6b,
    0xc3, 0xa7, 0x28, 0x88, 0x29, 0xe0, 0xa4, 0x0d,
};

/* SRAM buffer to store computed SHA3-384 hash value */
static uint32_t test_md_actual[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the SHA3-384 hash of the message */
    stat = Crypto_SHA3_384(&test_msg[0],
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
