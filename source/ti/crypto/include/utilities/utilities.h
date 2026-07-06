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
 *  @file       utilities.h
 *  @brief      Utility functions used by the cryptography library.
 *
 *  This module implements miscellaneous utilities.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_UTILITIES_H_
#define CRYPTO_UTILITIES_H_

#include <stddef.h>
#include <stdint.h>

/**
 *  @brief      Length of 32-bit block in 32-bit words.
 */
 #define CRYPTO_BLOCK32_LEN_WORDS (1U)

 /**
  *  @brief      Length of 32-bit block in 8-bit bytes.
  */
 #define CRYPTO_BLOCK32_LEN (CRYPTO_BLOCK32_LEN_WORDS*4U)

/**
 *  @brief      Length of 128-bit block in 32-bit words.
 */
#define CRYPTO_BLOCK128_LEN_WORDS (4U)

/**
 *  @brief      Length of 128-bit block in 8-bit bytes.
 */
#define CRYPTO_BLOCK128_LEN (CRYPTO_BLOCK128_LEN_WORDS*4U)

/**
 *  @brief      Length of 256-bit block in 32-bit words.
 */
#define CRYPTO_BLOCK256_LEN_WORDS (8U)

/**
 *  @brief      Length of 256-bit block in 8-bit bytes.
 */
#define CRYPTO_BLOCK256_LEN (CRYPTO_BLOCK256_LEN_WORDS*4U)

/**
 *  @brief      Delay value to wait for peripheral reset to propegate.
 *              In this case, we assume 8 loop iterations to be safe
 *              for the scenario where MCLK is 2X the ULPCLK.  The TRM
 *              specifies a delay of at least 4 ULPCLK cycles after
 *              enabling a peripheral before accessing its registers.
 */
#define CRYPTO_PMCU_DELAY (8U)

/**
 *  @brief      Data structure for enabling aligned safe byte/word access to a
 *              32-bit data chunk, as is commonly used with DRBG functions.
 */
typedef union {
    /** @brief  32-bit word aligned data */
    uint32_t w[CRYPTO_BLOCK32_LEN_WORDS];
    /** @brief  8-bit byte aligned data */
    uint8_t b[CRYPTO_BLOCK32_LEN];
} crypto_block32_t;

/**
 *  @brief      Data structure for enabling aligned safe byte/word access to a
 *              128-bit data chunk, as is commonly used with block ciphers.
 */
typedef union {
    /** @brief  32-bit word aligned data */
    uint32_t w[CRYPTO_BLOCK128_LEN_WORDS];
    /** @brief  8-bit byte aligned data */
    uint8_t b[CRYPTO_BLOCK128_LEN];
} crypto_block128_t;

/**
 *  @brief      Data structure for enabling aligned safe byte/word access to a
 *              256-bit data chunk, as is commonly used with block ciphers.
 */
 typedef union {
    /** @brief  32-bit word aligned data */
    uint32_t w[CRYPTO_BLOCK256_LEN_WORDS];
    /** @brief  8-bit byte aligned data */
    uint8_t b[CRYPTO_BLOCK256_LEN];
} crypto_block256_t;

/**
 *  @brief      Copy a specified number of 8-bit bytes from src to dest.
 *
 *  @param[in]  src            Pointer to the source.
 *  @param[out] dst            Pointer to the destination.
 *  @param[in]  byteCnt        The number of bytes to copy.
 *
 *  @return     None.
 */
extern void Crypto_copy8(const uint8_t *src, uint8_t *dst, uint32_t byteCnt);

/**
 *  @brief      Copy a specified number of 32-bit words from src to dest.
 *
 *  @param[in]  src            Pointer to the 32-bit word aligned source.
 *  @param[out] dst            Pointer to the 32-bit word aligned destination.
 *  @param[in]  wordCnt        The number of words to copy.
 *
 *  @return     None.
 */
extern void Crypto_copy32(const uint32_t *src, uint32_t *dst, uint32_t wordCnt);

/**
 *  @brief      Set a specified number of 8-bit byte in dest to a value.
 *
 *  @param[out] dst            Pointer to the destination.
 *  @param[in]  val            The 8-bit byte to set.
 *  @param[in]  byteCnt        The number of words to set.
 *
 *  @return     None.
 */
extern void Crypto_set8(uint8_t *dst, uint8_t val, uint32_t byteCnt);

