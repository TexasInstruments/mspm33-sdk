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

/**
 * \defgroup UART_HAL_MSPM33 UART HAL API
 * \ingroup HAL_MSPM33
 *
 * For a given UART instance, the APIs to use depends on the TX or RX operation mode
 * selected during \ref Uart_Hal_Init. Only APIs valid in a operation mode should be used
 * by the application, else behaviour is undefined.
 *
 * \note All transfer APIs are non-blocking unless specified otherwise, i.e they
 *       dont wait for the RX/TX operation to complete.
 *
 * UART RX/TX APIs can be used in below operation modes
 * - Polling mode
 *   - APIs to use for data transfer \ref Uart_Hal_TxPolling, \ref Uart_Hal_RxPolling.
 *   - APIs to use to cancel or abort a data transfer \ref Uart_Hal_TxAbort, \ref Uart_Hal_RxAbort
 *   - APIs to use to check for transfer completion,
 *     - \ref Uart_Hal_TxContinuePolling or \ref Uart_Hal_RxContinuePolling is called in polling mode
 *       to wait for completion of the transaction
 *   - \ref Uart_Hal_InterruptHandler MUST not be registered to CPU interrupt line
 *   - \ref Uart_Hal_TxCompleteCallback, \ref Uart_Hal_RxCompleteCallback, are not called in this mode
 *
 * - Interrupt mode
 *   - APIs to use for data transfer \ref Uart_Hal_TxInterrupt, \ref Uart_Hal_RxInterrupt
 *   - APIs to use to cancel or abort a data transfer \ref Uart_Hal_TxAbort, \ref Uart_Hal_RxAbort
 *   - APIs to use to check for transfer completion,
 *     - \ref Uart_Hal_TxCompleteCallback or \ref Uart_Hal_RxCompleteCallback is called when the transfer
 *       completes
 *   - \ref Uart_Hal_InterruptHandler MUST be registered to CPU interrupt line
 *   - \ref Uart_Hal_TxContinuePolling, \ref Uart_Hal_RxContinuePolling should NOT be used
 *
 * - DMA
 *   - APIs to use for data transfer \ref Uart_Hal_TxDma, \ref Uart_Hal_RxDma
 *   - APIs to use to cancel or abort a data transfer \ref Uart_Hal_TxAbortDma, \ref Uart_Hal_RxAbortDma
 *   - APIs to use to check for transfer completion,
 *     - \ref Uart_Hal_TxCompleteCallback or \ref Uart_Hal_RxCompleteCallback is called when the transfer
 *       completes
 *   - \ref Uart_Hal_InterruptHandler MUST be registered to CPU interrupt line. In addition DMA interrupt
 *     handler MUST also be registered
 *   - \ref Uart_Hal_TxContinuePolling, \ref Uart_Hal_RxContinuePolling should NOT be used
 *
 * @{
 */

#ifndef UART_HAL_H
#define UART_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/hal/mspm33/include/Uart_Hal_Cfg.h>
#include <ti/driverlib/dl_unicomm.h>
#include <ti/driverlib/dl_unicommuart.h>

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/**
 * \anchor Uart_Hal_ErrorType
 * \name UART HAL module error codes
 *
 * All APIs in this module return below error codes
 *
 * @{
 */

/** \brief No error */
#define UART_HAL_ERROR_NONE                (0)
/** \brief Invalid instance pointer */
#define UART_HAL_ERROR_INVALID_INSTANCE    (1)
/** \brief Invalid parameters */
#define UART_HAL_ERROR_INVALID_PARAMS      (2)
/** \brief Unsupported feature */
#define UART_HAL_ERROR_UNSUPPORTED_FEATURE (3)
/** \brief Hardware busy */
#define UART_HAL_ERROR_HW_BUSY             (4)

