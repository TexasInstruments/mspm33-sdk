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
 *  @file       aes_hw_aesadv.h
 *  @brief      AES cipher implementation - hardware accelerated with AESADV
 * 
 *  This cipher module implements the advanced encryption standard using the
 *  MSP AESADV peripheral hardware.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_AES_HW_AESADV_H_
#define CRYPTO_AES_HW_AESADV_H_

#if CRYPTO_HW_USE_MSPM0_AESADV == 1 || CRYPTO_HW_USE_MSPM33_AESADV == 1

#include <stdint.h>
#include "ti/crypto/include/cipher/aes_type.h"

/* External function declarations */

/**
 *  @brief      Initialize the AESADV engine for subsequent encrypt/decrypt.
 *              This routine enables power to the AESADV engine and resets it.
 *
 *  @return     None.
 */
extern void Crypto_AES_HW_AESADV_init(void);

/**
 *  @brief      De-initialize the AESADV engine after use.  
 *              This resets the AESADV engine and disables its power.
 *
 *  @return     None.
 */
extern void Crypto_AES_HW_AESADV_deinit(void);

/**
 *  @brief      Load an AES session key to the AESADV engine.  This key may
 *               then be selected for use in subsequent encrypt/decrypt calls.
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
extern crypto_aes_stat_t Crypto_AES_HW_AESADV_setSessionKey( \
    const uint32_t *key, crypto_aes_keytype_t keyType);

/**
 *  @brief      Run a single block AES operation on the AESADV
 *              hardware accelerator.
 *
 *  @param[in]  in             Pointer to the input ciphertext block which
 *                             must be a buffer of 128 bits (4x 32-bit words).
 *  @param[out] out            Pointer to the output plaintext block which
 *                             must be a buffer of 128 bits (4x 32-bit words).
 *  @param[in]  key            The AES engine key selection (which key to use).
 *  @param[in]  op             Selects encryption or decryption.
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if pointers are not 32-bit aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_HW_AESADV_runBlock( \
    const uint32_t *in, uint32_t *out, \
    crypto_aes_key_t key, crypto_aes_op_t op);

/**
 *  @brief      Perform an AES cipher block chained (CBC) operation sequence
 *              on the AESADV hardware accelerator.
 *
 *  @param[in]  in             Pointer to the input buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            The length of the input in bytes.
 *                             The input must be a multiple of 16 bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *  @param[in]  iv             Pointer to the initialization vector (IV) which
 *                             must be 32-bit word aligned.
 *  @param[in]  op             Specify if this is encrypt, decrypt, or MAC.
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not a block multiple,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_HW_AESADV_CBC_runSeq( \
    const uint32_t *in, uint32_t *out, \
    uint32_t len, const uint32_t *iv, \
    crypto_aes_key_t key, crypto_aes_op_t op);

/**
 *  @brief      Perform an AES counter (CTR) operation sequence
 *              on the AESADV hardware accelerator.  The CTR mode
 *              always uses a 96-bit nonce value and a 32-bit counter value.
 *
 *  @param[in]  in             Pointer to the input buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output buffer which
 *                             must be 32-bit word aligned.
 *  @param[in]  len            The length of the input in bytes.
 *                             The input must be a multiple of 16 bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *  @param[in]  nonce          Pointer to the 96-bit (12-byte) nonce,
 *                             must be 32-bit word aligned.
 *  @param[in]  counter        The default counter value to start from,
 *                             usually 0 or 1.
 *  @param[in]  op             Specify if this is encrypt or decrypt.
 *
 *  @return     Status of the operation, one of crypto_aes_stat_t.
 *  @retval     CRYPTO_AES_OK if the operation completed successfully.
 *  @retval     CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval     CRYPTO_AES_ALIGNMENT_ERR if the length is not a block multiple,
 *              or pointers to buffers are not 32-bit word aligned.
 *  @retval     CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_HW_AESADV_CTR_runSeq( \
    const uint32_t *in, uint32_t *out, \
    uint32_t len, const uint32_t *nonce, uint32_t counter, \
    crypto_aes_key_t key, crypto_aes_op_t op);

/**
 *  @brief      Perform an AES counter with CBC-MAC (CCM) operation sequence
 *              on the AESADV hardware accelerator.  CCM is an authenticate-then
 *              encrypt approach to constructing an AEAD (authenticated 
 *              encryption with associated data) cipher.
 *              NOTE: The CCM mode allows for 7, 8, 9, 10, 11, 12, 
 *              or 13 byte nonces.
 *              NOTE: The CCM mode allows for 4, 6, 8, 10, 12, 14, 
 *              or 16 byte tags.
 *
 *  @param[in]    nonce        Pointer to the nonce (always an input)
 *  @param[in]    nonceLen     Nonce length, in bytes. Must be between 7 and 13.
 *  @param[in]    aad          Pointer to the input additional associated data 
 *                             AAD buffer (always an input).  Must be aadLen
 *                             bytes in length.
 *  @param[in]    aadLen       The length of the additional associated data, in 
 *                             bytes.
 *  @param[in]    in           Pointer to the payload plaintest input (for 
 *                             encryption), or ciphertext input (decryption).
 *                             Must be 'len' bytes in length.
 *  @param[out]   out          Pointer to the ciphertext output (encryption),
 *                             or payload plaintext output (decryption).
 *                             Must be 'len' bytes in length.
 *  @param[in]    len          The length of the payload data, in bytes.
 *  @param[inout] tag          Pointer to the tag buffer (output in case of
 *                             encrypt, OR input in case of decrypt).  Must
 *                             be 'tagLen' bytes in length.
 *  @param[in]    tagLen       Tag length, in bytes. Must be one of { 4, 6, 8,
 *                             10, 12, 14, or 16 }.
 *  @param[in]    key          The AES engine key selection (which key to use).
 *  @param[in]    op           Specify if this is encrypt or decrypt.
 *
 *  @return       Status of the operation, one of crypto_aes_stat_t.
 *  @retval       CRYPTO_AES_OK if the operation completed successfully.
 *  @retval       CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval       CRYPTO_AES_ALIGNMENT_ERR if the length is not a block
 *                multiple, or pointers to buffers are not 32-bit word aligned.
 *  @retval       CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_HW_AESADV_CCM_runSeq( \
    const uint8_t *nonce, uint8_t nonceLen, \
    const uint8_t *aad, uint32_t aadLen, \
    const uint8_t *in,  uint8_t *out, uint32_t len, \
    uint8_t *tag, uint8_t tagLen, \
    crypto_aes_key_t key, crypto_aes_op_t op);

/**
 *  @brief      Perform an AES cipher based MAC (CMAC) operation sequence
 *              on the AESADV hardware accelerator.  Inputs must be 
 *              pre-padded with length that is a multiple of the block size.
 *
 *  @param[in]  in             Pointer to the input plaintext buffer which 
 *                             must be 32-bit word aligned.
 *  @param[out] out            Pointer to the output MAC tag buffer.
 *  @param[in]  mLen           The length of the input plaintext in bytes.
 *  @param[in]  tagLen         The desired length of the output tag in bytes.
 *  @param[in]  key            The AES engine key selection (which key to use).
 *
 *  @return       Status of the operation, one of crypto_aes_stat_t.
 *  @retval       CRYPTO_AES_OK if the operation completed successfully.
 *  @retval       CRYPTO_AES_PARAM_ERR if parameters were incorrect.
 *  @retval       CRYPTO_AES_ALIGNMENT_ERR if the length is not a block
 *                multiple, or pointers to buffers are not 32-bit word aligned.
 *  @retval       CRYPTO_AES_HW_ERR if the AES HW was not enabled.
 */
extern crypto_aes_stat_t Crypto_AES_HW_AESADV_CMAC_runSeq( \
    const uint32_t *in, uint8_t *out,
    uint32_t mLen, uint8_t tagLen, crypto_aes_key_t key);

#endif /* CRYPTO_HW_USE_MSPM0_AESADV == 1 || CRYPTO_HW_USE_MSPM33_AESADV == 1 */

#endif /* CRYPTO_AES_HW_AESADV_H_ */
