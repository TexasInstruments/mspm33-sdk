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
 *  @file       ec_curves.h
 *  @brief      Elliptic curve definitions for use with the ec module.
 *
 ******************************************************************************
 */

#include <stdint.h>

#ifndef CRYPTO_EC_CURVES_H_
#define CRYPTO_EC_CURVES_H_

/* External data type definitions */

/**
 *  @brief      Data structure which holds pointers to 
 *              elliptic curve parameters.
 */
typedef struct
{
    /** Pointer to the prime modulus p */
    const uint32_t *data_p;
    /** Pointer to the r-value */
    const uint32_t *data_r;
    /** Pointer to a coefficient */
    const uint32_t *data_a;
    /** Pointer to b coefficient */
    const uint32_t *data_b;
    /** Pointer to the x-coordinate of the base point */
    const uint32_t *data_Gx;
    /** Pointer to the y-coordinate of the base point */
    const uint32_t *data_Gy;
    /** The length of the data in bytes */
    uint8_t bufLen;
} crypto_ec_curve_t;

/* External global variable definitions */

/**
 *  @brief      Structure holding parameters for NIST P-256 (secp256r1) curve
 */
extern const crypto_ec_curve_t Crypto_EC_Curve_NIST256;

/**
 *  @brief      Structure holding parameters for NIST P-384 (secp384r1) curve
 */
extern const crypto_ec_curve_t Crypto_EC_Curve_NIST384;

/**
 *  @brief      Structure holding parameters for NIST P-521 (secp521r1) curve
 */
extern const crypto_ec_curve_t Crypto_EC_Curve_NIST521;

#endif /* CRYPTO_EC_CURVES_H_ */
