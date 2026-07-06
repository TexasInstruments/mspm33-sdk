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
 *  @file       ml-dsa-type.h
 *  @brief      Module lattice digital signature algorithm (ML-DSA)
 * 
 *  This module implements the FIPS 204 standard for post-quantum resistant
 *  digital signatures (Crystals: Dilithium).
 *
 ******************************************************************************
 */

#ifndef CRYPTO_ML_DSA_TYPE_H_
#define CRYPTO_ML_DSA_TYPE_H_

/* External data type definitions */

/**
 *  @brief      The ML-DSA module status response value set.
 */
typedef enum
{
    /* Mode was not a valid mode (pure, pre-hash ) */
    CRYPTO_MLDSA_STAT_INVALID_MODE = -8,
    /* Signature length was not valid for the specified security level */
    CRYPTO_MLDSA_STAT_SIG_LEN_INVALID = -7,
    /* The randomized mode was selected but the string was < 32B */
    CRYPTO_MLDSA_STAT_RAND_TOO_SMALL = -6,
    /* The context string was provided and the context length was > 255B */
    CRYPTO_MLDSA_STAT_CTX_TOO_LONG = -5,
    /* The seed was <32B */
    CRYPTO_MLDSA_STAT_SEED_TOO_SMALL = -4,
    /* The signature provided had an invalid form and was not used */
    CRYPTO_MLDSA_STAT_INVALID_SIGNATURE_FORM = -3,
    /* The workzone buffer was too small for the security level */
    CRYPTO_MLDSA_STAT_WORKZONE_TOO_SMALL = -2,
    /* The signature verification completed and the signature IS NOT valid */
    CRYPTO_MLDSA_STAT_INVALID_SIGNATURE = -1,
    /* The operation completed successfully */
    CRYPTO_MLDSA_STAT_OK = 0,
    /* The signature verification completed and the signature IS valid */
    CRYPTO_MLDSA_STAT_VALID_SIGNATURE = 65,
} crypto_mldsa_stat_t;

/**  
 * @brief      Specifiers for pure vs. pre-hash ML-DSA mode
 */
typedef enum {
    /* Pure mode (message hashed internally) */
    CRYPTO_MLDSA_SIG_MODE_PURE = 0,
    /* Pre-hash mode (message hashed externally) */
    CRYPTO_MLDSA_SIG_MODE_PREHASH = 1,
} crypto_mldsa_mode_t;

#endif /* CRYPTO_ML_DSA_TYPE_H_ */
