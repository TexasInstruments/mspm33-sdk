
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

#include <ti/hal/mspm33/include/Spi_Hal.h>

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */
#define SPI_HAL_FIFO_SIZE (16U)
#define SPI_HAL_TX_DUMMY_DATA (0x00U)

/* ========================================================================== */
/*                 Private Typedefs                                           */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Function Prototype                                 */
/* ========================================================================== */
static sint32 Spi_Hal_StartTransfer(Spi_Hal_InstanceType *instancePtr,
    void *txBufferPtr, void *rxBufferPtr, uint32 size);
static void Spi_Hal_ReadWriteFIFO(Spi_Hal_InstanceType *instancePtr);

/* ========================================================================== */
/*                 Private Variable Declaration                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

sint32 Spi_Hal_InitParamsSetDefault(Spi_Hal_InitParamsType *paramsPtr)
{
    sint32 status;

    status = SPI_HAL_ERROR_NONE;
#if SPI_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* SPI_HAL_CFG_ERROR_CHECK == STD_ON */

    if (SPI_HAL_ERROR_NONE == status) {
        paramsPtr->HwRegsPtr        = NULL_PTR;
        paramsPtr->UserArgsPtr      = NULL_PTR;
        paramsPtr->Mode             = SPI_HAL_MODE_CONTROLLER;
        paramsPtr->RxOnly           = FALSE;
        paramsPtr->DataWidth        = SPI_HAL_DATA_WIDTH_16;
        paramsPtr->FrameFormat      = SPI_HAL_FRAMEFORMAT_TI;
        paramsPtr->BitOrder         = SPI_HAL_BITORDER_MSB_FIRST;
        paramsPtr->Parity           = SPI_HAL_PARITY_NONE;
        paramsPtr->BitRateHz        = 100000U;
        paramsPtr->InputClockFreqHz = 30000000U;
        paramsPtr->ClockDivideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1;
    }
    return status;
}

