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
 *  @file       aes_type.h
 *  @brief      AES cipher implementation data types
 * 
 *  This file implements the advanced encryption standard data types.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_AES_TYPE_H_
#define CRYPTO_AES_TYPE_H_

#include <stdint.h>

/**
 * @brief Number of bytes in the AES state.
 */
#define CRYPTO_AES_STATE_BYTES (16U)

/**
 * @brief Number of 32-bit words in the AES state.
 */
#define CRYPTO_AES_STATE_WORDS (4U)

/**
 * @brief Number of 32-bit words in a 128-bit AES key.
 */
#define CRYPTO_AES_KEY_128_WORDS (4U)

/**
 * @brief Number of 32-bit words in a 256-bit AES key.
 */
#define CRYPTO_AES_KEY_256_WORDS (8U)

/**
 * @brief Number of 32-bit words in the AES-CTR nonce.
 */
#define CRYPTO_AES_CTR_NONCE_WORDS (3U)

/**
 * @brief Maximum number of nonce bytes in CCM mode
 */
#define CRYPTO_AES_CCM_NONCE_BYTES_MAX (13U)

/**
 * @brief Minimum number of nonce bytes in CCM mode
 */
#define CRYPTO_AES_CCM_NONCE_BYTES_MIN (7U)

/**
 * @brief Maximum number of tag bytes in CCM mode
 */
#define CRYPTO_AES_CCM_TAG_BYTES_MAX (16U)

/**
 * @brief Minimum number of tag bytes in CCM mode
 */
#define CRYPTO_AES_CCM_TAG_BYTES_MIN (4U)

/**
 * @brief The AES status values, returned by AES functions
 *        to indicate the status of the requested operation.
 */
typedef enum 
{
    /* The MAC tag verification failed during the operation. */
    CRYPTO_AES_TAG_INVALID_ERR = -5,
    /* A hardware accelerator error occured during the operation. */
    CRYPTO_AES_HW_ERR = -4,
    /* The key memory was not writeable during the key set operation. */
    CRYPTO_AES_KEYWRITE_ERR = -3,
    /* Parameters were not properly aligned. */
    CRYPTO_AES_ALIGNMENT_ERR = -2,
    /* One or more Parameters passed to the function were invalid. */
    CRYPTO_AES_PARAM_ERR = -1,
    /* The operation completed successfully. */
    CRYPTO_AES_OK = 0,
    /* The operation completed successfully and the MAC tag was valid. */
    CRYPTO_AES_OK_TAG_VALID = 1
} crypto_aes_stat_t;

/**
 * @brief Selects the AES key size when loading AES keys.
 */
typedef enum
{
    /* A 128-bit key is selected. */
    CRYPTO_AES_KEYTYPE_128 = 0U,
    /* A 256-bit key is selected. */
    CRYPTO_AES_KEYTYPE_256 = 2U,
} crypto_aes_keytype_t;

/**
 * @brief Selects the AES key slot to use for AES operations.
 */
typedef enum
{
    /* The AES session key (not from key store) is selected. */
    CRYPTO_AES_KEYSEL_SESSION = 0U,
    /* Key store stot 0 is selected.  This mode is currently unsupported. */
    CRYPTO_AES_KEYSEL_SLOT0 = 1U,
    /* Key store stot 1 is selected.  This mode is currently unsupported. */
    CRYPTO_AES_KEYSEL_SLOT1 = 2U,
    /* Key store stot 2 is selected.  This mode is currently unsupported. */
    CRYPTO_AES_KEYSEL_SLOT2 = 3U,
    /* Key store stot 3 is selected.  This mode is currently unsupported. */
    CRYPTO_AES_KEYSEL_SLOT3 = 4U
} crypto_aes_key_t;

/**
 * @brief Specifier for operation type when calling AES internal functions.
 */
typedef enum
{
    /* Run an encryption operation storing all ciphertext blocks in output. */
    CRYPTO_AES_OP_ENC = 0,
    /* Run a decryption operation storing all plaintext blocks in output. */
    CRYPTO_AES_OP_DEC = 1,
    /* Run a tag generation operation storing only the last block. */
    CRYPTO_AES_OP_TAG = 2,
} crypto_aes_op_t;

#endif /* CRYPTO_AES_TYPE_H_ */
