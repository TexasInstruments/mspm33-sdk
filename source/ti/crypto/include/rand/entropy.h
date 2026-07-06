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
 *  @file       entropy.h
 *  @brief      Random module entropy source programming interface
 * 
 *  This module implements an interface to the entropy source present on
 *  certain MSPM0 devices (the TRNG module).  It targets compatibility with
 *  with the entropy source recommendations in NIST SP 800-90B.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_RAND_ENTROPY_H_
#define CRYPTO_RAND_ENTROPY_H_

#include <stdint.h>

#if CRYPTO_HW_USE_MSPM0_TRNG == 1 || CRYPTO_HW_USE_MSPM33_TRNG == 1

/**
 * @brief The entropy source status values, returned by entropy functions
 *        to indicate the status of the requested operation.
 */
typedef enum 
{
    CRYPTO_ENTROPY_TRNG_PWROFF_ERR = -6,
    CRYPTO_ENTROPY_TRNG_HEATH_FAIL = -5,
    CRYPTO_ENTROPY_TRNG_POST_ANA_FAIL = -4,
    CRYPTO_ENTROPY_TRNG_POST_DIG_FAIL = -3,
    CRYPTO_ENTROPY_TRNG_TIMEOUT_ERR = -2,
    CRYPTO_ENTROPY_TRNG_CLOCK_SEL_ERR = -1,
    CRYPTO_ENTROPY_OK = 0
} crypto_entropy_stat_t;

/* Single Call Operation API 
 * The below API provides a single function for obtaining 'n' samples
 * of entropy including the power-on, self-test, sampling, and power-down
 * sequence.
 */

/**
 *  @brief      Generate true random bits (entropy) from the on-chip TRNG
 *              entropy source and store them in an output buffer.  This
 *              single function manages the power-on of the TRNG, execution
 *              of TRNG self-tests, configuration of clocking and decimation,
 *              extraction of the specified number of 32-bit samples, and
 *              power-down of the TRNG entropy source after the operation
 *              completes.  It also manages error detection.  Handling of
 *              an error condition is left to the application.  The entropy
 *              returned should NEVER be used if the return status is not
 *              CRYPTO_ENTROPY_OK.
 *
 *  @param[out] output         Pointer to the location to store entropy
 *  @param[in]  len            Number of 32-bit words of entropy to output
 *  @param[in]  inClkFreq      The frequency of the bus clock used by the TRNG
 *
 *  @return     The status of the operation, one of crypto_entropy_stat_t.
 *  @retval     CRYPTO_ENTROPY_OK if len words of random data were
 *              successfully collected and stored in the output buffer.
 *  @retval     CRYPTO_ENTROPY_TRNG_CLOCK_SEL_ERR if the TRNG input clock
 *              frequency did not support the correct TRNG clock range.
 *  @retval     CRYPTO_ENTROPY_TRNG_TIMEOUT_ERR if a command timed out.
 *  @retval     CRYPTO_ENTROPY_TRNG_POST_DIG_FAIL if the power-on self-test
 *              of the TRNG entropy source digital logic failed during sampling.
 *  @retval     CRYPTO_ENTROPY_TRNG_POST_ANA_FAIL if the power-on self-test
 *              of the TRNG entropy source analog module failed during sampling.
 *  @retval     CRYPTO_ENTROPY_TRNG_HEATH_FAIL if a continuous heath test failed
 *              during sample collection, after digital / analog POST passed.
 *  @retval     CRYPTO_ENTROPY_TRNG_PWROFF_ERR if the TRNG could not be powered
 *              off due to being already powered off or in an invalid state.
 */
extern crypto_entropy_stat_t Crypto_getEntropy(uint32_t *output, uint32_t len, 
                                               uint32_t inClkFreq);

/* Multi Call Operation API 
 * The below API provides separate functions for obtaining entropy.
 * The calling sequence for multi-call is:
 *   1. Crypto_powerOnTRNG()
 *   2. Crypto_getTRNGData(), multiple times if desired
 *   3. Crypto_powerOffTRNG()
 */

/**
 *  @brief      Power on the TRNG.  This enables power, initializes the TRNG,
 *              configures clocking and decimation, and executes the TRNG
 *              power-on self tests.  If this function returns with
 *              CRYPTO_ENTROPY_OK, the TRNG is ready and entropy can
 *              be extracted with Crypto_getTRNGData().  Once the TRNG
 *              is no longer needed, it may be switched back off with the
 *              Crypto_powerOffTRNG() function.
 *
 *  @param[in]  inClkFreq      The frequency of the bus clock used by the TRNG
 *  @param[in]  decimation     The decimation rate to use in the TRNG.  Must
 *                             be a value between 0 and 7, with 0 being
 *                             disabled. Values larger than 7 are truncated to 
 *                             the lowest three bits.
 *
 *  @return     The status of the operation, one of crypto_entropy_stat_t.
 *
 *  @retval     CRYPTO_ENTROPY_OK if data was successfully retrieved.
 *  @retval     CRYPTO_ENTROPY_TRNG_TIMEOUT_ERR if the data
 *              wasn't ready within timeout.
 *  @retval     CRYPTO_ENTROPY_TRNG_HEATH_FAIL if a TRNG health test failed.
 */
crypto_entropy_stat_t Crypto_powerOnTRNG(uint32_t inClkFreq, uint32_t decimation);

/**
 *  @brief      Get data from the TRNG.
 *
 *              This function retrieves a 32-bit random value from the TRNG
 *              and checks for any health test failures.
 *
 *  @param[out] data           Pointer to store the retrieved random data.
 *  @param[in]  timeout        Maximum number of iterations to wait for data.
 *
 *  @return     The status of the operation, one of crypto_entropy_stat_t.
 *
 *  @retval     CRYPTO_ENTROPY_OK if data was successfully retrieved.
 *  @retval     CRYPTO_ENTROPY_TRNG_TIMEOUT_ERR if the data
 *              wasn't ready within the timeout.
 *  @retval     CRYPTO_ENTROPY_TRNG_HEATH_FAIL if a TRNG health test failed.
 */
extern crypto_entropy_stat_t Crypto_getTRNGData(uint32_t *data, \
                                                uint32_t timeout);

/**
 *  @brief      Shut down the TRNG module.
 *              This function disables the TRNG.
 *
 *  @return     The status of the operation, one of crypto_entropy_stat_t.
 *
 *  @retval     CRYPTO_ENTROPY_OK if the TRNG was powered off successfully.
 *  @retval     CRYPTO_ENTROPY_TRNG_TIMEOUT_ERR if the OFF command timed out.
 *  @retval     CRYPTO_ENTROPY_TRNG_PWROFF_ERR if the TRNG could not be powered
 *              off due to being already powered off or in an invalid state.
 */
extern crypto_entropy_stat_t Crypto_powerOffTRNG(void);

#endif /* CRYPTO_HW_USE_MSPM0_TRNG == 1 || CRYPTO_HW_USE_MSPM33_TRNG == 1 */

#endif /* CRYPTO_RAND_ENTROPY_H_ */
