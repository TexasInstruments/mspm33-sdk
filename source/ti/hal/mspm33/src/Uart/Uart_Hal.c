/*
 * Copyright (c) 2024, Texas Instruments Incorporated
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

#include <ti/hal/mspm33/include/Uart_Hal.h>
#include <ti/driverlib/dl_unicommuart.h>
#include <ti/driverlib/dl_dma.h>

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

#define UART_HAL_RX_ERROR_INTR_MASK                                                                     \
    (DL_UART_INTERRUPT_OVERRUN_ERROR | DL_UART_INTERRUPT_BREAK_ERROR | DL_UART_INTERRUPT_PARITY_ERROR | \
     DL_UART_INTERRUPT_FRAMING_ERROR | DL_UART_INTERRUPT_NOISE_ERROR)
#define UART_HAL_IP_CLOCK_FREQ (80000000U)

/* ========================================================================== */
/*                 Private Typedefs                                           */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Function Prototype                                 */
/* ========================================================================== */

static void        Uart_Hal_TransmitRemainingBytes(Uart_Hal_InstanceType* instancePtr);
static void        Uart_Hal_ReceiveTillFifoEmptyOrDataOver(Uart_Hal_InstanceType* instancePtr);
static inline void Uart_Hal_RxCompleteHandler(Uart_Hal_InstanceType* instancePtr, Uart_Hal_RxStatusType* rxClptStatus);
static sint32      Uart_Hal_ClearTxFIFO(Uart_Hal_InstanceType* instancePtr);

/* ========================================================================== */
/*                 Private Variable Declaration                               */
/* ========================================================================== */

static const DL_UART_FLOW_CONTROL Uart_Hal_DL_FlowControlMap[UART_HAL_FLOWCONTROL_MAX] = {
    DL_UART_FLOW_CONTROL_NONE,   /* UART_HAL_FLOWCONTROL_NONE */
    DL_UART_FLOW_CONTROL_RTS,    /* UART_HAL_FLOWCONTROL_RTS */
    DL_UART_FLOW_CONTROL_CTS,    /* UART_HAL_FLOWCONTROL_CTS */
    DL_UART_FLOW_CONTROL_RTS_CTS /* UART_HAL_FLOWCONTROL_RTS_CTS */
};

/* Parity mapping array */
static const DL_UART_PARITY Uart_Hal_DL_ParityMap[UART_HAL_PARITY_MAX] = {
    DL_UART_PARITY_NONE,      /* UART_HAL_PARITY_NONE */
    DL_UART_PARITY_EVEN,      /* UART_HAL_PARITY_EVEN */
    DL_UART_PARITY_ODD,       /* UART_HAL_PARITY_ODD */
    DL_UART_PARITY_STICK_ONE, /* UART_HAL_PARITY_STICK_ONE */
    DL_UART_PARITY_STICK_ZERO /* UART_HAL_PARITY_STICK_ZERO */
};

/* Word length mapping array */
static const DL_UART_WORD_LENGTH Uart_Hal_DL_WordLengthMap[UART_HAL_DATASIZE_MAX] = {
    DL_UART_WORD_LENGTH_5_BITS, /* UART_HAL_DATASIZE_5_BITS */
    DL_UART_WORD_LENGTH_6_BITS, /* UART_HAL_DATASIZE_6_BITS */
    DL_UART_WORD_LENGTH_7_BITS, /* UART_HAL_DATASIZE_7_BITS */
    DL_UART_WORD_LENGTH_8_BITS  /* UART_HAL_DATASIZE_8_BITS */
};

/* Stop bits mapping array */
static const DL_UART_STOP_BITS Uart_Hal_DL_StopBitsMap[UART_HAL_STOPBITS_MAX] = {
    DL_UART_STOP_BITS_ONE, /* UART_HAL_STOPBITS_ONE */
    DL_UART_STOP_BITS_TWO  /* UART_HAL_STOPBITS_TWO */
};

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

