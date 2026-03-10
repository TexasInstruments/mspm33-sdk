/*
 * Copyright (c) 2025, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       dl_pka.h
 *  @brief      PKA Driver Library
 *  @defgroup   PKA
 *
 *  @anchor ti_dl_dl_pka_Overview
 *  # Overview
 *
 *  The PKA Driver Library allows full configuration of the MSPM33 PKA
 *  (Public Key Accelerator) module.
 *  The Public Key Accelerator (PKA) peripheral provides hardware acceleration
 *  for cryptographic operations including big number arithmetic, elliptic curve
 *  cryptography (ECC), and modular exponentiation. It supports multiple standard
 *  curves including NIST P-curves (P-224, P-256, P-384, P-521), Brainpool curves
 *  (P-256R1, P-384R1, P-512R1), and Curve25519 for efficient public key operations.
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup PKA
 * @{
 */

/**
 *
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 *
 */
#ifndef ti_dl_dl_pka__include
#define ti_dl_dl_pka__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

#define PKA_RAM_BASE (PKA_BASE + 0x00016000U)
#define PKA_RAM_TOT_BYTE_SIZE 2048U

typedef enum {
    DL_PKA_STATUS_SUCCESS = 0xCEF6A572U,
    DL_PKA_STATUS_FAILURE = 0xD20341DDU,
    DL_PKA_STATUS_SUCCESS_ZERO_RES =
        0x23810F9AU,  //!< Zero result success/pass return cod
    DL_PKA_STATUS_INVALID_PARAM            = 0xCE87496EU,
    DL_PKA_STATUS_BUF_UNDERFLOW            = 0xFF245881U,
    DL_PKA_STATUS_RESULT_0                 = 0x278AB559U,
    DL_PKA_STATUS_A_GREATER_THAN_B         = 0xFED219EFU,
    DL_PKA_STATUS_A_LESS_THAN_B            = 0xEFC02492U,
    DL_PKA_STATUS_EQUAL                    = 0x4A8038E3U,
    DL_PKA_STATUS_OPERATION_BUSY           = 0xDB277701U,
    DL_PKA_STATUS_OPERATION_RDY            = 0xFEB12399U,
    DL_PKA_STATUS_LOCATION_IN_USE          = 0x9B4C7D2EU,
    DL_PKA_STATUS_X_ZERO                   = 0xE5F31A8BU,
    DL_PKA_STATUS_Y_ZERO                   = 0x7D2B8C4AU,
    DL_PKA_STATUS_X_LARGER_THAN_PRIME      = 0x130EF272U,
    DL_PKA_STATUS_Y_LARGER_THAN_PRIME      = 0xFD3A93E5U,
    DL_PKA_STATUS_POINT_NOT_ON_CURVE       = 0x021CCF0FU,
    DL_PKA_STATUS_RESULT_ADDRESS_INCORRECT = 0x8819019FU,
    DL_PKA_STATUS_POINT_AT_INFINITY        = 0xA598DEC1U
} DL_PKA_Status;

typedef enum {
    DL_PKA_O_APTR_OFFSET     = 0x00000000U,
    DL_PKA_O_BPTR_OFFSET     = 0x00000004U,
    DL_PKA_O_CPTR_OFFSET     = 0x00000008U,
    DL_PKA_O_DPTR_OFFSET     = 0x0000000CU,
    DL_PKA_O_ALENGTH_OFFSET  = 0x00000010U,
    DL_PKA_O_BLENGTH_OFFSET  = 0x00000014U,
    DL_PKA_O_SHIFT_OFFSET    = 0x00000018U,
    DL_PKA_O_FUNCTION_OFFSET = 0x0000001CU,
    DL_PKA_O_COMPARE_OFFSET  = 0x00000020U,
    DL_PKA_O_MSW_OFFSET      = 0x00000024U,
    DL_PKA_O_DIVMSW_OFFSET   = 0x00000028U,
    DL_PKA_O_SEQCTRL_OFFSET  = 0x000000C8U,
    DL_PKA_O_OPTIONS_OFFSET  = 0x000000F4U,
    DL_PKA_O_FWREV_OFFSET    = 0x000000F8U,
    DL_PKA_O_HWREV_OFFSET    = 0x000000FCU
} DL_PKA_Offsets;

/**
 *
 * Length in bytes of NISTP224 parameters.
 *
 */
#define DL_PKA_NISTP224_PARAM_SIZE_BYTES 28

/**
 *
 * Length in bytes of NISTP256 parameters.
 *
 */
#define DL_PKA_NISTP256_PARAM_SIZE_BYTES 32

/**
 *
 * Length in bytes of NISTP384 parameters.
 *
 */
#define DL_PKA_NISTP384_PARAM_SIZE_BYTES 48

/**
 *
 * Length in bytes of DL_PKA_NISTP521 parameters.
 *
 */
