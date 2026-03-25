/*
 * Copyright (C) 2025 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 * Limited License.  
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell
 * ("Utilize") this software subject to the terms herein.  With respect to the
 * foregoing patent license, such license is granted  solely to the extent that
 * any such patent is necessary to Utilize the software alone.  The patent
 * license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI
 * (TI Devices).  No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this
 * license (including the above copyright notice and the disclaimer and (if
 * applicable) source code license limitations below) in the documentation
 * and/or other materials provided with the distribution.
 *
 * Redistribution and use in binary form, without modification, are permitted
 * provided that the following conditions are met:
 *
 *	* No reverse engineering, decompilation, or disassembly of this software is
 *     permitted with respect to any software provided in binary form.
 *	* Any redistribution and use are licensed by TI for use only with
 *     TI Devices.
 *	* Nothing shall obligate TI to provide you with source code for the software
 *     licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution
 * of the source code are permitted provided that the following conditions are
 * met:
 *
 *   * any redistribution and use of the source code, including any resulting
 *     derivative works, are licensed by TI for use only with TI Devices.
 *   * any redistribution and use of any object code compiled from the source
 *     code and any resulting derivative works, are licensed by TI for use only
 *     with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its
 * suppliers may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       ec.h
 *  @brief      Elliptic curve implementations for ECDSA, ECDH
 * 
 *  This cipher module implements elliptic curve cryptography for enabling
 *  ECDSA key generation, signing, and verification and ECDH key agreement
 *  based on cryptographic elliptic curves defined over a prime field Fp.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_EC_H_
#define CRYPTO_EC_H_

#include <stddef.h>
#include <stdint.h>
#include "ti/crypto/include/ec/ec_curves.h"

/* External data type definitions */

/**
 *  @brief      The EC module status response value set.
 */
typedef enum
{
    CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS = 0x28,
    CRYPTO_EC_STAT_ORDER_EVEN = 0x82,
    CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO = 0x23,
    CRYPTO_EC_STAT_ECC_KEY_TOO_LONG = 0x25,
    CRYPTO_EC_STAT_ECC_KEY_LENGTH_ZERO = 0x52,
    CRYPTO_EC_STAT_ECC_X_LARGER_THAN_PRIME = 0x11,
    CRYPTO_EC_STAT_ECC_Y_LARGER_THAN_PRIME = 0x12,
    CRYPTO_EC_STAT_ECC_X_ZERO = 0x13,
    CRYPTO_EC_STAT_ECC_Y_ZERO = 0x14,
    CRYPTO_EC_STAT_ECC_POINT_NOT_ON_CURVE = 0x15,
    CRYPTO_EC_STAT_ECC_POINT_ON_CURVE = 0x16,
    CRYPTO_EC_STAT_PRIVATE_KEY_ZERO = 0x17,
    CRYPTO_EC_STAT_PRIVATE_KEY_LARGER_EQUAL_ORDER = 0x18,
    CRYPTO_EC_STAT_PRIVATE_VALID = 0x19,
    CRYPTO_EC_STAT_DIGEST_TOO_LONG = 0x27,
    CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO = 0x72,
    CRYPTO_EC_STAT_ECDSA_SIGN_OK = 0x32,
    CRYPTO_EC_STAT_ECDSA_SIGN_PKA_ERROR = 0x34,
    CRYPTO_EC_STAT_ECDSA_INVALID_SIGNATURE = 0x5A,
    CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE = 0xA5,
    CRYPTO_EC_STAT_SIG_P1_TOO_LONG = 0x11,
    CRYPTO_EC_STAT_SIG_P1_LENGTH_ZERO = 0x12,
    CRYPTO_EC_STAT_SIG_P2_TOO_LONG = 0x22,
    CRYPTO_EC_STAT_SIG_P2_LENGTH_ZERO = 0x21,
    CRYPTO_EC_STAT_MODULUS_EVEN = 0xDC,
    CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS = 0xD2,
    CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO = 0x08,
    CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO = 0x30,
    CRYPTO_EC_STAT_SCALAR_TOO_LONG = 0x35,
    CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO = 0x53,
    CRYPTO_EC_STAT_ORDER_TOO_LONG = 0xC6,
    CRYPTO_EC_STAT_ORDER_LENGTH_ZERO = 0x6C,
    CRYPTO_EC_STAT_X_COORD_TOO_LONG = 0x3C,
    CRYPTO_EC_STAT_X_COORD_LENGTH_ZERO = 0xC3,
    CRYPTO_EC_STAT_Y_COORD_TOO_LONG = 0x65,
    CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO = 0x56,
    CRYPTO_EC_STAT_A_COEF_TOO_LONG = 0x5C,
    CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO = 0xC5,
    CRYPTO_EC_STAT_BAD_WINDOW_SIZE = 0x66,
    CRYPTO_EC_STAT_FAULT_DETECTION = 0x33,
    CRYPTO_EC_STAT_SCALAR_MUL_OK = 0x99,
    CRYPTO_EC_STAT_KEYGEN_OK = 0x99,
    CRYPTO_EC_STAT_SHARED_KEY_OK = 0x99
} crypto_ec_stat_t;

