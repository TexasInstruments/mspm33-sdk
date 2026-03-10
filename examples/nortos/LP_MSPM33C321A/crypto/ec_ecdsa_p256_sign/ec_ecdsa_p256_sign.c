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
 *  @file       ec_ecdsa_p256_sign.c
 *  @brief      Elliptic curve module ECDSA P-256 sign basic example
 * 
 *  This code example demonstrates how to use the ECDSA sign function
 *  over the NIST curve P-256 to generate a signature.
 *
 *  This example focuses on the ECDSA operation, and excludes any pre hashing.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* ECDSA P-256 operates on 32-byte parameters */
#define P256_UNIT_LEN (32U)

/* The NIST CAVP ECDSA sign message digest (MD) vector to sign*/
static const uint8_t cavp_ecdsa_sign_p256_md[P256_UNIT_LEN] = {
    0x44, 0xac, 0xf6, 0xb7, 0xe3, 0x6c, 0x13, 0x42, 0xc2, 0xc5, 0x89, 0x72, 
    0x4, 0xfe, 0x9, 0x50, 0x4e, 0x1e, 0x2e, 0xfb, 0x1a, 0x90, 0x3, 0x77, 
    0xdb, 0xc4, 0xe7, 0xa6, 0xa1, 0x33, 0xec, 0x56 
};

/* The NIST CAVP ECDSA sign secret key vector */
static const uint8_t cavp_ecdsa_sign_p256_sk[P256_UNIT_LEN] = {
    0x51, 0x9b, 0x42, 0x3d, 0x71, 0x5f, 0x8b, 0x58, 0x1f, 0x4f, 0xa8, 0xee, 
    0x59, 0xf4, 0x77, 0x1a, 0x5b, 0x44, 0xc8, 0x13, 0xb, 0x4e, 0x3e, 0xac, 
    0xca, 0x54, 0xa5, 0x6d, 0xda, 0x72, 0xb4, 0x64
};

/* The NIST CAVP ECDSA sign random secret nonce k vector */
const uint8_t cavp_ecdsa_sign_p256_k[P256_UNIT_LEN] = {
    0x94, 0xa1, 0xbb, 0xb1, 0x4b, 0x90, 0x6a, 0x61, 0xa2, 0x80, 0xf2, 0x45, 
    0xf9, 0xe9, 0x3c, 0x7f, 0x3b, 0x4a, 0x62, 0x47, 0x82, 0x4f, 0x5d, 0x33, 
    0xb9, 0x67, 0x7, 0x87, 0x64, 0x2a, 0x68, 0xde,
};

/* The NIST CAVP ML-DSA verify test #174 signature R vector */
static const uint8_t cavp_ecdsa_sign_p256_sig_r[P256_UNIT_LEN] =  {
   0xf3, 0xac, 0x80, 0x61, 0xb5, 0x14, 0x79, 0x5b, 0x88, 0x43, 0xe3, 0xd6, 
    0x62, 0x95, 0x27, 0xed, 0x2a, 0xfd, 0x6b, 0x1f, 0x6a, 0x55, 0x5a, 0x7a, 
    0xca, 0xbb, 0x5e, 0x6f, 0x79, 0xc8, 0xc2, 0xac
};

/* The NIST CAVP ML-DSA verify test #174 signature S vector */
static const uint8_t cavp_ecdsa_sign_p256_sig_s[P256_UNIT_LEN] = {
    0x8b, 0xf7, 0x78, 0x19, 0xca, 0x5, 0xa6, 0xb2, 0x78, 0x6c, 0x76, 0x26, 
    0x2b, 0xf7, 0x37, 0x1c, 0xef, 0x97, 0xb2, 0x18, 0xe9, 0x6f, 0x17, 0x5a, 
    0x3c, 0xcd, 0xda, 0x2a, 0xcc, 0x5, 0x89, 0x3
};

/* SRAM buffers for storing computed signature */
static uint8_t sig_r[P256_UNIT_LEN];
static uint8_t sig_s[P256_UNIT_LEN];

int main(void)
{
    crypto_ec_stat_t stat;
    uint32_t i;

    SYSCFG_DL_init();
 
    /* Run the sign operation */
    stat = Crypto_ECDSA_P256_sign(
        cavp_ecdsa_sign_p256_sk,
        cavp_ecdsa_sign_p256_md,
        cavp_ecdsa_sign_p256_k,
        sig_r,
        sig_s
        );

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_EC_STAT_ECDSA_SIGN_OK) {
        goto err;
    }

    /* Check generated signature against expected signature */
    for (i=0; i<P256_UNIT_LEN; i++) {
        if ((sig_r[i] != cavp_ecdsa_sign_p256_sig_r[i]) ||\
            (sig_s[i] != cavp_ecdsa_sign_p256_sig_s[i])) {
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
