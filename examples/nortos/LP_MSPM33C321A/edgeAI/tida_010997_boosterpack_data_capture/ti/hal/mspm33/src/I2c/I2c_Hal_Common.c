
/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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

#include <ti/hal/mspm33/include/I2c_Hal.h>

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

#define I2C_HAL_CONTROLLER_INTR_MASK                                       \
    (DL_I2CC_INTERRUPT_RXFIFO_TRIGGER | DL_I2CC_INTERRUPT_TXFIFO_TRIGGER | \
        DL_I2CC_INTERRUPT_STOP)

#define I2C_HAL_TARGET_INTR_MASK                                           \
    (DL_I2CT_INTERRUPT_RXFIFO_TRIGGER | DL_I2CT_INTERRUPT_TXFIFO_TRIGGER | \
        DL_I2CT_INTERRUPT_STOP)

/* ========================================================================== */
/*                 Private Typedefs                                           */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Function Prototype                                 */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Variable Declaration                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

sint32 I2c_Hal_ControllerTransferSetup(
    I2c_Hal_ControllerInstanceType *controllerInstancePtr,
    const I2c_Hal_ControllerTransactionType *controllerTransactionPtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, bool isInterruptMode)
{
    sint32 status = I2C_HAL_ERROR_NONE;

    transferStatusPtr->IsTransferComplete = FALSE;
    transferStatusPtr->ErrorCodeMask      = 0U;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr ||
        NULL_PTR == controllerTransactionPtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    } else
#endif
        if (I2C_HAL_STATE_READY != controllerInstancePtr->CurrentState) {
        status = I2C_HAL_ERROR_UNSUPPORTED_FEATURE;
    } else {
        uint32_t transferOptions = controllerTransactionPtr->TransferOptions;
        uint8_t *dataBufferPtr   = controllerTransactionPtr->BufferPtr;
        uint32_t dataSize        = controllerTransactionPtr->SizeInBytes;
        uint16_t targetAddress   = controllerTransactionPtr->TargetAddress;
        I2c_Hal_AddressingModeType addressingMode =
            controllerTransactionPtr->AddressingMode;

        transferStatusPtr->BufferPtr            = dataBufferPtr;
        transferStatusPtr->SizeInBytes          = dataSize;
        transferStatusPtr->RemainingSizeInBytes = dataSize;

        if (0U == dataSize || NULL_PTR == dataBufferPtr ||
            0U == targetAddress) {
            status = I2C_HAL_ERROR_INVALID_PARAMS;
        } else {
            if (addressingMode == I2C_HAL_ADDRESSING_MODE_7BIT) {
                if (targetAddress > 0x7FU) {
                    status = I2C_HAL_ERROR_INVALID_PARAMS;
                } else {
                    DL_I2CC_setAddressingMode(controllerInstancePtr->HwRegsPtr,
                        DL_I2CC_ADDRESSING_MODE_7_BIT);
                }
            } else if (addressingMode == I2C_HAL_ADDRESSING_MODE_10BIT) {
                if (targetAddress > 0x3FFU) {
                    status = I2C_HAL_ERROR_INVALID_PARAMS;
                } else {
                    DL_I2CC_setAddressingMode(controllerInstancePtr->HwRegsPtr,
                        DL_I2CC_ADDRESSING_MODE_10_BIT);
                }
            } else {
                status = I2C_HAL_ERROR_INVALID_PARAMS;
            }

            if (status == I2C_HAL_ERROR_NONE) {
                if (dataSize > 0xFFF) {
                    status = I2C_HAL_ERROR_INVALID_PARAMS;
                } else {
                    DL_I2CC_setTargetAddress(
                        controllerInstancePtr->HwRegsPtr, targetAddress);
                    controllerInstancePtr->BufferPtr   = dataBufferPtr;
                    controllerInstancePtr->SizeInBytes = dataSize;
                    controllerInstancePtr->RemainingSizeInBytes = dataSize;

                    if (0U !=
                        (transferOptions & I2C_HAL_TRANSFER_OPTION_READ)) {
                        DL_I2CC_setDirection(controllerInstancePtr->HwRegsPtr,
                            DL_I2CC_DIRECTION_RX);
                        controllerInstancePtr->CurrentMode =
                            I2C_HAL_OPERATING_MODE_RECEIVE;

                        if (isInterruptMode) {
                            DL_I2CC_enableInterrupt(
                                controllerInstancePtr->HwRegsPtr,
                                DL_I2CC_INTERRUPT_RXFIFO_TRIGGER |
                                    DL_I2CC_INTERRUPT_STOP);
                        } else {
                            DL_I2CC_disableInterrupt(
                                controllerInstancePtr->HwRegsPtr,
                                I2C_HAL_CONTROLLER_INTR_MASK);
                        }
                    } else if (0U != (transferOptions &
                                         I2C_HAL_TRANSFER_OPTION_WRITE)) {
                        DL_I2CC_setDirection(controllerInstancePtr->HwRegsPtr,
                            DL_I2CC_DIRECTION_TX);
                        controllerInstancePtr->CurrentMode =
                            I2C_HAL_OPERATING_MODE_TRANSMIT;

                        if (isInterruptMode) {
                            DL_I2CC_enableInterrupt(
                                controllerInstancePtr->HwRegsPtr,
                                DL_I2CC_INTERRUPT_TXFIFO_TRIGGER |
                                    DL_I2CC_INTERRUPT_STOP);
                        } else {
                            DL_I2CC_disableInterrupt(
                                controllerInstancePtr->HwRegsPtr,
                                I2C_HAL_CONTROLLER_INTR_MASK);
                        }
                    } else {
                        status = I2C_HAL_ERROR_INVALID_PARAMS;
                    }
                }
            }
        }
    }

    return status;
}