#ifdef CRYPTO_EC_WORKZONE_INSTRUMENTATION
/* Stores the number of 32-bit slots in the workzone used 
 * by the last EC operation.  Only used when
 * CRYPTO_EC_WORKZONE_INSTRUMENTATION is defined.
 */
extern uint32_t Crypto_ecWorkzoneUse;
#endif

/* External function declarations */

/**
 *  @brief      Generate a private/public key pair for the specified curve
 *              and the input random seed value.
 *
 *  @param[in]  curve          The elliptic curve to use for key pair
 *                             generation.
 *  @param[in]  rand           Pointer to the cryptographically random word
 *                             string to use in key pair generation.
 *  @param[out] sk             Pointer to the buffer to store the secret
 *                             (private) key output into.
 *  @param[out] pk_x           Pointer to the buffer to store the public 
 *                             key X-coordinate value into.
 *  @param[out] pk_y           Pointer to the buffer to store the public 
 *                             key Y-coordinate value into.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_KEYGEN_OK if the key pair generation was
 *              succesful.  All other return values indicate an error.
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO     
 */
extern crypto_ec_stat_t Crypto_EC_keyGen(
    const crypto_ec_curve_t *curve,
    const uint8_t *rand,
    uint8_t *sk,
    uint8_t *pk_x, uint8_t *pk_y);

