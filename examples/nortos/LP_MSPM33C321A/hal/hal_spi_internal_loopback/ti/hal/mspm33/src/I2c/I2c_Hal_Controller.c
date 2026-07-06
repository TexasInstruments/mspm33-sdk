
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

#define I2C_HAL_CONTROLLER_INTR_MASK                                       \
    (DL_I2CC_INTERRUPT_RXFIFO_TRIGGER | DL_I2CC_INTERRUPT_TXFIFO_TRIGGER | \
        DL_I2CC_INTERRUPT_STOP)

#define I2C_HAL_CONTROLLER_INPUT_CLOCK_FREQ_HZ (80000000U)

#define I2C_HAL_CONTROLLER_INTR_ERROR_MASK \
    (DL_I2CC_INTERRUPT_NACK | DL_I2CC_INTERRUPT_TIMEOUT_A)

/* ========================================================================== */
/*                 Private Typedefs                                           */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Function Prototype                                 */
/* ========================================================================== */

static sint32 I2c_Hal_ControllerProcessRemainingBytes(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr, boolean isInterruptMode);
static sint32 I2c_Hal_ControllerErrorProcessing(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr);

/* ========================================================================== */
/*                 Private Variable Declaration                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

sint32 I2c_Hal_ControllerInitParamsSetDefault(
    I2c_Hal_ControllerInitParamsType* paramsPtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        paramsPtr->UserArgsPtr             = NULL_PTR;
        paramsPtr->HwRegsPtr               = NULL_PTR;
        paramsPtr->ClockConfig.clockSel    = DL_I2CC_CLOCK_BUSCLK;
        paramsPtr->ClockConfig.divideRatio = DL_I2CC_CLOCK_DIVIDE_1;
        paramsPtr->Speed                   = I2C_HAL_SPEED_STANDARD;
        paramsPtr->InputClockFreqHz = I2C_HAL_CONTROLLER_INPUT_CLOCK_FREQ_HZ;
    }

    return status;
}

sint32 I2c_Hal_ControllerInit(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    const I2c_Hal_ControllerInitParamsType* paramsPtr)
{
    sint32 status;
    uint32_t desiredFrequencyHz    = 0;
    uint32_t inputClockFrequencyHz = 0;
    uint8_t timePeriodRegVal       = 0;
    uint32_t divisor               = 0;
    uint32_t divisionResult        = 0;
    status                         = I2C_HAL_ERROR_NONE;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr || NULL_PTR == controllerInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        switch (paramsPtr->Speed) {
            case I2C_HAL_SPEED_STANDARD:
                desiredFrequencyHz = 100000U;
                break;

            case I2C_HAL_SPEED_FAST:
                desiredFrequencyHz = 400000U;
                break;

            case I2C_HAL_SPEED_FAST_PLUS:
                desiredFrequencyHz = 1000000U;
                break;

            default:
                status = I2C_HAL_ERROR_INVALID_PARAMS;
                break;
        }
    }
    if (I2C_HAL_ERROR_NONE == status) {
        inputClockFrequencyHz = paramsPtr->InputClockFreqHz;
        divisor               = desiredFrequencyHz *
                  (I2C_HAL_SCL_LOW_PERIOD + I2C_HAL_SCL_HIGH_PERIOD);

        if (divisor == 0U) {
            status = I2C_HAL_ERROR_INVALID_PARAMS;
        } else {
            divisionResult = inputClockFrequencyHz / divisor;

            if (divisionResult == 0U) {
                status = I2C_HAL_ERROR_INVALID_PARAMS;
            } else {
                timePeriodRegVal = (uint8_t)(divisionResult - 1U);
            }
        }
    }

    if (I2C_HAL_ERROR_NONE == status) {
        if (timePeriodRegVal > 127U || timePeriodRegVal == 0U) {
            status = I2C_HAL_ERROR_INVALID_PARAMS;
        }
    }

    if (I2C_HAL_ERROR_NONE == status) {
        controllerInstancePtr->HwRegsPtr            = paramsPtr->HwRegsPtr;
        controllerInstancePtr->UserArgsPtr          = paramsPtr->UserArgsPtr;
        controllerInstancePtr->BufferPtr            = NULL_PTR;
        controllerInstancePtr->SizeInBytes          = 0;
        controllerInstancePtr->RemainingSizeInBytes = 0;

        DL_I2CC_reset(controllerInstancePtr->HwRegsPtr);

        if (FALSE ==
            DL_I2CC_isPowerEnabled(controllerInstancePtr->HwRegsPtr)) {
            DL_I2CC_enablePower(controllerInstancePtr->HwRegsPtr);
        }

        DL_UNICOMM_setIPMode(
            controllerInstancePtr->HwRegsPtr, DL_UNICOMM_I2C_CONTROLLER);

        DL_I2CC_disable(controllerInstancePtr->HwRegsPtr);

        DL_I2CC_ClockConfig clockConfig = {.clockSel = DL_I2CC_CLOCK_BUSCLK,
            .divideRatio                             = DL_I2CC_CLOCK_DIVIDE_1};
        DL_I2CC_setClockConfig(
            controllerInstancePtr->HwRegsPtr, &(clockConfig));

        DL_I2CC_setTimerPeriod(
            controllerInstancePtr->HwRegsPtr, timePeriodRegVal);

        DL_I2CC_flushTXFIFO(controllerInstancePtr->HwRegsPtr);
        DL_I2CC_flushRXFIFO(controllerInstancePtr->HwRegsPtr);

        DL_I2CC_disableInterrupt(controllerInstancePtr->HwRegsPtr,
            DL_I2CC_INTERRUPT_RXFIFO_TRIGGER |
                DL_I2CC_INTERRUPT_TXFIFO_TRIGGER);

        DL_I2CC_setTXFIFOThreshold(
            controllerInstancePtr->HwRegsPtr, DL_I2CC_TX_FIFO_LEVEL_1_2_EMPTY);
        DL_I2CC_setRXFIFOThreshold(
            controllerInstancePtr->HwRegsPtr, DL_I2CC_RX_FIFO_LEVEL_1_4_FULL);

        DL_I2CC_enableTimeoutA(controllerInstancePtr->HwRegsPtr);

        DL_I2CC_enableClockStretching(controllerInstancePtr->HwRegsPtr);

        DL_I2CC_enable(controllerInstancePtr->HwRegsPtr);

        controllerInstancePtr->CurrentState = I2C_HAL_STATE_READY;
    }
    return status;
}

sint32 I2c_Hal_ControllerTransferPolling(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    const I2c_Hal_ControllerTransactionType* controllerTransactionPtr)
{
    I2c_Hal_TransactionStatusType transferStatus;
    sint32 status = I2c_Hal_ControllerTransferSetup(controllerInstancePtr,
        controllerTransactionPtr, &transferStatus, false);

    if (I2C_HAL_ERROR_NONE == status) {
        status = I2c_Hal_ControllerProcessRemainingBytes(
            controllerInstancePtr, &transferStatus, FALSE);
    }

    return status;
}

sint32 I2c_Hal_ControllerTransferInterrupt(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    const I2c_Hal_ControllerTransactionType* controllerTransactionPtr)
{
    I2c_Hal_TransactionStatusType transferStatus;
    sint32 status = I2c_Hal_ControllerTransferSetup(controllerInstancePtr,
        controllerTransactionPtr, &transferStatus, true);

    if (I2C_HAL_ERROR_NONE == status) {
        status = I2c_Hal_ControllerProcessRemainingBytes(
            controllerInstancePtr, &transferStatus, TRUE);
    }

    return status;
}

sint32 I2c_Hal_ControllerTransferContinuePolling(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status;

    status = I2C_HAL_ERROR_NONE;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr || NULL_PTR == transferStatusPtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        status = I2c_Hal_ControllerProcessRemainingBytes(
            controllerInstancePtr, transferStatusPtr, FALSE);
        if (I2C_HAL_ERROR_NONE == status) {
            if (I2C_HAL_ERROR_NONE == status) {
                transferStatusPtr->BufferPtr =
                    controllerInstancePtr->BufferPtr;
                transferStatusPtr->SizeInBytes =
                    controllerInstancePtr->SizeInBytes;
                transferStatusPtr->RemainingSizeInBytes =
                    controllerInstancePtr->RemainingSizeInBytes;

                if (transferStatusPtr->IsTransferComplete) {
                    controllerInstancePtr->CurrentState = I2C_HAL_STATE_READY;
                }
            }
        }
    }

    return status;
}

sint32 I2c_Hal_GetControllerCurrentState(
    const I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    I2c_Hal_StateType* currentStatePtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr || NULL_PTR == currentStatePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        *currentStatePtr = controllerInstancePtr->CurrentState;
    }

    return status;
}

sint32 I2c_Hal_ControllerTransferAbort(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr, uint32 timeoutCount,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        transferStatusPtr->SizeInBytes = controllerInstancePtr->SizeInBytes;
        transferStatusPtr->RemainingSizeInBytes =
            controllerInstancePtr->SizeInBytes;
        transferStatusPtr->BufferPtr     = controllerInstancePtr->BufferPtr;
        transferStatusPtr->ErrorCodeMask = I2C_HAL_TRANSFER_ERROR_NONE;

        DL_I2CC_disableInterrupt(
            controllerInstancePtr->HwRegsPtr, I2C_HAL_CONTROLLER_INTR_MASK);
        DL_I2CC_clearInterruptStatus(
            controllerInstancePtr->HwRegsPtr, I2C_HAL_CONTROLLER_INTR_MASK);

        DL_I2CC_flushRXFIFO(controllerInstancePtr->HwRegsPtr);
        DL_I2CC_flushTXFIFO(controllerInstancePtr->HwRegsPtr);

        DL_I2CC_resetTransfer(controllerInstancePtr->HwRegsPtr);

        controllerInstancePtr->CurrentState   = I2C_HAL_STATE_READY;
        transferStatusPtr->IsTransferComplete = FALSE;

        volatile uint32_t timeoutCounter = 0U;
        volatile boolean timedOut        = TRUE;

        for (; timeoutCounter < timeoutCount;) {
            volatile boolean isBusy =
                DL_I2CC_getStatus(controllerInstancePtr->HwRegsPtr) &
                DL_I2CC_STATUS_BUSY;

            if (!isBusy) {
                timedOut = FALSE;
                break;
            }
            if (timeoutCounter < UINT32_MAX) {
                timeoutCounter = (uint32_t)((uint64_t) timeoutCounter + 1);
            }
        }

        if (timedOut) {
            volatile boolean isBusy =
                DL_I2CC_getStatus(controllerInstancePtr->HwRegsPtr) &
                DL_I2CC_STATUS_BUSY;
            if (isBusy) {
                transferStatusPtr->ErrorCodeMask =
                    I2C_HAL_TRANSFER_ERROR_BUS_BUSY;
                status = I2C_HAL_ERROR_HW_BUSY;
            }
        }
    }
    return status;
}

sint32 I2c_Hal_ControllerDeInit(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr)
{
    sint32 status;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        DL_I2CC_disableInterrupt(
            controllerInstancePtr->HwRegsPtr, I2C_HAL_CONTROLLER_INTR_MASK);

        DL_I2CC_disable(controllerInstancePtr->HwRegsPtr);
        DL_UNICOMM_disablePower(controllerInstancePtr->HwRegsPtr);
        DL_UNICOMM_reset(controllerInstancePtr->HwRegsPtr);

        controllerInstancePtr->CurrentState = I2C_HAL_STATE_UNCONFIGURED;
    }

    return status;
}

/* ========================================================================== */
/*                 Private Functions                                          */
/* ========================================================================== */

