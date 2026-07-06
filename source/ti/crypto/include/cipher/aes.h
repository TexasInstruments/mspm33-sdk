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
 *  @file       aes.h
 *  @brief      AES cipher implementation
 * 
 *  This cipher module implements the advanced encryption standard through a
 *  common top level API (presented in this interface) which can be mapped
 *  to different underlying hardware/software implementations.
 *
 *  Currently the following internal implementations are supported:
 *    - MSP AESADV HW peripheral accelerator
 *
 ******************************************************************************
 */

#ifndef CRYPTO_AES_H_
#define CRYPTO_AES_H_

#include <stdint.h>
#include "ti/crypto/include/cipher/aes_type.h"

/**
 *  @brief      Initialize the AES module for subsequent operations.
 *
 *  @return     None.
 */
extern void Crypto_AES_init(void);

/**
 *  @brief      De-initialize the AES module after use.  
 *
 *  @return     None.
 */
extern void Crypto_AES_deinit(void);

/**
 *  @brief      Load an AES session key to the AES module.  This key may then
 *              be selected for use in subsequent encrypt/decrypt calls.
 *
 *  @param[in]  key            Pointer to the key value to load to the engine.
 *  @param[in]  keyType        Type of AES key, one of crypto_aes_keytype_t.
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_KEYWRITE_ERR if the AES key memory was not writeable.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if pointer is not 32-bit aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_setSessionKey(const uint32_t *key, \
                                                  crypto_aes_keytype_t keyType);

/**
 *  @brief      Perform an AES encryption (forward) operation
 *              on a single 128-bit data block.
 *
 *  @param[in]  in             Pointer to the input plaintext block which
 *                             must be a buffer of 128 bits (4x 32-bit words).
 *  @param[out] out            Pointer to the output ciphertext block which
 *                             must be a buffer of 128 bits (4x 32-bit words).
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if pointers are not 32-bit aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_encrypt(const uint32_t *in, \
                                            uint32_t *out, \
                                            crypto_aes_key_t key);

/**
 *  @brief      Perform an AES decryption (reverse) operation
 *              on a single 128-bit data block.
 *
 *  @param[in]  in             Pointer to the input ciphertext block which
 *                             must be a buffer of 128 bits (4x 32-bit words).
 *  @param[out] out            Pointer to the output plaintext block which
 *                             must be a buffer of 128 bits (4x 32-bit words).
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if pointers are not 32-bit aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_decrypt(const uint32_t *in, \
                                            uint32_t *out, \
                                            crypto_aes_key_t key);

/**
 *  @brief      Perform an AES cipher block chained (CBC) encryption on a
 *              input plaintext buffer.  Inputs must be pre-padded and
 *              with length that is a multiple of the block size.
 *
 *  @param[in]  in             Pointer to the input plaintext buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output ciphertext buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            Pointer to the length of the input plaintext
 *                             in bytes.  The input must be a multiple of 
 *                             16 bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *  @param[in]  iv             Pointer to the initialization vector (IV) which
 *                             must be 32-bit word aligned.
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CBC_encrypt(const uint32_t *in, \
                                                uint32_t *out, \
                                                uint32_t len, \
                                                const uint32_t *iv, \
                                                crypto_aes_key_t key);

/**
 *  @brief      Perform an AES cipher block chained (CBC) decryption on a
 *              input ciphertext buffer.  Inputs must be pre-padded and
 *              with length that is a multiple of the block size.
 *
 *  @param[in]  in             Pointer to the input ciphertext buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output plaintext buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            Pointer to the length of the input ciphertext
 *                             in bytes.  The input must be a multiple of 
 *                             16 bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *  @param[in]  iv             Pointer to the initialization vector (IV) which
 *                             must be 32-bit word aligned.
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CBC_decrypt(const uint32_t *in, \
                                                uint32_t *out, \
                                                uint32_t len, \
                                                const uint32_t *iv, \
                                                crypto_aes_key_t key);

/**
 *  @brief      Perform an AES cipher block chained (CBC) MAC on a
 *              input plaintext buffer.  Inputs must be pre-padded and
 *              with length that is a multiple of the block size.
 *
 *              NOTE: CBC-MAC can be insecure in certain scenarios,
 *              including cases where the message length is variable.
 *              CMAC is recommended instead of CBC-MAC as it is a safer
 *              option.  CBC-MAC is still included here as it is utilized
 *              in DRBG random number generation.
 *
 *  @param[in]  in             Pointer to the input plaintext buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output MAC tag buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            Pointer to the length of the input plaintext
 *                             in bytes.  The input must be a multiple of 
 *                             16 bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CBC_MAC(const uint32_t *in, \
                                            uint32_t *out, \
                                            uint32_t len, \
                                            crypto_aes_key_t key);

/**
 *  @brief      Perform an AES counter (CTR) encryption on a
 *              input plaintext buffer.  Inputs must be pre-padded and
 *              with length that is a multiple of the block size.  The CTR mode
 *              always uses a 96-bit nonce value and a 32-bit counter value.
 *
 *  @param[in]  in             Pointer to the input plaintext buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output ciphertext buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            Pointer to the length of the input plaintext
 *                             in bytes.  The input must be a multiple of 
 *                             16 bytes.
 *  @param[in]  nonce          Pointer to the 96-bit nonce which
 *                             must be 32-bit word aligned.
 *  @param[in]  counter        Tthe 32-bit initial counter value.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CTR_encrypt(const uint32_t *in, \
                                                uint32_t *out, \
                                                uint32_t len, \
                                                const uint32_t *nonce, \
                                                uint32_t counter, \
                                                crypto_aes_key_t key);

/**
 *  @brief      Perform an AES counter (CTR) decryption on a
 *              input ciphertext buffer.  Inputs must be pre-padded and
 *              with length that is a multiple of the block size.  The CTR mode
 *              always uses a 96-bit nonce value and a 32-bit counter value.
 *
 *  @param[in]  in             Pointer to the input ciphertext buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output plaintext buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            Pointer to the length of the input ciphertext
 *                             in bytes.  The input must be a multiple of 
 *                             16 bytes.
 *  @param[in]  nonce          Pointer to the 96-bit nonce which
 *                             must be 32-bit word aligned.
 *  @param[in]  counter        The 32-bit initial counter value.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CTR_decrypt(const uint32_t *in, \
                                                uint32_t *out, \
                                                uint32_t len, \
                                                const uint32_t *nonce, \
                                                uint32_t counter, \
                                                crypto_aes_key_t key);

/**
 *  @brief      Perform an AES counter with CBC-MAC (CCM) encryption/generation
 *              operation on the input buffer, storing ciphertext of the payload
 *              in the output buffer and the generated tag in the tag buffer.
 *              It is valid to have only associated data with no payload, 
 *              or only payload with no associated data, but one of the
 *              two must be present.  Inputs are not required to be
 *              32-bit word aligned or pre-padded (the function will pad if 
 *              required during CBC-MAC tag generation).
 *
 *  @param[in]  nonce        Pointer to the nonce (always an input)
 *  @param[in]  nonceLen     Nonce length, in bytes. Must be between 7 and 13.
 *  @param[in]  aad          Pointer to the optional input additional associated
 *                           data AAD buffer (always an input).
 *                           Must be 'aadLen' bytes in length.
 *  @param[in]  aadLen       The length of the additional associated data, in 
 *                           bytes.  Set to zero if no AAD (only payload).
 *  @param[in]  in           Pointer to the payload plaintest input.
 *                           Must be 'len' bytes in length.
 *  @param[out] out          Pointer to the ciphertext output.
 *                           Must be 'len' bytes in length.
 *  @param[in]  len          The length of the payload, in bytes.
 *                           Set to zero of no payload (only AAD).
 *  @param[out] tag          Pointer to the tag buffer.
 *                           Must be 'tagLen' bytes in length.
 *  @param[in]  tagLen       Tag length, in bytes. Must be one of { 4, 6, 8,
 *                           10, 12, 14, or 16 }.
 *  @param[in]  key          The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not a block multiple,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CCM_encryptAndGen(const uint8_t *nonce, \
                                                      uint8_t nonceLen, \
                                                      const uint8_t *aad, \
                                                      uint32_t aadLen, \
                                                      const uint8_t *in, \
                                                      uint8_t *out, \
                                                      uint32_t len, \
                                                      uint8_t *tag, \
                                                      uint8_t tagLen, \
                                                      crypto_aes_key_t key);

/**
 *  @brief      Perform an AES counter with CBC-MAC (CCM) decryption/verify
 *              operation on the input buffer, storing plaintext of the payload
 *              in the output buffer.  It is valid to have only associated data
 *              with no payload, or only payload with no associated data, but
 *              one of the two must be present.  Inputs are not required to be
 *              32-bit word aligned or pre-padded (the function will pad if 
 *              required during CBC-MAC tag generation).
 *
 *  @param[in]  nonce        Pointer to the nonce (always an input)
 *  @param[in]  nonceLen     Nonce length, in bytes. Must be between 7 and 13.
 *  @param[in]  aad          Pointer to the optional input additional associated
 *                           data AAD buffer (always an input).
 *                           Must be 'aadLen' bytes in length.
 *  @param[in]  aadLen       The length of the additional associated data, in 
 *                           bytes.  Set to zero if no AAD (only payload).
 *  @param[in]  in           Pointer to the payload ciphertext input.
 *                           Must be 'len' bytes in length.
 *  @param[out] out          Pointer to the payload plaintext output.
 *                           Must be 'len' bytes in length.
 *  @param[in]  len          The length of the payload, in bytes.
 *                           Set to zero of no payload (only AAD).
 *  @param[in]  tag          Pointer to the tag buffer input.
 *                           Must be 'tagLen' bytes in length.
 *  @param[in]  tagLen       Tag length, in bytes. Must be one of { 4, 6, 8,
 *                           10, 12, 14, or 16 }.
 *  @param[in]  key          The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK_TAG_VALID if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not a block multiple,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 *  @retval     CRYPTO_AES_TAG_INVALID_ERR if the tag was invalid.
 */