#define DL_PKA_NISTP521_PARAM_SIZE_BYTES 66

/**
 *
 * Length in bytes of BrainpoolP256R1 parameters.
 *
 */
#define DL_BrainpoolP256R1_PARAM_SIZE_BYTES 32

/**
 *
 * Length in bytes of BrainpoolP384R1 parameters.
 *
 */
#define DL_BrainpoolP384R1_PARAM_SIZE_BYTES 48

/**
 *
 * Length in bytes of BrainpoolP512R1 parameters.
 *
 */
#define DL_BrainpoolP512R1_PARAM_SIZE_BYTES 64

/**
 *
 * Length in bytes of Curve25519 parameters.
 *
 */
#define DL_Curve25519_PARAM_SIZE_BYTES 32

/**
 * @brief Union for NISTP224 curve parameters with 32-bit alignment
 *
 * This union provides both byte and word access to NISTP224 curve parameters,
 * ensuring 32-bit alignment.
 */
typedef union {
    uint8_t byte[DL_PKA_NISTP224_PARAM_SIZE_BYTES]; /*!< Byte array access */
    uint32_t word[DL_PKA_NISTP224_PARAM_SIZE_BYTES /
                  sizeof(uint32_t)]; /*!< Word array access */
} DL_PKA_EccParam224;

/**
 * @brief Union for NISTP256 curve parameters with 32-bit alignment
 *
 * This union provides both byte and word access to NISTP256 curve parameters,
 * ensuring 32-bit alignment.
 */
typedef union {
    uint8_t byte[DL_PKA_NISTP256_PARAM_SIZE_BYTES]; /*!< Byte array access */
    uint32_t word[DL_PKA_NISTP256_PARAM_SIZE_BYTES /
                  sizeof(uint32_t)]; /*!< Word array access */
} DL_PKA_EccParam256;

/**
 * @brief Union for NISTP384 curve parameters with 32-bit alignment
 *
 * This union provides both byte and word access to NISTP384 curve parameters,
 * ensuring 32-bit alignment.
 */
typedef union {
    uint8_t byte[DL_PKA_NISTP384_PARAM_SIZE_BYTES]; /*!< Byte array access */
    uint32_t word[DL_PKA_NISTP384_PARAM_SIZE_BYTES /
                  sizeof(uint32_t)]; /*!< Word array access */
} DL_PKA_EccParam384;

/**
 * @brief Union for BrainpoolP512R1 curve parameters with 32-bit alignment
 *
 * This union provides both byte and word access to BrainpoolP512R1 curve parameters,
 * ensuring 32-bit alignment.
 */
typedef union {
    uint8_t
        byte[DL_BrainpoolP512R1_PARAM_SIZE_BYTES]; /*!< Byte array access */
    uint32_t word[DL_BrainpoolP512R1_PARAM_SIZE_BYTES /
                  sizeof(uint32_t)]; /*!< Word array access */
} DL_PKA_EccParam512;

/**
 * @brief Union for NISTP521 curve parameters with 32-bit alignment
 *
 * This union provides both byte and word access to NISTP521 curve parameters,
 * ensuring 32-bit alignment.
 */
typedef union {
    uint8_t byte[DL_PKA_NISTP521_PARAM_SIZE_BYTES]; /*!< Byte array access */
    uint32_t word[DL_PKA_NISTP521_PARAM_SIZE_BYTES /
                  sizeof(uint32_t)]; /*!< Word array access */
} DL_PKA_EccParam521;

/**
 *
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 *
 */

/**
 * @brief Structure representing a point on the NISTP224 elliptic curve
 *
 * This structure contains the X and Y coordinates of a point on the NISTP224 curve.
 */
typedef struct {
    DL_PKA_EccParam224 x; /*!< X coordinate of the curve point */
    DL_PKA_EccParam224 y; /*!< Y coordinate of the curve point */
} DL_PKA_EccPoint224;

/**
 * @brief Structure representing a point on the NISTP256 elliptic curve
 *
 * This structure contains the X and Y coordinates of a point on the NISTP256 curve.
 */
typedef struct {
    DL_PKA_EccParam256 x; /*!< X coordinate of the curve point */
    DL_PKA_EccParam256 y; /*!< Y coordinate of the curve point */
} DL_PKA_EccPoint256;

/**
 * @brief Structure representing a point on the NISTP384 elliptic curve
 *
 * This structure contains the X and Y coordinates of a point on the NISTP384 curve.
 */
typedef struct {
    DL_PKA_EccParam384 x; /*!< X coordinate of the curve point */
    DL_PKA_EccParam384 y; /*!< Y coordinate of the curve point */
} DL_PKA_EccPoint384;

/**
 * @brief Structure representing a point on the BrainpoolP512R1 elliptic curve
 *
 * This structure contains the X and Y coordinates of a point on the BrainpoolP512R1 curve.
 */