static sint32 I2c_Hal_ControllerProcessRemainingBytes(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr, boolean isInterruptMode)
{
    sint32 status = I2C_HAL_ERROR_NONE;
    UNICOMM_Inst_Regs* unicomm;
    boolean isReceiveMode;
    volatile boolean isTransferInProgress;
    volatile uint32_t hwStatus;
    uint32_t controllerMode;

    unicomm = controllerInstancePtr->HwRegsPtr;

    hwStatus             = DL_I2CC_getStatus(unicomm);
    isTransferInProgress = (hwStatus & DL_I2CC_STATUS_BUSY);

    transferStatusPtr->SizeInBytes = controllerInstancePtr->SizeInBytes;
    transferStatusPtr->RemainingSizeInBytes =
        controllerInstancePtr->RemainingSizeInBytes;
    transferStatusPtr->BufferPtr = controllerInstancePtr->BufferPtr;

    controllerMode = controllerInstancePtr->CurrentMode;
    switch (controllerMode) {
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
    status = I2c_Hal_ControllerErrorProcessing(
        controllerInstancePtr, transferStatusPtr);

    if (transferStatusPtr->ErrorCodeMask != I2C_HAL_TRANSFER_ERROR_NONE) {
        status = I2C_HAL_ERROR_TRANSFER;
    }

    if (I2C_HAL_ERROR_NONE == status) {
        if (controllerInstancePtr->CurrentState != I2C_HAL_STATE_TRANSFER) {
            controllerInstancePtr->CurrentState = I2C_HAL_STATE_TRANSFER;
        }

        if (!isReceiveMode) {
            uint16_t filledBytes = 0;
            boolean isFirstTransfer =
                (controllerInstancePtr->SizeInBytes ==
                    controllerInstancePtr->RemainingSizeInBytes);

            if (!DL_I2CC_isTXFIFOFull(unicomm) &&
                controllerInstancePtr->RemainingSizeInBytes > 0) {
                uint8_t* currentBufferPtr =
                    controllerInstancePtr->BufferPtr +
                    (controllerInstancePtr->SizeInBytes -
                        controllerInstancePtr->RemainingSizeInBytes);

                filledBytes = DL_I2CC_fillTXFIFO(unicomm, currentBufferPtr,
                    controllerInstancePtr->RemainingSizeInBytes);

                if (filledBytes >
                    controllerInstancePtr->RemainingSizeInBytes) {
                    controllerInstancePtr->RemainingSizeInBytes = 0U;
                } else {
                    controllerInstancePtr->RemainingSizeInBytes = (uint32_t)(
                        (uint64_t)
                            controllerInstancePtr->RemainingSizeInBytes -
                        (uint64_t) filledBytes);
                }
            }

            if (!isTransferInProgress && filledBytes > 0) {
                DL_I2CC_START startCondition = isFirstTransfer
                                                   ? DL_I2CC_START_ENABLE
                                                   : DL_I2CC_START_DISABLE;
                DL_I2CC_STOP stopCondition = DL_I2CC_STOP_ENABLE;

                uint16_t targetAddress = DL_I2CC_getTargetAddress(unicomm);

                DL_I2CC_startTransferAdvanced(unicomm, targetAddress,
                    DL_I2CC_DIRECTION_TX, controllerInstancePtr->SizeInBytes,
                    startCondition, stopCondition, DL_I2CC_ACK_DISABLE);
            }

            volatile boolean isTransferComplete =
                (controllerInstancePtr->RemainingSizeInBytes == 0);
            volatile boolean isFIFOEmpty = DL_I2CC_isTXFIFOEmpty(unicomm);
            volatile boolean isBusy      = FALSE;

            if (!isInterruptMode) {
                isBusy = (DL_I2CC_getStatus(unicomm) & DL_I2CC_STATUS_BUSY);
            }

            transferStatusPtr->IsTransferComplete =
                (isTransferComplete && isFIFOEmpty && !isBusy);
        } else {
            if (!DL_I2CC_isRXFIFOEmpty(unicomm) &&
                controllerInstancePtr->RemainingSizeInBytes > 0) {
                uint8_t* currentBufferPtr =
                    controllerInstancePtr->BufferPtr +
                    (controllerInstancePtr->SizeInBytes -
                        controllerInstancePtr->RemainingSizeInBytes);

                while (!DL_I2CC_isRXFIFOEmpty(unicomm) &&
                       controllerInstancePtr->RemainingSizeInBytes > 0) {
                    *currentBufferPtr = DL_I2CC_receiveData(unicomm);
                    currentBufferPtr++;
                    controllerInstancePtr->RemainingSizeInBytes--;
                }
            }

            if (!isTransferInProgress &&
                controllerInstancePtr->RemainingSizeInBytes > 0) {
                boolean isFirstTransfer =
                    (controllerInstancePtr->SizeInBytes ==
                        controllerInstancePtr->RemainingSizeInBytes);
                DL_I2CC_START startCondition = isFirstTransfer
                                                   ? DL_I2CC_START_ENABLE
                                                   : DL_I2CC_START_DISABLE;
                DL_I2CC_STOP stopCondition = DL_I2CC_STOP_ENABLE;

                uint16_t targetAddress = DL_I2CC_getTargetAddress(unicomm);

                DL_I2CC_startTransferAdvanced(unicomm, targetAddress,
                    DL_I2CC_DIRECTION_RX,
                    controllerInstancePtr->RemainingSizeInBytes,
                    startCondition, stopCondition, DL_I2CC_ACK_DISABLE);
            }

            transferStatusPtr->RemainingSizeInBytes =
                controllerInstancePtr->RemainingSizeInBytes;

            volatile boolean isTransferComplete =
                (controllerInstancePtr->RemainingSizeInBytes == 0);
            volatile boolean isRXFIFOEmpty = DL_I2CC_isRXFIFOEmpty(unicomm);
            volatile boolean isBusy =
                (DL_I2CC_getStatus(unicomm) & DL_I2CC_STATUS_BUSY);

            transferStatusPtr->IsTransferComplete =
                (isTransferComplete && isRXFIFOEmpty && !isBusy);
        }

        if (transferStatusPtr->IsTransferComplete) {
            controllerInstancePtr->CurrentState = I2C_HAL_STATE_READY;
        }
    }

    return status;
}

sint32 I2c_Hal_ControllerErrorProcessing(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr,
    I2c_Hal_TransactionStatusType* transferStatusPtr)
{
    sint32 status;
    UNICOMM_Inst_Regs* unicomm;
    status = I2C_HAL_ERROR_NONE;
#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif
    if (I2C_HAL_ERROR_NONE == status) {
        uint32_t errorMask          = I2C_HAL_TRANSFER_ERROR_NONE;
        unicomm                     = controllerInstancePtr->HwRegsPtr;
        uint32_t rawInterruptStatus = DL_I2CC_getRawInterruptStatus(
            unicomm, I2C_HAL_CONTROLLER_INTR_ERROR_MASK);
        if (rawInterruptStatus & DL_I2CC_INTERRUPT_NACK) {
            DL_I2CC_clearInterruptStatus(unicomm, DL_I2CC_INTERRUPT_NACK);
            errorMask |= I2C_HAL_TRANSFER_ERROR_TARGET_NACK;
        }
        if (rawInterruptStatus & DL_I2CC_INTERRUPT_TIMEOUT_A) {
            DL_I2CC_clearInterruptStatus(unicomm, DL_I2CC_INTERRUPT_TIMEOUT_A);
            errorMask |= I2C_HAL_TRANSFER_ERROR_CLOCK_LOW_TIMEOUT;
        }
        transferStatusPtr->ErrorCodeMask = errorMask;
    }

    return status;
}

void I2c_Hal_ControllerInterruptHandler(
    I2c_Hal_ControllerInstanceType* controllerInstancePtr)
{
    sint32 status;
    uint32_t interruptStatus;
    UNICOMM_Inst_Regs* unicomm;
    I2c_Hal_TransactionStatusType transferStatus;

    status = I2C_HAL_ERROR_NONE;

#if I2C_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == controllerInstancePtr) {
        status = I2C_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (I2C_HAL_ERROR_NONE == status) {
        transferStatus.RemainingSizeInBytes =
            controllerInstancePtr->RemainingSizeInBytes;
        unicomm         = controllerInstancePtr->HwRegsPtr;
        interruptStatus = DL_I2CC_getPendingInterrupt(unicomm);
        switch (interruptStatus) {
            case DL_I2CC_IIDX_TXFIFO_TRIGGER:
                if (controllerInstancePtr->RemainingSizeInBytes > 0) {
                    status = I2c_Hal_ControllerProcessRemainingBytes(
                        controllerInstancePtr, &transferStatus, TRUE);
                    if (I2C_HAL_ERROR_NONE != status) {
                        I2c_Hal_ControllerErrorCallback(controllerInstancePtr,
                            &transferStatus,
                            controllerInstancePtr->UserArgsPtr);

                        break;
                    }
                }
                if (controllerInstancePtr->RemainingSizeInBytes == 0) {
                    DL_I2CC_disableInterrupt(
                        unicomm, DL_I2CC_INTERRUPT_TXFIFO_TRIGGER);
                }
                break;

            case DL_I2CC_IIDX_RXFIFO_TRIGGER:

                if (I2C_HAL_OPERATING_MODE_RECEIVE ==
                    controllerInstancePtr->CurrentMode) {
                    status = I2c_Hal_ControllerProcessRemainingBytes(
                        controllerInstancePtr, &transferStatus, TRUE);
                    if (I2C_HAL_ERROR_NONE != status) {
                        I2c_Hal_ControllerErrorCallback(controllerInstancePtr,
                            &transferStatus,
                            controllerInstancePtr->UserArgsPtr);

                        break;
                    }
                }

                if (transferStatus.RemainingSizeInBytes == 0) {
                    DL_I2CC_disableInterrupt(
                        unicomm, DL_I2CC_INTERRUPT_RXFIFO_TRIGGER);
                }
                break;

            case DL_I2CC_IIDX_STOP:

                status = I2c_Hal_ControllerProcessRemainingBytes(
                    controllerInstancePtr, &transferStatus, TRUE);
                if (I2C_HAL_ERROR_NONE != status) {
                    I2c_Hal_ControllerErrorCallback(controllerInstancePtr,
                        &transferStatus, controllerInstancePtr->UserArgsPtr);

                    break;
                }

                DL_I2CC_disableInterrupt(unicomm, DL_I2CC_INTERRUPT_STOP);

                if (transferStatus.RemainingSizeInBytes == 0) {
                    DL_I2CC_disableInterrupt(
                        unicomm, DL_I2CC_INTERRUPT_TXFIFO_TRIGGER |
                                     DL_I2CC_INTERRUPT_RXFIFO_TRIGGER);
                    transferStatus.IsTransferComplete = TRUE;
                    I2c_Hal_ControllerTransferCompleteCallback(
                        controllerInstancePtr, &transferStatus,
                        controllerInstancePtr->UserArgsPtr);
                }
                break;

            default:
                break;
        }
    }
}
