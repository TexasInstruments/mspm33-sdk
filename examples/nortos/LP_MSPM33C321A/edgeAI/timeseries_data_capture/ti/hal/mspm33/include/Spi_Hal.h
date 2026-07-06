
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


#ifndef SPI_HAL_H
#define SPI_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Spi_Hal_Cfg.h"
#include <ti/hal/compiler/m33_ti_arm_clang/Std_Types.h>
#include <ti/driverlib/dl_unicommspi.h>

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/**
 * \anchor Spi_Hal_ErrorType
 * \name SPI HAL module error codes
 *
 * All APIs in this module return below error codes
 *
 * @{
 */
/** \brief No error */
#define SPI_HAL_ERROR_NONE                (0)
/** \brief Invalid instance pointer */
#define SPI_HAL_ERROR_INVALID_INSTANCE    (1)
/** \brief Invalid parameters */
#define SPI_HAL_ERROR_INVALID_PARAMS      (2)
/** \brief Unsupported feature */
#define SPI_HAL_ERROR_UNSUPPORTED_FEATURE (3)
/** \brief Transfer fail since underlying HW was busy */
#define SPI_HAL_ERROR_HW_BUSY             (4)
/** @} */

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/*! @brief SPI Mode */
typedef enum
{
    SPI_HAL_MODE_CONTROLLER,
    SPI_HAL_MODE_PERIPHERAL
} Spi_Hal_ModeType;

/*! @brief SPI Data Size */
typedef enum
{
    SPI_HAL_DATA_WIDTH_4  = 4U,
    SPI_HAL_DATA_WIDTH_5  = 5U,
    SPI_HAL_DATA_WIDTH_6  = 6U,
    SPI_HAL_DATA_WIDTH_7  = 7U,
    SPI_HAL_DATA_WIDTH_8  = 8U,
    SPI_HAL_DATA_WIDTH_9  = 9U,
    SPI_HAL_DATA_WIDTH_10 = 10U,
    SPI_HAL_DATA_WIDTH_11 = 11U,
    SPI_HAL_DATA_WIDTH_12 = 12U,
    SPI_HAL_DATA_WIDTH_13 = 13U,
    SPI_HAL_DATA_WIDTH_14 = 14U,
    SPI_HAL_DATA_WIDTH_15 = 15U,
    SPI_HAL_DATA_WIDTH_16 = 16U
} Spi_Hal_DataWidthType;

/*! @brief SPI Frame Format */
typedef enum
{
    SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL0_PHA0,
    SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL0_PHA1,
    SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL1_PHA0,
    SPI_HAL_FRAMEFORMAT_MOTO_3WIRE_POL1_PHA1,
    SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL0_PHA0,
    SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL0_PHA1,
    SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL1_PHA0,
    SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL1_PHA1,
    SPI_HAL_FRAMEFORMAT_TI
} Spi_Hal_FrameFormatType;

/*! @brief SPI Bit Order */
typedef enum
{
    SPI_HAL_BITORDER_MSB_FIRST,
    SPI_HAL_BITORDER_LSB_FIRST
} Spi_Hal_BitOrderType;

/*! @brief SPI Parity */
typedef enum
{
    SPI_HAL_PARITY_NONE,
    SPI_HAL_PARITY_ODD,
    SPI_HAL_PARITY_EVEN
} Spi_Hal_ParityType;

/*! @brief SPI State */
typedef enum
{
    SPI_HAL_STATE_UNCONFIGURED,
    SPI_HAL_STATE_READY,
    SPI_HAL_STATE_TRANSFER,
} Spi_Hal_StateType;

/** \brief SPI HW initialization parameters */
typedef struct
{
    /** \brief Pointer to SPI HW registers */
    UNICOMM_Inst_Regs     *HwRegsPtr;
    /** \brief Pointer to user arguments, returned back to user when any SPI callback is called */
    void                     *UserArgsPtr;
    /*! \brief SPI mode */
    Spi_Hal_ModeType          Mode;
    /*! \brief Disable Tx, Only applicable in peripheral mode */
    boolean                   RxOnly;
    /*! \brief Data Width */
    Spi_Hal_DataWidthType     DataWidth;
    /*! \brief Frame format */
    Spi_Hal_FrameFormatType   FrameFormat;
    /*! \brief Bit order MSB/LSB First */
    Spi_Hal_BitOrderType      BitOrder;
    /*! \brief Parity */
    Spi_Hal_ParityType        Parity;
    /*! \brief Bitrate (in Hz) */
    uint32                    BitRateHz;
    /*! \brief Input clock Frequency (in Hz) */
    uint32                    InputClockFreqHz;
    /*! \brief Clock divide ratio */
    DL_SPI_CLOCK_DIVIDE_RATIO ClockDivideRatio;

} Spi_Hal_InitParamsType;