typedef struct {
    DL_PKA_EccParam512 x; /*!< X coordinate of the curve point */
    DL_PKA_EccParam512 y; /*!< Y coordinate of the curve point */
} DL_PKA_EccPoint512;

/**
 * @brief Structure representing a point on the NISTP521 elliptic curve
 *
 * This structure contains the X and Y coordinates of a point on the NISTP521 curve.
 */
typedef struct {
    DL_PKA_EccParam521 x; /*!< X coordinate of the curve point */
    DL_PKA_EccParam521 y; /*!< Y coordinate of the curve point */
} DL_PKA_EccPoint521;

/**
 *
 * X coordinate of the generator point of the NISTP224 curve.
 *
 */
extern const DL_PKA_EccPoint224 NISTP224_generator;

/**
 *
 * Prime of the NISTP224 curve.
 *
 */
extern const DL_PKA_EccParam224 NISTP224_prime;

/**
 *
 * a constant of the NISTP224 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam224 NISTP224_a;

/**
 *
 * b constant of the NISTP224 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam224 NISTP224_b;

/**
 *
 * Order of the NISTP224 curve.
 *
 */
extern const DL_PKA_EccParam224 NISTP224_order;

/**
 *
 * X coordinate of the generator point of the NISTP256 curve.
 *
 */
extern const DL_PKA_EccPoint256 NISTP256_generator;

/**
 *
 * Prime of the NISTP256 curve.
 *
 */
extern const DL_PKA_EccParam256 NISTP256_prime;

/**
 *
 * a constant of the NISTP256 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam256 NISTP256_a;

/**
 *
 * b constant of the NISTP256 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam256 NISTP256_b;

/**
 *
 * Order of the NISTP256 curve.
 *
 */
extern const DL_PKA_EccParam256 NISTP256_order;

/**
 *
 * X coordinate of the generator point of the NISTP384 curve.
 *
 */
extern const DL_PKA_EccParam256 NISTP384_generator;

/**
 *
 * @brief Prime of the NISTP384 curve.
 *
 */
extern const DL_PKA_EccParam384 NISTP384_prime;

/**
 *
 * @brief a constant of the NISTP384 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam384 NISTP384_a;

/**
 *
 * @brief b constant of the NISTP384 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam384 NISTP384_b;

/**
 *
 * @brief Order of the NISTP384 curve.
 *
 */
extern const DL_PKA_EccParam384 NISTP384_order;

/**
 *
 * @brief X coordinate of the generator point of the NISTP521 curve.
 *
 */
extern const DL_PKA_EccParam256 NISTP521_generator;

/**
 *
 * @brief Prime of the NISTP521 curve.
 *
 */
extern const DL_PKA_EccParam521 NISTP521_prime;

/**
 *
 * @brief a constant of the NISTP521 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam521 NISTP521_a;

/**
 *
 * @brief b constant of the NISTP521 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam521 NISTP521_b;

/**
 *
 * @brief Order of the NISTP521 curve.
 *
 */
extern const DL_PKA_EccParam521 NISTP521_order;

/**
 *
 * @brief X coordinate of the generator point of the BrainpoolP256R1 curve.
 *
 */
extern const DL_PKA_EccPoint256 BrainpoolP256R1_generator;

/**
 *
 * @brief Prime of the BrainpoolP256R1 curve.
 *
 */
extern const DL_PKA_EccParam256 BrainpoolP256R1_prime;

/**
 *
 * @brief a constant of the BrainpoolP256R1 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam256 BrainpoolP256R1_a;

/**
 *
 * @brief b constant of the BrainpoolP256R1 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam256 BrainpoolP256R1_b;

/**
 *
 * @brief Order of the BrainpoolP256R1 curve.
 *
 */
extern const DL_PKA_EccParam256 BrainpoolP256R1_order;

/**
 *
 * @brief X coordinate of the generator point of the BrainpoolP384R1 curve.
 *
 */
extern const DL_PKA_EccParam256 BrainpoolP384R1_generator;

/**
 *
 * @brief Prime of the BrainpoolP384R1 curve.
 *
 */
extern const DL_PKA_EccParam384 BrainpoolP384R1_prime;

/**
 *
 * @brief a constant of the BrainpoolP384R1 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam384 BrainpoolP384R1_a;

/**
 *
 * @brief b constant of the BrainpoolP384R1 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam384 BrainpoolP384R1_b;

/**
 *
 * @brief Order of the BrainpoolP384R1 curve.
 *
 */
extern const DL_PKA_EccParam384 BrainpoolP384R1_order;

/**
 *
 * @brief X coordinate of the generator point of the BrainpoolP512R1 curve.
 *
 */
extern const DL_PKA_EccParam256 BrainpoolP512R1_generator;

