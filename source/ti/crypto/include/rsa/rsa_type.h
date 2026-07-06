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
 *  @file       rsa_type.h
 *  @brief      Type defintions used for RSA operations.
 * 
 *  This file defines the RSA types.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_RSA_TYPE_H_
#define CRYPTO_RSA_TYPE_H_

/** @brief Key size for RSA: default to 2048-bit key length */
#define CRYPTO_RSA_KEY_SIZE_BITS 2048 

/** @brief RSA big number size in bytes */
#define CRYPTO_RSA_NUMBYTES ((CRYPTO_RSA_KEY_SIZE_BITS) / 8)

/** @brief RSA big number size in 32-bit words */
#define CRYPTO_RSA_NUMWORDS (CRYPTO_RSA_NUMBYTES / sizeof(uint32_t))

/**
 *  @brief      The RSA module status response value set.
 */
typedef enum
{
    CRYPTO_RSA_STAT_OK = 0x00,
    CRYPTO_RSA_STAT_ERR_PARAM = 0x10,
    CRYPTO_RSA_STAT_ERR_OVERFLOW = 0x11,
    CRYPTO_RSA_STAT_ERR_BNMATH = 0x12,
    CRYPTO_RSA_STAT_ERR_UNSUPPORTED_MD_ALG = 0x13,
    CRYPTO_RSA_STAT_ERR_BAD_EB = 0x14,
    CRYPTO_RSA_STAT_ERR_BAD_TAG = 0x15,
    CRYPTO_RSA_STAT_ERR_PSS_INCONSISTENT_EM = 0x16,
    CRYPTO_RSA_STAT_INVALID_SIGNATURE = 0x5A,
    CRYPTO_RSA_STAT_VALID_SIGNATURE = 0xA5,
} crypto_rsa_stat_t;

/**
 *  @brief      Selection for whether an input is stored in little
 *              endian or big endian format.
 */
typedef enum
{
    CRYPTO_RSA_LITTLE_ENDIAN = 0x00,
    CRYPTO_RSA_BIG_ENDIAN = 0x01
} crypto_rsa_endianness_t;

/**
 *  @brief      The RSA module digital signature hash code options.
 */
typedef enum
{
    CRYPTO_RSA_MD_SHA256 = 0x01,
} crypto_rsa_md_t;

/**
 * RSA public key data structure for fast verification routines
 */
typedef struct {
    /** Public key modulus as little endian array */
	uint32_t n[CRYPTO_RSA_NUMWORDS];
    /** Pre-computed R^2 value stored as a little endian array */
	uint32_t rr[CRYPTO_RSA_NUMWORDS];
    /** Pre-computed -1 / n[0] mod 2^32 value stored as an integer */
	uint32_t n0inv;
} crypto_rsa_fast_verify_pk_t;

#endif /* CRYPTO_RSA_TYPE_H_ */
