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
 *  @file       ec_ecdsa_p256_sha256_verify.c
 *  @brief      Elliptic curve module ECDSA P-256 verify basic example
 * 
 *  This code example demonstrates how to use the ECDSA verify function
 *  over the NIST curve P-256 to verify a signature.
 *
 *  This example uses SHA-256 for the message pre-hash.
 *
 *  The NIST CAVP ECDSA verify test case ID #x is used here
 *  to demonstate the sign computation.  The routine is expected
 *  to return a VALID status.  To test the INVALID signature case, change
 *  any part of the message and re-run the example to confirm that INVALID is
 *  reported.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* Force 32-bit alignment of 8-bit arrays */
#define CRYPTO_ALIGN32 __attribute__((aligned(4)))

/* Length of SHA-256 message digest in 32-bit words */
#define MD_LEN (8U)

/* The NIST CAVP ECDSA verify test #174 message vector */
static const uint8_t cavp_ecdsa_p256_sha256_verify_id174_msg[128] = {
    0xBA, 0x91, 0x8D, 0x7A, 0xC0, 0xC5, 0x8A, 0x71, 0x4E, 0xF1, 0x2D, 0x97,
    0xBA, 0xFD, 0x17, 0xDE, 0x60, 0x4F, 0xD5, 0xD4, 0xA8, 0x8B, 0xCC, 0x8B,
    0x66, 0xDB, 0x58, 0xDF, 0x31, 0x39, 0x52, 0xBE, 0x8D, 0xBD, 0x46, 0x8F,
    0xF3, 0x8D, 0x95, 0x12, 0x23, 0x5B, 0x48, 0x7E, 0xAD, 0xAC, 0x71, 0x3E,
    0xC4, 0xEE, 0x42, 0xA4, 0x49, 0x2F, 0x56, 0x01, 0x20, 0x81, 0x07, 0x55,
    0xD7, 0xAD, 0xCD, 0x97, 0x70, 0xDA, 0x43, 0x2B, 0x3B, 0xDB, 0xCC, 0xA2,
    0xF4, 0x1E, 0x04, 0xFE, 0x8C, 0xED, 0x0C, 0x6D, 0x30, 0x30, 0x8C, 0xFE,
    0xD0, 0xC2, 0x99, 0xD0, 0xE8, 0x82, 0x6A, 0xC0, 0x2F, 0x89, 0x31, 0x27,
    0x5A, 0xD4, 0x6B, 0xB9, 0x19, 0xA2, 0x6E, 0x50, 0xEC, 0x13, 0x66, 0xE2,
    0x00, 0x3F, 0x83, 0x8A, 0x41, 0x37, 0xDE, 0x09, 0xD5, 0x28, 0x6B, 0xDC,
    0x94, 0x5D, 0xCE, 0xAA, 0x2C, 0xB9, 0xB6, 0x5A
};

/* The NIST CAVP ECDSA verify test #174 pk X vector */
static const uint8_t cavp_ecdsa_p256_sha256_verify_id174_pk_x[32] = {
    0x21, 0x68, 0xDC, 0xF5, 0xE4, 0x2F, 0x55, 0x1D, 0x39, 0xCA, 0x7E, 0x81,
    0x30, 0x37, 0x5C, 0x60, 0xB9, 0x3F, 0x63, 0x0A, 0x5E, 0xD7, 0x03, 0x98,
    0x5F, 0x6E, 0x72, 0x6B, 0x79, 0x17, 0x79, 0xBF
};

/* The NIST CAVP ECDSA verify test #174 pk Y vector */
static const uint8_t cavp_ecdsa_p256_sha256_verify_id174_pk_y[32] = {
    0xF9, 0xA9, 0x84, 0x19, 0x37, 0x6D, 0x5B, 0xD4, 0x80, 0xB9, 0x58, 0x11,
    0x30, 0x73, 0x59, 0x0A, 0x4B, 0x10, 0x96, 0xEE, 0xF0, 0x9A, 0x27, 0x5D,
    0x0E, 0x2F, 0xBC, 0x67, 0x5F, 0x6B, 0x11, 0x20
};

/* The NIST CAVP ECDSA verify test #174 signature R vector */
static const uint8_t cavp_ecdsa_p256_sha256_verify_id174_sig_r[32] =  {
    0xB1, 0xE6, 0x1A, 0x25, 0xBE, 0xFF, 0xCA, 0xA1, 0x55, 0x24, 0x91, 0xFD,
    0x75, 0xBD, 0x9C, 0x62, 0x87, 0x66, 0x77, 0x32, 0x06, 0x84, 0xCD, 0x21,
    0x47, 0x44, 0x3E, 0x16, 0xB2, 0xCA, 0xDD, 0xA4
};

/* The NIST CAVP ECDSA verify test #174 signature S vector */
static const uint8_t cavp_ecdsa_p256_sha256_verify_id174_sig_s[32] = {
    0x06, 0xFC, 0x00, 0xEB, 0xE4, 0x9C, 0xB3, 0x81, 0x35, 0xB1, 0x35, 0xED,
    0x5B, 0x8B, 0x8D, 0xED, 0x2F, 0x1B, 0x77, 0x68, 0x9B, 0xC1, 0x38, 0xA5,
    0xE1, 0x95, 0xF3, 0x8B, 0x4E, 0x6B, 0xA9, 0xD0
};

/* SRAM buffer to store the SHA-256 message digest (32 bytes / 8 words) */
static uint32_t md[MD_LEN];

int main(void)
{
    crypto_hash_stat_t hashStat;
    crypto_ec_stat_t ecdsaStat;
    uint32_t i;
    uint8_t *p_md;

    SYSCFG_DL_init();

    /* Compute the message SHA-256 hash digest */
    hashStat = Crypto_SHA256(cavp_ecdsa_p256_sha256_verify_id174_msg, 
                             sizeof(cavp_ecdsa_p256_sha256_verify_id174_msg), 
                             md);

    /* If the operation was not successful, jump to error trap */
    if (hashStat != CRYPTO_HASH_STAT_OK) {
        goto err;
    }
 
    /* Run the verify operation */
    ecdsaStat = Crypto_ECDSA_P256_verify(
        cavp_ecdsa_p256_sha256_verify_id174_pk_x,
        cavp_ecdsa_p256_sha256_verify_id174_pk_y,
        (uint8_t*)md,
        cavp_ecdsa_p256_sha256_verify_id174_sig_r,
        cavp_ecdsa_p256_sha256_verify_id174_sig_s
        );

    /* If the operation was not successful, jump to error trap */
    if (ecdsaStat != CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE) {
        goto err;
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