extern crypto_aes_stat_t Crypto_AES_CCM_decryptAndVer(const uint8_t *nonce, \
                                                      uint8_t nonceLen, \
                                                      const uint8_t *aad, \
                                                      uint32_t aadLen, \
                                                      const uint8_t *in, \
                                                      uint8_t *out, \
                                                      uint32_t len, \
                                                      uint8_t *tag, \
                                                      uint8_t tagLen, \
                                                      crypto_aes_key_t key);

/**
 *  @brief      Generate an AES cipher based MAC (CMAC) tag from a
 *              input plaintext buffer and key.
 *
 *  @param[in]  in             Pointer to the input buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output MAC tag buffer.
 *  @param[in]  mLen           The length of the input in bytes.
 *  @param[in]  tagLen         The desired length of the output tag in bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CMAC_genTag(const uint32_t *in, \
                                                uint8_t *out, \
                                                uint32_t mLen, \
                                                uint8_t tagLen, \
                                                crypto_aes_key_t key);

/**
 *  @brief      Verify an AES cipher based MAC (CMAC) tag for a given
 *              input plaintext buffer and key.
 *
 *  @param[in]  in             Pointer to the input buffer which 
 *                             must be 32-bit word aligned.
 *  @param[in]  tag            Pointer to the expected MAC tag buffer.
 *  @param[in]  mlen           The length of the input in bytes.
 *  @param[in]  tagLen         The desired length of the output tag in bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK_TAG_VALID if the tag is valid for the msg, key.
 *  @retval     CRYPTO_AES_TAG_INVALID_ERR if the tag is not valid.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not block aligned,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_CMAC_verTag(const uint32_t *in, \
                                                const uint8_t *tag, \
                                                uint32_t mlen, \
                                                uint8_t tagLen, \
                                                crypto_aes_key_t key);

#endif /* CRYPTO_AES_H_ */