/** @} */

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/*! @brief Hardware flow control */
typedef enum
{
    UART_HAL_FLOWCONTROL_NONE,    /**< Selects no flow control */
    UART_HAL_FLOWCONTROL_RTS,     /**< Selects RTS for flow control */
    UART_HAL_FLOWCONTROL_CTS,     /**< Selects CTS for flow control */
    UART_HAL_FLOWCONTROL_RTS_CTS, /**< Selects RTS and CTS for flow control */
    UART_HAL_FLOWCONTROL_MAX,     /**< Maximum enum value for bounds checking */
} Uart_Hal_FlowControlType;

/*! @brief Parity type*/
typedef enum
{
    UART_HAL_PARITY_NONE,       /**< Selects None Parity */
    UART_HAL_PARITY_EVEN,       /**< Selects Even Parity */
    UART_HAL_PARITY_ODD,        /**< Selects Odd Parity */
    UART_HAL_PARITY_STICK_ONE,  /**< Selects Sticky One Parity */
    UART_HAL_PARITY_STICK_ZERO, /**< Selects Sticky Zero Parity */
    UART_HAL_PARITY_MAX,        /**< Maximum enum value for bounds checking */
} Uart_Hal_ParityType;

/*! @brief Size of data bits*/
typedef enum
{
    UART_HAL_DATASIZE_5_BITS, /**< Selects 5 Data Bits*/
    UART_HAL_DATASIZE_6_BITS, /**< Selects 6 Data Bits*/
    UART_HAL_DATASIZE_7_BITS, /**< Selects 7 Data Bits*/
    UART_HAL_DATASIZE_8_BITS, /**< Selects 8 Data Bits*/
    UART_HAL_DATASIZE_MAX,    /**< Maximum enum value for bounds checking */
} Uart_Hal_DataSizeType;

/*! @brief Number of stop bits*/
typedef enum
{
    UART_HAL_STOPBITS_ONE, /**< Selects 1 Stop Bits*/
    UART_HAL_STOPBITS_TWO, /**< Selects 2 Stop Bits*/
    UART_HAL_STOPBITS_MAX, /**< Maximum enum value for bounds checking */
} Uart_Hal_StopBitsType;

/*! @brief List of state of the module */
typedef enum
{
    UART_HAL_STATE_UNCONFIGURED, /**< UART Module is unconfigured*/
    UART_HAL_STATE_READY,        /**< UART Module is initialized and ready for use*/
    UART_HAL_STATE_TRANSMIT,     /**< UART Module is transmitting out the data*/
    UART_HAL_STATE_RECEIVE,      /**< UART Module is recieving in the data*/
} Uart_Hal_StateType;

/** \brief UART HW initialization parameters */
typedef struct
{
    /** \brief Pointer to UART HW registers */
    UNICOMM_Inst_Regs      *HwRegsPtr;
    /** \brief Pointer to user arguments, returned back to user when any UART callback is called */
    void                      *UserArgsPtr;
    /** \brief The flow control configuration */
    Uart_Hal_FlowControlType   FlowControl;
    /** \brief The parity configuration */
    Uart_Hal_ParityType        Parity;
    /** \brief The size of the data transfer */
    Uart_Hal_DataSizeType      WordLength;
    /** \brief Stop bits */
    Uart_Hal_StopBitsType      StopBits;
    /** \brief Selects the divide ratio */
    DL_UART_CLOCK_DIVIDE_RATIO DivideRatio;
    /** \brief wait for this amount before RX timeout */
    uint32                     RxInterruptTimeoutTicks;
    /** \brief Baudrate */
    uint32                     BaudRateBps;
    /** \brief Clock given to Uart Module */
    uint32                     InputClockHz;
} Uart_Hal_InitParamsType;

