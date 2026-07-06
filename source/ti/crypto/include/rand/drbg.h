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
 *  @file       drbg.h
 *  @brief      Lightweight random module deterministic random bit generator
 *              programming interface based on a minimal feature set CTR DRBG.
 * 
 *  This module implements a deterministic random bit generator (DRBG) for use
 *  in generating cryptographically secure pseudorandom numbers based on
 *  entropy seed data for use in cryptographic operations including key
 *  generation, initialization vectors, and more.
 *
 *  This module targets compatibility with with the DRBG recommedations
 *  in NIST SP 800-90A, with certain features excluded from the implementation
 *  to maintain a compact implementation for embedded use cases.
 *
 *  Specifically, this module implements the following DRBG configuration:
 *    - Type: CTR-DRBG
 *    - Cipher: AES-256
 *    - Derivation function (DF) used: Yes
 *    - Block size: 128 bits
 *    - Key size: 256 bits
 *    - Seed size: 384 bits
 *    - Security strength level: 256 bits
 *    - No support for context strings or additional data strings
 *    - Maximum number of requests before reseed is limited to 2^32-1 requests
 *    - Maximum number of data per request is limited to 16KiB
 *    - Reseeding is not supported.  If a reseed is required, un-init and
 *      re-init the instance with fresh entropy and nonce.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_RAND_DRBG_H_
#define CRYPTO_RAND_DRBG_H_

#include <stdint.h>
#include <stdbool.h>
#include "ti/crypto/include/utilities/utilities.h"

/**
 * @brief The CTR DRBG maximum requests per reseed; this is artificially
 *        capped at 2^32 requests for efficiency and practicality, as most
 *        embedded applications will not request more than 2^32 values.
 *        NIST SP 800-90A allows for up to 2^48 with CTR-DRBG based on AES,
 *        so this implementation is a subset of this.
 */
#define CRYPTO_DRBG_CTR_RESEED_INTERVAL (UINT32_MAX)

/**
 * @brief The CTR DRBG maximum number 32-bit words per request, calculated as:
 *        2^19 bits DIV 8b/1B DIV 4B/1W = 16,384 32-bit words per request.
 */
#define CRYPTO_DRBG_CTR_MAX_WORDS_PER_REQ (16384U)

/**
 * @brief The DRBG status values, returned by DRBG functions
 *        to indicate the status of the requested operation.
 */
typedef enum 
{
    /** An error with the HW AES module occured */
    CRYPTO_DRBG_AES_ERR = -4,
    /** A reseed (re-init) is required before more values can be generated */
    CRYPTO_DRBG_RESEED_REQUIRED = -3,
    /** Generated bits were requested but the context was not initialized */
    CRYPTO_DRBG_NOINIT_ERR = -2,
    /** There was an error with the parameters passed 
    * to the function */
    CRYPTO_DRBG_PARAM_ERR = -1,
    /* The function completed successfully */
    CRYPTO_DRBG_OK = 0
} crypto_drbg_stat_t;

/**
 * @brief The context structure for a DRBG instantiation.
 */
typedef struct
{
    /** Stores the current state AES key 'k', 256 bits */
    crypto_block256_t k;
    /** Stores the current state AES block 'v', 128 bits */
    crypto_block128_t v;
    /** Stores the current count value since seed / reseed */
    uint32_t reseedCounter;
    /** Stores a magic value once initialized */
    uint32_t initialized;
} crypto_drbg_ctx_t;

/**
 *  @brief      Initialize a counter DRBG instance.
 *              An entropy string of 256 bits and a nonce string of 128 bits
 *              must be provided to this routine for initialization.
 *              This routine implements NIST SP 800-90A CTR-DRBG with
 *              AES-256, DF, and no personalization string.
 *
 *  @param[out] ctx            Pointer to the context structure for the
 *                             instance to initialize.
 *  @param[in]  entropy        Pointer to buffer containing entropy input.
 *                             Must be 32-bit word aligned and contain
 *                             8 words (256 bits) of entropy.
 *  @param[in]  nonce          Pointer to buffer containing nonce input.
 *                             Must be 32-bit word aligned and contain
 *                             4 words (128 bits) of nonce content.
 *
 *  @return     The status of the operation, one of crypto_drbg_stat_t.
 *  @retval     CRYPTO_DRBG_OK if the DRBG was successfully initialized.
 *  @retval     CRYPTO_DRBG_PARAM_ERR if an input parameter/pointer is invalid.
 *  @retval     CRYPTO_DRBG_AES_ERR if the AES module encountered an error.
 */
extern crypto_drbg_stat_t Crypto_DRBG_initialize(crypto_drbg_ctx_t *ctx,
                                                 const uint32_t *entropy,
                                                 const uint32_t *nonce);

/**
 *  @brief      Generate random values from a counter DRBG instance.
 *              The DRBG instance must have been previously initialized.
 *
 *  @param[out] ctx            Pointer to the context structure for the
 *                             instance to generate from.
 *  @param[in]  output         Pointer to location to store the generated
 *                             random values into.
 *  @param[in]  outputLen      Specifies the number of 32-bit words of 
 *                             random data to generate.  The memory
 *                             pointed to by output must have pre-allocated
 *                             at least outputLen space.
 *
 *  @return     The status of the operation, one of crypto_drbg_stat_t.
 *  @retval     CRYPTO_DRBG_OK if the requested number of values was generated.
 *  @retval     CRYPTO_DRBG_PARAM_ERR if an input parameter/pointer is invalid.
 *  @retval     CRYPTO_DRBG_NOINIT_ERR if the context was not initialized.
 *  @retval     CRYPTO_DRBG_RESEED_REQUIRED if a reseed is required.
 *  @retval     CRYPTO_DRBG_AES_ERR if the AES module encountered an error.
 */
extern crypto_drbg_stat_t Crypto_DRBG_generate(crypto_drbg_ctx_t *ctx,
                                               uint32_t *output,
                                               uint32_t outputLen);

/**
 *  @brief      Uninitialize a counter DRBG instance.  The state context is
 *              cleared from memory and the initialization tag is removed.
 *
 *  @param[out] ctx            Pointer to the context structure for the
 *                             instance to uninitialize.
 *
 *  @return     The status of the operation, one of crypto_drbg_stat_t.
 *  @retval     CRYPTO_DRBG_OK if the DRBG was successfully uninitialized.
 *  @retval     CRYPTO_DRBG_PARAM_ERR if the DRBG context pointer is invalid.
 *  @retval     CRYPTO_DRBG_NOINIT_ERR if the context was not initialized.
 */
extern crypto_drbg_stat_t Crypto_DRBG_uninit(crypto_drbg_ctx_t *ctx);              
                                     
#endif /* CRYPTO_RAND_DRBG_H_ */