sint32 Uart_Hal_InitParamsSetDefault(Uart_Hal_InitParamsType* paramsPtr)
{
    sint32 status;

    status = UART_HAL_ERROR_NONE;
#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        paramsPtr->HwRegsPtr               = NULL_PTR;
        paramsPtr->UserArgsPtr             = NULL_PTR;
        paramsPtr->FlowControl             = UART_HAL_FLOWCONTROL_NONE;
        paramsPtr->Parity                  = UART_HAL_PARITY_NONE;
        paramsPtr->WordLength              = UART_HAL_DATASIZE_8_BITS;
        paramsPtr->StopBits                = UART_HAL_STOPBITS_ONE;
        paramsPtr->BaudRateBps             = 115200U;
        paramsPtr->DivideRatio             = DL_UART_CLOCK_DIVIDE_RATIO_1;
        paramsPtr->RxInterruptTimeoutTicks = 8U;
        paramsPtr->InputClockHz            = UART_HAL_IP_CLOCK_FREQ;
    }

    return status;
}

sint32 Uart_Hal_Init(Uart_Hal_InstanceType* instancePtr, const Uart_Hal_InitParamsType* paramsPtr)
{
    sint32               status;
    DL_UART_Config       config;
    DL_UART_ClockConfig  clockConfig;
    DL_UART_FLOW_CONTROL flowControl;
    DL_UART_PARITY       parity;
    DL_UART_WORD_LENGTH  wordLength;
    DL_UART_STOP_BITS    stopbits;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == paramsPtr || NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (paramsPtr->FlowControl < UART_HAL_FLOWCONTROL_MAX)
        {
            flowControl = Uart_Hal_DL_FlowControlMap[paramsPtr->FlowControl];
        }
        else
        {
            status = UART_HAL_ERROR_INVALID_PARAMS;
        }

        if ((status == UART_HAL_ERROR_NONE) && (paramsPtr->Parity < UART_HAL_PARITY_MAX))
        {
            parity = Uart_Hal_DL_ParityMap[paramsPtr->Parity];
        }
        else
        {
            status = UART_HAL_ERROR_INVALID_PARAMS;
        }

        if ((status == UART_HAL_ERROR_NONE) && (paramsPtr->WordLength < UART_HAL_DATASIZE_MAX))
        {
            wordLength = Uart_Hal_DL_WordLengthMap[paramsPtr->WordLength];
        }
        else
        {
            status = UART_HAL_ERROR_INVALID_PARAMS;
        }

        if ((status == UART_HAL_ERROR_NONE) && (paramsPtr->StopBits < UART_HAL_STOPBITS_MAX))
        {
            stopbits = Uart_Hal_DL_StopBitsMap[paramsPtr->StopBits];
        }
        else
        {
            status = UART_HAL_ERROR_INVALID_PARAMS;
        }
    }

    if (UART_HAL_ERROR_NONE == status)
    {
        instancePtr->HwRegsPtr              = paramsPtr->HwRegsPtr;
        instancePtr->UserArgsPtr            = paramsPtr->UserArgsPtr;
        instancePtr->TxBufferPtr            = NULL_PTR;
        instancePtr->TxSizeInBytes          = 0U;
        instancePtr->TxRemainingSizeInBytes = 0U;
        instancePtr->RxBufferPtr            = NULL_PTR;
        instancePtr->RxSizeInBytes          = 0U;
        instancePtr->RxRemainingSizeInBytes = 0U;

        config.mode        = DL_UART_MODE_NORMAL;
        config.direction   = DL_UART_DIRECTION_TX_RX;
        config.flowControl = flowControl;
        config.parity      = parity;
        config.wordLength  = wordLength;
        config.stopBits    = stopbits;

        DL_UART_reset(instancePtr->HwRegsPtr);

        if (FALSE == DL_UART_isPowerEnabled(instancePtr->HwRegsPtr))
        {
            DL_UNICOMM_enablePower(instancePtr->HwRegsPtr);
        }
                
        DL_UART_disable(instancePtr->HwRegsPtr);

        DL_UART_init(instancePtr->HwRegsPtr, &config);
        DL_UART_disableInterrupt(instancePtr->HwRegsPtr, DL_UART_INTERRUPT_TX | DL_UART_INTERRUPT_RX |
                                                             DL_UART_INTERRUPT_EOT_DONE | DL_UART_INTERRUPT_LINE_TIMEOUT);
        DL_UART_clearInterruptStatus(instancePtr->HwRegsPtr, DL_UART_INTERRUPT_TX | DL_UART_INTERRUPT_RX |
                                                                 DL_UART_INTERRUPT_EOT_DONE | DL_UART_INTERRUPT_LINE_TIMEOUT);

        clockConfig.clockSel    = DL_UART_CLOCK_BUSCLK;
        clockConfig.divideRatio = paramsPtr->DivideRatio;

        DL_UART_setClockConfig(instancePtr->HwRegsPtr, &clockConfig);

        DL_UART_configBaudRate(instancePtr->HwRegsPtr, paramsPtr->InputClockHz, paramsPtr->BaudRateBps);

        DL_UART_setTXFIFOThreshold(instancePtr->HwRegsPtr, DL_UART_TX_FIFO_LEVEL_1_2_EMPTY);
        /*
         * RX FIFO threshold is set to ONE_ENTRY (1 byte) instead of 1/2 full (8 bytes) to avoid
         * a race condition in interrupt mode. With a higher threshold, the RX interrupt only fires
         * when the FIFO reaches that level. For transfers smaller than the threshold, the driver
         * relies on the line timeout interrupt (LTOUT). However, LTOUT fires after a brief idle
         * period on the RX line, which can occur between bytes during transmission. This causes
         * LTOUT to fire prematurely before all data is received, resulting in incomplete transfers.
         * With threshold = 1, the RX interrupt fires on the first byte, ensuring data is drained
         * promptly without relying on timeout for normal operation.
         */
        DL_UART_setRXFIFOThreshold(instancePtr->HwRegsPtr, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);

        DL_UART_setRXInterruptTimeout(instancePtr->HwRegsPtr, paramsPtr->RxInterruptTimeoutTicks);

        DL_UART_enable(instancePtr->HwRegsPtr);

        Uart_Hal_ClearTxFIFO(instancePtr);
        Uart_Hal_ClearRxFIFO(instancePtr);

        instancePtr->TxCurrentState = UART_HAL_STATE_READY;
        instancePtr->RxCurrentState = UART_HAL_STATE_READY;
    }

    return status;
}

