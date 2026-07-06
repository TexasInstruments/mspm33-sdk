/*
 * Copyright (C) 2026 Texas Instruments Incorporated
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
 *  @file       ml-kem.h
 *  @brief      Module lattice key encapsulation mechanism (ML-KEM)
 * 
 *  This module implements the FIPS 203 standard for a post-quantum resistant
 *  key encapsulation mechanism (Crystals: Kyber) , providing a KEM
 *  implementation with the following properties:
 *    - IND-CPA secure (indistinguishability under a chosen plaintext attack)
 *    - IND-CCA secure (indistinguishability under a chosen ciphertext attack)
 *
 ******************************************************************************
 */

#ifndef CRYPTO_ML_KEM_H_
#define CRYPTO_ML_KEM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "ti/crypto/include/pqc/ml-kem-type.h"

/* External constant definitions */

/** @brief ML-KEM key generation random seed length in bytes */
#define CRYPTO_MLKEM_KEYGEN_SEED_LEN (64U)

/** @brief ML-KEM encapsulation random seed length in bytes */
#define CRYPTO_MLKEM_ENCAPS_SEED_LEN (32U)

/** @brief ML-KEM-1024 encapsulation (private) key length in bytes */
#define CRYPTO_MLKEM_512_EK_LEN (800U)

/** @brief ML-KEM-1024 decapsulation (private) key length in bytes */
#define CRYPTO_MLKEM_512_DK_LEN (1632U)

/** @brief ML-KEM-1024 cipher text length in bytes */
#define CRYPTO_MLKEM_512_CIPHER_TEXT_LEN (768U)

/** @brief ML-KEM-768 encapsulation (private) key length in bytes */
#define CRYPTO_MLKEM_768_EK_LEN (1184U)

/** @brief ML-KEM-768 decapsulation (private) key length in bytes */
#define CRYPTO_MLKEM_768_DK_LEN (2400U)

/** @brief ML-KEM-768 cipher text length in bytes */
#define CRYPTO_MLKEM_768_CIPHER_TEXT_LEN (1088U)

/** @brief ML-KEM-1024 encapsulation (private) key length in bytes */
#define CRYPTO_MLKEM_1024_EK_LEN (1568U)

/** @brief ML-KEM-1024 decapsulation (private) key length in bytes */
#define CRYPTO_MLKEM_1024_DK_LEN (3168U)

/** @brief ML-KEM-1024 cipher text length in bytes */
#define CRYPTO_MLKEM_1024_CIPHER_TEXT_LEN (1568U)

/** @brief ML-KEM shared secret length in bytes */
#define CRYPTO_MLKEM_SHARED_SECRET_LEN (32U)

/* External function declarations */

