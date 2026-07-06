/*****************************************************************************

  Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

   Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

   Neither the name of Texas Instruments Incorporated nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/
/* This file automatically includes the specific device header file
   without the need to include a specific device header.
   The device #define is set automatically through the toolchain on basis
   of the device chosen in the device selection menu (.e.g -D__MICRO1__).      */
/** @defgroup   PKA  */

#include "dl_pka.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define INRANGE(x, y, z) ((x) > (y) && (x) < (z))

#define IS_WORD_ALIGNED(ptr) (((uintptr_t)(ptr) << 30U) == 0U)

/*
*
* Used in DL_PKAWritePkaParam() and DL_PKAWritePkaParamExtraOffset() to specify that
* the base address of the parameter should not be written to a NPTR register.
*
*/
#define PKA_NO_POINTER_REG 0xFFU

/*
*
* Enable PKA IP
*
*/

void DL_PKA_enable(PKA_Regs* pka)
{
    DL_Common_updateReg(&pka->GPRCM.PWREN,
        (PKA_PWREN_KEY_UNLOCK_W | PKA_PWREN_ENABLE_ENABLE),
        (PKA_PWREN_KEY_MASK | PKA_PWREN_ENABLE_MASK));
}

/*
*
* Assert reset to the PKA peripheral
*
*/
void DL_PKA_resetAssert(PKA_Regs* pka)
{
    DL_Common_updateReg(&pka->GPRCM.RSTCTL,
        (PKA_RSTCTL_KEY_UNLOCK_W | PKA_RSTCTL_RESETASSERT_ASSERT),
        (PKA_RSTCTL_KEY_MASK | PKA_RSTCTL_RESETASSERT_MASK));
}

/*
*
* @brief Write a PKA parameter to the PKA module
*
* @param pka Pointer to the PKA register structure
* @param param Pointer to the parameter to write
* @param paramLength Length of the parameter in bytes
* @param paramOffset Offset in PKA RAM where to write the parameter
* @param ptrRegOffset Register offset for the parameter pointer (A, B, C, or D)
*
* @return Returns the offset for the next parameter
*
*/
static uint32_t DL_PKAWritePkaParam(PKA_Regs* pka, const uint8_t* param,
    uint32_t paramLength, uint32_t paramOffset, uint32_t ptrRegOffset)
{
    uint32_t i;
    uint_fast8_t j;
    uint32_t tempWord;
    uint32_t* paramWordAlias = (uint32_t*) param;
    /* Take the floor of paramLength in 32-bit words */
    uint32_t paramLengthInWords = paramLength / (sizeof(uint32_t));
    volatile uint32_t* pkaRamPtr;

    /* Only copy data if it is specified. We may wish to simply allocate another buffer and get
       the required offset. */
    if (param != 0U) {
        /* Load the data in PKA RAM */
        for (i = (uint32_t) 0U; i < paramLengthInWords; i++) {
            pkaRamPtr = (volatile uint32_t*) &pka->PKA_REGS.PKA_RAM_DATA_0_ +
                        (paramOffset / sizeof(uint32_t)) + i;

            /* If param address is word aligned, param could be an address on PKA RAM,
               which must be word aligned. Since PKA RAM only word addressable, i.e, it only
               allows reading and writing a word, when param address is word aligned,
               write the data in param as a word in PKA RAM. When param adddress is not word aligned,
               copy the data as bytes in a temp word buffer and write the temp buffer in PKA RAM. */
            if (IS_WORD_ALIGNED(paramWordAlias)) {
                /* Since PKA RAM is only word addressable, load the data as word in PKA RAM */
                *pkaRamPtr = paramWordAlias[i];
            } else {
                /* Copy as bytes in temp buffer */
                tempWord = (uint32_t) 0U;
                for (j = (uint_fast8_t) 0U; j < sizeof(tempWord); j++) {
                    tempWord |=
                        ((uint32_t)
                                param[(i * sizeof(uint32_t)) + (uint32_t) j]
                            << (uint32_t)(8U * (uint32_t) j));
                }

                /* Since PKA RAM is only word addressable, load temp buffer as word in PKA RAM */
                *pkaRamPtr = tempWord;
            }
        }

        /* If the length is not a word-multiple, fill up a temporary word and copy that in
           to avoid a bus error. The extra zeros at the end should not matter, as the large
           number is little-endian and thus has no effect.
           We could have correctly calculated ceiling(paramLength / sizeof(uint32_t)) above.
           However, we would not have been able to zero-out the extra few most significant
           bytes of the most significant word. That would have resulted in doing maths operations
           on whatever follows param in RAM. */
        if ((paramLength % sizeof(uint32_t)) != 0U) {
            tempWord = (uint32_t) 0U;
            uint8_t countVar;

            if (IS_WORD_ALIGNED(paramWordAlias)) {
                /* Load the entire word line of the param remainder */
                tempWord = paramWordAlias[i];
                /* Zero-out all bytes beyond the end of the param */
                for (countVar = paramLength % sizeof(uint32_t);
                     countVar < sizeof(uint32_t); countVar++) {
                    ((uint8_t*) &tempWord)[countVar] = (uint8_t) 0U;
                }
            } else {
                /* Only copy the param remainder bytes.
                   The rest of the bytes are already zero initialized. */
                for (countVar = 0U;
                     countVar < (paramLength % sizeof(uint32_t)); countVar++) {
                    tempWord |= ((uint32_t) param[(i * sizeof(uint32_t)) +
                                                  (uint32_t) countVar]
                                 << (uint32_t)(8U * (uint32_t) countVar));
                }
            }

            pkaRamPtr = (volatile uint32_t*) &pka->PKA_REGS.PKA_RAM_DATA_0_ +
                        (paramOffset / sizeof(uint32_t)) + i;
            *pkaRamPtr = tempWord;

            /* Increment paramLengthInWords since we take the ceiling of length / sizeof(uint32_t) */
            paramLengthInWords++;
        }
    }

    /* Update the A, B, C, or D pointer with the offset address of the PKA RAM location
       where the number will be stored. */
    switch (ptrRegOffset) {
        case DL_PKA_O_APTR_OFFSET:
            pka->PKA_REGS.PKA_APTR    = paramOffset >> 2U;
            pka->PKA_REGS.PKA_ALENGTH = paramLengthInWords;
            break;
        case DL_PKA_O_BPTR_OFFSET:
            pka->PKA_REGS.PKA_BPTR    = paramOffset >> 2U;
            pka->PKA_REGS.PKA_BLENGTH = paramLengthInWords;
            break;
        case DL_PKA_O_CPTR_OFFSET:
            pka->PKA_REGS.PKA_CPTR = paramOffset >> 2U;
            break;
        case DL_PKA_O_DPTR_OFFSET:
            pka->PKA_REGS.PKA_DPTR = paramOffset >> 2U;
            break;
        default:
            /* Handle unexpected values of ptrRegOffset*/
            /* Fix MISRA.SWITCH.WELL_FORMED.2012 */
            break;
    }

    /* Ensure 8-byte alignment of next parameter.
       Returns the offset for the next parameter. */
    return (paramOffset + (sizeof(uint32_t) * (paramLengthInWords +
                                                  (paramLengthInWords % 2U))));
}