sint32 Uart_Hal_DeInit(Uart_Hal_InstanceType* instancePtr)
{
    sint32 status;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        DL_UART_disableInterrupt(instancePtr->HwRegsPtr, DL_UART_INTERRUPT_TX | DL_UART_INTERRUPT_RX |
                                                             DL_UART_INTERRUPT_EOT_DONE | DL_UART_INTERRUPT_LINE_TIMEOUT);

        DL_UART_clearInterruptStatus(instancePtr->HwRegsPtr, DL_UART_INTERRUPT_TX | DL_UART_INTERRUPT_RX |
                                                                 DL_UART_INTERRUPT_EOT_DONE | DL_UART_INTERRUPT_LINE_TIMEOUT);

        DL_UART_clearInterruptStatus(instancePtr->HwRegsPtr,
                                     DL_UART_INTERRUPT_OVERRUN_ERROR | DL_UART_INTERRUPT_BREAK_ERROR |
                                         DL_UART_INTERRUPT_PARITY_ERROR | DL_UART_INTERRUPT_FRAMING_ERROR |
                                         DL_UART_INTERRUPT_RX_TIMEOUT_ERROR | DL_UART_INTERRUPT_NOISE_ERROR);

        DL_UART_disable(instancePtr->HwRegsPtr);

        instancePtr->TxCurrentState = UART_HAL_STATE_UNCONFIGURED;
        instancePtr->RxCurrentState = UART_HAL_STATE_UNCONFIGURED;
    }

    return status;
}

sint32 Uart_Hal_TxPolling(Uart_Hal_InstanceType* instancePtr, uint8* bufferPtr, uint32 sizeInBytes)
{
    sint32 status;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == bufferPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_READY != instancePtr->TxCurrentState)
        {
            status = UART_HAL_ERROR_HW_BUSY;
        }
        else
        {
            instancePtr->TxBufferPtr            = bufferPtr;
            instancePtr->TxSizeInBytes          = sizeInBytes;
            instancePtr->TxRemainingSizeInBytes = sizeInBytes;

            Uart_Hal_TransmitRemainingBytes(instancePtr);

            instancePtr->TxCurrentState = UART_HAL_STATE_TRANSMIT;
        }
    }

    return status;
}

