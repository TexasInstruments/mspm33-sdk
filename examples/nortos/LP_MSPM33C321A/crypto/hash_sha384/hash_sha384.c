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
 *  @file       hash_sha384.c
 *  @brief      Hash module SHA-384 basic example
 *
 *  This code example demonstrates how to use the one-shot SHA-384 function
 *  to compute the SHA-384 hash of a message.
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

/* Length of the test message SHA384 digest, in 32-bit words */
#define TEST_MD_LEN_WORDS (12U)

/* Length of the test message SHA384 digest, in 8-bit bytes */
#define TEST_MD_LEN_BYTES (TEST_MD_LEN_WORDS << 2U)

/* The NIST CAVP SHA384 ShortMsg LEN=128b test vector message (msg) */
static const uint8_t cavp_sha384shortmsg_128_msg[TEST_MSG_LEN_BYTES] = {
    0xe1, 0xbb, 0x96, 0x7b, 0x5d, 0x37, 0x9a, 0x4a,
    0xa3, 0x90, 0x50, 0x27, 0x4d, 0x09, 0xbd, 0x93,
};

/* The NIST CAVP SHA384 ShortMsg LEN=128b test vector message digest (MD) */
static const uint8_t cavp_sha384shortmsg_128_md_exp[TEST_MD_LEN_BYTES] = {
    0x3b, 0x04, 0xf9, 0x69, 0x65, 0xad, 0x2f, 0xba,
    0xbd, 0x4d, 0xf2, 0x5d, 0x5d, 0x8c, 0x95, 0x58,
    0x9d, 0x06, 0x9c, 0x31, 0x2e, 0xe4, 0x85, 0x39,
    0x09, 0x0b, 0x2d, 0x7b, 0x49, 0x5d, 0x24, 0x46,
    0xc3, 0x1e, 0xb2, 0xb8, 0xf8, 0xff, 0xb3, 0x01,
    0x2b, 0xdc, 0xe0, 0x65, 0x32, 0x3d, 0x9f, 0x48,
};

/* SRAM buffer to store computed SHA-384 hash value */
static uint32_t nist_cavp_sha384shortmsg_128_md_act[TEST_MD_LEN_WORDS];

int main(void)
{
    crypto_hash_stat_t stat;
    uint8_t *act;
    uint32_t i;

    SYSCFG_DL_init();

    /* Compute the hash of the message */
    stat = Crypto_SHA384(&cavp_sha384shortmsg_128_msg[0],
                         TEST_MSG_LEN_BYTES,
                         &nist_cavp_sha384shortmsg_128_md_act[0]);

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_HASH_STAT_OK) {
        goto err;
    }

    /* Check computed hash against expected hash and
     * jump to error trap if not equal
     */
    act = (uint8_t*)&nist_cavp_sha384shortmsg_128_md_act[0];
    for (i=0; i<TEST_MD_LEN_BYTES; i++) {
        if (act[i] != cavp_sha384shortmsg_128_md_exp[i]) {
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
