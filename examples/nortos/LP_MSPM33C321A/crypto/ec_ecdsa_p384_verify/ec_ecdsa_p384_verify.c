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
 *  @file       ec_ecdsa_p384_verify.c
 *  @brief      Elliptic curve module ECDSA P-384 verify basic example
 * 
 *  This code example demonstrates how to use the ECDSA verify function
 *  over the NIST curve P-384 to verify a signature.
 *
 *  This example focuses on the ECDSA operation, and excludes any pre hashing.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* ECDSA P-384 operates on 48-byte parameters */
#define P384_UNIT_LEN (48U)

/* The NIST CAVP ECDSA sign message digest (MD) vector to sign*/
static const uint8_t cavp_ecdsa_verify_p384_md[P384_UNIT_LEN] = {
    0x31, 0xa4, 0x52, 0xd6, 0x16, 0x4d, 0x90, 0x4b, 0xb5, 0x72, 0x4c, 0x87, 
    0x82, 0x80, 0x23, 0x1e, 0xae, 0x70, 0x5c, 0x29, 0xce, 0x9d, 0x4b, 0xc7, 
    0xd5, 0x8e, 0x2, 0xe, 0x10, 0x85, 0xf1, 0x7e, 0xeb, 0xcc, 0x1a, 0x38, 
    0xf0, 0xed, 0xb, 0xf2, 0xb3, 0x44, 0xd8, 0x1f, 0xbd, 0x89, 0x68, 0x25
};

/* The NIST CAVP ECDSA sign secret key vector */
static const uint8_t cavp_ecdsa_verify_p384_pk_x[P384_UNIT_LEN] = {
    0xc2, 0xb4, 0x79, 0x44, 0xfb, 0x5d, 0xe3, 0x42, 0xd0, 0x32, 0x85, 0x88, 
    0x1, 0x77, 0xca, 0x5f, 0x7d, 0xf, 0x2f, 0xca, 0xd7, 0x67, 0x8c, 0xce, 
    0x42, 0x29, 0xd6, 0xe1, 0x93, 0x2f, 0xca, 0xc1, 0x1b, 0xfc, 0x3c, 0x3e, 
    0x97, 0xd9, 0x42, 0xa3, 0xc5, 0x6b, 0xf3, 0x41, 0x23, 0x1, 0x3d, 0xbf
};

/* The NIST CAVP ECDSA sign random secret nonce k vector */
const uint8_t cavp_ecdsa_verify_p384_pk_y[P384_UNIT_LEN] = {
    0x37, 0x25, 0x79, 0x6, 0xa8, 0x22, 0x38, 0x66, 0xed, 0xa0, 0x74, 0x3c, 
    0x51, 0x96, 0x16, 0xa7, 0x6a, 0x75, 0x8a, 0xe5, 0x8a, 0xee, 0x81, 0xc5, 
    0xfd, 0x35, 0xfb, 0xf3, 0xa8, 0x55, 0xb7, 0x75, 0x4a, 0x36, 0xd4, 0xa0, 
    0x67, 0x2d, 0xf9, 0x5d, 0x6c, 0x44, 0xa8, 0x1c, 0xf7, 0x62, 0xc, 0x2d
};

/* The NIST CAVP ML-DSA verify test #174 signature R vector */
static const uint8_t cavp_ecdsa_verify_p384_sig_r[P384_UNIT_LEN] =  {
    0x50, 0x83, 0x5a, 0x92, 0x51, 0xba, 0xd0, 0x8, 0x10, 0x61, 0x77, 0xef, 
    0x0, 0x4b, 0x9, 0x1a, 0x1e, 0x42, 0x35, 0xcd, 0xd, 0xa8, 0x4f, 0xff, 
    0x54, 0x54, 0x2b, 0xe, 0xd7, 0x55, 0xc1, 0xd6, 0xf2, 0x51, 0x60, 0x9d, 
    0x14, 0xec, 0xf1, 0x8f, 0x9e, 0x1d, 0xdf, 0xe6, 0x9b, 0x94, 0x6e, 0x32
};

/* The NIST CAVP ML-DSA verify test #174 signature S vector */
static const uint8_t cavp_ecdsa_verify_p384_sig_s[P384_UNIT_LEN] = {
    0x4, 0x75, 0xf3, 0xd3, 0xc, 0x64, 0x63, 0xb6, 0x46, 0xe8, 0xd3, 0xbf, 
    0x24, 0x55, 0x83, 0x3, 0x14, 0x61, 0x1c, 0xbd, 0xe4, 0x4, 0xbe, 0x51, 
    0x8b, 0x14, 0x46, 0x4f, 0xdb, 0x19, 0x5f, 0xdc, 0xc9, 0x2e, 0xb2, 0x22, 
    0xe6, 0x1f, 0x42, 0x6a, 0x4a, 0x59, 0x2c, 0x0, 0xa6, 0xa8, 0x97, 0x21
};

int main(void)
{
    crypto_ec_stat_t stat;
    uint32_t i;

    SYSCFG_DL_init();
 
    /* Run the sign operation */
    stat = Crypto_ECDSA_P384_verify(
        cavp_ecdsa_verify_p384_pk_x,
        cavp_ecdsa_verify_p384_pk_y,
        cavp_ecdsa_verify_p384_md,
        cavp_ecdsa_verify_p384_sig_r,
        cavp_ecdsa_verify_p384_sig_s
        );

    /* If the operation was not successful, jump to error trap */
    if (stat != CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE) {
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
