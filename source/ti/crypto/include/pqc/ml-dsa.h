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
 *  @file       ml-dsa.h
 *  @brief      Module lattice digital signature algorithm (ML-DSA)
 * 
 *  This module implements the FIPS 204 standard for post-quantum resistant
 *  digital signatures (Crystals: Dilithium).
 *
 ******************************************************************************
 */

#ifndef CRYPTO_ML_DSA_H_
#define CRYPTO_ML_DSA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "ti/crypto/include/pqc/ml-dsa-type.h"

/* External constant definitions */

/** @brief ML-DSA key generation random seed length in bytes */
#define CRYPTO_MLDSA_KEYGEN_SEED_LEN (32U)

/** @brief ML-DSA-44 secret (private) signing key length in bytes */
#define CRYPTO_MLDSA_44_SK_LEN (2560U)

/** @brief ML-DSA-44 public verification key length in bytes */
#define CRYPTO_MLDSA_44_PK_LEN (1312U)

/** @brief ML-DSA-44 signature length in bytes */
#define CRYPTO_MLDSA_44_SIG_LEN (2420U)

/** @brief ML-DSA-65 secret (private) signing key length in bytes */
#define CRYPTO_MLDSA_65_SK_LEN (4032U)

/** @brief ML-DSA-65 public verification key length in bytes */
#define CRYPTO_MLDSA_65_PK_LEN (1952U)

/** @brief ML-DSA-65 signature length in bytes */
#define CRYPTO_MLDSA_65_SIG_LEN (3309U)

/** @brief ML-DSA-87 secret (private) signing key length in bytes */
#define CRYPTO_MLDSA_87_SK_LEN (4896U)

/** @brief ML-DSA-87 public verification key length in bytes */
#define CRYPTO_MLDSA_87_PK_LEN (2592U)

/** @brief ML-DSA-87 signature length in bytes */
#define CRYPTO_MLDSA_87_SIG_LEN (4627U)

/* External function declarations */