/**
 *  @brief      Set a specified number of 32-bit words in dest to a value.
 *
 *  @param[out] dst            Pointer to the 32-bit word aligned destination.
 *  @param[in]  val            The 32-bit value to set.
 *  @param[in]  wordCnt        The number of words to set.
 *
 *  @return     None.
 */
extern void Crypto_set32(uint32_t *dst, uint32_t val, uint32_t wordCnt);

/**
 *  @brief      Bit-wise XOR a specified number of 32-bit words in dst with src.
 *              This operation takes the XOR of word strings 'x' and 'y' and
 *              stores the result in 'x'.
 *
 *  @param[out] x              Input 'x' and destiation for the operation.
 *  @param[in]  y              Input 'y', not modified by the operation.
 *  @param[in]  wordCnt        The number of words to copy.
 *
 *  @return     None.
 */
extern void Crypto_xor32(uint32_t *x, const uint32_t *y, uint32_t wordCnt);

/**
 *  @brief      Reverse a byte endianness of 32-bit balue.
 *
 *  @param[in]  x              Input value
 *
 *  @return     Output (byte reversed) value
 */
extern uint32_t Crypto_reverseBytes32(uint32_t x);

/**
 *  @brief      Reverse the byte order of a source array and store output
 *              in a new array.
 *
 *  @param[in]  src            Pointer to the source.
 *  @param[out] dst            Pointer to the destination.
 *  @param[in]  len            The number of bytes to reverse.
 *
 *  @return     Little endian representation of x.
 */
extern void Crypto_copyReverse8(const uint8_t *src, uint8_t *dst, uint32_t len);

/**
 *  @brief      Delay the processor in a while loop for
 *              a specified number of loop iterations.
 *
 *  @param[in]  iterations     Number of while() loop iterations to delay for.
 *
 *  @return     None.
 */
extern void Crypto_delay(volatile uint32_t iterations);

/**
 *  @brief      Compare two byte arrays in constant time to prevent 
 *              timing attacks.
 *
 *  This function performs a byte-by-byte comparison of two arrays in a way that
 *  takes the same amount of time regardless of where the first difference occurs.
 *  This helps prevent timing-based side-channel attacks when comparing 
 *  sensitive data.
 *
 *  @param[in]  x              Pointer to the first byte array.
 *  @param[in]  y              Pointer to the second byte array.
 *  @param[in]  len            The number of bytes to compare.
 *
 *  @return     Returns 0 if the arrays are identical, non-zero otherwise.
 *              Note: Unlike standard memcmp, this doesn't return -1, 0, or 1
 *              to indicate ordering.
 */
extern uint8_t Crypto_memcmp(const uint8_t *x, const uint8_t *y, const uint32_t len);

/**
 *  @brief      Constant-time comparison of two 32-bit word-aligned buffers
 *
 *  @param[in]  buffer0           Pointer to first buffer (must be 4-byte aligned)
 *  @param[in]  buffer1           Pointer to second buffer (must be 4-byte aligned)
 *  @param[in]  bufferByteLength  Length of buffers in bytes (must be multiple of 4)
 *
 *  @return     1 if buffers match and are properly aligned, 0 otherwise
 */
extern uint32_t Crypto_memcmp32(
    const volatile uint32_t* volatile buffer0,
    const volatile uint32_t* volatile buffer1, uint32_t bufferByteLength);

/**
 *  @brief      Copies source to destination in reverse word order with byte-swapping
 *              within each word, and zero-pads the final word if needed. This utility
 *              supports data formatting for PKA (Public Key Accelerator) hardware.
 *
 *  @param[in]  source        Pointer to source buffer
 *  @param[out] destination   Pointer to destination buffer (must have space for
 *                            ceiling(sourceLength/4) 32-bit words)
 *  @param[in]  sourceLength  Length of source buffer in bytes
 *
 *  @return     None
 */
extern void Crypto_copyReversePad(
    const void* source, uint32_t* destination, uint32_t sourceLength);

/**
 * \brief       Reverses the byte order of a buffer in place
 *
 * @param[inout] buffer            Pointer to the buffer to reverse
 * @param[in]    bufferByteLength  Length of the buffer in bytes
 * 
 * @return       None
 */
extern void Crypto_reverseInPlaceBytewise(void* buffer, size_t bufferByteLength);

#endif /* CRYPTO_UTILITIES_H_ */