/** \brief SPI HW reconfiguration parameters */
typedef struct
{
    /*! \brief Data size */
    Spi_Hal_DataWidthType   DataWidth;
    /*! \brief Frame format */
    Spi_Hal_FrameFormatType FrameFormat;
    /*! \brief Bit order MSB/LSB First */
    Spi_Hal_BitOrderType    BitOrder;
    /*! \brief Parity */
    Spi_Hal_ParityType      Parity;
    /*! \brief Bitrate (in Hz) */
    uint32                  BitRateHz;
} Spi_Hal_ReConfigParamsType;

/** \brief SPI HW instance structure. Should not be modified by application directly */
typedef struct
{
    /** \brief Pointer to SPI HW registers */
    UNICOMM_Inst_Regs *HwRegsPtr;

    /** \brief Pointer to user arguments, returned back to user when any SPI callback is called */
    void                 *UserArgsPtr;
    /** \brief SPI state */
    Spi_Hal_StateType     State;
    /** \brief buffer from which Transfer sends the data */
    uint8                *TxBufferPtr;
    /** \brief buffer into which Transfer receives the data */
    uint8                *RxBufferPtr;
    /** \brief Size of data that is remaining to be sent */
    uint32                RemainingTXSize;
    /** \brief Size of data that is remaining to be read */
    uint32                RemainingRXSize;
    /** \brief Data Width */
    Spi_Hal_DataWidthType DataWidth;
    /** \brief Error code, this is a bit mask of errors, 0 means no error */
    uint32                ErrorCodeMask;
} Spi_Hal_InstanceType;

/** \brief SPI Transfer status*/
typedef struct
{
    /** \brief Transfer status */
    boolean IsTransferComplete;
    /** \brief Error code, this is a bit mask of errors, 0 means no error.
     *  If an error, it can have a bit mask of the following:
     *  - DL_SPI_INTERRUPT_PARITY_ERROR
     *  - DL_SPI_INTERRUPT_RX_OVERFLOW
     *  - DL_SPI_INTERRUPT_TX_UNDERFLOW
     */
    uint32  ErrorCodeMask;
    /** \brief Size of data that is remaining to be sent */
    uint32  RemainingTXSize;
    /** \brief Size of data that is remaining to be read */
    uint32  RemainingRXSize;
} Spi_Hal_TransferStatusType;

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

/**
 * \brief Initialize params
 *
 * \param[out] paramsPtr Pointer to SPI HW initialization parameters
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_InitParamsSetDefault(Spi_Hal_InitParamsType *paramsPtr);

/**
 * \brief Initialize a SPI HW instance
 *
 * Initializes the SPI hardware instance with the provided configuration parameters.
 * The function configures timing parameters, operating parameters and prepares hardware for transmission.
 *
 * \param[inout] instancePtr Pointer to SPI HW instance
 * \param[in]  paramsPtr Pointer to SPI HW initialization parameters
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_Init(Spi_Hal_InstanceType *instancePtr, const Spi_Hal_InitParamsType *paramsPtr);

/**
 * \brief Reconfigure a SPI HW instance to interface with a peripheral device with different configuration
 */
sint32 Spi_Hal_Reconfigure(Spi_Hal_InstanceType *instancePtr, const Spi_Hal_ReConfigParamsType *paramsPtr);

