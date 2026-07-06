
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
#include "I2c_Hal_Common.h"

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

#define I2C_HAL_TARGET_INTR_MASK                                           \
    (DL_I2CT_INTERRUPT_RXFIFO_TRIGGER | DL_I2CT_INTERRUPT_TXFIFO_TRIGGER | \
        DL_I2CT_INTERRUPT_STOP)

#define I2C_HAL_TARGET_DEFAULT_ADDRESS (0x50)

#define I2C_HAL_TARGET_INTR_ERROR_MASK                                 \
    (DL_I2CT_INTERRUPT_TIMEOUT_A | DL_I2CT_INTERRUPT_RXFIFO_OVERFLOW | \
        DL_I2CT_INTERRUPT_TXFIFO_UNDERFLOW)

/* ========================================================================== */
/*                 Private Typedefs                                           */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Function Prototype                                 */
/* ========================================================================== */

static sint32 I2c_Hal_TargetProcessRemainingBytes(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr);
static sint32 I2c_Hal_TargetErrorProcessing(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr);

/* ========================================================================== */
/*                 Private Variable Declaration                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

sint32 I2c_Hal_TargetInitParamsSetDefault(
    I2c_Hal_TargetInitParamsType* paramsPtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        paramsPtr->ClockConfig.clockSel    = DL_I2CT_CLOCK_BUSCLK;
        paramsPtr->ClockConfig.divideRatio = DL_I2CT_CLOCK_DIVIDE_1;
        paramsPtr->UserArgsPtr             = NULL_PTR;
        paramsPtr->HwRegsPtr               = NULL_PTR;
        paramsPtr->OwnAddress              = I2C_HAL_TARGET_DEFAULT_ADDRESS;
    }

    return status;
}

sint32 I2c_Hal_TargetInit(I2c_Hal_TargetInstanceType* targetInstancePtr,
    const I2c_Hal_TargetInitParamsType* paramsPtr)
{
    sint32 status;
    uint16_t ownAddress = 0;
    status              = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr || NULL_PTR == targetInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        targetInstancePtr->HwRegsPtr            = paramsPtr->HwRegsPtr;
        targetInstancePtr->UserArgsPtr          = paramsPtr->UserArgsPtr;
        ownAddress                              = paramsPtr->OwnAddress;
        targetInstancePtr->BufferPtr            = NULL_PTR;
        targetInstancePtr->SizeInBytes          = 0;
        targetInstancePtr->RemainingSizeInBytes = 0;

        DL_I2CT_reset(targetInstancePtr->HwRegsPtr);

        if (FALSE == DL_I2CT_isPowerEnabled(targetInstancePtr->HwRegsPtr)) {
            DL_I2CT_enablePower(targetInstancePtr->HwRegsPtr);
        }

        DL_UNICOMM_setIPMode(
            targetInstancePtr->HwRegsPtr, DL_UNICOMM_I2C_TARGET);

        DL_I2CT_disable(targetInstancePtr->HwRegsPtr);

        DL_I2CT_ClockConfig clockConfig = {.clockSel = DL_I2CT_CLOCK_BUSCLK,
            .divideRatio                             = DL_I2CT_CLOCK_DIVIDE_1};
        DL_I2CT_setClockConfig(targetInstancePtr->HwRegsPtr, &(clockConfig));

        DL_I2CT_setOwnAddress(targetInstancePtr->HwRegsPtr, ownAddress);

        DL_I2CT_flushTXFIFO(targetInstancePtr->HwRegsPtr);
        DL_I2CT_flushRXFIFO(targetInstancePtr->HwRegsPtr);

        DL_I2CT_disableInterrupt(targetInstancePtr->HwRegsPtr,
            DL_I2CT_INTERRUPT_RXFIFO_TRIGGER |
                DL_I2CT_INTERRUPT_TXFIFO_TRIGGER);

        DL_I2CT_setTXFIFOThreshold(
            targetInstancePtr->HwRegsPtr, DL_I2CT_TX_FIFO_LEVEL_1_2_EMPTY);
        DL_I2CT_setRXFIFOThreshold(
            targetInstancePtr->HwRegsPtr, DL_I2CT_RX_FIFO_LEVEL_1_4_FULL);

        DL_I2CT_enableTimeoutA(targetInstancePtr->HwRegsPtr);

        DL_I2CT_enableClockStretching(targetInstancePtr->HwRegsPtr);

        /* Workaround for errata I2C_ERR_04 */
        DL_I2CT_disableWakeup(targetInstancePtr->HwRegsPtr);

        DL_I2CT_enable(targetInstancePtr->HwRegsPtr);

        targetInstancePtr->CurrentState = I2C_HAL_STATE_READY;
    }

    return status;
}