/** \brief UART HW instance structure. Should not be modified by application directly */
typedef struct
{
    /** \brief Pointer to UART HW registers */
    UNICOMM_Inst_Regs *HwRegsPtr;
    /** \brief Pointer to user arguments, returned back to user when any UART callback is called */
    void                 *UserArgsPtr;
    /** \brief Holds the current state of the Uart TX Module */
    Uart_Hal_StateType    TxCurrentState;
    /** \brief Holds the current state of the Uart RX Module */
    Uart_Hal_StateType    RxCurrentState;
    /** \brief Holds pointer to the tx buffer */
    uint8                *TxBufferPtr;
    /** \brief Size of data that needs to be TX'ed */
    uint32                TxSizeInBytes;
    /** \brief Holds the remaining bytes to be sent of tx Buffer */
    uint32                TxRemainingSizeInBytes;
    /** \brief Holds pointer to the rx buffer */
    uint8                *RxBufferPtr;
    /** \brief Size of data that needs to be RX'ed */
    uint32                RxSizeInBytes;
    /** \brief Holds the remaining bytes to be sent of rx Buffer */
    uint32                RxRemainingSizeInBytes;
} Uart_Hal_InstanceType;

/** \brief TX status */
typedef struct
{
    /** \brief buffer from which TX sending data */
    uint8  *BufferPtr;
    /** \brief Size of data that needs to be TX'ed */
    uint32  SizeInBytes;
    /** \brief Size of data that is remaining to be TX'ed */
    uint32  RemainingSizeInBytes;
    /** \brief Is TX completed or still in progress */
    boolean IsTxComplete;
    /** \brief Error code, this is a bit mask of errors, 0 means no error */
    uint32  ErrorCodeMask;
} Uart_Hal_TxStatusType;

/** \brief RX status */
typedef struct
{
    /** \brief buffer into which RX is receiving data */
    uint8  *BufferPtr;
    /** \brief Size of data that needs to be RX'ed */
    uint32  SizeInBytes;
    /** \brief Size of data that is remaining to be RX'ed */
    uint32  RemainingSizeInBytes;
    /** \brief Is RX completed or still in progress */
    boolean IsRxComplete;
    /** \brief Error code, this is a bit mask of errors, 0 means no error
     *
     * If an error, it can have a bit mask of the following:
     * - DL_UART_INTERRUPT_OVERRUN_ERROR
     * - DL_UART_INTERRUPT_BREAK_ERROR
     * - DL_UART_INTERRUPT_PARITY_ERROR
     * - DL_UART_INTERRUPT_FRAMING_ERROR
     * - DL_UART_INTERRUPT_RX_TIMEOUT_ERROR
     * - DL_UART_INTERRUPT_NOISE_ERROR
     */
    uint32  ErrorCodeMask;
} Uart_Hal_RxStatusType;

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

/**
 * \brief Initialize params
 */
sint32 Uart_Hal_InitParamsSetDefault(Uart_Hal_InitParamsType *paramsPtr);

/**
 * \brief Initialize a UART HW instance
 */
sint32 Uart_Hal_Init(Uart_Hal_InstanceType *instancePtr, const Uart_Hal_InitParamsType *paramsPtr);

/**
 * \brief Tx a buffer of data in polling mode
 */
sint32 Uart_Hal_TxPolling(Uart_Hal_InstanceType *instancePtr, uint8 *bufferPtr, uint32 sizeInBytes);

/**
 * \brief Rx a buffer of data in polling mode
 */
sint32 Uart_Hal_RxPolling(Uart_Hal_InstanceType *instancePtr, uint8 *bufferPtr, uint32 sizeInBytes);

/**
 * \brief Tx a buffer of data in interrupt mode
 */
sint32 Uart_Hal_TxInterrupt(Uart_Hal_InstanceType *instancePtr, uint8 *bufferPtr, uint32 sizeInBytes);

/**
 * \brief Rx a buffer of data in interrupt mode
 */
sint32 Uart_Hal_RxInterrupt(Uart_Hal_InstanceType *instancePtr, uint8 *bufferPtr, uint32 sizeInBytes);

/**
 * \brief Tx a buffer of data in DMA mode
 */
sint32 Uart_Hal_TxDma(Uart_Hal_InstanceType *instancePtr, uint8 *bufferPtr, uint32 sizeInBytes);

/**
 * \brief Rx a buffer of data in DMA mode
 */
sint32 Uart_Hal_RxDma(Uart_Hal_InstanceType *instancePtr, uint8 *bufferPtr, uint32 sizeInBytes);

/**
 * \brief Abort TX operation. Only valid in polling and interrupt mode
 */