/**
 *  @brief      Compute the ECDSA digital signature for a message digest
 *              using a ECDSA private key and a secret random signing nonce.
 *
 *  @param[in]  curve          The elliptic curve to use for the operation.
 *  @param[in]  sk             Pointer to the buffer containing the private
 *                             (secret) key to use for signing.
 *  @param[in]  md             Pointer to the message digest to sign.
 *  @param[in]  k              Pointer to the secret, random, nonce value used
 *                             for this digital signature.
 *  @param[out] sig_r          Pointer for the memory location to store the
 *                             resulting digital signature r-value.
 *                             This is x-coordinate(k.G).
 *  @param[out] sig_s          Pointer for the memory location to store the
 *                             resulting digital signature s-value.
 *                             This is (h + x.s1) k^(-1) mod r.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_SIGN_OK if the signature was generated
 *              successfully.  All other return vaules indicate an error.
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_sign(
    const crypto_ec_curve_t *curve, 
    const uint8_t *sk,
    const uint8_t *md,
    const uint8_t *k,
    uint8_t *sig_r, uint8_t *sig_s);

/**
 *  @brief      Compute the ECDSA  digital signature for a message digest  
 *              using a P-256 private key and a secret random signing nonce.
 *              This API is for the NIST curve P-256 only with higher
 *              SRAM stack optimization than the generic Crypto_ECDSA_sign API.
 *
 *  @param[in]  sk             Pointer to the buffer containing the private
 *                             (secret) key to use for signing.
 *  @param[in]  md             Pointer to the message digest to sign.
 *  @param[in]  k              Pointer to the secret, random, nonce value used
 *                             for this digital signature.
 *  @param[out] sig_r          Pointer for the memory location to store the
 *                             resulting digital signature r-value.
 *                             This is x-coordinate(k.G).
 *  @param[out] sig_s          Pointer for the memory location to store the
 *                             resulting digital signature s-value.
 *                             This is (h + x.s1) k^(-1) mod r.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_SIGN_OK if the signature was generated
 *              successfully.  All other return vaules indicate an error.
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_P256_sign(
    const uint8_t *sk,
    const uint8_t *md,
    const uint8_t *k,
    uint8_t *sig_r, uint8_t *sig_s);

/**
 *  @brief      Compute the ECDSA  digital signature for a message digest  
 *              using a P-384 private key and a secret random signing nonce.
 *              This API is for the NIST curve P-384 only with higher
 *              SRAM stack optimization than the generic Crypto_ECDSA_sign API.
 *
 *  @param[in]  sk             Pointer to the buffer containing the private
 *                             (secret) key to use for signing.
 *  @param[in]  md             Pointer to the message digest to sign.
 *  @param[in]  k              Pointer to the secret, random, nonce value used
 *                             for this digital signature.
 *  @param[out] sig_r          Pointer for the memory location to store the
 *                             resulting digital signature r-value.
 *                             This is x-coordinate(k.G).
 *  @param[out] sig_s          Pointer for the memory location to store the
 *                             resulting digital signature s-value.
 *                             This is (h + x.s1) k^(-1) mod r.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_SIGN_OK if the signature was generated
 *              successfully.  All other return vaules indicate an error.
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_P384_sign(
    const uint8_t *sk,
    const uint8_t *md,
    const uint8_t *k,
    uint8_t *sig_r, uint8_t *sig_s);

/**
 *  @brief      Compute the ECDSA  digital signature for a message digest  
 *              using a P-521 private key and a secret random signing nonce.
 *              This API is for the NIST curve P-521 only with higher
 *              SRAM stack optimization than the generic Crypto_ECDSA_sign API.
 *
 *  @param[in]  sk             Pointer to the buffer containing the private
 *                             (secret) key to use for signing.
 *  @param[in]  md             Pointer to the message digest to sign.
 *  @param[in]  k              Pointer to the secret, random, nonce value used
 *                             for this digital signature.
 *  @param[out] sig_r          Pointer for the memory location to store the
 *                             resulting digital signature r-value.
 *                             This is x-coordinate(k.G).
 *  @param[out] sig_s          Pointer for the memory location to store the
 *                             resulting digital signature s-value.
 *                             This is (h + x.s1) k^(-1) mod r.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_SIGN_OK if the signature was generated
 *              successfully.  All other return vaules indicate an error.
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_ECC_KEY_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_P521_sign(
    const uint8_t *sk,
    const uint8_t *md,
    const uint8_t *k,
    uint8_t *sig_r, uint8_t *sig_s);

/**
 *  @brief      Verify the ECDSA digital signature for a message digest using 
 *              an ECDSA public key on the specified curve.
 *
 *  @param[in]  curve          The elliptic curve to use for the operation.
 *  @param[in]  pk_x           Pointer to the buffer containing the public 
 *                             key X-coordinate.
 *  @param[in]  pk_y           Pointer to the buffer containing the public 
 *                             key Y-coordinate.
 *  @param[in]  md             Pointer to the message digest.
 *  @param[in]  sig_r          Pointer for the memory location containing the
 *                             digital signature r-value.
 *  @param[in]  sig_s          Pointer for the memory location containing the
 *                             digital signature s-value.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE if the signature was valid for the
 *              message digest and public key.
 *  @retval     CRYPTO_EC_STAT_ECDSA_INVALID_SIGNATURE if the operation completed
 *              (valid parameters) but the signature was not valid for the
 *              message digest and public key.
 *  @retval     CRYPTO_EC_STAT_SIG_P1_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P1_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_SIG_P2_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P2_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_verify(
    const crypto_ec_curve_t *curve,
    const uint8_t *pk_x, const uint8_t *pk_y,
    const uint8_t *md,
    const uint8_t *sig_r, const uint8_t *sig_s);

/**
 *  @brief      Verify the ECDSA  digital signature for a message digest using 
 *              an ECDSA public key on NIST curve P-256 only with higher
 *              SRAM stack optimization than the generic Crypto_ECDSA_verify.
 *
 *  @param[in]  pk_x           Pointer to the buffer containing the public 
 *                             key X-coordinate.
 *  @param[in]  pk_y           Pointer to the buffer containing the public 
 *                             key Y-coordinate.
 *  @param[in]  md             Pointer to the message digest.
 *  @param[in]  sig_r          Pointer for the memory location containing the
 *                             digital signature r-value.
 *  @param[in]  sig_s          Pointer for the memory location containing the
 *                             digital signature s-value.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_EC_STAT_ECDSA_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_EC_STAT_SIG_P1_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P1_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_SIG_P2_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P2_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_P256_verify(
    const uint8_t *pk_x, const uint8_t *pk_y,
    const uint8_t *md,
    const uint8_t *sig_r, const uint8_t *sig_s);

/**
 *  @brief      Verify the ECDSA  digital signature for a message digest using 
 *              an ECDSA public key on NIST curve P-384 only with higher
 *              SRAM stack optimization than the generic Crypto_ECDSA_verify.
 *
 *  @param[in]  pk_x           Pointer to the buffer containing the public 
 *                             key X-coordinate.
 *  @param[in]  pk_y           Pointer to the buffer containing the public 
 *                             key Y-coordinate.
 *  @param[in]  md             Pointer to the message digest.
 *  @param[in]  sig_r          Pointer for the memory location containing the
 *                             digital signature r-value.
 *  @param[in]  sig_s          Pointer for the memory location containing the
 *                             digital signature s-value.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_EC_STAT_ECDSA_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_EC_STAT_SIG_P1_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P1_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_SIG_P2_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P2_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_P384_verify(
    const uint8_t *pk_x, const uint8_t *pk_y,
    const uint8_t *md,
    const uint8_t *sig_r, const uint8_t *sig_s);

/**
 *  @brief      Verify the ECDSA  digital signature for a message digest using 
 *              an ECDSA public key on NIST curve P-521 only with higher
 *              SRAM stack optimization than the generic Crypto_ECDSA_verify.
 *
 *  @param[in]  pk_x           Pointer to the buffer containing the public 
 *                             key X-coordinate.
 *  @param[in]  pk_y           Pointer to the buffer containing the public 
 *                             key Y-coordinate.
 *  @param[in]  md             Pointer to the message digest.
 *  @param[in]  sig_r          Pointer for the memory location containing the
 *                             digital signature r-value.
 *  @param[in]  sig_s          Pointer for the memory location containing the
 *                             digital signature s-value.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_EC_STAT_ECDSA_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_EC_STAT_SIG_P1_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P1_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_DIGEST_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_DIGEST_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_SIG_P2_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SIG_P2_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_EVEN
 *  @retval     CRYPTO_EC_STAT_ORDER_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDSA_P521_verify(
    const uint8_t *pk_x, const uint8_t *pk_y,
    const uint8_t *md,
    const uint8_t *sig_r, const uint8_t *sig_s);

/**
 *  @brief      Get a shared secret point value using elliptic curve
 *              Diffie-Hellman (ECDH) on the specified curve together
 *              with a private value (first party) and 
 *              public X/Y coordinate(second party).
 *
 *  @param[in]  curve          The elliptic curve to use for the operation.
 *  @param[in]  sk             Pointer to the buffer containing the private
 *                             (secret) key for the first party.
 *  @param[in]  pk_x           Pointer to the buffer containing the public 
 *                             key X-coordinate for the second party.
 *  @param[in]  pk_y           Pointer to the buffer containing the public 
 *                             key Y-coordinate for the second party.
 *  @param[out]  out_x         Pointer to memory location to store the shared
 *                             secret X-coordinate result.
 *  @param[out]  out_y         Pointer to memory location to store the shared
 *                             secret Y-coordinate result.
 *
 *  @return     The resulting status of the operation, one of crypto_ec_stat_t.
 *  @retval     CRYPTO_EC_STAT_SHARED_KEY_OK if the shared secret points
 *              were correctly computed.  All other return values indicate
 *              that an error occured during computation.
 *  @retval     CRYPTO_EC_STAT_MODULUS_EVEN
 *  @retval     CRYPTO_EC_STAT_MODULUS_MSW_IS_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_MODULUS_LARGER_THAN_255_WORDS
 *  @retval     CRYPTO_EC_STAT_SCALAR_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_SCALAR_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_ORDER_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_BAD_WINDOW_SIZE if window size is not [2-5]
 *  @retval     CRYPTO_EC_STAT_Y_COORD_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_Y_COORD_LENGTH_ZERO
 *  @retval     CRYPTO_EC_STAT_A_COEF_TOO_LONG
 *  @retval     CRYPTO_EC_STAT_A_COEF_LENGTH_ZERO
 */
extern crypto_ec_stat_t Crypto_ECDH_getSharedSecret(
    const crypto_ec_curve_t *curve,
    uint32_t *sk,
    uint32_t *pk_x, uint32_t *pk_y,
    uint32_t *out_x, uint32_t *out_y);

#endif /* CRYPTO_EC_H_ */