sint32 Uart_Hal_TxContinuePolling(Uart_Hal_InstanceType* instancePtr, Uart_Hal_TxStatusType* statusPtr)
{
    sint32                status;
    UNICOMM_Inst_Regs* unicomm;
    boolean               isTransferComplete;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == statusPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_TRANSMIT != instancePtr->TxCurrentState)
        {
            status = UART_HAL_ERROR_UNSUPPORTED_FEATURE;
        }
        else
        {
            unicomm = instancePtr->HwRegsPtr;
            Uart_Hal_TransmitRemainingBytes(instancePtr);
            isTransferComplete = (instancePtr->TxRemainingSizeInBytes) == 0U && DL_UART_isTXFIFOEmpty(unicomm);
            if (isTransferComplete)
            {
                instancePtr->TxCurrentState = UART_HAL_STATE_READY;
            }
            statusPtr->BufferPtr            = instancePtr->TxBufferPtr;
            statusPtr->SizeInBytes          = instancePtr->TxSizeInBytes;
            statusPtr->RemainingSizeInBytes = instancePtr->TxRemainingSizeInBytes;
            statusPtr->IsTxComplete         = isTransferComplete;
            statusPtr->ErrorCodeMask        = 0U;
        }
    }

    return status;
}

sint32 Uart_Hal_RxPolling(Uart_Hal_InstanceType* instancePtr, uint8* bufferPtr, uint32 sizeInBytes)
{
    sint32                status;
    UNICOMM_Inst_Regs* unicomm;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == bufferPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_READY != instancePtr->RxCurrentState)
        {
            status = UART_HAL_ERROR_HW_BUSY;
        }
        else
        {
            unicomm                             = instancePtr->HwRegsPtr;
            instancePtr->RxBufferPtr            = bufferPtr;
            instancePtr->RxSizeInBytes          = sizeInBytes;
            instancePtr->RxRemainingSizeInBytes = sizeInBytes;

            DL_UART_clearInterruptStatus(unicomm, DL_UART_INTERRUPT_OVERRUN_ERROR | DL_UART_INTERRUPT_BREAK_ERROR |
                                                      DL_UART_INTERRUPT_PARITY_ERROR | DL_UART_INTERRUPT_FRAMING_ERROR |
                                                      DL_UART_INTERRUPT_RX_TIMEOUT_ERROR |
                                                      DL_UART_INTERRUPT_NOISE_ERROR);

            Uart_Hal_ReceiveTillFifoEmptyOrDataOver(instancePtr);

            instancePtr->RxCurrentState = UART_HAL_STATE_RECEIVE;
        }
    }

    return status;
}

sint32 Uart_Hal_RxContinuePolling(Uart_Hal_InstanceType* instancePtr, Uart_Hal_RxStatusType* statusPtr)
{
    sint32                status;
    boolean               isRecieveComplete;
    UNICOMM_Inst_Regs* unicomm;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == statusPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_RECEIVE != instancePtr->RxCurrentState)
        {
            status = UART_HAL_ERROR_UNSUPPORTED_FEATURE;
        }
        else
        {
            unicomm = instancePtr->HwRegsPtr;

            Uart_Hal_ReceiveTillFifoEmptyOrDataOver(instancePtr);

            isRecieveComplete = (instancePtr->RxRemainingSizeInBytes) == 0U;
            if (isRecieveComplete)
            {
                instancePtr->RxCurrentState = UART_HAL_STATE_READY;
            }
            statusPtr->BufferPtr            = instancePtr->RxBufferPtr;
            statusPtr->SizeInBytes          = instancePtr->RxSizeInBytes;
            statusPtr->IsRxComplete         = isRecieveComplete;
            statusPtr->RemainingSizeInBytes = instancePtr->RxRemainingSizeInBytes;
            statusPtr->ErrorCodeMask        = DL_UART_getRawInterruptStatus(unicomm, UART_HAL_RX_ERROR_INTR_MASK);
            DL_UART_clearInterruptStatus(unicomm, UART_HAL_RX_ERROR_INTR_MASK);
        }
    }

    return status;
}