/**
 * \brief Transfers data in Polling mode
 *
 * Can transfer data in both Controller and Peripheral mode
 *
 * Depending on the transfer options, it can perform:
 * - Transmitting data
 * - Receiving data
 * - Both transmitting and receiving data
 *
 * The function:
 * - Performs data transfer (TX and/or RX)
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 * \param[in] txBufferPtr Pointer to transfer buffer. Can be NULL if only receiving data.
 *                        When NULL, dummy data will be transmitted.
 * \param[in] rxBufferPtr Pointer to receive buffer. Can be NULL if only transmitting data.
 *                        When NULL, received data will be discarded.
 * \param[in] size Number of SPI frames to be transmitted
 *
 * \note Either txBufferPtr or rxBufferPtr must be non-NULL. Both cannot be NULL simultaneously.
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_TransferPolling(Spi_Hal_InstanceType *instancePtr, void *txBufferPtr, void *rxBufferPtr, uint32 size);

/**
 * \brief Continues the transfer of remaining bytes and gets transfer status in polling mode.
 * Can be called only when interrupt mode is disabled.
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 * \param[inout] statusPtr Pointer that stores the status of current transfer
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_TransferContinuePolling(Spi_Hal_InstanceType *instancePtr, Spi_Hal_TransferStatusType *statusPtr);

/**
 * \brief Transfers data in Interrupt Mode
 *
 * Can transfer data in both Controller and Peripheral mode
 *
 * Depending on the transfer options, it can perform:
 * - Transmitting data
 * - Receiving data
 * - Both transmitting and receiving data
 *
 * The function:
 * - Performs data transfer (TX and/or RX)
 *
 * The function returns immediately, and the transfer is handled asynchronously
 * through interrupts. When the transfer is complete, the configured callback
 * function will be invoked.
 *
 * See also @ref Spi_Hal_TransferCompleteCallback
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 * \param[in] txBufferPtr Pointer to transfer buffer. Can be NULL if only receiving data.
 *                        When NULL, dummy data will be transmitted.
 * \param[in] rxBufferPtr Pointer to receive buffer. Can be NULL if only transmitting data.
 *                        When NULL, received data will be discarded.
 * \param[in] size Number of SPI frames to be transmitted
 *
 * \note Either txBufferPtr or rxBufferPtr must be non-NULL. Both cannot be NULL simultaneously.
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_TransferInterrupt(Spi_Hal_InstanceType *instancePtr, void *txBufferPtr, void *rxBufferPtr, uint32 size);

/**
 * \brief Transfers data in DMA mode
 *
 * Can transfer data in both Controller and Peripheral mode
 *
 * Depending on the transfer options, it can perform:
 * - Transmitting data
 * - Receiving data
 * - Both transmitting and receiving data
 *
 * The function:
 * - Performs data transfer (TX and/or RX)
 *
 * See also @ref Spi_Hal_TransferCompleteCallback
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 * \param[in] txBufferPtr Pointer to transfer buffer
 * \param[in] rxBufferPtr Pointer to receive buffer
 * \param[in] size Number of SPI frames to be transmitted
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_TransferDma(Spi_Hal_InstanceType *instancePtr, void *txBufferPtr, void *rxBufferPtr, uint32 size);
/**
 * \brief Abort Transfer operation. Only valid in polling and interrupt mode
 */
sint32 Spi_Hal_TransferAbort(Spi_Hal_InstanceType *instancePtr);

/**
 * \brief Abort Transfer operation. Only valid in DMA mode
 */
sint32 Spi_Hal_TransferAbortDma(Spi_Hal_InstanceType *instancePtr);

/**
 * \brief Callback that is called when Transfer completes. Only valid in interrupt or DMA mode
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 * \param[in] userArgsPtr Pointer to user arguments passed during initialization
 */
void Spi_Hal_TransferCompleteCallback(Spi_Hal_InstanceType *instancePtr, void *userArgsPtr);

/**
 * \brief Callback that is called in case of any Transfer error. Only valid in interrupt or DMA mode
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 * \param[in] errorCodeMask Error code mask indicating the type of error
 * \param[in] userArgsPtr Pointer to user arguments passed during initialization
 */
void Spi_Hal_ErrorCallback(Spi_Hal_InstanceType *instancePtr, uint32 errorCodeMask, void *userArgsPtr);

/**
 * \brief Interrupt handler. Needs to be called from the ISR registered in the device's interrupt controller
 *
 * Application MUST register an ISR against the appropriate CPU interrupt number after
 * \ref Spi_Hal_Init. This function must be called as part of the ISR with the right parameters.
 * When polling is enabled as part of \ref Spi_Hal_Init, the interrupt handler should not be registered.
 *
 * The interrupt handler will call one of below callbacks, depending on the conditions detected
 * within the interrupt handler. The interrupt handler will also clear status and interrupt bits
 * as needed for correct operation.
 *
 * In each of the callbacks, user arguments passed during initialization are passed back to the
 * user.
 *
 * The callbacks need to be implemented by the application. This driver provides weak definitions for these.
 *
 *
 * The supported callbacks are,
 * - \ref Spi_Hal_TransferCompleteCallback
 * - \ref Spi_Hal_ErrorCallback
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 */
void Spi_Hal_InterruptHandler(Spi_Hal_InstanceType *instancePtr);

/**
 * \brief De-initialize a SPI HW instance
 *
 * Disable the HW and any enabled interrupts at the SPI HW.
 * Interrupts at CPU should be disabled by the application before calling this API.
 *
 * \param[in] instancePtr Pointer to SPI HW instance
 *
 * \return \ref SPI_HAL_ERROR_NONE on success, otherwise error code \ref Spi_Hal_ErrorType
 */
sint32 Spi_Hal_DeInit(Spi_Hal_InstanceType *instancePtr);

#ifdef __cplusplus
}
#endif

#endif /* SPI_HAL_H */

/** @} */
