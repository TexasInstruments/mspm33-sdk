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
 *  @file       curve25519.h
 *  @brief      Edwards curve (Ed25519) digital signature verification
 *
 *  This module implements Ed25519 signature verification as defined in RFC 8032.
 *  Ed25519 is a signature scheme using a twisted Edwards curve that is
 *  birationally equivalent to curve25519.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_ED25519_H_
#define CRYPTO_ED25519_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief      The X25519 key length in bytes
 */
#define X25519_KEY_LENGTH 32

/**
 * @brief      The Ed25519 public key length in bytes
 */
#define ED25519_PUBLIC_KEY_LENGTH   32

/**
 * @brief      The Ed25519 private key length in bytes
 */
#define ED25519_PRIVATE_KEY_LENGTH  32

/**
 * @brief      The Ed25519 signature length in bytes
 */
#define ED25519_SIGNATURE_LENGTH    64

/* External data type definitions */

/**
 *  @brief      The X25519 module status response value set.
 */
typedef enum
{
    CRYPTO_X25519_STAT_SUCCESS = 0x99,
    CRYPTO_X25519_STAT_INVALID_PARAM = 0x22,
    CRYPTO_X25519_STAT_ERROR = 0x11,
} crypto_x25519_stat_t;
/**
 *  @brief      The Ed25519 module status response value set.
 */
typedef enum
{
    CRYPTO_ED25519_STAT_SUCCESS = 0x99,
    CRYPTO_ED25519_STAT_VALID_SIGNATURE = 0xA5,
    CRYPTO_ED25519_STAT_INVALID_SIGNATURE = 0x5A,
    CRYPTO_ED25519_STAT_INVALID_PARAM = 0x22,
    CRYPTO_ED25519_STAT_ERROR = 0x11,
} crypto_ed25519_stat_t;

/* External function declarations */

/**
 *  @brief      Generate an X25519 public/private key pair.
 *
 *  @param[in]  rand  Buffer to hold the 32-byte random entropy (passed as the
 *                    secret key).
 *  @param[out] pk    Buffer to hold the generated 32-byte public key.
 *  @param[out] sk    Buffer to hold the 32-byte private key. This has some
 *                    modifications from the random seed, and thus should be
 *                    used
 *
 *  @return     The resulting status of the operation, one of crypto_x25519_stat_t.
 *  @retval     CRYPTO_X25519_STAT_SUCCESS if the key pair was generated successfully.
 *  @retval     CRYPTO_X25519_STAT_ERROR if an error occurred during key generation.
 *  @retval     CRYPTO_X25519_STAT_INVALID_PARAM if any of the input parameters are invalid.
 */
extern crypto_x25519_stat_t Crypto_X25519_keyGen(
    const uint8_t *rand,
    uint8_t pk[X25519_KEY_LENGTH],
    uint8_t sk[X25519_KEY_LENGTH]);

/**
 *  @brief      Performs an X25519 key exchange operation.
 *
 *  This function computes a shared secret using the X25519 Diffie-Hellman key
 *  exchange protocol. It takes the caller's private key and the peer's public
 *  key as input and produces a shared secret that can be derived by both parties.
 *
 *  @param[in]  sk             The caller's 32-byte private key.
 *  @param[in]  peer_pk        The peer's 32-byte public key.
 *  @param[out] shared_secret  Buffer to hold the 32-byte computed shared secret.
 *
 *  @return     The resulting status of the operation, one of crypto_x25519_stat_t.
 *  @retval     CRYPTO_X25519_STAT_SUCCESS if the key exchange was successful.
 *  @retval     CRYPTO_X25519_STAT_ERROR if an error occurred during key exchange,
 *              such as the peer's public key being a point of small order.
 *  @retval     CRYPTO_X25519_STAT_INVALID_PARAM if any of the input parameters are invalid.
 *
 *  @note       The shared secret should not be used directly as an encryption key.
 *              It should be passed through a key derivation function (KDF).
 */
