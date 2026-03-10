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
 *  @file       rsa.h
 *  @brief      Compact implementations of RSA signature verification
 * 
 *  This cipher module implements signature verification for RSA type
 *  digital signatures.  Currently, the following parameters are supported:
 * 
 *  Public exponents: 65537 (performance optimal and most common use case)
 *  Modulus size: 2048-bit (256-byte)
 *  Padding schemes: EMSA-PKCS1-v1_5, EMSA-PSS-VERIFY
 *  Hash functions: SHA-256 (for message digests and use inside the MGF)
 *
 *  There are two ways to use the module:
 *    (1) Slower, but single call: use Crypto_RSA_PKCS1P5_verify() or
 *        Crypto_RSA_PSS_verify() and pass the modulus directly.  This is
 *        a 1-step process but uses more CPU cycles because the public key
 *        parameters for verification need to be computed from the modulus.
 *    (2) Faster, but 2 calls: use Crypto_RSA_preparePkFromModulus() first
 *        to prepare a crypto_rsa_fast_verify_pk_t for each public key to
 *        be used.  Then, use the Crypto_RSA_PKCS1P5_fastVerify() or the
 *        Crypto_RSA_PSS_fastVerify() function to perform RSA verification
 *        on the public key after the public key parameters are pre-computed
 *        in advance.  This is faster at runtime.  It is possible to call
 *        Crypto_RSA_preparePkFromModulus() during application development / 
 *        key installation, and store the crypto_rsa_fast_verify_pk_t data
 *        into flash.  Then, only the fast verify functions need to be used
 *        at run time.
 *
 *  NOTE: These routines expect the application to compute the message digest
 *        of the message being verified using SHA-256.  A pointer to the
 *        SHA-256 message digest is then passed in to the RSA verification
 *        routines for verification.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_RSA_H_
#define CRYPTO_RSA_H_

#include <stdint.h>
#include "ti/crypto/include/rsa/rsa_type.h"

/**
 *  @brief      Populate the fields in a crypto_rsa_fast_verify_pk_t
 *              data structure based on the modulus of a public key.
 *              The library assumes the use of the default RSA exponent
 *              of 65537.  Once a crypto_rsa_fast_verify_pk_t structure
 *              for a public key is populated, it may be used by the
 *              fast verify functions to quickly authenticate signatures.
 *
 *  @param[in]  modulus        Pointer to the buffer containing the public 
 *                             key modulus in little-endian byte order.
 *  @param[in]  modulusLen     Byte length of the modulus.
 *  @param[in]  endianness     The endianness of the modulus input (big/little)
 *  @param[out] pk             Pointer to the public key data structure to
 *                             populate.
 *
 *  @return     The resulting status of the operation, one of crypto_rsa_stat_t.
 *  @retval     CRYPTO_RSA_STAT_OK if the function was successful.
 *  @retval     CRYPTO_RSA_STAT_ERR_PARAM if there was a parameter error.
 */
extern crypto_rsa_stat_t Crypto_RSA_preparePkFromModulus(
    const uint8_t *modulus, uint16_t modulusLen, 
    crypto_rsa_endianness_t endianness,
    crypto_rsa_fast_verify_pk_t *pk);

/**
 *  @brief      Verify a RSA RSASSA-PKCS1-v1.5 digital signature 
 *              for a message digest which has also been pre-computed,
 *              using an RSA public key which had its parameters pre-computed.
 *
 *              NOTE: This implementation assumes a public exponent of 65537.
 *
 *  @param[in]  pk             Pointer to the public key structure.  All fields
 *                             in the structure must have been pre-computed.
 *  @param[in]  md             Pointer to the pre-computed message digest.
 *  @param[in]  mdAlg          The message digest secure hash algorithm.
 *  @param[in]  sig            Pointer for the memory location containing the
 *                             signature.
 *  @param[in]  sigLen         The length of the signature (in bytes).
 *
 *  @return     The resulting status of the operation, one of crypto_rsa_stat_t.
 *  @retval     CRYPTO_RSA_STAT_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_ERR_PARAM if there was a parameter error.
 *  @retval     CRYPTO_RSA_STAT_ERR_BAD_EB if the EB was malformed.
 */
extern crypto_rsa_stat_t Crypto_RSA_PKCS1P5_fastVerify(
    const crypto_rsa_fast_verify_pk_t *pk, 
    const uint8_t *md, crypto_rsa_md_t mdAlg,
    const uint8_t *sig, uint32_t sigLen);