/*
*
* Write a PKA parameter to the PKA module but return a larger offset.
*
*/
static uint32_t DL_PKAWritePkaParamExtraOffset(PKA_Regs* pka,
    const uint8_t* param, uint32_t paramLength, uint32_t paramOffset,
    uint32_t ptrRegOffset)
{
    /* Ensure 16-byte alignment. */
    return (sizeof(uint32_t) * 2U) + DL_PKAWritePkaParam(pka, param,
                                         paramLength, paramOffset,
                                         ptrRegOffset);
}

/*
*
* Writes the result of a large number arithmetic operation to a provided buffer.
*
*/
static DL_PKA_Status DL_PKAGetBigNumResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t* resultLength, uint32_t resultPKAMemAddr)
{
    uint32_t mswOffset;
    uint32_t lswOffset;
    uint32_t lengthInWords;
    uint32_t i;
    uint_fast8_t j;
    uint32_t tempWord;
    uint32_t* resultWordAlias = (uint32_t*) resultBuf;
    DL_PKA_Status status      = DL_PKA_STATUS_SUCCESS;
    volatile uint32_t* wordPtr;

    /* Verify that the operation is complete. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    /* Get the MSW register value. */
    mswOffset = pka->PKA_REGS.PKA_MSW;

    /* If the result vector is zero, write back one zero byte so the caller does not need
       to handle a special error for the perhaps valid result of zero.
       They will only get the error status if they do not provide a buffer */
    if ((mswOffset & PKA_PKA_MSW_ZERO_RESULT_VECTOR_MASK) != 0U) {
        if (*resultLength != 0U) {
            if (resultBuf != NULL) {
                resultBuf[0] = 0U;
            }

            *resultLength = 1U;

            status = DL_PKA_STATUS_SUCCESS_ZERO_RES;
        } else {
            status = DL_PKA_STATUS_BUF_UNDERFLOW;
        }
    }

    if (status == DL_PKA_STATUS_SUCCESS) {
        /* Get the length of the result */
        mswOffset = ((mswOffset & PKA_PKA_MSW_ADDRESS_MASK) + 1U);
        lswOffset =
            ((resultPKAMemAddr - (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_) >>
                2U);

        if (mswOffset >= lswOffset) {
            lengthInWords = mswOffset - lswOffset;
        } else {
            status = DL_PKA_STATUS_RESULT_ADDRESS_INCORRECT;
        }

        if (status == DL_PKA_STATUS_SUCCESS) {
            /* Check if the provided buffer length is adequate to store the result data. */
            if (*resultLength < (lengthInWords * sizeof(uint32_t))) {
                status = DL_PKA_STATUS_BUF_UNDERFLOW;
            }

            if (status == DL_PKA_STATUS_SUCCESS) {
                /* Copy the resultant length. */
                *resultLength = lengthInWords * sizeof(uint32_t);

                if (resultBuf != NULL) {
                    /* If resultBuf address is word aligned, it could be an address on PKA RAM,
                       which must be word aligned. Since PKA RAM is only word addressable, i.e, it only
                       allows reading and writing a word, when resultBuf address is word aligned,
                       read the result in PKA RAM as a word directly. When resultBuf adddress is not word aligned,
                       use a temp word buffer to store the result from PKA RAM and write the result as bytes
                       in resultBuf from the temp buffer. */
                    for (i = (uint32_t) 0U; i < lengthInWords; i++) {
                        if (IS_WORD_ALIGNED(resultWordAlias)) {
                            wordPtr =
                                (volatile uint32_t*) (resultPKAMemAddr +
                                                      (sizeof(uint32_t) * i));
                            resultWordAlias[i] = *wordPtr;
                        } else {
                            /* Since PKA RAM is only word addressable,
                               copy the result as word in a temp buffer from PKA RAM */
                            wordPtr =
                                (volatile uint32_t*) (resultPKAMemAddr +
                                                      (sizeof(uint32_t) * i));
                            tempWord = *wordPtr;

                            /* Copy the result from temp buffer as bytes */
                            for (j = (uint_fast8_t) 0U; j < sizeof(tempWord);
                                 j++) {
                                resultBuf[(i * sizeof(tempWord)) + j] =
                                    ((uint8_t*) &tempWord)[j];
                            }
                        }
                    }
                }
            } else {
                status = DL_PKA_STATUS_FAILURE;
            }
        } else {
            status = DL_PKA_STATUS_FAILURE;
        }
    }

    if (status == DL_PKA_STATUS_SUCCESS_ZERO_RES) {
        status = DL_PKA_STATUS_SUCCESS;
    }
    return status;
}

/*
*
* Retrieve the result of a modulo operation or the remainder of a division.
*
*/
static DL_PKA_Status DL_PKAGetBigNumResultRemainder(PKA_Regs* pka,
    uint8_t* resultBuf, uint32_t* resultLength, uint32_t resultPKAMemAddr)
{
    uint32_t regMSWVal;
    uint32_t lengthInWords;
    uint32_t i;
    uint_fast8_t j;
    uint32_t tempWord;
    uint32_t* resultWordAlias = (uint32_t*) resultBuf;
    DL_PKA_Status status      = DL_PKA_STATUS_SUCCESS;
    volatile uint32_t* wordPtr;

    /* Verify that the operation is complete. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    /* Get the MSW register value. */
    regMSWVal = pka->PKA_REGS.PKA_DIVMSW;

    /* If the result vector is zero, write back one zero byte so the caller does not need
       to handle a special error for the perhaps valid result of zero.
       They will only get the error status if they do not provide a buffer */
    if ((regMSWVal & PKA_PKA_DIVMSW_ZERO_REMAINDER_VECTOR_MASK) != 0U) {
        if (*resultLength != 0U) {
            if (resultBuf != NULL) {
                resultBuf[0] = 0U;
            }

            *resultLength = 1U;

            status = DL_PKA_STATUS_SUCCESS_ZERO_RES;
        } else {
            status = DL_PKA_STATUS_BUF_UNDERFLOW;
        }
    }

    if (status == DL_PKA_STATUS_SUCCESS) {
        /* Get the length of the result */
        lengthInWords = (uint32_t)(
            ((regMSWVal & PKA_PKA_DIVMSW_MSW_ADDRESS_MASK) + 1U) -
            ((resultPKAMemAddr - (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_) >>
                2U));

        /* Check if the provided buffer length is adequate to store the result data. */
        if (*resultLength < (lengthInWords * sizeof(uint32_t))) {
            status = DL_PKA_STATUS_BUF_UNDERFLOW;
        }

        if (status == DL_PKA_STATUS_SUCCESS) {
            /* Copy the resultant length. */
            *resultLength = lengthInWords * sizeof(uint32_t);

            if (resultBuf != NULL) {
                /* If resultBuf address is word aligned, it could be an address on PKA RAM,
                   which must be word aligned. Since PKA RAM is only word addressable, i.e, it only
                   allows reading and writing a word, when resultBuf address is word aligned,
                   read the result in PKA RAM as a word directly. When resultBuf adddress is not word aligned,
                   use a temp word buffer to store the result from PKA RAM and write the result as bytes
                   in resultBuf from the temp buffer. */
                for (i = (uint32_t) 0U; i < lengthInWords; i++) {
                    if (IS_WORD_ALIGNED(resultWordAlias)) {
                        /* Since PKA RAM is only word addressable, copy the result as word from PKA RAM */
                        wordPtr =
                            (volatile uint32_t*) (resultPKAMemAddr +
                                                  (sizeof(uint32_t) * i));
                        resultWordAlias[i] = *wordPtr;
                    } else {
                        /* Since PKA RAM is only word addressable,
                           copy the result as word in a temp buffer from PKA RAM */
                        wordPtr =
                            (volatile uint32_t*) (resultPKAMemAddr +
                                                  (sizeof(uint32_t) * i));
                        tempWord = *wordPtr;

                        /* Copy the result from temp buffer as bytes */
                        for (j = (uint_fast8_t) 0U; j < sizeof(tempWord);
                             j++) {
                            resultBuf[(i * sizeof(tempWord)) + j] =
                                ((uint8_t*) &tempWord)[j];
                        }
                    }
                }
            }
        }
    }
    if (status == DL_PKA_STATUS_SUCCESS_ZERO_RES) {
        status = DL_PKA_STATUS_SUCCESS;
    }
    return status;
}

/*
*
* Writes the resultant curve point of an ECC operation to the provided buffer.
*
*/
static DL_PKA_Status DL_PKAGetECCResult(PKA_Regs* pka, uint8_t* curvePointX,
    uint8_t* curvePointY, uint32_t resultPKAMemAddr, uint32_t length)
{
    uint32_t resultPKA            = resultPKAMemAddr;
    uint32_t i                    = 0U;
    uint32_t lengthInWordsCeiling = 0U;
    uint32_t tempWord             = 0U;
    uint_fast8_t j                = 0U;
    uint32_t* xWordAlias          = (uint32_t*) curvePointX;
    uint32_t* yWordAlias          = (uint32_t*) curvePointY;
    DL_PKA_Status status          = DL_PKA_STATUS_SUCCESS;
    volatile uint32_t* wordPtr;

    /* Verify that the operation is completed. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    if (pka->PKA_REGS.PKA_SHIFT != 0U) {
        status = DL_PKA_STATUS_FAILURE;
    }

    /* Check to make sure that the result vector is not the point at infinity. */
    if ((pka->PKA_REGS.PKA_MSW & PKA_PKA_MSW_ZERO_RESULT_VECTOR_MASK) != 0U) {
        status = DL_PKA_STATUS_POINT_AT_INFINITY;
    }

    if (status == DL_PKA_STATUS_SUCCESS) {
        if (curvePointX != NULL) {
            /* Copy the x coordinate value of the result from vector D into
               the curvePoint.
               If curvePointX address is word aligned, it could be an address on PKA RAM,
               which must be word aligned. Since PKA RAM is only word addressable, i.e, it only
               allows reading and writing a word, when curvePointX address is word aligned,
               read the result in PKA RAM as a word directly. When curvePointX adddress is not word aligned,
               use a temp word buffer to store the result from PKA RAM and write the result as bytes
               in curvePointX from the temp buffer. */
            for (i = (uint32_t) 0U; i < (length / sizeof(uint32_t)); i++) {
                /* Check for word aligned address in x coordinate buffer */
                if (IS_WORD_ALIGNED(curvePointX)) {
                    /* Since PKA RAM is only word addressable, copy x coordinate as a word from PKA RAM */
                    wordPtr       = (volatile uint32_t*) (resultPKA +
                                                    (sizeof(uint32_t) * i));
                    xWordAlias[i] = *wordPtr;
                } else {
                    /* Copy x Coordinate as bytes
                       Since PKA RAM is only word addressable, temporarily load
                       the entire word line of the coordinate */
                    wordPtr  = (volatile uint32_t*) (resultPKA +
                                                    (sizeof(uint32_t) * i));
                    tempWord = *wordPtr;

                    /* Write the bytes to the X coordinate */
                    for (j = (uint_fast8_t) 0U; j < sizeof(tempWord); j++) {
                        curvePointX[(i * sizeof(tempWord)) + j] =
                            ((uint8_t*) &tempWord)[j];
                    }
                }
            }

            /* If the length is not a word-multiple, fill up a temporary word and copy that in
               to avoid a bus error. */
            if ((length % sizeof(uint32_t)) != 0U) {
                /* Load the entire word line of the coordinate remainder */
                wordPtr =
                    (volatile uint32_t*) (resultPKA + (sizeof(uint32_t) * i));
                tempWord = *wordPtr;

                /* Write all remaining bytes to the coordinate */
                for (j = (uint_fast8_t) 0U; j < (length % sizeof(uint32_t));
                     j++) {
                    curvePointX[(i * sizeof(uint32_t)) + j] =
                        ((uint8_t*) &tempWord)[j];
                }
            }
        }

        lengthInWordsCeiling = ((length % sizeof(uint32_t)) > 0U)
                                   ? ((length / sizeof(uint32_t)) + 1U)
                                   : (length / sizeof(uint32_t));

        resultPKA += sizeof(uint32_t) *
                     (2U + lengthInWordsCeiling + (lengthInWordsCeiling % 2U));

        if (curvePointY != NULL) {
            /* Copy the y coordinate value of the result from vector D into
               the curvePoint.
               If curvePointY address is word aligned, it could be an address on PKA RAM,
               which must be word aligned. Since PKA RAM is only word addressable, i.e, it only
               allows reading and writing a word, when curvePointY address is word aligned,
               read the result in PKA RAM as a word directly. When curvePointY adddress is not word aligned,
               use a temp word buffer to store the result from PKA RAM and write the result as bytes
               in curvePointY from the temp buffer. */
            for (i = (uint32_t) 0U; i < (length / sizeof(uint32_t)); i++) {
                /* Check for word aligned address in y coordinate buffer */
                if (IS_WORD_ALIGNED(curvePointY)) {
                    /* Since PKA RAM is only word addressable, copy y coordinate as a word from PKA RAM */
                    wordPtr       = (volatile uint32_t*) (resultPKA +
                                                    (sizeof(uint32_t) * i));
                    yWordAlias[i] = *wordPtr;
                } else {
                    /* Copy y Coordinate as bytes
                       Since PKA RAM is only word addressable, temporarily load
                       the entire word line of the coordinate */
                    wordPtr  = (volatile uint32_t*) (resultPKA +
                                                    (sizeof(uint32_t) * i));
                    tempWord = *wordPtr;

                    /* Write the bytes to the Y coordinate */
                    for (j = (uint_fast8_t) 0U; j < sizeof(tempWord); j++) {
                        curvePointY[(i * sizeof(tempWord)) + j] =
                            ((uint8_t*) &tempWord)[j];
                    }
                }
            }

            /* If the length is not a word-multiple, fill up a temporary word and copy that in
               to avoid a bus error. */
            if ((length % sizeof(uint32_t)) != 0U) {
                /* Load the entire word line of the coordinate remainder */
                wordPtr =
                    (volatile uint32_t*) (resultPKA + (sizeof(uint32_t) * i));
                tempWord = *wordPtr;

                /* Write all remaining bytes to the coordinate */
                for (j = (uint_fast8_t) 0U; j < (length % sizeof(uint32_t));
                     j++) {
                    curvePointY[(i * sizeof(uint32_t)) + j] =
                        ((uint8_t*) &tempWord)[j];
                }
            }
        }
    }
    return status;
}

/*
*
* Provides the PKA operation status.
*
*/
DL_PKA_Status DL_PKAGetOpsStatus(PKA_Regs* pka)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    if ((pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
        status = DL_PKA_STATUS_OPERATION_BUSY;
    } else {
        status = DL_PKA_STATUS_OPERATION_RDY;
    }
    return status;
}

/*
*
* Check if an array consists only of zeros.
*
*/
uint8_t DL_PKAArrayAllZeros(
    PKA_Regs* pka, const uint8_t* array, uint32_t arrayLength)
{
    uint32_t i;
    uint8_t arrayBits = 0U;
    uint8_t result    = 1U;

    /* We could speed things up by comparing word-wise rather than byte-wise.
       However, this extra overhead is inconsequential compared to running an
       actual PKA operation. Especially ECC operations. */
    for (i = (uint32_t) 0U; i < arrayLength; i++) {
        arrayBits |= array[i];
    }

    if (arrayBits != 0U) {
        result = 0U;
    }
    return result;
}

/*
*
* Fill an array with zeros
*
*/
void DL_PKAZeroOutArray(const uint8_t* array, uint32_t arrayLength)
{
    uint32_t i;
    /* Take the floor of paramLength in 32-bit words */
    uint32_t arrayLengthInWords = arrayLength / sizeof(uint32_t);
    volatile uint32_t* wordPtr;
    volatile uint8_t* bytePtr;

    /* Zero-out the array word-wise until i >= arrayLength */
    for (i = (uint32_t) 0U; i < arrayLengthInWords; i++) {
        wordPtr  = (volatile uint32_t*) (array + (i * sizeof(uint32_t)));
        *wordPtr = (uint32_t) 0U;
    }

    /* If i != arrayLength, there are some remaining bytes to zero-out */
    if ((arrayLength % sizeof(uint32_t) != 0U)) {
        /* Zero out the remaining bytes */
        for (i = arrayLengthInWords * sizeof(uint32_t); i < arrayLength; i++) {
            bytePtr  = (volatile uint8_t*) (array + i);
            *bytePtr = (uint8_t) 0U;
        }
    }
}

/*
*
* Start the big number modulus operation.
*
*/
DL_PKA_Status DL_PKABigNumModStart(PKA_Regs* pka, const uint8_t* bigNum,
    uint32_t bigNumLength, const uint8_t* modulus, uint32_t modulusLength,
    uint32_t* resultPKAMemAddr)
{
    uint32_t offset      = 0U;
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, bigNum, bigNumLength, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, modulus, modulusLength, offset, DL_PKA_O_BPTR_OFFSET);

    /* Copy the result vector address location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load C pointer with the result location in PKA RAM */
    pka->PKA_REGS.PKA_CPTR = offset >> 2U;

    /* Start the PKCP modulo operation by setting the PKA Function register. */
    pka->PKA_REGS.PKA_FUNCTION =
        (PKA_PKA_FUNCTION_RUNFUNC_MASK | PKA_PKA_FUNCTION_MODULO_MASK);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the big number modulus operation.
*
*/
DL_PKA_Status DL_PKABigNumModGetResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t length, uint32_t resultPKAMemAddr)
{
    uint32_t len = length;
    /* Zero-out array in case modulo result is shorter than length */
    DL_PKAZeroOutArray(resultBuf, len);

    return DL_PKAGetBigNumResultRemainder(
        pka, resultBuf, &len, resultPKAMemAddr);
}

/*
*
* Start the big number divide operation.
*
*/
DL_PKA_Status DL_PKABigNumDivideStart(PKA_Regs* pka, const uint8_t* dividend,
    uint32_t dividendLength, const uint8_t* divisor, uint32_t divisorLength,
    uint32_t* resultQuotientMemAddr, uint32_t* resultRemainderMemAddr)
{
    uint32_t offset      = 0U;
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }
    offset = DL_PKAWritePkaParam(
        pka, dividend, dividendLength, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, divisor, divisorLength, offset, DL_PKA_O_BPTR_OFFSET);

    /* Copy the remainder result vector address location. */
    if (resultRemainderMemAddr != NULL) {
        *resultRemainderMemAddr =
            (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;
    }

    /* The remainder cannot ever be larger than the divisor. It should fit inside
       a buffer of that size. */
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, 0, divisorLength, offset, DL_PKA_O_CPTR_OFFSET);

    /* Copy the remainder result vector address location. */
    if (resultQuotientMemAddr != NULL) {
        *resultQuotientMemAddr =
            (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;
    }

    /* Load D pointer with the quotient location in PKA RAM */
    pka->PKA_REGS.PKA_DPTR = offset >> 2U;

    /* Start the PKCP modulo operation by setting the PKA Function register. */
    pka->PKA_REGS.PKA_FUNCTION =
        (PKA_PKA_FUNCTION_RUNFUNC_MASK | PKA_PKA_FUNCTION_DIVIDE_MASK);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the quotient of the big number divide operation.
*
*/
DL_PKA_Status DL_PKABigNumDivideGetQuotient(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t* length, uint32_t resultQuotientMemAddr)
{
    return DL_PKAGetBigNumResult(
        pka, resultBuf, length, resultQuotientMemAddr);
}

/*
*
* Get the remainder of the big number divide operation.
*
*/
DL_PKA_Status DL_PKABigNumDivideGetRemainder(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t* length, uint32_t resultQuotientMemAddr)
{
    return DL_PKAGetBigNumResultRemainder(
        pka, resultBuf, length, resultQuotientMemAddr);
}

/*
*
* Start the comparison of two big numbers.
*
*/
DL_PKA_Status DL_PKABigNumCmpStart(PKA_Regs* pka, const uint8_t* bigNum1,
    const uint8_t* bigNum2, uint32_t length)
{
    uint32_t offset      = 0U;
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, bigNum1, length, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParam(
        pka, bigNum2, length, offset, DL_PKA_O_BPTR_OFFSET);

    /* Set the PKA Function register for the Compare operation
       and start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        (PKA_PKA_FUNCTION_RUNFUNC_MASK | PKA_PKA_FUNCTION_COMPARE_MASK);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the comparison operation of two big numbers.
*
*/
DL_PKA_Status DL_PKABigNumCmpGetResult(PKA_Regs* pka)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;

    /* verify that the operation is complete. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    /* Check the COMPARE register. */
    switch (pka->PKA_REGS.PKA_COMPARE) {
        case PKA_PKA_COMPARE_AEQB_MASK:
            status = DL_PKA_STATUS_EQUAL;
            break;

        case PKA_PKA_COMPARE_AGTB_MASK:
            status = DL_PKA_STATUS_A_GREATER_THAN_B;
            break;

        case PKA_PKA_COMPARE_ALTB_MASK:
            status = DL_PKA_STATUS_A_LESS_THAN_B;
            break;

        default:
            status = DL_PKA_STATUS_FAILURE;
            break;
    }

    return status;
}

/*
*
* Start the big number inverse modulo operation.
*
*/
DL_PKA_Status DL_PKABigNumInvModStart(PKA_Regs* pka, const uint8_t* bigNum,
    uint32_t bigNumLength, const uint8_t* modulus, uint32_t modulusLength,
    uint32_t* resultPKAMemAddr)
{
    uint32_t offset      = 0U;
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, bigNum, bigNumLength, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParam(
        pka, modulus, modulusLength, offset, DL_PKA_O_BPTR_OFFSET);

    /* Copy the result vector address location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load D pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_DPTR = offset >> 2U;

    /* set the PKA function to InvMod operation and the start the operation.
       TODO - this address should be #defined somewhere? */
    pka->PKA_REGS.PKA_FUNCTION = 0x0000F000U;

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the big number inverse modulo operation.
*
*/
DL_PKA_Status DL_PKABigNumInvModGetResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t length, uint32_t resultPKAMemAddr)
{
    uint32_t len = length;
    /* Zero-out array in case modulo result is shorter than length */
    DL_PKAZeroOutArray(resultBuf, len);

    return DL_PKAGetBigNumResult(pka, resultBuf, &len, resultPKAMemAddr);
}

/*
*
* Start the big number modular exponentiation operation.
*
*/
DL_PKA_Status DL_PKABigNumExpModStart(PKA_Regs* pka, const uint8_t* base,
    uint32_t baseLength, const uint8_t* exponent, uint32_t exponentLength,
    const uint8_t* modulus, uint32_t modulusLength, uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, exponent, exponentLength, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, modulus, modulusLength, offset, DL_PKA_O_BPTR_OFFSET);

    offset = DL_PKAWritePkaParam(
        pka, base, baseLength, offset, DL_PKA_O_CPTR_OFFSET);

    /* Copy the result vector address location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load D pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_DPTR = offset >> 2U;

    /* set the PKA function to ExpMod operation and the start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        PKA_PKA_FUNCTION_RUNFUNC_MASK | (0x04U << PKA_PKA_FUNCTION_SEQOP_OFS);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the big number inverse modulo operation.
*
*/
DL_PKA_Status DL_PKABigNumExpModGetResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t length, uint32_t resultPKAMemAddr)
{
    uint32_t len = length;
    /* Zero-out array in case modulo result is shorter than length */
    DL_PKAZeroOutArray(resultBuf, len);

    return DL_PKAGetBigNumResult(pka, resultBuf, &len, resultPKAMemAddr);
}

/*
*
* Start the big number multiplication.
*
*/
DL_PKA_Status DL_PKABigNumMultiplyStart(PKA_Regs* pka,
    const uint8_t* multiplicand, uint32_t multiplicandLength,
    const uint8_t* multiplier, uint32_t multiplierLength,
    uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, multiplicand, multiplicandLength, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParam(
        pka, multiplier, multiplierLength, offset, DL_PKA_O_BPTR_OFFSET);

    /* Copy the result vector address location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load C pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_CPTR = offset >> 2U;

    /* Set the PKA function to the multiplication and start it. */
    pka->PKA_REGS.PKA_FUNCTION =
        (PKA_PKA_FUNCTION_RUNFUNC_MASK | PKA_PKA_FUNCTION_MULTIPLY_MASK);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the results of the big number multiplication.
*
*/
DL_PKA_Status DL_PKABigNumMultGetResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t* resultLength, uint32_t resultPKAMemAddr)
{
    return DL_PKAGetBigNumResult(
        pka, resultBuf, resultLength, resultPKAMemAddr);
}

/*
*
* Start the addition of two big number.
*
*/
DL_PKA_Status DL_PKABigNumAddStart(PKA_Regs* pka, const uint8_t* bigNum1,
    uint32_t bigNum1Length, const uint8_t* bigNum2, uint32_t bigNum2Length,
    uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, bigNum1, bigNum1Length, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParam(
        pka, bigNum2, bigNum2Length, offset, DL_PKA_O_BPTR_OFFSET);

    /* Copy the result vector address location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load C pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_CPTR = offset >> 2U;

    /* Set the function for the add operation and start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        (PKA_PKA_FUNCTION_RUNFUNC_MASK | PKA_PKA_FUNCTION_ADD_MASK);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the addition operation on two big number.
*
*/
DL_PKA_Status DL_PKABigNumSubGetResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t* resultLength, uint32_t resultPKAMemAddr)
{
    return DL_PKAGetBigNumResult(
        pka, resultBuf, resultLength, resultPKAMemAddr);
}

/*
*
* Start the addition of two big number.
*
*/
DL_PKA_Status DL_PKABigNumSubStart(PKA_Regs* pka, const uint8_t* minuend,
    uint32_t minuendLength, const uint8_t* subtrahend,
    uint32_t subtrahendLength, uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParam(
        pka, minuend, minuendLength, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParam(
        pka, subtrahend, subtrahendLength, offset, DL_PKA_O_BPTR_OFFSET);

    /* Copy the result vector address location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load C pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_CPTR = offset >> 2U;

    /* Set the function for the add operation and start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        (PKA_PKA_FUNCTION_RUNFUNC_MASK | PKA_PKA_FUNCTION_SUBTRACT_MASK);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the addition operation on two big number.
*
*/
DL_PKA_Status DL_PKABigNumAddGetResult(PKA_Regs* pka, uint8_t* resultBuf,
    uint32_t* resultLength, uint32_t resultPKAMemAddr)
{
    return DL_PKAGetBigNumResult(
        pka, resultBuf, resultLength, resultPKAMemAddr);
}

/*
*
* Start ECC Multiplication.
*
*/
DL_PKA_Status DL_PKAEccMultiplyStart(PKA_Regs* pka, const uint8_t* scalar,
    const uint8_t* curvePointX, const uint8_t* curvePointY,
    const uint8_t* prime, const uint8_t* a, const uint8_t* b, uint32_t length,
    uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;

    /* Make sure no PKA operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset =
        DL_PKAWritePkaParam(pka, scalar, length, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, prime, length, offset, DL_PKA_O_BPTR_OFFSET);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, a, length, offset, PKA_NO_POINTER_REG);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, b, length, offset, PKA_NO_POINTER_REG);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePointX, length, offset, DL_PKA_O_CPTR_OFFSET);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePointY, length, offset, PKA_NO_POINTER_REG);

    /* Update the result location.
       The resultPKAMemAddr may be 0 if we only want to check that we generated the point at infinity */
    if (resultPKAMemAddr != NULL) {
        *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;
    }

    /* Load D pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_DPTR = offset >> 2U;

    /* Set the PKA function to ECC-MULT and start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        PKA_PKA_FUNCTION_RUNFUNC_MASK | (0x05U << PKA_PKA_FUNCTION_SEQOP_OFS);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Start ECC Montgomery Multiplication.
*
*/
DL_PKA_Status DL_PKAEccMontgomeryMultiplyStart(PKA_Regs* pka,
    const uint8_t* scalar, const uint8_t* curvePointX, const uint8_t* prime,
    const uint8_t* a, uint32_t length, uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;
    volatile uint16_t* pkaPtr;

    /* Make sure no PKA operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset =
        DL_PKAWritePkaParam(pka, scalar, length, offset, DL_PKA_O_APTR_OFFSET);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, prime, length, offset, DL_PKA_O_BPTR_OFFSET);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, a, length, offset, PKA_NO_POINTER_REG);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePointX, length, offset, DL_PKA_O_CPTR_OFFSET);

    /* Update the result location.
       The resultPKAMemAddr may be 0 if we only want to check that we generated the point at infinity */
    if (resultPKAMemAddr != NULL) {
        *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;
    }

    /* Load D pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_DPTR = offset >> 2U;

    /* Set the PKA function to Montgomery ECC-MULT and start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        PKA_PKA_FUNCTION_RUNFUNC_MASK | (0x02U << PKA_PKA_FUNCTION_SEQOP_OFS);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of ECC Multiplication
*
*/
DL_PKA_Status DL_PKAEccMultiplyGetResult(PKA_Regs* pka, uint8_t* curvePointX,
    uint8_t* curvePointY, uint32_t resultPKAMemAddr, uint32_t length)
{
    return DL_PKAGetECCResult(
        pka, curvePointX, curvePointY, resultPKAMemAddr, length);
}

/*
*
* Start the ECC Addition.
*
*/
DL_PKA_Status DL_PKAEccAddStart(PKA_Regs* pka, const uint8_t* curvePoint1X,
    const uint8_t* curvePoint1Y, const uint8_t* curvePoint2X,
    const uint8_t* curvePoint2Y, const uint8_t* prime, const uint8_t* a,
    const uint8_t* b, uint32_t length, uint32_t* resultPKAMemAddr)
{
    DL_PKA_Status status = DL_PKA_STATUS_SUCCESS;
    uint32_t offset      = 0U;
    volatile uint32_t* pkaPtr;

    /* Make sure no operation is in progress. */
    while (
        (pka->PKA_REGS.PKA_FUNCTION & PKA_PKA_FUNCTION_RUNFUNC_MASK) != 0U) {
    }

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePoint1X, length, offset, DL_PKA_O_APTR_OFFSET);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePoint1Y, length, offset, PKA_NO_POINTER_REG);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, prime, length, offset, DL_PKA_O_BPTR_OFFSET);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, a, length, offset, PKA_NO_POINTER_REG);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, b, length, offset, PKA_NO_POINTER_REG);

    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePoint2X, length, offset, DL_PKA_O_CPTR_OFFSET);
    offset = DL_PKAWritePkaParamExtraOffset(
        pka, curvePoint2Y, length, offset, PKA_NO_POINTER_REG);

    /* Copy the result vector location. */
    *resultPKAMemAddr = (uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ + offset;

    /* Load D pointer with the result location in PKA RAM. */
    pka->PKA_REGS.PKA_DPTR = offset >> 2U;

    /* Set the PKA Function to ECC-ADD and start the operation. */
    pka->PKA_REGS.PKA_FUNCTION =
        PKA_PKA_FUNCTION_RUNFUNC_MASK | (0x03U << PKA_PKA_FUNCTION_SEQOP_OFS);

    status = DL_PKA_STATUS_SUCCESS;
    return status;
}