/**
 *
 * @brief Prime of the BrainpoolP512R1 curve.
 *
 */
extern const DL_PKA_EccParam512 BrainpoolP512R1_prime;

/**
 *
 * @brief a constant of the BrainpoolP512R1 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam512 BrainpoolP512R1_a;

/**
 *
 * @brief b constant of the BrainpoolP512R1 curve when expressed in short
 * Weierstrass form (y^2 = x^3 + a*x + b).
 *
 */
extern const DL_PKA_EccParam512 BrainpoolP512R1_b;

/**
 *
 * @brief Order of the BrainpoolP512R1 curve.
 *
 */
extern const DL_PKA_EccParam512 BrainpoolP512R1_order;

/**
 *
 * @brief X coordinate of the generator point of the Curve25519 curve.
 *
 */
extern const DL_PKA_EccPoint256 Curve25519_generator;

/**
 *
 * @brief Prime of the Curve25519 curve.
 *
 */
extern const DL_PKA_EccParam256 Curve25519_prime;

/**
 *
 * @brief a constant of the Curve25519 curve when expressed in Montgomery
 * form (By^2 = x^3 + a*x^2 + x).
 *
 */
extern const DL_PKA_EccParam256 Curve25519_a;

/**
 *
 * @brief b constant of the Curve25519 curve when expressed in Montgomery
 * form (By^2 = x^3 + a*x^2 + x).
 *
 */
extern const DL_PKA_EccParam256 Curve25519_b;

/**
 *
 * @brief Order of the Curve25519 curve.
 *
 */
extern const DL_PKA_EccParam256 Curve25519_order;

/**
 *
 * @brief Enable PKA IP via sysctrl register
 *
 * @param pka Pointer to the PKA register structure base address
 *            Structure is of type PKA_Regs defined in hw_pka.h
 *
 * @note The caller must ensure that a valid PKA register structure pointer is passed.
 *       Accessing an invalid address will result in undefined behavior.
 *
 * Example usage:
 * @code
 *     PKA_Regs *pka = (PKA_Regs *)PKA_BASE;
 *     PKAEnable(pka);
 * @endcode
 *
 * @return None
 *
 */
extern void DL_PKA_enable(PKA_Regs* pka);

/**
*
* @brief Clear PKA Data RAM
*
* @param pka Pointer to the PKA register structure base address
*
*/
extern void DL_PKAClearDataRam(PKA_Regs* pka);

/**
*
* @brief Gets the PKA operation status.
*
* This function gets information on whether any PKA operation is in
* progress or not. This function allows to check the PKA operation status
* before starting any new PKA operation.
*
* @param pka Pointer to the PKA register structure base address
*
* @return Returns a status code.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA operation is in progress.
* - @ref PKA_STATUS_OPERATION_RDY if the PKA operation is not in progress.
*
*/
extern DL_PKA_Status DL_PKAGetOpsStatus(PKA_Regs* pka);

/**
*
* @brief Checks whether and array only consists of zeros
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] array is the array to check.
*
* @param [in] arrayLength is the length of the array.
*
* @return Returns true if the array contains only zeros and false if one
* or more bits are set.
*
*/
extern uint8_t DL_PKAArrayAllZeros(
    PKA_Regs* pka, const uint8_t* array, uint32_t arrayLength);

/**
*
* @brief Zeros-out an array
*
* @param [in] array is the array to zero-out.
*
* @param [in] arrayLength is the length of the array.
*
*/
extern void DL_PKAZeroOutArray(const uint8_t* array, uint32_t arrayLength);

/**
*
* @brief Starts a big number modulus operation.
*
* This function starts the modulo operation on the big number @c bigNum
* using the divisor @c modulus. The PKA RAM location where the result
* will be available is stored in @c resultPKAMemAddr.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] bigNum is the pointer to the big number on which modulo operation
*        needs to be carried out.
*
* @param [in] bigNumLength is the size of the big number @c bigNum in bytes.
*
* @param [in] modulus is the pointer to the divisor.
*
* @param [in] modulusLength is the size of the divisor @c modulus in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY, if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumModGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumModStart(PKA_Regs* pka, const uint8_t* bigNum,
    uint32_t bigNumLength, const uint8_t* modulus, uint32_t modulusLength,
    uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the big number modulus operation.
*
* This function gets the result of the big number modulus operation which was
* previously started using the function DL_PKABigNumModStart().
* The function will zero-out @c resultBuf prior to copying in the result of
* the modulo operation.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result needs to
*        be stored.
*
* @param [in] length is the size of the provided buffer in bytes.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumModStart().
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the @c length is less than the length
*        of the result.
*
* @sa DL_PKABigNumModStart()
*
*/
extern DL_PKA_Status DL_PKABigNumModGetResult(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t length, uint32_t resultPKAMemAddr);