extern crypto_x25519_stat_t Crypto_X25519_keyExchange(
    const uint8_t sk[X25519_KEY_LENGTH],
    const uint8_t peer_pk[X25519_KEY_LENGTH],
    uint8_t shared_secret[X25519_KEY_LENGTH]);

/**
 *  @brief      Generate an Ed25519 public/privatekey pair.
 *
 *  @param[in]  rand  Buffer to hold the 32-byte random entropy (passed as the
 *                    secret key).
 *  @param[out] pk    Buffer to hold the generated 32-byte public key.
 *  @param[out] sk    Buffer to hold the 32-byte private key. This is the same
 *                    value as the passed random value, but is included for
 *                    continuity with other APIs.
 *
 *  @return     The resulting status of the operation, one of crypto_ed25519_stat_t.
 *  @retval     CRYPTO_ED25519_STAT_SUCCESS if the key pair was generated successfully.
 *  @retval     CRYPTO_ED25519_STAT_INVALID_PARAM if any of the input parameters are invalid.
 */
extern crypto_ed25519_stat_t Crypto_ED25519_keyGen(
    const uint8_t *rand,
    uint8_t pk[ED25519_PUBLIC_KEY_LENGTH],
    uint8_t sk[ED25519_PRIVATE_KEY_LENGTH]);

/**
 *  @brief      Sign a message using Ed25519.
 *
 *  @param[in]  msg            Pointer to the message to sign.
 *  @param[in]  msg_len        Length of the message in bytes.
 *  @param[in]  pk             The 32-byte public key.
 *  @param[in]  sk             The 32-byte private key (secret key).
 *  @param[out] sig            Buffer to hold the 64-byte signature.
 *
 *  @return     The resulting status of the operation, one of crypto_ed25519_stat_t.
 *  @retval     CRYPTO_ED25519_STAT_SUCCESS if the message was signed successfully.
 *  @retval     CRYPTO_ED25519_STAT_INVALID_PARAM if any of the input parameters are invalid.
 *  @retval     CRYPTO_ED25519_STAT_ERROR if an error occurred during signing.
 */
extern crypto_ed25519_stat_t Crypto_ED25519_sign(
    const uint8_t *msg, const size_t msg_len,
    const uint8_t pk[ED25519_PUBLIC_KEY_LENGTH],
    const uint8_t sk[ED25519_PRIVATE_KEY_LENGTH],
    uint8_t sig[ED25519_SIGNATURE_LENGTH]);


/**
 *  @brief      Verify an Ed25519 digital signature for a message using
 *              an Ed25519 public key.
 *
 *  @param[in]  msg            Pointer to the message verify.
 *  @param[in]  msg_len        Length of the message in bytes.
 *  @param[in]  sig            Pointer to the 64-byte Ed25519 signature.
 *                             The first 32 bytes contain the R value,
 *                             the second 32 bytes contain the S value.
 *  @param[in]  pk             Pointer to the 32-byte Ed25519 public key.
 *
 *  @return     The resulting status of the operation, one of crypto_ed25519_stat_t.
 *  @retval     CRYPTO_ED25519_STAT_VALID_SIGNATURE if the signature was valid for the
 *              message and public key.
 *  @retval     CRYPTO_ED25519_STAT_INVALID_SIGNATURE if the signature was not valid for the
 *              message and public key.
 *  @retval     CRYPTO_ED25519_STAT_INVALID_PARAM if any of the input parameters are invalid.
 *  @retval     CRYPTO_ED25519_STAT_ERROR if an error occurred during verification.
 */
extern crypto_ed25519_stat_t Crypto_ED25519_verify(
    const uint8_t *msg, const size_t msg_len,
    const uint8_t sig[ED25519_SIGNATURE_LENGTH],
    const uint8_t pk[ED25519_PUBLIC_KEY_LENGTH]);


#endif /* CRYPTO_ED25519_H_ */