sint32 Uart_Hal_GetTxCurrentState(const Uart_Hal_InstanceType* instancePtr, Uart_Hal_StateType* statePtr)
{
    sint32 status;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == statePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        *statePtr = instancePtr->TxCurrentState;
    }

    return status;
}

sint32 Uart_Hal_GetRxCurrentState(const Uart_Hal_InstanceType* instancePtr, Uart_Hal_StateType* statePtr)
{
    sint32 status;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == statePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        *statePtr = instancePtr->RxCurrentState;
    }

    return status;
}

sint32 Uart_Hal_TxInterrupt(Uart_Hal_InstanceType* instancePtr, uint8* bufferPtr, uint32 sizeInBytes)
{
    sint32                status;
    uint32                txSentBytes;
    UNICOMM_Inst_Regs* unicomm;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == bufferPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_READY != instancePtr->TxCurrentState)
        {
            status = UART_HAL_ERROR_HW_BUSY;
        }
        else
        {
            unicomm                             = instancePtr->HwRegsPtr;
            instancePtr->TxBufferPtr            = bufferPtr;
            instancePtr->TxSizeInBytes          = sizeInBytes;
            instancePtr->TxRemainingSizeInBytes = sizeInBytes;

            instancePtr->TxCurrentState = UART_HAL_STATE_TRANSMIT;

            DL_UART_clearInterruptStatus(unicomm, DL_UART_INTERRUPT_TX | DL_UART_INTERRUPT_EOT_DONE);

            txSentBytes = DL_UART_fillTXFIFO(unicomm, instancePtr->TxBufferPtr, instancePtr->TxRemainingSizeInBytes);

            instancePtr->TxRemainingSizeInBytes -= txSentBytes;

            if (instancePtr->TxRemainingSizeInBytes == 0U)
            {
                DL_UART_enableInterrupt(unicomm, DL_UART_INTERRUPT_EOT_DONE);
            }
            else
            {
                DL_UART_enableInterrupt(unicomm, DL_UART_INTERRUPT_TX);
            }
        }
    }

    return status;
}

sint32 Uart_Hal_RxInterrupt(Uart_Hal_InstanceType* instancePtr, uint8* bufferPtr, uint32 sizeInBytes)
{
    sint32                status;
    UNICOMM_Inst_Regs* unicomm;
    Uart_Hal_RxStatusType rxClptStatus;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr || NULL_PTR == bufferPtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_READY != instancePtr->RxCurrentState)
        {
            status = UART_HAL_ERROR_HW_BUSY;
        }
        else
        {
            unicomm                             = instancePtr->HwRegsPtr;
            instancePtr->RxBufferPtr            = bufferPtr;
            instancePtr->RxSizeInBytes          = sizeInBytes;
            instancePtr->RxRemainingSizeInBytes = sizeInBytes;
            instancePtr->RxCurrentState         = UART_HAL_STATE_RECEIVE;

            DL_UART_clearInterruptStatus(unicomm, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_LINE_TIMEOUT);

            Uart_Hal_ReceiveTillFifoEmptyOrDataOver(instancePtr);

            if (instancePtr->RxRemainingSizeInBytes > 0U)
            {
                DL_UART_enableInterrupt(unicomm, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_LINE_TIMEOUT);
            }
            else
            {
                instancePtr->RxCurrentState = UART_HAL_STATE_READY;
                Uart_Hal_RxCompleteHandler(instancePtr, &rxClptStatus);
            }
        }
    }

    return status;
}

sint32 Uart_Hal_RxAbort(Uart_Hal_InstanceType* instancePtr)
{
    sint32                status;
    UNICOMM_Inst_Regs* unicomm;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_UNCONFIGURED == instancePtr->RxCurrentState)
        {
            status = UART_HAL_ERROR_UNSUPPORTED_FEATURE;
        }
    }

    if (UART_HAL_ERROR_NONE == status)
    {
        unicomm = instancePtr->HwRegsPtr;

        DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_LINE_TIMEOUT);
        DL_UART_clearInterruptStatus(unicomm,
                                     DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_LINE_TIMEOUT | UART_HAL_RX_ERROR_INTR_MASK);

        instancePtr->RxCurrentState = UART_HAL_STATE_READY;
    }

    return status;
}