/**
*
* @brief Starts a big number divide operation.
*
* This function starts the dive operation on the big number @c bigNum
* using the @c divisor. The PKA RAM location where the result
* will be available is stored in @c resultPKAMemAddr.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] dividend is the pointer to the big number to be divided.
*
* @param [in] dividendLength is the size of the big number @c dividend in bytes.
*
* @param [in] divisor is the pointer to the divisor.
*
* @param [in] divisorLength is the size of the @c divisor in bytes.
*
* @param [out] resultQuotientMemAddr is the pointer to the quotient vector location
*        which will be set by this function.
*
* @param [out] resultRemainderMemAddr is the pointer to the remainder vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY, if the PKA module is busy doing
*        some other operation.
*
* @sa PKABigNumDivideGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumDivideStart(PKA_Regs* pka,
    const uint8_t* dividend, uint32_t dividendLength, const uint8_t* divisor,
    uint32_t divisorLength, uint32_t* resultQuotientMemAddr,
    uint32_t* resultRemainderMemAddr);

/**
*
* @brief Gets the quotient of the big number divide operation.
*
* This function gets the quotient of the big number divide operation which was
* previously started using the function DL_PKABigNumDivideStart().
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result needs to
*        be stored.
*
* @param [in] length is the size of the provided buffer in bytes.
*
* @param [in] resultQuotientMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumDivideStart().
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the @c length is less than the length
*        of the result.
*
* @sa DL_PKABigNumDivideStart()
*
*/
extern DL_PKA_Status DL_PKABigNumDivideGetQuotient(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t* length, uint32_t resultQuotientMemAddr);

/**
*
* @brief Gets the remainder of the big number divide operation.
*
* This function gets the remainder of the big number divide operation which was
* previously started using the function DL_PKABigNumDivideStart().
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result needs to
*        be stored.
*
* @param [in] length is the size of the provided buffer in bytes.
*
* @param [in] resultRemainderMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumDivideStart().
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the @c length is less than the length
*        of the result.
*
* @sa DL_PKABigNumDivideStart()
*
*/
extern DL_PKA_Status DL_PKABigNumDivideGetRemainder(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t* length, uint32_t resultRemainderMemAddr);

/**
*
* @brief Starts the comparison of two big numbers.
*
* This function starts the comparison of two big numbers pointed by
* @c bigNum1 and @c bigNum2.
*
* @param pka Pointer to the PKA register structure base address
*
* @note @c bigNum1 and @c bigNum2 must have same size.
*
* @param [in] bigNum1 is the pointer to the first big number.
*
* @param [in] bigNum2 is the pointer to the second big number.
*
* @param [in] length is the size of the big numbers in bytes.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumCmpGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumCmpStart(PKA_Regs* pka,
    const uint8_t* bigNum1, const uint8_t* bigNum2, uint32_t length);

/**
*
* @brief Gets the result of the comparison operation of two big numbers.
*
* This function provides the results of the comparison of two big numbers
* which was started using the DL_PKABigNumCmpStart().
*
* @param pka Pointer to the PKA register structure base address
*
* @return Returns a status code.
* - @ref PKA_STATUS_OPERATION_BUSY if the operation is in progress.
* - @ref PKA_STATUS_SUCCESS if the two big numbers are equal.
* - @ref PKA_STATUS_A_GREATER_THAN_B  if the first number is greater than the second.
* - @ref PKA_STATUS_A_LESS_THAN_B if the first number is less than the second.
*
* @sa DL_PKABigNumCmpStart()
*
*/
extern DL_PKA_Status DL_PKABigNumCmpGetResult(PKA_Regs* pka);

/**
*
* @brief Starts a big number inverse modulo operation.
*
* This function starts the inverse modulo operation on @c bigNum
* using the divisor @c modulus.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] bigNum is the pointer to the buffer containing the big number
*        (dividend).
*
* @param [in] bigNumLength is the size of the @c bigNum in bytes.
*
* @param [in] modulus is the pointer to the buffer containing the divisor.
*
* @param [in] modulusLength is the size of the divisor in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumInvModGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumInvModStart(PKA_Regs* pka,
    const uint8_t* bigNum, uint32_t bigNumLength, const uint8_t* modulus,
    uint32_t modulusLength, uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the big number inverse modulo operation.
*
* This function gets the result of the big number inverse modulo operation
* previously started using the function DL_PKABigNumInvModStart().
* The function will zero-out @c resultBuf prior to copying in the result of
* the inverse modulo operation.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result needs to be
*        stored.
*
* @param [in] length is the size of the provided buffer in bytes.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumInvModStart().
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
*        than the result.
*
* @sa DL_PKABigNumInvModStart()
*
*/
extern DL_PKA_Status DL_PKABigNumInvModGetResult(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t length, uint32_t resultPKAMemAddr);