/**
 *  @brief      Generate a ML-KEM-512 key pair.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the key pair.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[out] ek             Pointer to location to store the 
 *                             encapsulation (public) key.
 *  @param[out] dk             Pointer to location to store the 
 *                             decapsulation (private) key.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLKEM_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than CRYPTO_MLKEM_KEYGEN_SEED_LEN bytes.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_512_keyGen( \
    const uint8_t *seed, size_t seedLen, \
    uint8_t *ek, uint8_t *dk);

/**
 *  @brief      Using ML-KEM-512, generate a shared secret and associated 
 *              ciphertext based on a provided encapsulation key and 
 *              a random seed value.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the shared secret and cipher
 *                             text.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[in]  ek             Pointer to the encapsulation key, a byte array.
 *  @param[out] ct             Pointer to location to store the 
 *                             cipher text output, a byte array.
 *  @param[out] sharedSecret   Pointer to location to store the 
 *                             shared secret output, a byte array.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLKEM_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than CRYPTO_MLKEM_ENCAPS_SEED_LEN bytes.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_512_encaps( \
    const uint8_t *seed, size_t seedLen, \
    const uint8_t *ek, \
    uint8_t *ct, \
    uint8_t *sharedSecret);

/**
 *  @brief      Using ML-KEM-512, compute a shared secret from a associated ciphertext
 *              and provided decapsulation key.
 *
 *  @param[in]  dk             Pointer to the decapsulation key, a byte array.
 *  @param[in]  ct             Pointer to location to the  cipher text output, 
 *                             a byte array.
 *  @param[out] sharedSecret   Pointer to location to store the 
 *                             shared secret output, a byte array.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK in all cases.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_512_decaps( \
    const uint8_t *dk, \
    const uint8_t *ct, \
    uint8_t *sharedSecret);

/**
 *  @brief      Generate a ML-KEM-768 key pair.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the key pair.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[out] ek             Pointer to location to store the 
 *                             encapsulation (public) key.
 *  @param[out] dk             Pointer to location to store the 
 *                             decapsulation (private) key.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLKEM_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than CRYPTO_MLKEM_KEYGEN_SEED_LEN bytes.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_768_keyGen( \
    const uint8_t *seed, size_t seedLen, \
    uint8_t *ek, uint8_t *dk);

/**
 *  @brief      Using ML-KEM-768, generate a shared secret and associated 
 *              ciphertext based on a provided encapsulation key and 
 *              a random seed value.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the shared secret and cipher
 *                             text.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[in]  ek             Pointer to the encapsulation key, a byte array.
 *  @param[out] ct             Pointer to location to store the 
 *                             cipher text output, a byte array.
 *  @param[out] sharedSecret   Pointer to location to store the 
 *                             shared secret output, a byte array.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLKEM_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than CRYPTO_MLKEM_ENCAPS_SEED_LEN bytes.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_768_encaps( \
    const uint8_t *seed, size_t seedLen, \
    const uint8_t *ek, \
    uint8_t *ct, \
    uint8_t *sharedSecret);

/**
 *  @brief      Using ML-KEM-768, compute a shared secret from a associated ciphertext
 *              and provided decapsulation key.
 *
 *  @param[in]  dk             Pointer to the decapsulation key, a byte array.
 *  @param[in]  ct             Pointer to location to the  cipher text output, 
 *                             a byte array.
 *  @param[out] sharedSecret   Pointer to location to store the 
 *                             shared secret output, a byte array.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK in all cases.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_768_decaps( \
    const uint8_t *dk, \
    const uint8_t *ct, \
    uint8_t *sharedSecret);

/**
 *  @brief      Generate a ML-KEM-1024 key pair.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the key pair.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[out] ek             Pointer to location to store the 
 *                             encapsulation (public) key.
 *  @param[out] dk             Pointer to location to store the 
 *                             decapsulation (private) key.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLKEM_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than CRYPTO_MLKEM_KEYGEN_SEED_LEN bytes.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_1024_keyGen( \
    const uint8_t *seed, size_t seedLen, \
    uint8_t *ek, uint8_t *dk);

/**
 *  @brief      Using ML-KEM-1024, generate a shared secret and associated 
 *              ciphertext based on a provided encapsulation key and 
 *              a random seed value.
 *
 *  @param[in]  seed           Pointer to the seed value (source of randomness)
 *                             from which to derive the shared secret and cipher
 *                             text.
 *  @param[in]  seedLen        Length of the seed value in bytes.
 *  @param[in]  ek             Pointer to the encapsulation key, a byte array.
 *  @param[out] ct             Pointer to location to store the 
 *                             cipher text output, a byte array.
 *  @param[out] sharedSecret   Pointer to location to store the 
 *                             shared secret output, a byte array.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK if the key pair was generated correctly.
 *  @retval     CRYPTO_MLKEM_STAT_SEED_TOO_SMALL if the seed length was less 
 *              than CRYPTO_MLKEM_ENCAPS_SEED_LEN bytes.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_1024_encaps( \
    const uint8_t *seed, size_t seedLen, \
    const uint8_t *ek, \
    uint8_t *ct, \
    uint8_t *sharedSecret);

/**
 *  @brief      Using ML-KEM-1024, compute a shared secret from a associated ciphertext
 *              and provided decapsulation key.
 *
 *  @param[in]  dk             Pointer to the decapsulation key, a byte array.
 *  @param[in]  ct             Pointer to location to the  cipher text output, 
 *                             a byte array.
 *  @param[out] sharedSecret   Pointer to location to store the 
 *                             shared secret output, a byte array.
 *
 *  @return     The result of the operation, one of crypto_mlkem_stat_t.
 *  @retval     CRYPTO_MLKEM_STAT_OK in all cases.
 */
extern crypto_mlkem_stat_t Crypto_MLKEM_1024_decaps( \
    const uint8_t *dk, \
    const uint8_t *ct, \
    uint8_t *sharedSecret);

#endif /* CRYPTO_ML_KEM_H_ */