sint32 I2c_Hal_TargetTransferPolling(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TargetTransactionType* targetTransactionPtr)
{
    I2c_Hal_TransactionStatusType transferStatus;
    sint32 status = I2c_Hal_TargetTransferSetup(
        targetInstancePtr, targetTransactionPtr, &transferStatus, false);

    if (I2C_HAL_ERROR_NONE == status) {
        status = I2c_Hal_TargetProcessRemainingBytes(
            targetInstancePtr, &transferStatus);
    }

    return status;
}

sint32 I2c_Hal_TargetTransferInterrupt(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TargetTransactionType* targetTransactionPtr)
{
    I2c_Hal_TransactionStatusType transferStatus;
    sint32 status = I2c_Hal_TargetTransferSetup(
        targetInstancePtr, targetTransactionPtr, &transferStatus, true);

    if (I2C_HAL_ERROR_NONE == status) {
        status = I2c_Hal_TargetProcessRemainingBytes(
            targetInstancePtr, &transferStatus);
    }

    return status;
}

sint32 I2c_Hal_TargetTransferContinuePolling(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr || NULL_PTR == transferStatusPtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        status = I2c_Hal_TargetProcessRemainingBytes(
            targetInstancePtr, transferStatusPtr);

        if (I2C_HAL_ERROR_NONE == status) {
            transferStatusPtr->SizeInBytes = targetInstancePtr->SizeInBytes;
            transferStatusPtr->RemainingSizeInBytes =
                targetInstancePtr->RemainingSizeInBytes;
            transferStatusPtr->BufferPtr = targetInstancePtr->BufferPtr;

            if (transferStatusPtr->IsTransferComplete) {
                targetInstancePtr->CurrentState = I2C_HAL_STATE_READY;
            }
        }
    }
    return status;
}

sint32 I2c_Hal_GetTargetCurrentState(
    const I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_StateType* currentStatePtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr || NULL_PTR == currentStatePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        *currentStatePtr = targetInstancePtr->CurrentState;
    }

    return status;
}

sint32 I2c_Hal_TargetTransferAbort(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        transferStatusPtr->SizeInBytes = targetInstancePtr->SizeInBytes;
        transferStatusPtr->RemainingSizeInBytes =
            targetInstancePtr->SizeInBytes;
        transferStatusPtr->BufferPtr     = targetInstancePtr->BufferPtr;
        transferStatusPtr->ErrorCodeMask = I2C_HAL_TRANSFER_ERROR_NONE;

        DL_I2CT_disableInterrupt(targetInstancePtr->HwRegsPtr,
            I2C_HAL_TARGET_INTR_MASK | I2C_HAL_TARGET_INTR_ERROR_MASK);
        DL_I2CT_clearInterruptStatus(targetInstancePtr->HwRegsPtr,
            I2C_HAL_TARGET_INTR_MASK | I2C_HAL_TARGET_INTR_ERROR_MASK);

        DL_I2CT_flushRXFIFO(targetInstancePtr->HwRegsPtr);
        DL_I2CT_flushTXFIFO(targetInstancePtr->HwRegsPtr);

        targetInstancePtr->CurrentState       = I2C_HAL_STATE_READY;
        transferStatusPtr->IsTransferComplete = FALSE;
    }

    return status;
}

sint32 I2c_Hal_TargetDeInit(I2c_Hal_TargetInstanceType* targetInstancePtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        DL_I2CT_disableInterrupt(
            targetInstancePtr->HwRegsPtr, I2C_HAL_TARGET_INTR_MASK);

        DL_I2CT_disable(targetInstancePtr->HwRegsPtr);
        DL_UNICOMM_disablePower(targetInstancePtr->HwRegsPtr);
        DL_UNICOMM_reset(targetInstancePtr->HwRegsPtr);

        targetInstancePtr->CurrentState = I2C_HAL_STATE_UNCONFIGURED;
    }
    return status;
}

/* ========================================================================== */
/*                 Private Functions                                          */
/* ========================================================================== */