sint32 Spi_Hal_Init(
    Spi_Hal_InstanceType *instancePtr, const Spi_Hal_InitParamsType *paramsPtr)
{
    sint32 status = SPI_HAL_ERROR_NONE;
    DL_SPI_Config config;
    DL_SPI_ClockConfig clockConfig;
    uint32 SCR             = 0U;
    uint32 computedBitRate = 0U;
    uint32_t scrTemp       = 0U;

#if SPI_HAL_CFG_ERROR_CHECK == STD_ON
    /* Check parameters */
    if ((instancePtr == NULL_PTR) || (paramsPtr == NULL_PTR)) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    } else if ((paramsPtr->InputClockFreqHz == 0U) ||
               (paramsPtr->BitRateHz == 0U)) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    }
    /* RxOnly mode is only applicable if SPI is configured in peripheral mode */
    else if ((paramsPtr->Mode == SPI_HAL_MODE_CONTROLLER) &&
             paramsPtr->RxOnly) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (status == SPI_HAL_ERROR_NONE) {
        /* Compute the serial clock divider (SCR)
            SPI bit rate = (SPI functional clock) / ((1 + SCR) *2)
            SCR = ((SPI functional clock) / (2 * SPI bit rate)) - 1
        */
        scrTemp = paramsPtr->InputClockFreqHz / (2U * paramsPtr->BitRateHz);
        if (scrTemp == 0U) {
            status = SPI_HAL_ERROR_INVALID_PARAMS;
        } else {
            SCR             = scrTemp - 1U;
            computedBitRate = paramsPtr->InputClockFreqHz / ((1U + SCR) * 2U);
        }
    }

    if (status == SPI_HAL_ERROR_NONE) {
        /* Always validate SCR and computed bit rate - arithmetic correctness checks must never be conditional */
        if ((SCR > UNICOMMSPI_CLKCTL_SCR_MAXIMUM) ||
            (computedBitRate != paramsPtr->BitRateHz)) {
            status = SPI_HAL_ERROR_INVALID_PARAMS;
        }

#if SPI_HAL_CFG_ERROR_CHECK == STD_ON
        /* Additional optional parameter checks when error checking is enabled */
#endif

        if (status == SPI_HAL_ERROR_NONE) {
            instancePtr->HwRegsPtr   = paramsPtr->HwRegsPtr;
            instancePtr->UserArgsPtr = paramsPtr->UserArgsPtr;

            /* Enable power on selected SPI module. And configure Unicomm to SPI mode */
            DL_SPI_enablePower(instancePtr->HwRegsPtr);

            /* Configure the DL_SPI parameters */
            clockConfig.clockSel    = DL_SPI_CLOCK_BUSCLK;
            clockConfig.divideRatio = paramsPtr->ClockDivideRatio;

            /* Convert the enums to DL_ enums */
            config.mode = (paramsPtr->Mode == SPI_HAL_MODE_CONTROLLER)
                              ? DL_SPI_MODE_CONTROLLER
                              : DL_SPI_MODE_PERIPHERAL;
            config.dataSize = (DL_SPI_DATA_SIZE)(paramsPtr->DataWidth - 1U);
            config.bitOrder =
                (paramsPtr->BitOrder == SPI_HAL_BITORDER_MSB_FIRST)
                    ? DL_SPI_BIT_ORDER_MSB_FIRST
                    : DL_SPI_BIT_ORDER_LSB_FIRST;
            config.parity = (paramsPtr->Parity == SPI_HAL_PARITY_NONE)
                                ? DL_SPI_PARITY_NONE
                                : (paramsPtr->Parity == SPI_HAL_PARITY_ODD)
                                      ? DL_SPI_PARITY_ODD
                                      : DL_SPI_PARITY_EVEN;
            config.chipSelectPin = DL_SPI_CHIP_SELECT_0;
            switch (paramsPtr->FrameFormat) {
                case SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL0_PHA0:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL0_PHA1:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA1;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL1_PHA0:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL1_PHA0;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL1_PHA1:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL1_PHA1;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL0_PHA0:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL0_PHA1:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA1;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL1_PHA0:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL1_PHA0;
                    break;
                case SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL1_PHA1:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL1_PHA1;
                    break;
                case SPI_HAL_FRAMEFORMAT_TI:
                    config.frameFormat = DL_SPI_FRAME_FORMAT_TI_SYNC;
                    break;
                default:
                    status = SPI_HAL_ERROR_INVALID_PARAMS;
                    break;
            }

            if (status == SPI_HAL_ERROR_NONE) {
                /* SPI Initialization */
                DL_SPI_setClockConfig(instancePtr->HwRegsPtr, &clockConfig);
                DL_SPI_init(instancePtr->HwRegsPtr, &config);

                /* Disable Tx if RxOnly */
                if (paramsPtr->RxOnly) {
                    DL_SPI_disablePeripheralDataOutput(instancePtr->HwRegsPtr);
                }

                /* Set the bit rate if in controller mode */
                if (paramsPtr->Mode == SPI_HAL_MODE_CONTROLLER) {
                    DL_SPI_setBitRateSerialClockDivider(
                        instancePtr->HwRegsPtr, SCR);
                }

                /* Configure the FIFO threshold. Interrupt is only enabled when the transfer is initiated */
                DL_SPI_setFIFOThreshold(instancePtr->HwRegsPtr,
                    DL_SPI_RX_FIFO_LEVEL_ONE_FRAME,
                    DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

                /* Enable module */
                DL_SPI_enable(instancePtr->HwRegsPtr);

                /* Update the state */
                instancePtr->DataWidth = paramsPtr->DataWidth;
                instancePtr->State     = SPI_HAL_STATE_READY;
            }
        }
    }
    return status;
}

sint32 Spi_Hal_DeInit(Spi_Hal_InstanceType *instancePtr)
{
    sint32 status      = SPI_HAL_ERROR_NONE;
    instancePtr->State = SPI_HAL_STATE_UNCONFIGURED;
    DL_SPI_reset(instancePtr->HwRegsPtr);
    return status;
}

sint32 Spi_Hal_TransferPolling(Spi_Hal_InstanceType *instancePtr,
    void *txBufferPtr, void *rxBufferPtr, uint32 size)
{
    sint32 status = SPI_HAL_ERROR_NONE;

#if SPI_HAL_CFG_ERROR_CHECK == STD_ON
    /* Check parameters */
    if ((instancePtr == NULL_PTR) || (size == 0U)) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    } else if ((txBufferPtr == NULL_PTR) && (rxBufferPtr == NULL_PTR)) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (status == SPI_HAL_ERROR_NONE) {
        status =
            Spi_Hal_StartTransfer(instancePtr, txBufferPtr, rxBufferPtr, size);
        if (status == SPI_HAL_ERROR_NONE) {
            Spi_Hal_ReadWriteFIFO(instancePtr);
        }
    }
    return status;
}

sint32 Spi_Hal_TransferContinuePolling(
    Spi_Hal_InstanceType *instancePtr, Spi_Hal_TransferStatusType *statusPtr)
{
    sint32 status = SPI_HAL_ERROR_NONE;

#if SPI_HAL_CFG_ERROR_CHECK == STD_ON
    /* Check parameters */
    if (instancePtr == NULL_PTR) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (status == SPI_HAL_ERROR_NONE) {
        if (instancePtr->State == SPI_HAL_STATE_TRANSFER) {
            Spi_Hal_ReadWriteFIFO(instancePtr);

            if (statusPtr != NULL_PTR) {
                statusPtr->IsTransferComplete =
                    (instancePtr->RemainingRXSize == 0U);
                statusPtr->ErrorCodeMask   = instancePtr->ErrorCodeMask;
                statusPtr->RemainingTXSize = instancePtr->RemainingTXSize;
                statusPtr->RemainingRXSize = instancePtr->RemainingRXSize;

                /* If RX is complete, update the state */
                if (instancePtr->RemainingRXSize == 0U) {
                    instancePtr->State = SPI_HAL_STATE_READY;
                }
            }
        } else {
            status = SPI_HAL_ERROR_UNSUPPORTED_FEATURE;
        }
    }
    return status;
}