sint32 Uart_Hal_TxAbort(Uart_Hal_InstanceType* instancePtr)
{
    sint32                status;
    UNICOMM_Inst_Regs* unicomm;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        if (UART_HAL_STATE_UNCONFIGURED == instancePtr->TxCurrentState)
        {
            status = UART_HAL_ERROR_UNSUPPORTED_FEATURE;
        }
    }

    if (UART_HAL_ERROR_NONE == status)
    {
        unicomm = instancePtr->HwRegsPtr;

        DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_TX | DL_UART_INTERRUPT_EOT_DONE);

        instancePtr->TxCurrentState = UART_HAL_STATE_READY;
    }

    return status;
}

void Uart_Hal_InterruptHandler(Uart_Hal_InstanceType* instancePtr)
{
    /*
        1. Each unicomm IP has a separate interrupt, meaning for UC0 has UC0_INT_IRQn
           and UCx has UCx_INT_IRQn.
        2. Application is where UCx is assigned to an Uart_Hal_InstanceType object
        3. Hence, at application level, for each UCx there will be an interrupt handler
           which would call Uart_Hal_InterruptHandler and pass the instancePtr.
    */
    sint32                status;
    uint32                interruptStatus;
    UNICOMM_Inst_Regs* unicomm;
    Uart_Hal_TxStatusType txClptStatus;
    Uart_Hal_RxStatusType rxClptStatus;

    status = UART_HAL_ERROR_NONE;

#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        unicomm         = instancePtr->HwRegsPtr;
        interruptStatus = DL_UART_getPendingInterrupt(unicomm);

        switch (interruptStatus)
        {
            case DL_UART_IIDX_TX:
                if (instancePtr->TxRemainingSizeInBytes > 0U)
                {
                    uint8* buff =
                        instancePtr->TxBufferPtr + (instancePtr->TxSizeInBytes - instancePtr->TxRemainingSizeInBytes);
                    uint32 sentBytes = DL_UART_fillTXFIFO(unicomm, buff, instancePtr->TxRemainingSizeInBytes);
                    instancePtr->TxRemainingSizeInBytes -= sentBytes;
                }
                else
                {
                    DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_TX);
                    DL_UART_enableInterrupt(unicomm, DL_UART_INTERRUPT_EOT_DONE);
                }
                break;

            case DL_UART_IIDX_EOT_DONE:
                txClptStatus.BufferPtr            = instancePtr->TxBufferPtr;
                txClptStatus.SizeInBytes          = instancePtr->TxSizeInBytes;
                txClptStatus.RemainingSizeInBytes = instancePtr->TxRemainingSizeInBytes;
                txClptStatus.ErrorCodeMask        = 0U;
                txClptStatus.IsTxComplete         = TRUE;
                instancePtr->TxCurrentState       = UART_HAL_STATE_READY;
                DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_EOT_DONE);
                Uart_Hal_TxCompleteCallback(instancePtr, &txClptStatus, instancePtr->UserArgsPtr);
                break;

            case DL_UART_IIDX_RX:
                if (UART_HAL_STATE_RECEIVE == instancePtr->RxCurrentState)
                {
                    Uart_Hal_ReceiveTillFifoEmptyOrDataOver(instancePtr);
                }
                /* RxRemainingSizeInBytes would have been updated by Uart_Hal_ReceiveTillFifoEmptyOrDataOver */
                if (instancePtr->RxRemainingSizeInBytes == 0U)
                {
                    instancePtr->RxCurrentState = UART_HAL_STATE_READY;
                    Uart_Hal_RxCompleteHandler(instancePtr, &rxClptStatus);
                }
                break;

            case DL_UART_IIDX_LINE_TIMEOUT:
                if (UART_HAL_STATE_RECEIVE == instancePtr->RxCurrentState)
                {
                    Uart_Hal_ReceiveTillFifoEmptyOrDataOver(instancePtr);
                    instancePtr->RxCurrentState = UART_HAL_STATE_READY;
                    Uart_Hal_RxCompleteHandler(instancePtr, &rxClptStatus);
                }
                break;

            default:
                break;
        }
    }
}