/**
*
* @brief Starts a big number modular exponentiation operation.
*
* This function starts the exponentiation operation on @c base with
* @c exponent and modulo @c modulus.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] base is the pointer to the buffer containing the big number
*             to exponentiate.
*
* @param [in] baseLength is the size of the @c base in bytes.
*
* @param [in] exponent is the pointer to the buffer containing the big number
*             that exponentiates.
*
* @param [in] exponentLength is the size of the @c exponent in bytes.

* @param [in] modulus is the pointer to the buffer containing the divisor.
*
* @param [in] modulusLength is the size of the divisor in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumExpModGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumExpModStart(PKA_Regs* pka,
    const uint8_t* base, uint32_t baseLength, const uint8_t* exponent,
    uint32_t exponentLength, const uint8_t* modulus, uint32_t modulusLength,
    uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the big number modular exponentiation operation.
*
* This function gets the result of the big number modular exponentiation
* operation previously started using the function DL_PKABigNumExpModStart().
* The function will zero-out @c resultBuf prior to copying in the result of
* the modular exponentiation operation.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result needs to be
*        stored.
*
* @param [in] length is the size of the provided buffer in bytes.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumExpModStart().
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
*        than the result.
*
* @sa DL_PKABigNumExpModStart()
*
*/
extern DL_PKA_Status DL_PKABigNumExpModGetResult(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t length, uint32_t resultPKAMemAddr);

/**
*
* @brief Starts the multiplication of two big numbers.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] multiplicand is the pointer to the buffer containing the big
*        number multiplicand.
*
* @param [in] multiplicandLength is the size of the multiplicand in bytes.
*
* @param [in] multiplier is the pointer to the buffer containing the big
*        number multiplier.
*
* @param [in] multiplierLength is the size of the multiplier in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumMultGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumMultiplyStart(PKA_Regs* pka,
    const uint8_t* multiplicand, uint32_t multiplicandLength,
    const uint8_t* multiplier, uint32_t multiplierLength,
    uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the big number multiplication.
*
* This function gets the result of the multiplication of two big numbers
* operation previously started using the function DL_PKABigNumMultiplyStart().
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result needs to be
*        stored.
*
* @param [in, out] resultLength is the address of the variable containing the length of the
*        buffer in bytes. After the operation, the actual length of the resultant is stored
*        at this address.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumMultiplyStart().
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_FAILURE if the operation is not successful.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
*        then the length of the result.
*
* @sa DL_PKABigNumMultiplyStart()
*
*/
extern DL_PKA_Status DL_PKABigNumMultGetResult(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t* resultLength, uint32_t resultPKAMemAddr);

/**
*
* @brief Starts the addition of two big numbers.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] bigNum1 is the pointer to the buffer containing the first
*        big number.
*
* @param [in] bigNum1Length is the size of the first big number in bytes.
*
* @param [in] bigNum2 is the pointer to the buffer containing the second
*        big number.
*
* @param [in] bigNum2Length is the size of the second big number in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumAddGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumAddStart(PKA_Regs* pka,
    const uint8_t* bigNum1, uint32_t bigNum1Length, const uint8_t* bigNum2,
    uint32_t bigNum2Length, uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the addition operation on two big numbers.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result
*        needs to be stored.
*
* @param [in, out] resultLength is the address of the variable containing
*        the length of the buffer.  After the operation the actual length of the
*        resultant is stored at this address.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumAddStart().
*
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_FAILURE if the operation is not successful.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
*        then the length of the result.
*
* @sa DL_PKABigNumAddStart()
*
*/
extern DL_PKA_Status DL_PKABigNumAddGetResult(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t* resultLength, uint32_t resultPKAMemAddr);

/**
*
* @brief Starts the subtraction of one big number from another.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] minuend is the pointer to the buffer containing the big number
*             to be subtracted from.
*
* @param [in] minuendLength is the size of the minuend in bytes.
*
* @param [in] subtrahend is the pointer to the buffer containing the big
*             number to subtract from the @c minuend.
*
* @param [in] subtrahendLength is the size of the subtrahend in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKABigNumSubGetResult()
*
*/
extern DL_PKA_Status DL_PKABigNumSubStart(PKA_Regs* pka,
    const uint8_t* minuend, uint32_t minuendLength, const uint8_t* subtrahend,
    uint32_t subtrahendLength, uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the subtraction operation on two big numbers.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] resultBuf is the pointer to buffer where the result
*        needs to be stored.
*
* @param [in, out] resultLength is the address of the variable containing
*        the length of the buffer.  After the operation the actual length of the
*        resultant is stored at this address.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKABigNumAddStart().
*
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_FAILURE if the operation is not successful.
* - @ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
*        then the length of the result.
*
* @sa DL_PKABigNumSubStart()
*
*/
extern DL_PKA_Status DL_PKABigNumSubGetResult(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t* resultLength, uint32_t resultPKAMemAddr);