static sint32 I2c_Hal_TargetProcessRemainingBytes(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status = I2C_HAL_ERROR_NONE;
    UNICOMM_Inst_Regs* unicomm;
    boolean isReceiveMode;
    uint32_t errorCodeMask = I2C_HAL_TRANSFER_ERROR_NONE;
    uint32_t targetMode;

    unicomm = targetInstancePtr->HwRegsPtr;

    targetMode = targetInstancePtr->CurrentMode;
    switch (targetMode) {
        case I2C_HAL_OPERATING_MODE_RECEIVE:
            isReceiveMode = TRUE;
            break;
        case I2C_HAL_OPERATING_MODE_TRANSMIT:
            isReceiveMode = FALSE;
            break;
        default:
            status = I2C_HAL_ERROR_INVALID_PARAMS;
            break;
    }
    if (I2C_HAL_ERROR_NONE == status) {
        transferStatusPtr->ErrorCodeMask = errorCodeMask;

        status = I2c_Hal_TargetErrorProcessing(
            targetInstancePtr, transferStatusPtr);

        if (transferStatusPtr->ErrorCodeMask != I2C_HAL_TRANSFER_ERROR_NONE) {
            status = I2C_HAL_ERROR_TRANSFER;
        }

        if (I2C_HAL_ERROR_NONE == status) {
            transferStatusPtr->IsTransferComplete = FALSE;

            if (isReceiveMode) {
                if (!DL_I2CT_isRXFIFOEmpty(unicomm) &&
                    targetInstancePtr->RemainingSizeInBytes > 0) {
                    uint8_t* currentBufferPtr =
                        targetInstancePtr->BufferPtr +
                        (targetInstancePtr->SizeInBytes -
                            targetInstancePtr->RemainingSizeInBytes);

                    uint8_t* rxBufferEndPtr = targetInstancePtr->BufferPtr +
                                              targetInstancePtr->SizeInBytes;

                    while (!DL_I2CT_isRXFIFOEmpty(unicomm) &&
                           targetInstancePtr->RemainingSizeInBytes > 0 &&
                           currentBufferPtr < rxBufferEndPtr) {
                        *currentBufferPtr = DL_I2CT_receiveData(unicomm);
                        currentBufferPtr++;
                        targetInstancePtr->RemainingSizeInBytes--;
                    }
                }

                transferStatusPtr->RemainingSizeInBytes =
                    targetInstancePtr->RemainingSizeInBytes;

                boolean isStopDetected =
                    (DL_I2CT_getRawInterruptStatus(
                         unicomm, DL_I2CT_INTERRUPT_STOP) != 0);

                transferStatusPtr->IsTransferComplete =
                    (targetInstancePtr->RemainingSizeInBytes == 0 &&
                        isStopDetected);
            } else {
                uint8_t* txBufferEndPtr = targetInstancePtr->BufferPtr +
                                          targetInstancePtr->SizeInBytes;

                uint8_t* currentBufferPtr =
                    targetInstancePtr->BufferPtr +
                    (targetInstancePtr->SizeInBytes -
                        targetInstancePtr->RemainingSizeInBytes);

                if (targetInstancePtr->RemainingSizeInBytes > 0 &&
                    currentBufferPtr < txBufferEndPtr) {
                    uint16_t filledBytes =
                        DL_I2CT_fillTXFIFO(unicomm, currentBufferPtr,
                            targetInstancePtr->RemainingSizeInBytes);

                    targetInstancePtr->RemainingSizeInBytes -= filledBytes;
                }

                transferStatusPtr->RemainingSizeInBytes =
                    targetInstancePtr->RemainingSizeInBytes;

                boolean isStopDetected =
                    (DL_I2CT_getRawInterruptStatus(
                         unicomm, DL_I2CT_INTERRUPT_STOP) != 0);

                transferStatusPtr->IsTransferComplete =
                    (isStopDetected &&
                        targetInstancePtr->RemainingSizeInBytes == 0) ||
                    (targetInstancePtr->RemainingSizeInBytes == 0 &&
                        DL_I2CT_isTXFIFOEmpty(unicomm));
            }

            if (transferStatusPtr->IsTransferComplete) {
                targetInstancePtr->CurrentState = I2C_HAL_STATE_READY;
            }
        }
    }
    return status;
}

