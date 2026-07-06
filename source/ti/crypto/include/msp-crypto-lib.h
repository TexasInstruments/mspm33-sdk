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
 *  @file       msp-crypto-lib.h
 *  @brief      TI cryptography library for MSP microcontroller products
 *
 *  This library implements cryptographic modules for use on TI
 *  microcontroller products including the TI MSPM0, MSPM33 family of MCUs.
 *
 *  Hardware acceleration is utilized where applicable, else software
 *  based implementations are used.
 *
 *  The following modules are provided by this package:
 *    - Ciphers (inc. AES)
 *    - Hashes (inc. SHA)
 *    - Elliptic curve (EC) functions
 *    - Post quantum (PQC) functions
 *    - Random number generation functions (rand)
 *    - General utilities for string manipulation (utilities)
 *
 *  This library package is delivered pre-built and is expected to be used
 *  without any re-compilation.  Sources are provided for certain modules
 *  to give transparency on the underlying implementations.
 *
 ******************************************************************************
 */

#ifndef MSP_CRYPTO_LIB_H_
#define MSP_CRYPTO_LIB_H_

#include "ti/crypto/include/cipher/aes.h"
#include "ti/crypto/include/curve25519/curve25519.h"
#include "ti/crypto/include/ec/ec.h"
#include "ti/crypto/include/hash/sha.h"
#include "ti/crypto/include/pqc/ml-dsa.h"
#include "ti/crypto/include/pqc/ml-kem.h"
#include "ti/crypto/include/rand/drbg.h"
#include "ti/crypto/include/rand/entropy.h"
#include "ti/crypto/include/rsa/rsa.h"
#include "ti/crypto/include/utilities/utilities.h"

#endif /* MSP_CRYPTO_LIB_H_ */