/**
 *  @brief      Generate a ML-DSA-44 key pair.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the key pair.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[out] pk             Pointer to location to store the 
 *                             public verification key.
 *  @param[out] sk             Pointer to location to store the 
 *                             secret (private) signing key.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLDSA_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than 32 bytes.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_44_keyGen( \
    const uint8_t *seed, size_t seedLen, \
    uint8_t *pk, uint8_t *sk);

/**
 *  @brief      Generate a ML-DSA-44 digital signature for a message using 
 *              an ML-DSA secret (private) key.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[out] sig            Pointer for the memory location to store the
 *                             generated ML-DSA digital signature output.
 *  @param[out] sigLen         Pointer to variable to store the length of 
 *                             the signature in bytes.
 *  @param[in]  m              Pointer to the message corresponding to the
 *                             digital signature.
 *  @param[in]  mLen           Length of the signature in bytes.
 *  @param[in]  ctx            Pointer to the context string.
 *  @param[in]  ctxLen         Length of the context string in bytes.
 *  @param[in]  rand           Pointer to the a random string (for non-det
 *                             signing, else NULL (0).
 *  @param[in]  randLen        Length of the context string in bytes, or
 *                             a (0) value if deterministic signing is used.
 *  @param[in]  sk             Pointer to the secret (private) signing key.
 *  @param[in]  hashMode       Set to CRYPTO_MLDSA_SIG_MODE_PURE for pure 
 *                             ML-DSA, or CRYPTO_MLDSA_SIG_MODE_PREHASH for
 *                             pre-hash ML-DSA.  If using pre-hash, then 
 *                             parameter m shall be the string concatenation of
 *                             ( OID || PH(M) ) where OID is the DER encoded 
 *                             hash object identifier and PH(M) is the output 
 *                             of the hash function taken over the message 
 *                             before signing.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_OK if the signature was generated correctly.
 *  @retval     CRYPTO_MLDSA_STAT_CTX_TOO_LONG if the context was > 255 bytes.
 *  @retval     CRYPTO_MLDSA_STAT_RAND_TOO_SMALL if the randomized signing
 *              was selected but the randLen was < 32 bytes.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_44_sign( \
    uint8_t *sig, size_t *sigLen, \
    const uint8_t *m, size_t mLen, \
    const uint8_t *ctx, size_t ctxLen, \
    const uint8_t *rand, size_t randLen, \
    const uint8_t *sk, crypto_mldsa_mode_t hashMode);

/**
 *  @brief      Verify the ML-DSA-44 digital signature for a message using 
 *              an ML-DSA public key.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[in]  sig            Pointer for the memory location containing
 *                             the ML-DSA digital signature.
 *  @param[in]  sigLen         Length of the signature in bytes.
 *  @param[in]  m              Pointer to the message corresponding to the
 *                             digital signature.
 *  @param[in]  mLen           Length of the signature in bytes.
 *  @param[in]  ctx            Pointer to the context string.
 *  @param[in]  ctxLen         Length of the context string in bytes.
 *  @param[in]  pk             Pointer to the public key.
 *  @param[in]  hashMode       Set to CRYPTO_MLDSA_SIG_MODE_PURE for pure 
 *                             ML-DSA, or CRYPTO_MLDSA_SIG_MODE_PREHASH for
 *                             pre-hash ML-DSA.  If using pre-hash, then 
 *                             parameter m shall be the string concatenation of
 *                             ( OID || PH(M) ) where OID is the DER encoded 
 *                             hash object identifier and PH(M) is the output 
 *                             of the hash function taken over the message 
 *                             before signing.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_VALID_SIGNATURE for a successful verification.
 *  @retval     CRYPTO_MLDSA_STAT_INVALID_SIGNATURE for a failed verification.
 *  @retval     CRYPTO_MLDSA_STAT_CTX_TOO_LONG if the context was > 255 bytes.
 *  @retval     CRYPTO_MLDSA_STAT_SIG_LEN_INVALID if the signature length is
 *              not valid.
 *  @retval     CRYPTO_MLDSA_STAT_INVALID_SIGNATURE_FORM if the signature 
 *              values are not valid for verification.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_44_verify( \
    const uint8_t *sig, size_t sigLen, \
    const uint8_t *m, size_t mLen, \
    const uint8_t *ctx, size_t ctxLen, \
    const uint8_t *pk, crypto_mldsa_mode_t hashMode);

/**
 *  @brief      Generate a ML-DSA-65 key pair.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the key pair.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[out] pk             Pointer to location to store the 
 *                             public verification key.
 *  @param[out] sk             Pointer to location to store the 
 *                             secret (private) signing key.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLDSA_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than 32 bytes.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_65_keyGen( \
    const uint8_t *seed, size_t seedLen, \
    uint8_t *pk, uint8_t *sk);

/**
 *  @brief      Generate a ML-DSA-65 digital signature for a message using 
 *              an ML-DSA secret (private) key.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[out] sig            Pointer for the memory location to store the
 *                             generated ML-DSA digital signature output.
 *  @param[out] sigLen         Pointer to variable to store the length of 
 *                             the signature in bytes.
 *  @param[in]  m              Pointer to the message corresponding to the
 *                             digital signature.
 *  @param[in]  mLen           Length of the signature in bytes.
 *  @param[in]  ctx            Pointer to the context string.
 *  @param[in]  ctxLen         Length of the context string in bytes.
 *  @param[in]  rand           Pointer to the a random string (for non-det
 *                             signing, else NULL (0).
 *  @param[in]  randLen        Length of the context string in bytes, or
 *                             a (0) value if deterministic signing is used.
 *  @param[in]  sk             Pointer to the secret (private) signing key.
 *  @param[in]  hashMode       Set to CRYPTO_MLDSA_SIG_MODE_PURE for pure 
 *                             ML-DSA, or CRYPTO_MLDSA_SIG_MODE_PREHASH for
 *                             pre-hash ML-DSA.  If using pre-hash, then 
 *                             parameter m shall be the string concatenation of
 *                             ( OID || PH(M) ) where OID is the DER encoded 
 *                             hash object identifier and PH(M) is the output 
 *                             of the hash function taken over the message 
 *                             before signing.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_OK if the signature was generated correctly.
 *  @retval     CRYPTO_MLDSA_STAT_CTX_TOO_LONG if the context was > 255 bytes.
 *  @retval     CRYPTO_MLDSA_STAT_RAND_TOO_SMALL if the randomized signing
 *              was selected but the randLen was < 32 bytes.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_65_sign( \
    uint8_t *sig, size_t *sigLen, \
    const uint8_t *m, size_t mLen, \
    const uint8_t *ctx, size_t ctxLen, \
    const uint8_t *rand, size_t randLen, \
    const uint8_t *sk, crypto_mldsa_mode_t hashMode);

/**
 *  @brief      Verify the ML-DSA-65 digital signature for a message using 
 *              an ML-DSA public key.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[in]  sig            Pointer for the memory location containing
 *                             the ML-DSA digital signature.
 *  @param[in]  sigLen         Length of the signature in bytes.
 *  @param[in]  m              Pointer to the message corresponding to the
 *                             digital signature.
 *  @param[in]  mLen           Length of the signature in bytes.
 *  @param[in]  ctx            Pointer to the context string.
 *  @param[in]  ctxLen         Length of the context string in bytes.
 *  @param[in]  pk             Pointer to the public key.
 *  @param[in]  hashMode       Set to CRYPTO_MLDSA_SIG_MODE_PURE for pure 
 *                             ML-DSA, or CRYPTO_MLDSA_SIG_MODE_PREHASH for
 *                             pre-hash ML-DSA.  If using pre-hash, then 
 *                             parameter m shall be the string concatenation of
 *                             ( OID || PH(M) ) where OID is the DER encoded 
 *                             hash object identifier and PH(M) is the output 
 *                             of the hash function taken over the message 
 *                             before signing.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_VALID_SIGNATURE for a successful verification.
 *  @retval     CRYPTO_MLDSA_STAT_INVALID_SIGNATURE for a failed verification.
 *  @retval     CRYPTO_MLDSA_STAT_CTX_TOO_LONG if the context was > 255 bytes.
 *  @retval     CRYPTO_MLDSA_STAT_SIG_LEN_INVALID if the signature length is
 *              not valid.
 *  @retval     CRYPTO_MLDSA_STAT_INVALID_SIGNATURE_FORM if the signature 
 *              values are not valid for verification.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_65_verify( \
    const uint8_t *sig, size_t sigLen, \
    const uint8_t *m, size_t mLen, \
    const uint8_t *ctx, size_t ctxLen, \
    const uint8_t *pk, crypto_mldsa_mode_t hashMode);

/**
 *  @brief      Generate a ML-DSA-87 key pair.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the key pair.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[out] pk             Pointer to location to store the 
 *                             public verification key.
 *  @param[out] sk             Pointer to location to store the 
 *                             secret (private) signing key.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLDSA_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than 32 bytes.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_87_keyGen( \
    const uint8_t *seed, size_t seedLen, \
    uint8_t *pk, uint8_t *sk);

/**
 *  @brief      Generate a ML-DSA-87 digital signature for a message using 
 *              an ML-DSA secret (private) key.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[out] sig            Pointer for the memory location to store the
 *                             generated ML-DSA digital signature output.
 *  @param[out] sigLen         Pointer to variable to store the length of 
 *                             the signature in bytes.
 *  @param[in]  m              Pointer to the message corresponding to the
 *                             digital signature.
 *  @param[in]  mLen           Length of the signature in bytes.
 *  @param[in]  ctx            Pointer to the context string.
 *  @param[in]  ctxLen         Length of the context string in bytes.
 *  @param[in]  rand           Pointer to the a random string (for non-det
 *                             signing, else NULL (0).
 *  @param[in]  randLen        Length of the context string in bytes, or
 *                             a (0) value if deterministic signing is used.
 *  @param[in]  sk             Pointer to the secret (private) signing key.
 *  @param[in]  hashMode       Set to CRYPTO_MLDSA_SIG_MODE_PURE for pure 
 *                             ML-DSA, or CRYPTO_MLDSA_SIG_MODE_PREHASH for
 *                             pre-hash ML-DSA.  If using pre-hash, then 
 *                             parameter m shall be the string concatenation of
 *                             ( OID || PH(M) ) where OID is the DER encoded 
 *                             hash object identifier and PH(M) is the output 
 *                             of the hash function taken over the message 
 *                             before signing.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_OK if the signature was generated correctly.
 *  @retval     CRYPTO_MLDSA_STAT_CTX_TOO_LONG if the context was > 255 bytes.
 *  @retval     CRYPTO_MLDSA_STAT_RAND_TOO_SMALL if the randomized signing
 *              was selected but the randLen was < 32 bytes.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_87_sign( \
    uint8_t *sig, size_t *sigLen, \
    const uint8_t *m, size_t mLen, \
    const uint8_t *ctx, size_t ctxLen, \
    const uint8_t *rand, size_t randLen, \
    const uint8_t *sk, crypto_mldsa_mode_t hashMode);

/**
 *  @brief      Verify the ML-DSA-87 digital signature for a message using 
 *              an ML-DSA public key.
 *
 *              NOTE: This function call requires a significant amount
 *                    of available stack memory.  Refer to the user's guide
 *                    for the stack requirements of this function.
 *
 *  @param[in]  sig            Pointer for the memory location containing
 *                             the ML-DSA digital signature.
 *  @param[in]  sigLen         Length of the signature in bytes.
 *  @param[in]  m              Pointer to the message corresponding to the
 *                             digital signature.
 *  @param[in]  mLen           Length of the signature in bytes.
 *  @param[in]  ctx            Pointer to the context string.
 *  @param[in]  ctxLen         Length of the context string in bytes.
 *  @param[in]  pk             Pointer to the public key.
 *  @param[in]  hashMode       Set to CRYPTO_MLDSA_SIG_MODE_PURE for pure 
 *                             ML-DSA, or CRYPTO_MLDSA_SIG_MODE_PREHASH for
 *                             pre-hash ML-DSA.  If using pre-hash, then 
 *                             parameter m shall be the string concatenation of
 *                             ( OID || PH(M) ) where OID is the DER encoded 
 *                             hash object identifier and PH(M) is the output 
 *                             of the hash function taken over the message 
 *                             before signing.
 *
 *  @return     The result of the operation, one of crypto_mldsa_stat_t.
 *  @retval     CRYPTO_MLDSA_STAT_VALID_SIGNATURE for a successful verification.
 *  @retval     CRYPTO_MLDSA_STAT_INVALID_SIGNATURE for a failed verification.
 *  @retval     CRYPTO_MLDSA_STAT_CTX_TOO_LONG if the context was > 255 bytes.
 *  @retval     CRYPTO_MLDSA_STAT_SIG_LEN_INVALID if the signature length is
 *              not valid.
 *  @retval     CRYPTO_MLDSA_STAT_INVALID_SIGNATURE_FORM if the signature 
 *              values are not valid for verification.
 */
extern crypto_mldsa_stat_t Crypto_MLDSA_87_verify( \
    const uint8_t *sig, size_t sigLen, \
    const uint8_t *m, size_t mLen, \
    const uint8_t *ctx, size_t ctxLen, \
    const uint8_t *pk, crypto_mldsa_mode_t hashMode);

#endif /* CRYPTO_ML_DSA_H_ */