sint32 Uart_Hal_TxAbort(Uart_Hal_InstanceType *instancePtr);

/**
 * \brief Abort RX operation. Only valid in polling and interrupt mode
 */
sint32 Uart_Hal_RxAbort(Uart_Hal_InstanceType *instancePtr);

/**
 * \brief Abort TX operation. Only valid in DMA mode
 */
sint32 Uart_Hal_TxAbortDma(Uart_Hal_InstanceType *instancePtr);

/**
 * \brief Abort RX operation. Only valid in DMA mode
 */
sint32 Uart_Hal_RxAbortDma(Uart_Hal_InstanceType *instancePtr);

/**
 * \brief Continue TX polling and get transfer status. Only valid in polling mode.
 *
 * This function transmits remaining bytes to the FIFO and updates the status.
 *
 * If TX state is not \ref UART_HAL_STATE_TRANSMIT, i.e transmit is not in progress then
 * \ref UART_HAL_ERROR_UNSUPPORTED_FEATURE is returned.
 *
 */
sint32 Uart_Hal_TxContinuePolling(Uart_Hal_InstanceType *instancePtr, Uart_Hal_TxStatusType *statusPtr);

/**
 * \brief Continue RX polling and get transfer status. Only valid in polling mode.
 *
 * This function reads available data from the FIFO into the buffer and updates the status.
 *
 * If RX state is not \ref UART_HAL_STATE_RECEIVE, i.e receive is not in progress then
 * \ref UART_HAL_ERROR_UNSUPPORTED_FEATURE is returned.
 *
 */
sint32 Uart_Hal_RxContinuePolling(Uart_Hal_InstanceType *instancePtr, Uart_Hal_RxStatusType *statusPtr);

/**
 * \brief Get the current status of TX
 */
sint32 Uart_Hal_GetTxCurrentState(const Uart_Hal_InstanceType *instancePtr, Uart_Hal_StateType *statePtr);

/**
 * \brief Get the current status of RX
 */
sint32 Uart_Hal_GetRxCurrentState(const Uart_Hal_InstanceType *instancePtr, Uart_Hal_StateType *statePtr);

/**
 * \brief Callback that is called when TX completes. Only valid in interrupt or DMA mode
 */
void Uart_Hal_TxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_TxStatusType *statusPtr,
                                 void *userArgsPtr);

/**
 * \brief Callback that is called when RX completes. Only valid in interrupt or DMA mode
 */
void Uart_Hal_RxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_RxStatusType *statusPtr,
                                 void *userArgsPtr);

/**
 * \brief Clear Rx FiFo
 */
sint32 Uart_Hal_ClearRxFIFO(const Uart_Hal_InstanceType *instancePtr);

/**
 * \brief De-initialize a UART HW instance
 *
 * Disable the HW and any enabled interrupts at the UART HW.
 * Interrupts at CPU should be disabled by the application before calling this API.
 *
 */
sint32 Uart_Hal_DeInit(Uart_Hal_InstanceType *instancePtr);

/**
 * \brief Interrupt handler.
 *
 * After \ref Uart_Hal_Init, A function should be registerd in NVIC which would then call this
 * function. Do not register this function directly in NVIC. When polling is enabled as part of
 * \ref Uart_Hal_Init, the interrupt handler should not be registered.
 *
 * The interrupt handler will call one of below callbacks, depending on the conditions detected
 * within the interrupt handler. The interrupt handler will also clear status and interrupt bits
 * as needed for correct operation.
 *
 * In each of the callbacks, user arguments passed during initialization are passed back to the
 * user.
 *
 * The callbacks need to be implemented by the application else there will be a linker error.
 *
 * The supported callbacks are,
 * - \ref Uart_Hal_TxCompleteCallback
 * - \ref Uart_Hal_RxCompleteCallback
 */
void Uart_Hal_InterruptHandler(Uart_Hal_InstanceType *instancePtr);

#ifdef __cplusplus
}
#endif

#endif /* UART_HAL_H */

/** @} */