sint32 I2c_Hal_TargetTransferSetup(
    I2c_Hal_TargetInstanceType *targetInstancePtr,
    const I2c_Hal_TargetTransactionType *targetTransactionPtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, bool isInterruptMode)
{
    sint32 status = I2C_HAL_ERROR_NONE;

    transferStatusPtr->IsTransferComplete = FALSE;
    transferStatusPtr->ErrorCodeMask      = 0U;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr || NULL_PTR == targetTransactionPtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    } else
#endif
        if (I2C_HAL_STATE_READY != targetInstancePtr->CurrentState) {
        status = I2C_HAL_ERROR_UNSUPPORTED_FEATURE;
    } else {
        uint32_t transferOptions = targetTransactionPtr->TransferOptions;
        uint8_t *dataBufferPtr   = targetTransactionPtr->BufferPtr;
        uint32_t dataSize        = targetTransactionPtr->SizeInBytes;

        transferStatusPtr->BufferPtr            = dataBufferPtr;
        transferStatusPtr->SizeInBytes          = dataSize;
        transferStatusPtr->RemainingSizeInBytes = dataSize;

        if (0U == dataSize || NULL_PTR == dataBufferPtr) {
            status = I2C_HAL_ERROR_INVALID_PARAMS;
        } else {
            if (0U != (transferOptions &
                          I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE)) {
                DL_I2CT_enableClockStretching(targetInstancePtr->HwRegsPtr);
            } else {
                DL_I2CT_disableClockStretching(targetInstancePtr->HwRegsPtr);
            }

            targetInstancePtr->BufferPtr            = dataBufferPtr;
            targetInstancePtr->SizeInBytes          = dataSize;
            targetInstancePtr->RemainingSizeInBytes = dataSize;

            if (0U != (transferOptions & I2C_HAL_TRANSFER_OPTION_READ)) {
                targetInstancePtr->CurrentMode =
                    I2C_HAL_OPERATING_MODE_RECEIVE;

                if (isInterruptMode) {
                    DL_I2CT_enableInterrupt(targetInstancePtr->HwRegsPtr,
                        DL_I2CT_INTERRUPT_RXFIFO_TRIGGER |
                            DL_I2CT_INTERRUPT_STOP);
                } else {
                    DL_I2CT_disableInterrupt(targetInstancePtr->HwRegsPtr,
                        DL_I2CT_INTERRUPT_RXFIFO_TRIGGER);
                }
            } else {
                targetInstancePtr->CurrentMode =
                    I2C_HAL_OPERATING_MODE_TRANSMIT;

                if (isInterruptMode) {
                    DL_I2CT_enableInterrupt(targetInstancePtr->HwRegsPtr,
                        DL_I2CT_INTERRUPT_TXFIFO_TRIGGER |
                            DL_I2CT_INTERRUPT_STOP);
                } else {
                    DL_I2CT_disableInterrupt(targetInstancePtr->HwRegsPtr,
                        DL_I2CT_INTERRUPT_TXFIFO_TRIGGER);
                }
            }
        }
    }

    return status;
}

/* ========================================================================== */
/*                 Weak Callback Implementations                              */
/* ========================================================================== */

__WEAK void I2c_Hal_ControllerTransferCompleteCallback(
    I2c_Hal_ControllerInstanceType *controllerInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    (void) controllerInstancePtr;
    (void) transferStatusPtr;
    (void) userArgsPtr;
}

__WEAK void I2c_Hal_ControllerErrorCallback(
    I2c_Hal_ControllerInstanceType *controllerInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    (void) controllerInstancePtr;
    (void) transferStatusPtr;
    (void) userArgsPtr;
}

__WEAK void I2c_Hal_TargetTransferCompleteCallback(
    I2c_Hal_TargetInstanceType *targetInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    (void) targetInstancePtr;
    (void) transferStatusPtr;
    (void) userArgsPtr;
}

__WEAK void I2c_Hal_TargetErrorCallback(
    I2c_Hal_TargetInstanceType *targetInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    (void) targetInstancePtr;
    (void) transferStatusPtr;
    (void) userArgsPtr;
}

/* ========================================================================== */
/*                 Private Functions                                          */
/* ========================================================================== */
