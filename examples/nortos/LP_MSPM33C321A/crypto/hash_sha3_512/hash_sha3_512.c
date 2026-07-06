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
 *  @file       hash_sha3_512.c
 *  @brief      Hash module SHA3-512 basic example
 *
 *  This code example demonstrates how to use the SHA3-512 hash function
 *  to compute the SHA3-512 hash of a message.
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
 * NIST CAVP SHA3-512 ShortMsg test vector: Len = 128 bits = 16 bytes
 */
#define TEST_MSG_LEN_BYTES (16U)

/* Length of the SHA3-512 digest, in 32-bit words (64 bytes / 4) */
#define TEST_MD_LEN_WORDS (16U)

/* Length of the SHA3-512 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* Test message from NIST CAVP SHA3-512 test vector
 * Len = 128 bits (16 bytes)
 * Msg = 054095ba531eec22113cc345e83795c7
 */
static const uint8_t test_msg[TEST_MSG_LEN_BYTES] = {
    0x05, 0x40, 0x95, 0xba, 0x53, 0x1e, 0xec, 0x22,
    0x11, 0x3c, 0xc3, 0x45, 0xe8, 0x37, 0x95, 0xc7,
};

/* Expected SHA3-512 digest for the test message
 * MD = f3adf5ccf2830cd621958021ef998252f2b6bc4c135096839586d5064a2978154ea076c600a97364bce0e9aab43b7f1f2da93537089de950557674ae6251ca4d
 */
static const uint8_t test_md_expected[TEST_MD_LEN_BYTES] = {
    0xf3, 0xad, 0xf5, 0xcc, 0xf2, 0x83, 0x0c, 0xd6,
    0x21, 0x95, 0x80, 0x21, 0xef, 0x99, 0x82, 0x52,
    0xf2, 0xb6, 0xbc, 0x4c, 0x13, 0x50, 0x96, 0x83,
    0x95, 0x86, 0xd5, 0x06, 0x4a, 0x29, 0x78, 0x15,
    0x4e, 0xa0, 0x76, 0xc6, 0x00, 0xa9, 0x73, 0x64,
    0xbc, 0xe0, 0xe9, 0xaa, 0xb4, 0x3b, 0x7f, 0x1f,
    0x2d, 0xa9, 0x35, 0x37, 0x08, 0x9d, 0xe9, 0x50,
    0x55, 0x76, 0x74, 0xae, 0x62, 0x51, 0xca, 0x4d,
};

/* SRAM buffer to store computed SHA3-512 hash value */
static uint32_t test_md_actual[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the SHA3-512 hash of the message */
    stat = Crypto_SHA3_512(&test_msg[0],
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