sint32 Spi_Hal_TransferInterrupt(Spi_Hal_InstanceType *instancePtr,
    void *txBufferPtr, void *rxBufferPtr, uint32 size)
{
    sint32 status = SPI_HAL_ERROR_NONE;

#if SPI_HAL_CFG_ERROR_CHECK == STD_ON
    /* Check parameters */
    if ((instancePtr == NULL_PTR) || (size == 0U)) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    } else if ((txBufferPtr == NULL_PTR) && (rxBufferPtr == NULL_PTR)) {
        status = SPI_HAL_ERROR_INVALID_PARAMS;
    }
#endif

    if (status == SPI_HAL_ERROR_NONE) {
        status =
            Spi_Hal_StartTransfer(instancePtr, txBufferPtr, rxBufferPtr, size);

        /* Enable interrupt for Tx. Rx interrupt is not used here */
        DL_SPI_enableInterrupt(instancePtr->HwRegsPtr,
            (DL_SPI_INTERRUPT_TX | DL_SPI_INTERRUPT_TX_EMPTY));
    }

    return status;
}

/* ========================================================================== */
/*                 Private Functions                                          */
/* ========================================================================== */

static sint32 Spi_Hal_StartTransfer(Spi_Hal_InstanceType *instancePtr,
    void *txBufferPtr, void *rxBufferPtr, uint32 size)
{
    sint32 status = SPI_HAL_ERROR_NONE;

    if (instancePtr->State != SPI_HAL_STATE_READY) {
        status = SPI_HAL_ERROR_HW_BUSY;
    } else {
        /* Update the state */
        instancePtr->State           = SPI_HAL_STATE_TRANSFER;
        instancePtr->TxBufferPtr     = txBufferPtr;
        instancePtr->RxBufferPtr     = rxBufferPtr;
        instancePtr->RemainingTXSize = size;
        instancePtr->RemainingRXSize = size;
        instancePtr->ErrorCodeMask   = 0U;
    }
    return status;
}