sint32 Uart_Hal_ClearRxFIFO(const Uart_Hal_InstanceType* instancePtr)
{
    sint32 status;
    uint8  buf[4];

    status = UART_HAL_ERROR_NONE;
#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        while (DL_UART_drainRXFIFO((UNICOMM_Inst_Regs*)(instancePtr->HwRegsPtr), buf, sizeof(buf)) != 0U)
        {
        }
    }

    return status;
}

/* ========================================================================== */
/*                 Private Functions                                          */
/* ========================================================================== */

static sint32 Uart_Hal_ClearTxFIFO(Uart_Hal_InstanceType* instancePtr)
{
    sint32 status;

    status = UART_HAL_ERROR_NONE;
#if UART_HAL_CFG_ERROR_CHECK == STD_ON
    if (NULL_PTR == instancePtr)
    {
        status = UART_HAL_ERROR_INVALID_PARAMS;
    }
#endif /* UART_HAL_CFG_ERROR_CHECK == STD_ON */

    if (UART_HAL_ERROR_NONE == status)
    {
        while (!DL_UART_isTXFIFOEmpty((UNICOMM_Inst_Regs*)(instancePtr->HwRegsPtr)))
        {
        }
    }

    return status;
}

static void Uart_Hal_TransmitRemainingBytes(Uart_Hal_InstanceType* instancePtr)
{
    UNICOMM_Inst_Regs* unicomm = instancePtr->HwRegsPtr;
    uint8*                buffPtr;
    while (!DL_UART_isTXFIFOFull(unicomm) && (instancePtr->TxRemainingSizeInBytes) > 0U)
    {
        /* Transmit byte wise */
        buffPtr = instancePtr->TxBufferPtr + (instancePtr->TxSizeInBytes - instancePtr->TxRemainingSizeInBytes);
        DL_UART_transmitData(unicomm, *(buffPtr));
        instancePtr->TxRemainingSizeInBytes -= 1U;
    }
}

static void Uart_Hal_ReceiveTillFifoEmptyOrDataOver(Uart_Hal_InstanceType* instancePtr)
{
    UNICOMM_Inst_Regs* unicomm;
    uint8*                buffPtr;
    uint32                rxReceivedBytes;

    unicomm = instancePtr->HwRegsPtr;

    while ((instancePtr->RxRemainingSizeInBytes) > 0U && !DL_UART_isRXFIFOEmpty(unicomm))
    {
        buffPtr         = instancePtr->RxBufferPtr + (instancePtr->RxSizeInBytes - instancePtr->RxRemainingSizeInBytes);
        rxReceivedBytes = DL_UART_drainRXFIFO(unicomm, buffPtr, instancePtr->RxRemainingSizeInBytes);
        instancePtr->RxRemainingSizeInBytes -= rxReceivedBytes;
    }
}

static inline void Uart_Hal_RxCompleteHandler(Uart_Hal_InstanceType* instancePtr, Uart_Hal_RxStatusType* rxClptStatus)
{
    UNICOMM_Inst_Regs* unicomm = instancePtr->HwRegsPtr;

    rxClptStatus->BufferPtr            = instancePtr->RxBufferPtr;
    rxClptStatus->SizeInBytes          = instancePtr->RxSizeInBytes;
    rxClptStatus->RemainingSizeInBytes = instancePtr->RxRemainingSizeInBytes;
    rxClptStatus->IsRxComplete         = TRUE;
    rxClptStatus->ErrorCodeMask        = DL_UART_getRawInterruptStatus(unicomm, UART_HAL_RX_ERROR_INTR_MASK);
    ;
    DL_UART_clearInterruptStatus(unicomm, UART_HAL_RX_ERROR_INTR_MASK);
    DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_LINE_TIMEOUT);
    Uart_Hal_RxCompleteCallback(instancePtr, rxClptStatus, instancePtr->UserArgsPtr);
}

/* ========================================================================== */
/*                 Private Callback Handlers                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                 Test/Debug/Other Sections                                  */
/* ========================================================================== */