/**
*
* @brief Starts ECC multiplication.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] scalar is pointer to the buffer containing the scalar
*        value to be multiplied.
*
* @param [in] curvePointX is the pointer to the buffer containing the
*        X coordinate of the elliptic curve point to be multiplied.
*        The point must be on the given curve.
*
* @param [in] curvePointY is the pointer to the buffer containing the
*        Y coordinate of the elliptic curve point to be multiplied.
*        The point must be on the given curve.
*
* @param [in] prime is the prime of the curve.
*
* @param [in] a is the a constant of the curve when the curve equation is expressed
*        in short Weierstrass form (y^3 = x^2 + a*x + b).
*
* @param [in] b is the b constant of the curve when the curve equation is expressed
*        in short Weierstrass form (y^3 = x^2 + a*x + b).
*
* @param [in] length is the length of the curve parameters in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKAEccMultiplyGetResult()
*
*/
extern DL_PKA_Status DL_PKAEccMultiplyStart(PKA_Regs* pka,
    const uint8_t* scalar, const uint8_t* curvePointX,
    const uint8_t* curvePointY, const uint8_t* prime, const uint8_t* a,
    const uint8_t* b, uint32_t length, uint32_t* resultPKAMemAddr);

/**
*
* @brief Starts ECC Montgomery multiplication.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] scalar is pointer to the buffer containing the scalar
*        value to be multiplied.
*
* @param [in] curvePointX is the pointer to the buffer containing the
*        X coordinate of the elliptic curve point to be multiplied.
*        The point must be on the given curve.
*
* @param [in] prime is the prime of the curve.
*
* @param [in] a is the a constant of the curve when the curve equation is expressed
*        in short Weierstrass form (y^3 = x^2 + a*x + b).
*
* @param [in] length is the length of the curve parameters in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKAEccMultiplyGetResult()
*
*/
extern DL_PKA_Status DL_PKAEccMontgomeryMultiplyStart(PKA_Regs* pka,
    const uint8_t* scalar, const uint8_t* curvePointX, const uint8_t* prime,
    const uint8_t* a, uint32_t length, uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of ECC multiplication
*
* This function gets the result of ECC point multiplication operation on the
* EC point and the scalar value, previously started using the function
* DL_PKAEccMultiplyStart().
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] curvePointX is the pointer to the structure where the X coordinate
*         of the resultant EC point will be stored.
*
* @param [out] curvePointY is the pointer to the structure where the Y coordinate
*         of the resultant EC point will be stored.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKAEccMultiplyStart().
*
* @param [in] length is the length of the curve parameters in bytes.
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
*        the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_FAILURE if the operation is not successful.
*
* @sa DL_PKAEccMultiplyStart()
*
*/
extern DL_PKA_Status DL_PKAEccMultiplyGetResult(PKA_Regs* pka,
    uint8_t* curvePointX, uint8_t* curvePointY, uint32_t resultPKAMemAddr,
    uint32_t length);

/**
*
* @brief Starts the ECC addition.
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] curvePoint1X is the pointer to the buffer containing the
*        X coordinate of the first elliptic curve point to be added.
*        The point must be on the given curve.
*
* @param [in] curvePoint1Y is the pointer to the buffer containing the
*        Y coordinate of the first elliptic curve point to be added.
*        The point must be on the given curve.
*
* @param [in] curvePoint2X is the pointer to the buffer containing the
*        X coordinate of the second elliptic curve point to be added.
*        The point must be on the given curve.
*
* @param [in] curvePoint2Y is the pointer to the buffer containing the
*        Y coordinate of the second elliptic curve point to be added.
*        The point must be on the given curve.
*
* @param [in] prime is the prime of the curve.
*
* @param [in] a is the a constant of the curve when the curve equation is expressed
*        in short Weierstrass form (y^3 = x^2 + a*x + b).
*
* @param [in] length is the length of the curve parameters in bytes.
*
* @param [out] resultPKAMemAddr is the pointer to the result vector location
*        which will be set by this function.
*
*@return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if successful in starting the operation.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
*        some other operation.
*
* @sa DL_PKAEccAddGetResult()
*
*/
extern DL_PKA_Status DL_PKAEccAddStart(PKA_Regs* pka,
    const uint8_t* curvePoint1X, const uint8_t* curvePoint1Y,
    const uint8_t* curvePoint2X, const uint8_t* curvePoint2Y,
    const uint8_t* prime, const uint8_t* a, const uint8_t* b, uint32_t length,
    uint32_t* resultPKAMemAddr);