/* Function to fill the TX FIFO and read from RX FIFO. This does not wait for the transfer completion */
static void Spi_Hal_ReadWriteFIFO(Spi_Hal_InstanceType *instancePtr)
{
    uint32 intStatus;

    if (instancePtr->DataWidth <= SPI_HAL_DATA_WIDTH_8) {
        /* Read from RX FIFO before transmitting. This is skipped in the first iteration */
        while ((instancePtr->RemainingRXSize > 0U) &&
               !DL_SPI_isRXFIFOEmpty(instancePtr->HwRegsPtr)) {
            if (instancePtr->RxBufferPtr != NULL_PTR) {
                *(uint8 *) (instancePtr->RxBufferPtr) =
                    DL_SPI_receiveData8(instancePtr->HwRegsPtr);
                instancePtr->RxBufferPtr++;
            } else {
                /* Read & discard data */
                (void) DL_SPI_receiveData8(instancePtr->HwRegsPtr);
            }
            instancePtr->RemainingRXSize--;
        }

        /* Send data till TX FIFO is full or till the TX and RX size difference is beyond FIFO size */
        while ((instancePtr->RemainingTXSize > 0U) &&
               !DL_SPI_isTXFIFOFull(instancePtr->HwRegsPtr)) {
            if (instancePtr->TxBufferPtr != NULL_PTR) {
                DL_SPI_transmitData8(instancePtr->HwRegsPtr,
                    *(uint8 *) (instancePtr->TxBufferPtr));
                instancePtr->TxBufferPtr++;
            } else {
                /* Transmit dummy data */
                DL_SPI_transmitData8(
                    instancePtr->HwRegsPtr, SPI_HAL_TX_DUMMY_DATA);
            }
            instancePtr->RemainingTXSize--;

            /* If the RX and TX difference is more than fifo size, there is a possibility of
                RX overflow. Stop further TX. */
            if ((instancePtr->RemainingRXSize >=
                    instancePtr->RemainingTXSize) &&
                ((instancePtr->RemainingRXSize -
                     instancePtr->RemainingTXSize) >= SPI_HAL_FIFO_SIZE)) {
                break;
            }
        }
    } else {
        /* Read from RX FIFO before transmitting. This is skipped in the first iteration */
        while ((instancePtr->RemainingRXSize > 0U) &&
               !DL_SPI_isRXFIFOEmpty(instancePtr->HwRegsPtr)) {
            if (instancePtr->RxBufferPtr != NULL_PTR) {
                *(uint16 *) (instancePtr->RxBufferPtr) =
                    DL_SPI_receiveData16(instancePtr->HwRegsPtr);
                instancePtr->RxBufferPtr += 2U;
            } else {
                /* Read & discard data */
                (void) DL_SPI_receiveData16(instancePtr->HwRegsPtr);
            }
            instancePtr->RemainingRXSize--;
        }

        /* Send data till TX FIFO is full or till the TX and RX size difference is beyond FIFO size */
        while ((instancePtr->RemainingTXSize > 0U) &&
               !DL_SPI_isTXFIFOFull(instancePtr->HwRegsPtr)) {
            if (instancePtr->TxBufferPtr != NULL_PTR) {
                DL_SPI_transmitData16(instancePtr->HwRegsPtr,
                    *(uint16 *) (instancePtr->TxBufferPtr));
                instancePtr->TxBufferPtr += 2U;
            } else {
                /* Transmit dummy data */
                DL_SPI_transmitData16(
                    instancePtr->HwRegsPtr, (uint16) SPI_HAL_TX_DUMMY_DATA);
            }
            instancePtr->RemainingTXSize--;

            if ((instancePtr->RemainingRXSize >=
                    instancePtr->RemainingTXSize) &&
                ((instancePtr->RemainingRXSize -
                     instancePtr->RemainingTXSize) >= SPI_HAL_FIFO_SIZE)) {
                break;
            }
        }
    }

    /* Read error status */
    intStatus = DL_SPI_getRawInterruptStatus(instancePtr->HwRegsPtr,
        DL_SPI_INTERRUPT_PARITY_ERROR | DL_SPI_INTERRUPT_RX_OVERFLOW |
            DL_SPI_INTERRUPT_TX_UNDERFLOW);

    /* Clear the error status flags */
    DL_SPI_clearInterruptStatus(instancePtr->HwRegsPtr, intStatus);

    /* Update the error mask */
    instancePtr->ErrorCodeMask |= intStatus;
}

/* ========================================================================== */
/*                 Private Callback Handlers                                  */
/* ========================================================================== */
void Spi_Hal_InterruptHandler(Spi_Hal_InstanceType *instancePtr)
{
    /* Continue the transfer */
    Spi_Hal_ReadWriteFIFO(instancePtr);

    if (instancePtr->ErrorCodeMask != 0U) {
        // Invoke error callback function
        Spi_Hal_ErrorCallback(
            instancePtr, instancePtr->ErrorCodeMask, instancePtr->UserArgsPtr);
        // TODO: Should we disable interrupts and further transmit, or leave it to application?
        // If disabling, how to abort ongoing transmit?
        // Should we do instancePtr->ErrorCodeMask = 0 here to avoid callback being called in the next interrupt
    }

    if (instancePtr->RemainingTXSize == 0U) {
        /* Once there are no more data to be sent, TX interrupt is not re-triggered.
           Disable the TX interrupt and enable the RX interrupt */
        if (DL_SPI_getEnabledInterrupts(instancePtr->HwRegsPtr,
                (DL_SPI_INTERRUPT_TX | DL_SPI_INTERRUPT_TX_EMPTY)) != 0U) {
            DL_SPI_disableInterrupt(instancePtr->HwRegsPtr,
                (DL_SPI_INTERRUPT_TX | DL_SPI_INTERRUPT_TX_EMPTY));
            DL_SPI_enableInterrupt(
                instancePtr->HwRegsPtr, DL_SPI_INTERRUPT_RX);
        }
    }

    if (instancePtr->RemainingRXSize == 0U) {
        /* Disable the interrupt */
        DL_SPI_disableInterrupt(instancePtr->HwRegsPtr, DL_SPI_INTERRUPT_RX);

        /* Invoke the user callback function */
        Spi_Hal_TransferCompleteCallback(
            instancePtr, instancePtr->UserArgsPtr);

        /* Update the state */
        instancePtr->State = SPI_HAL_STATE_READY;
    }
}

__WEAK void Spi_Hal_TransferCompleteCallback(
    Spi_Hal_InstanceType *instancePtr, void *userArgsPtr)
{
}

__WEAK void Spi_Hal_ErrorCallback(
    Spi_Hal_InstanceType *instancePtr, uint32 errorCodeMask, void *userArgsPtr)
{
}

/* ========================================================================== */
/*                 Test/Debug/Other Sections                                  */
/* ========================================================================== */