/*
*
* Get the result of the ECC Addition
*
*/
DL_PKA_Status DL_PKAEccAddGetResult(PKA_Regs* pka, uint8_t* curvePointX,
    uint8_t* curvePointY, uint32_t resultPKAMemAddr, uint32_t length)
{
    return DL_PKAGetECCResult(
        pka, curvePointX, curvePointY, resultPKAMemAddr, length);
}

/*
*
* Verify a public key against the supplied elliptic curve equation
*
*/
DL_PKA_Status DL_PKAEccVerifyPublicKeyWeierstrassStart(PKA_Regs* pka,
    const uint8_t* curvePointX, const uint8_t* curvePointY,
    const uint8_t* prime, const uint8_t* a, const uint8_t* b,
    const uint8_t* order, uint32_t length)
{
    uint32_t pkaResult;
    uint32_t resultLength;
    uint32_t resultAddress = 0U;  // Assign a value to avoid compiler warnings.
    uint8_t* scratchBuffer =
        (uint8_t*) ((uint32_t) &pka->PKA_REGS.PKA_RAM_DATA_0_ +
                    (PKA_RAM_TOT_BYTE_SIZE / 2U));
    uint8_t* scratchBuffer2 = scratchBuffer + 512U;
    DL_PKA_Status status    = DL_PKA_STATUS_SUCCESS;

    /* Verify X in range [0, prime - 1] */
    DL_PKABigNumCmpStart(pka, curvePointX, prime, length);

    while (DL_PKAGetOpsStatus(pka) == DL_PKA_STATUS_OPERATION_BUSY) {
    }

    pkaResult = (uint32_t) DL_PKABigNumCmpGetResult(pka);

    if (pkaResult != (uint32_t) DL_PKA_STATUS_A_LESS_THAN_B) {
        status = DL_PKA_STATUS_X_LARGER_THAN_PRIME;
    }

    /* Verify Y in range [0, prime - 1] */
    DL_PKABigNumCmpStart(pka, curvePointY, prime, length);

    while (DL_PKAGetOpsStatus(pka) == DL_PKA_STATUS_OPERATION_BUSY) {
    }

    pkaResult = (uint32_t) DL_PKABigNumCmpGetResult(pka);

    if (pkaResult != (uint32_t) DL_PKA_STATUS_A_LESS_THAN_B) {
        status = DL_PKA_STATUS_Y_LARGER_THAN_PRIME;
    }

    if (status == DL_PKA_STATUS_SUCCESS) {
        /* Verify point on curve
           Short-Weierstrass equation: Y ^ 2 = X ^3 + a * X + b mod P
           Reduced: Y ^ 2 = X * (X ^ 2 + a) + b */

        /* tmp = X ^ 2 */
        DL_PKABigNumMultiplyStart(
            pka, curvePointX, length, curvePointX, length, &resultAddress);

        while (DL_PKAGetOpsStatus(pka) == DL_PKA_STATUS_OPERATION_BUSY) {
        }

        resultLength = 200U;
        pkaResult    = (uint32_t) DL_PKABigNumMultGetResult(
            pka, scratchBuffer, &resultLength, resultAddress);

        if (pkaResult != (uint32_t) DL_PKA_STATUS_SUCCESS) {
            status = DL_PKA_STATUS_FAILURE;
        }
        if (status == DL_PKA_STATUS_SUCCESS) {
            /* tmp += a */
            DL_PKABigNumAddStart(
                pka, scratchBuffer, resultLength, a, length, &resultAddress);

            while (DL_PKAGetOpsStatus(pka) == DL_PKA_STATUS_OPERATION_BUSY) {
            }

            resultLength = 200U;
            pkaResult    = (uint32_t) DL_PKABigNumAddGetResult(
                pka, scratchBuffer, &resultLength, resultAddress);

            if (pkaResult != (uint32_t) DL_PKA_STATUS_SUCCESS) {
                status = DL_PKA_STATUS_FAILURE;
            }
            if (status == DL_PKA_STATUS_SUCCESS) {
                /* tmp *= x */
                DL_PKABigNumMultiplyStart(pka, scratchBuffer, resultLength,
                    curvePointX, length, &resultAddress);

                while (
                    DL_PKAGetOpsStatus(pka) == DL_PKA_STATUS_OPERATION_BUSY) {
                }

                resultLength = 200U;
                pkaResult    = (uint32_t) DL_PKABigNumMultGetResult(
                    pka, scratchBuffer, &resultLength, resultAddress);

                if (pkaResult != (uint32_t) DL_PKA_STATUS_SUCCESS) {
                    status = DL_PKA_STATUS_FAILURE;
                }
                if (status == DL_PKA_STATUS_SUCCESS) {
                    /* tmp += b */
                    DL_PKABigNumAddStart(pka, scratchBuffer, resultLength, b,
                        length, &resultAddress);

                    while (DL_PKAGetOpsStatus(pka) ==
                           DL_PKA_STATUS_OPERATION_BUSY) {
                    }

                    resultLength = 200U;
                    pkaResult    = (uint32_t) DL_PKABigNumAddGetResult(
                        pka, scratchBuffer, &resultLength, resultAddress);

                    if (pkaResult != (uint32_t) DL_PKA_STATUS_SUCCESS) {
                        status = DL_PKA_STATUS_FAILURE;
                    }
                    if (status == DL_PKA_STATUS_SUCCESS) {
                        /* tmp2 = tmp % prime to ensure we have no fraction in the division.
                           The number will only shrink from here on out. */
                        DL_PKABigNumModStart(pka, scratchBuffer, resultLength,
                            prime, length, &resultAddress);

                        while (DL_PKAGetOpsStatus(pka) ==
                               DL_PKA_STATUS_OPERATION_BUSY) {
                        }

                        /* If the result is not a multiple of the word-length, the PKA HW will round up
                           because it deals in words only. That means that using 'length' directly
                           would cause and underflow, since length refers to the actual length in bytes of
                           the curve parameters while the PKA HW reports that rounded up to the next
                           word boundary.
                           Use 200 as the resultLength instead since we are copying to the scratch buffer
                           anyway.
                           Practically, this only happens with curves such as NIST-P521 that are not word
                           multiples. */
                        resultLength = 200U;
                        pkaResult    = (uint32_t) DL_PKABigNumModGetResult(
                            pka, scratchBuffer2, resultLength, resultAddress);

                        if (pkaResult != (uint32_t) DL_PKA_STATUS_SUCCESS) {
                            status = DL_PKA_STATUS_FAILURE;
                        }
                        if (status == DL_PKA_STATUS_SUCCESS) {
                            /* tmp = y^2 */
                            DL_PKABigNumMultiplyStart(pka, curvePointY, length,
                                curvePointY, length, &resultAddress);

                            while (DL_PKAGetOpsStatus(pka) ==
                                   DL_PKA_STATUS_OPERATION_BUSY) {
                            }

                            resultLength = 200U;
                            pkaResult = (uint32_t) DL_PKABigNumMultGetResult(
                                pka, scratchBuffer, &resultLength,
                                resultAddress);

                            if (pkaResult !=
                                (uint32_t) DL_PKA_STATUS_SUCCESS) {
                                status = DL_PKA_STATUS_FAILURE;
                            }

                            if (status == DL_PKA_STATUS_SUCCESS) {
                                /* tmp %= prime */
                                DL_PKABigNumModStart(pka, scratchBuffer,
                                    resultLength, prime, length,
                                    &resultAddress);

                                while (DL_PKAGetOpsStatus(pka) ==
                                       DL_PKA_STATUS_OPERATION_BUSY) {
                                }

                                /* If the result is not a multiple of the word-length, the PKA HW will round up
                                   because it deals in words only. That means that using 'length' directly
                                   would cause and underflow, since length refers to the actual length in bytes of
                                   the curve parameters while the PKA HW reports that rounded up to the next
                                   word boundary.
                                   Use 200 as the resultLength instead since we are copying to the scratch buffer
                                   anyway.
                                   Practically, this only happens with curves such as NIST-P521 that are not word
                                   multiples. */
                                resultLength = 200U;
                                pkaResult =
                                    (uint32_t) DL_PKABigNumModGetResult(pka,
                                        scratchBuffer, resultLength,
                                        resultAddress);

                                if (pkaResult !=
                                    (uint32_t) DL_PKA_STATUS_SUCCESS) {
                                    status = DL_PKA_STATUS_FAILURE;
                                }

                                if (status == DL_PKA_STATUS_SUCCESS) {
                                    /* tmp ?= tmp2 */
                                    DL_PKABigNumCmpStart(pka, scratchBuffer,
                                        scratchBuffer2, length);

                                    while (DL_PKAGetOpsStatus(pka) ==
                                           DL_PKA_STATUS_OPERATION_BUSY) {
                                    }

                                    pkaResult =
                                        (uint32_t) DL_PKABigNumCmpGetResult(
                                            pka);

                                    if (pkaResult !=
                                        (uint32_t) DL_PKA_STATUS_EQUAL) {
                                        status =
                                            DL_PKA_STATUS_POINT_NOT_ON_CURVE;
                                    } else {
                                        status = DL_PKA_STATUS_SUCCESS;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return status;
}

/*
*
* Clear PKA Data RAM
*
*/

void DL_PKAClearDataRam(PKA_Regs* pka)
{
    DL_PKAZeroOutArray(
        (uint8_t*) &pka->PKA_REGS.PKA_RAM_DATA_0_, PKA_RAM_TOT_BYTE_SIZE);
}

/*
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
void DL_PKA_resetStickyFlagClear(PKA_Regs* pka)
{
    DL_Common_updateReg(&pka->GPRCM.RSTCTL,
        (PKA_RSTCTL_KEY_UNLOCK_W | PKA_RSTCTL_RESETSTKYCLR_CLR),
        (PKA_RSTCTL_KEY_MASK | PKA_RSTCTL_RESETSTKYCLR_MASK));
}

/*
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
bool DL_PKA_getResetStatus(PKA_Regs* pka)
{
    return ((pka->GPRCM.STAT & PKA_STAT_RESETSTKY_MASK) ==
            PKA_STAT_RESETSTKY_RESET);
}