/**
 *  @brief      Verify a RSA RSASSA-PKCS1-v1.5 digital signature 
 *              for a message digest which has also been pre-computed,
 *              using an RSA public key modulus which is provided directly
 *              in big-endian or little-endian format.
 *
 *              NOTE: This is the slower of the two verify methods, as it 
 *                    computes the additionial parameters for the public key
 *                    every time it is called.  Optionally, these values
 *                    may be pre-computed at compile time for a given public
 *                    key using Crypto_RSA_preparePkFromModulus() and stored
 *                    in flash, and then Crypto_RSA_PKCS1P5_fastVerify() may
 *                    be called at runtime to avoid having to re-compute the
 *                    parameters.
 *
 *              NOTE: This implementation assumes a public exponent of 65537.
 *
 *  @param[in]  pkMod          Pointer to the public key modulus.  All fields
 *                             in the structure must have been pre-computed.
 *  @param[in]  pkModLen       The length of the modulus in bytes.
 *  @param[in]  endianness     The endianness of the modulus input (big/little)
 *  @param[in]  md             Pointer to the pre-computed message digest.
 *  @param[in]  mdAlg          The message digest secure hash algorithm.
 *  @param[in]  sig            Pointer for the memory location containing the
 *                             signature.
 *  @param[in]  sigLen         The length of the signature (in bytes).
 *
 *  @return     The resulting status of the operation, one of crypto_rsa_stat_t.
 *  @retval     CRYPTO_RSA_STAT_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_ERR_PARAM if there was a parameter error.
 *  @retval     CRYPTO_RSA_STAT_ERR_BAD_EB if the EB was malformed.
 */
extern crypto_rsa_stat_t Crypto_RSA_PKCS1P5_verify(
    const uint8_t *pkMod, uint16_t pkModLen, crypto_rsa_endianness_t endianness,
    const uint8_t *md, crypto_rsa_md_t mdAlg,
    const uint8_t *sig, uint32_t sigLen);

/**
 *  @brief      Verify a RSA RSASSA-PSS digital signature for a message digest 
 *              which has also been pre-computed, using an RSA public key
 *              which had its parameters pre-computed.
 *
 *              NOTE: This implementation assumes a public exponent of 65537.
 *
 *  @param[in]  pk             Pointer to the public key structure.  All fields
 *                             in the structure must have been pre-computed.
 *  @param[in]  md             Pointer to the pre-computed message digest.
 *  @param[in]  mdAlg          The message digest secure hash algorithm.
 *  @param[in]  sig            Pointer for the memory location containing the
 *                             signature.
 *  @param[in]  sigLen         The length of the signature (in bytes).
 *  @param[in]  saltLen        The length of the signature salt (in bytes).
 *
 *  @return     The resulting status of the operation, one of crypto_rsa_stat_t.
 *  @retval     CRYPTO_RSA_STAT_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_ERR_PARAM if there was a parameter error.
 *  @retval     CRYPTO_RSA_STAT_ERR_PSS_INCONSISTENT_EM if the EB was malformed.
 */
extern crypto_rsa_stat_t Crypto_RSA_PSS_fastVerify(
    const crypto_rsa_fast_verify_pk_t *pk, 
    const uint8_t *md, crypto_rsa_md_t mdAlg,
    const uint8_t *sig, uint32_t sigLen,
    uint32_t saltLen);

/**
 *  @brief      Verify a RSA RSASSA-PSS digital signature for a message digest 
 *              which has also been pre-computed, using an RSA public key
 *              modulus provided in big-endian or little-endian format.
 *              
 *              NOTE: This is the slower of the two verify methods, as it 
 *                    computes the additionial parameters for the public key
 *                    every time it is called.  Optionally, these values
 *                    may be pre-computed at compile time for a given public
 *                    key using Crypto_RSA_preparePkFromModulus() and stored
 *                    in flash, and then Crypto_RSA_PSS_fastVerify() may be
 *                    called at runtime to avoid having to re-compute the
 *                    parameters.
 *              
 *              NOTE: This implementation assumes a public exponent of 65537.
 *
 *  @param[in]  pkMod          Pointer to the public key modulus.  All fields
 *                             in the structure must have been pre-computed.
 *  @param[in]  pkModLen       The length of the modulus in bytes.
 *  @param[in]  endianness     The endianness of the modulus input (big/little)
 *  @param[in]  md             Pointer to the pre-computed message digest.
 *  @param[in]  mdAlg          The message digest secure hash algorithm.
 *  @param[in]  sig            Pointer for the memory location containing the
 *                             signature.
 *  @param[in]  sigLen         The length of the signature (in bytes).
 *  @param[in]  saltLen        The length of the signature salt (in bytes).
 *
 *  @return     The resulting status of the operation, one of crypto_rsa_stat_t.
 *  @retval     CRYPTO_RSA_STAT_VALID_SIGNATURE if the signature was valid
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_INVALID_SIGNATURE if the operation 
 *              completed (valid parameters) but the signature was not valid 
 *              for the message digest and public key.
 *  @retval     CRYPTO_RSA_STAT_ERR_PARAM if there was a parameter error.
 *  @retval     CRYPTO_RSA_STAT_ERR_PSS_INCONSISTENT_EM if the EB was malformed.
 */
extern crypto_rsa_stat_t Crypto_RSA_PSS_verify(
    const uint8_t *pkMod, uint16_t pkModLen, crypto_rsa_endianness_t endianness,
    const uint8_t *md, crypto_rsa_md_t mdAlg,
    const uint8_t *sig, uint32_t sigLen,
    uint32_t saltLen);

#endif /* CRYPTO_RSA_H_ */