/**
*
* @brief Gets the result of the ECC addition
*
* This function gets the result of ECC point addition operation on the
* on the two given EC points, previously started using the function
* DL_PKAEccAddStart().
*
* @param pka Pointer to the PKA register structure base address
*
* @param [out] curvePointX is the pointer to the structure where the X coordinate
*         of the resultant EC point will be stored.
*
* @param [out] curvePointY is the pointer to the structure where the Y coordinate
*         of the resultant EC point will be stored.
*
* @param [in] resultPKAMemAddr is the address of the result location which
*        was provided by the start function DL_PKAEccAddGetResult().
*
* @param [in] length is the length of the curve parameters in bytes.
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing the operation.
* - @ref PKA_STATUS_RESULT_0 if the result is all zeros.
* - @ref PKA_STATUS_FAILURE if the operation is not successful.
*
* @sa DL_PKAEccAddStart()
*
*/
extern DL_PKA_Status DL_PKAEccAddGetResult(PKA_Regs* pka, uint8_t* curvePointX,
    uint8_t* curvePointY, uint32_t resultPKAMemAddr, uint32_t length);

/**
*
* @brief Begins the validation of a public key against a Short-Weierstrass curve
*
* This function validates a public key against a curve.
* After performing multiple smaller PKA operations in polling mode,
* it starts an ECC scalar multiplication.
*
* The function verifies that:
*  - X and Y are in the range [1, prime - 1]
*  - The point is not the point at infinity
*  - X and Y satisfy the Short-Weierstrass curve equation Y^2 = X^3 + a*X + b mod P
*  - Multiplying the point by the order of the curve yields the point at infinity
*
* @param pka Pointer to the PKA register structure base address
*
* @param [in] curvePointX is the pointer to the buffer containing the
*        X coordinate of the elliptic curve point to verify.
*
* @param [in] curvePointY is the pointer to the buffer containing the
*        Y coordinate of the elliptic curve point to verify.
*
* @param [in] prime is the prime of the curve.
*
* @param [in] a is the a constant of the curve when the curve equation is expressed
*        in Short-Weierstrass form (y^3 = x^2 + a*x + b).
*
* @param [in] b is the b constant of the curve when the curve equation is expressed
*        in Short-Weierstrass form (y^3 = x^2 + a*x + b).
*
* @param [in] order is the order of the curve.
*
* @param [in] length is the length of the curve parameters in bytes.
*
* @return Returns a status code.
* - @ref PKA_STATUS_SUCCESS if the operation is successful.
* - @ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing the operation.
* - @ref PKA_STATUS_FAILURE if the operation is not successful.
* - @ref PKA_STATUS_X_ZERO if X is zero.
* - @ref PKA_STATUS_Y_ZERO if Y is zero.
* - @ref PKA_STATUS_X_LARGER_THAN_PRIME if X is larger than the curve prime
* - @ref PKA_STATUS_Y_LARGER_THAN_PRIME if Y is larger than the curve prime
* - @ref PKA_STATUS_POINT_NOT_ON_CURVE if X and Y do not satisfy the curve equation
*
* @sa PKAEccVerifyPublicKeyGetResult()
*
*/
extern DL_PKA_Status DL_PKAEccVerifyPublicKeyWeierstrassStart(PKA_Regs* pka,
    const uint8_t* curvePointX, const uint8_t* curvePointY,
    const uint8_t* prime, const uint8_t* a, const uint8_t* b,
    const uint8_t* order, uint32_t length);

/**
*
* @brief Assert reset to the PKA peripheral
*
* This function asserts a reset signal to the PKA peripheral by setting the
* RESETASSERT bit in the GPRCM.RSTCTL register. Writing a 1 to the RESETASSERT
* bit will trigger a reset of the peripheral.
*
* @param pka Pointer to the PKA register structure base address
*
* @return None
*
*/
extern void DL_PKA_resetAssert(PKA_Regs* pka);

/**
*
* @brief Clear the RESETSTKY bit in the STAT register
*
* This function clears the RESETSTKY bit in the STAT register by setting the
* RESETSTKYCLR bit in the GPRCM.RSTCTL register. Writing a 1 to the RESETSTKYCLR
* bit will clear the reset sticky bit.
*
* @param pka Pointer to the PKA register structure base address
*
* @return None
*
*/
extern void DL_PKA_resetStickyFlagClear(PKA_Regs* pka);

/**
*
* @brief Get the reset status from the STAT register
*
* This function reads the RESETSTKY bit in the STAT register to determine
* if the peripheral was reset since the bit was last cleared using
* DL_PKA_resetStickyFlagClear().
*
* @param pka Pointer to the PKA register structure base address
*
* @return Returns true if the peripheral was reset since the last bit clear,
*         false otherwise
*
*/
extern bool DL_PKA_getResetStatus(PKA_Regs* pka);
/**
*
* Mark the end of the C bindings section for C++ compilers.
*
*/
#ifdef __cplusplus
}
#endif

#endif