sint32 I2c_Hal_TargetErrorProcessing(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
    UNICOMM_Inst_Regs* unicomm;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        uint32_t errorMask          = I2C_HAL_TRANSFER_ERROR_NONE;
        unicomm                     = targetInstancePtr->HwRegsPtr;
        uint32_t rawInterruptStatus = DL_I2CT_getRawInterruptStatus(
            unicomm, I2C_HAL_TARGET_INTR_ERROR_MASK);
        if (rawInterruptStatus & DL_I2CT_INTERRUPT_TIMEOUT_A) {
            DL_I2CT_clearInterruptStatus(unicomm, DL_I2CT_INTERRUPT_TIMEOUT_A);
            errorMask |= I2C_HAL_TRANSFER_ERROR_CLOCK_LOW_TIMEOUT;
        }
        if (rawInterruptStatus & DL_I2CT_INTERRUPT_RXFIFO_OVERFLOW) {
            DL_I2CT_clearInterruptStatus(
                unicomm, DL_I2CT_INTERRUPT_RXFIFO_OVERFLOW);
            errorMask |= I2C_HAL_TRANSFER_ERROR_TARGET_RXFIFO_OVERFLOW;
        }
        if (rawInterruptStatus & DL_I2CT_INTERRUPT_TXFIFO_UNDERFLOW) {
            DL_I2CT_clearInterruptStatus(
                unicomm, DL_I2CT_INTERRUPT_TXFIFO_UNDERFLOW);
            errorMask |= I2C_HAL_TRANSFER_ERROR_TARGET_TXFIFO_UNDERFLOW;
        }
        transferStatusPtr->ErrorCodeMask = errorMask;
    }
    return status;
}

void I2c_Hal_TargetInterruptHandler(
    I2c_Hal_TargetInstanceType* targetInstancePtr)
{
    sint32 status;
    uint32_t interruptStatus;
    UNICOMM_Inst_Regs* unicomm;
    I2c_Hal_TransactionStatusType transferStatus;

    status = I2C_HAL_ERROR_NONE;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == targetInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        transferStatus.RemainingSizeInBytes =
            targetInstancePtr->RemainingSizeInBytes;
        unicomm         = targetInstancePtr->HwRegsPtr;
        interruptStatus = DL_I2CT_getPendingInterrupt(unicomm);
        switch (interruptStatus) {
            case DL_I2CT_IIDX_TXFIFO_TRIGGER:
                if (targetInstancePtr->RemainingSizeInBytes > 0) {
                    status = I2c_Hal_TargetProcessRemainingBytes(
                        targetInstancePtr, &transferStatus);
                    if (I2C_HAL_ERROR_NONE != status) {
                        I2c_Hal_TargetErrorCallback(targetInstancePtr,
                            &transferStatus, targetInstancePtr->UserArgsPtr);
                        break;
                    }
                }
                if (targetInstancePtr->RemainingSizeInBytes == 0) {
                    DL_I2CT_disableInterrupt(
                        unicomm, DL_I2CT_INTERRUPT_TXFIFO_TRIGGER);
                }
                break;

            case DL_I2CT_IIDX_RXFIFO_TRIGGER:

                if (I2C_HAL_OPERATING_MODE_RECEIVE ==
                    targetInstancePtr->CurrentMode) {
                    status = I2c_Hal_TargetProcessRemainingBytes(
                        targetInstancePtr, &transferStatus);
                    if (I2C_HAL_ERROR_NONE != status) {
                        I2c_Hal_TargetErrorCallback(targetInstancePtr,
                            &transferStatus, targetInstancePtr->UserArgsPtr);
                        break;
                    }
                }

                if (transferStatus.RemainingSizeInBytes == 0) {
                    DL_I2CT_disableInterrupt(
                        unicomm, DL_I2CT_INTERRUPT_RXFIFO_TRIGGER);
                }
                break;

            case DL_I2CT_IIDX_STOP:

                status = I2c_Hal_TargetProcessRemainingBytes(
                    targetInstancePtr, &transferStatus);
                if (I2C_HAL_ERROR_NONE != status) {
                    I2c_Hal_TargetErrorCallback(targetInstancePtr,
                        &transferStatus, targetInstancePtr->UserArgsPtr);
                    break;
                }

                DL_I2CT_disableInterrupt(unicomm, DL_I2CT_INTERRUPT_STOP);

                if (transferStatus.RemainingSizeInBytes == 0) {
                    DL_I2CT_disableInterrupt(
                        unicomm, DL_I2CT_INTERRUPT_TXFIFO_TRIGGER |
                                     DL_I2CT_INTERRUPT_RXFIFO_TRIGGER);

                    transferStatus.IsTransferComplete = TRUE;
                    I2c_Hal_TargetTransferCompleteCallback(targetInstancePtr,
                        &transferStatus, targetInstancePtr->UserArgsPtr);
                }

                break;

            default:
                break;
        }
    }
}

__WEAK sint32 I2c_Hal_TargetAddressMatchCallback(
    I2c_Hal_TargetInstanceType* targetInstancePtr,
    boolean isTransferDirectionRead, uint16_t matchedAddress,
    void* userArgsPtr)
{
    (void) targetInstancePtr;
    (void) isTransferDirectionRead;
    (void) matchedAddress;
    (void) userArgsPtr;

    return I2C_HAL_ERROR_NONE;
}
