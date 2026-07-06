
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


#ifndef I2C_HAL_H
#define I2C_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "I2c_Hal_Cfg.h"
#include <ti/hal/compiler/m33_ti_arm_clang/Std_Types.h>
#include <ti/driverlib/dl_unicomm.h>
#include <ti/driverlib/dl_unicommi2cc.h>
#include <ti/driverlib/dl_unicommi2ct.h>

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/**
 *
 * @anchor I2c_Hal_ErrorType
 * @name I2C HAL API error codes
 *
 * All APIs in this module return below error codes
 *
 * @{
 */
/** @brief No error */
#define I2C_HAL_ERROR_NONE                (0)
/** @brief Invalid instance pointer */
#define I2C_HAL_ERROR_INVALID_INSTANCE    (1)
/** @brief Invalid parameters */
#define I2C_HAL_ERROR_INVALID_PARAMS      (2)
/** @brief Unsupported Feature (e.g., target API in controller mode) */
#define I2C_HAL_ERROR_UNSUPPORTED_FEATURE (3)
/** @brief HW Busy Error */
#define I2C_HAL_ERROR_HW_BUSY             (4)
/** @brief API Transfer Error */
#define I2C_HAL_ERROR_TRANSFER            (5)
/** @} */

/**
 *
 * @anchor I2c_Hal_TransferErrorType
 * @name I2C transfer error code bit mask values
 *
 * @{
 */

/** @brief No error during transfer */
#define I2C_HAL_TRANSFER_ERROR_NONE (0x00000000U)

/** @brief Controller operation timed out */
#define I2C_HAL_TRANSFER_ERROR_CONTROLLER_TIMEOUT (0x00000001U)

/** @brief Serial Clock Low Timeout Error*/
#define I2C_HAL_TRANSFER_ERROR_CLOCK_LOW_TIMEOUT (0x00000002U)

/** @brief Transfer Abort/ Terminate Error */
#define I2C_HAL_TRANSFER_ERROR_ABORT (0x00000004U)

/** @brief Target Receive FIFO Overflow Error*/
#define I2C_HAL_TRANSFER_ERROR_TARGET_RXFIFO_OVERFLOW (0x00000008U)

/** @brief Target NACKed the data from controller */
#define I2C_HAL_TRANSFER_ERROR_CONTROLLER_NACK_DATA (0x00000010U)

/** @brief Target NACKed the address from controller */
#define I2C_HAL_TRANSFER_ERROR_CONTROLLER_NACK_ADDR (0x00000020U)

/** @brief Controller lost arbitration in multi-master mode */
#define I2C_HAL_TRANSFER_ERROR_CONTROLLER_ARB_LOST (0x00000040U)

/** @brief Bus is busy, cannot initiate transfer */
#define I2C_HAL_TRANSFER_ERROR_BUS_BUSY (0x00000080U)

/** @brief Target NACK Error */
#define I2C_HAL_TRANSFER_ERROR_TARGET_NACK (0x00000100U)

/** @brief Bus Error */
#define I2C_HAL_TRANSFER_ERROR_BUS_ERROR (0x00000200U)

/** @brief Target Transmit FIFO Underflow Error */
#define I2C_HAL_TRANSFER_ERROR_TARGET_TXFIFO_UNDERFLOW (0x00000400U)

/** @brief Unspecified error */
#define I2C_HAL_TRANSFER_ERROR_UNSPECIFIED (0x02000000U)

/** @} */

/**
 *
 * @anchor I2c_Hal_TransferOptionsType
 * @name I2C transfer options
 *
 * @{
 */
/** @brief None specified */
#define I2C_HAL_TRANSFER_OPTION_NONE (0x00000000U)

/** @brief Enable START condition */
#define I2C_HAL_TRANSFER_OPTION_START_ENABLE (0x00000001U)

/** @brief Enable STOP condition */
#define I2C_HAL_TRANSFER_OPTION_STOP_ENABLE (0x00000002U)

/** @brief Write operation */
#define I2C_HAL_TRANSFER_OPTION_WRITE (0x00000004U)

/** @brief Read operation */
#define I2C_HAL_TRANSFER_OPTION_READ (0x00000008U)

/** @brief Enable clock stretching */
#define I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE (0x00000010U)

/** @brief Enable Packet Error Checking (PEC) */
#define I2C_HAL_TRANSFER_OPTION_PEC_ENABLE (0x00000020U)

/** @} */

/**
 *
 * @anchor I2c_Hal_ClockPeriodType
 * @name I2C Clock Low/High Period in I2C clock cycles
 *
 * @{
 */

/** @brief SCL low period in I2C clock cycles */
#define I2C_HAL_SCL_LOW_PERIOD (6U)

/** @brief SCL high period in I2C clock cycles */
#define I2C_HAL_SCL_HIGH_PERIOD (4U)

/** @} */

/* ========================================================================== */
/*                 Private Definitions                                            */
/* ========================================================================== */

/** @brief Controller transmit transfer options */
#define I2C_HAL_TRANSFER_OPTION_CONTROLLER_TX_DEFAULT \
    (I2C_HAL_TRANSFER_OPTION_START_ENABLE | I2C_HAL_TRANSFER_OPTION_STOP_ENABLE | I2C_HAL_TRANSFER_OPTION_WRITE)

/** @brief Controller receive transfer options */
#define I2C_HAL_TRANSFER_OPTION_CONTROLLER_RX_DEFAULT \
    (I2C_HAL_TRANSFER_OPTION_START_ENABLE | I2C_HAL_TRANSFER_OPTION_STOP_ENABLE | I2C_HAL_TRANSFER_OPTION_READ)

/** @brief Target transmit transfer options */
#define I2C_HAL_TRANSFER_OPTION_TARGET_TX_DEFAULT \
    (I2C_HAL_TRANSFER_OPTION_WRITE | I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE)

/** @brief Target receive transfer options */
#define I2C_HAL_TRANSFER_OPTION_TARGET_RX_DEFAULT \
    (I2C_HAL_TRANSFER_OPTION_READ | I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE)

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/**
 * @brief I2C instance states
 */
typedef enum
{
    I2C_HAL_STATE_UNCONFIGURED = 0U, /**< Instance not initialized or disabled */
    I2C_HAL_STATE_READY        = 1U, /**< Instance initialized and ready for use */
    I2C_HAL_STATE_TRANSFER     = 2U, /**< Data transfer is ongoing */
} I2c_Hal_StateType;

/**
 * @brief I2C speed modes
 */
typedef enum
{
    I2C_HAL_SPEED_STANDARD  = 0U, /**< Standard mode (100 kHz) */
    I2C_HAL_SPEED_FAST      = 1U, /**< Fast mode (400 kHz) */
    I2C_HAL_SPEED_FAST_PLUS = 2U, /**< Fast mode plus (1 MHz) */
    I2C_HAL_SPEED_MAX
} I2c_Hal_SpeedType;

/**
 * @brief I2C addressing modes
 */
typedef enum
{
    I2C_HAL_ADDRESSING_MODE_7BIT, /**< 7-bit addressing mode */
    I2C_HAL_ADDRESSING_MODE_10BIT /**< 10-bit addressing mode */
} I2c_Hal_AddressingModeType;

/**
 * @brief  I2C Operational Modes
 *
 */
typedef enum
{
    I2C_HAL_OPERATING_MODE_TRANSMIT, /**< I2C Transmit Mode */
    I2C_HAL_OPERATING_MODE_RECEIVE   /**< I2C Receive Mode */
} I2c_Hal_OperatingModeType;

/**
 * @brief I2C Controller initialization parameters
 */
typedef struct
{
    UNICOMM_Inst_Regs     *HwRegsPtr;        /**< Pointer to I2C HW registers */
    DL_I2CC_ClockConfig    ClockConfig;      /**< Clock configuration */
    I2c_Hal_SpeedType      Speed;            /**< Desired Speed mode */
    uint32_t               InputClockFreqHz; /**< Clock frequency in Hz */
    void                  *UserArgsPtr;      /**< Pointer to custom user Argument */
} I2c_Hal_ControllerInitParamsType;

/**
 * @brief I2C Controller instance
 *
 * This maintain state of the I2C instance in controller mode. Application
 * should not dereference and read/write fields in this structure.
 */
typedef struct
{
    /** \brief Pointer to I2C HW registers */
    UNICOMM_Inst_Regs        *HwRegsPtr;
    /** \brief Pointer to custom user Argument */
    void                     *UserArgsPtr;
    /** \brief  Current state of I2C Instance */
    I2c_Hal_StateType         CurrentState;
    /** \brief  Current Mode of I2C Instance */
    I2c_Hal_OperatingModeType CurrentMode;
    /** \brief Holds pointer to the Transfer buffer */
    uint8_t                  *BufferPtr;
    /** \brief Size of data that needs to be Transfered */
    uint32_t                  SizeInBytes;
    /** \brief Holds the remaining bytes to be Transfered */
    uint32_t                  RemainingSizeInBytes;

} I2c_Hal_ControllerInstanceType;

/**
 * @brief  I2C Controller transaction parameters
 */
typedef struct
{
    uint16_t                   TargetAddress;   /**< Target address for the transaction */
    I2c_Hal_AddressingModeType AddressingMode;  /**< Addressing mode (7-bit or 10-bit) */
    uint32_t                   TransferOptions; /**< Transfer options, see \ref I2c_Hal_TransferOptionsType */
    uint8_t                   *BufferPtr;       /**< Pointer to data buffer */
    uint32_t                   SizeInBytes;     /**< Size of the buffer in bytes */

} I2c_Hal_ControllerTransactionType;

/**
 * @brief I2C Target initialization parameters
 */
typedef struct
{
    UNICOMM_Inst_Regs     *HwRegsPtr;   /**< Pointer to I2C HW registers */
    DL_I2CT_ClockConfig    ClockConfig; /**< Clock configuration */
    uint16_t               OwnAddress;  /**< Own address (for target mode) */
    void                  *UserArgsPtr; /**< Pointer to custom user Argument */
} I2c_Hal_TargetInitParamsType;

/**
 * @brief I2C Target instance
 *
 * This maintain state of the I2C instance in controller mode. Application
 * should not dereference and read/write fields in this structure.
 */
typedef struct
{
    /** \brief Pointer to I2C HW registers */
    UNICOMM_Inst_Regs        *HwRegsPtr;
    /** \brief Pointer to custom user Argument */
    void                     *UserArgsPtr;
    /** \brief Current state of I2C HAL */
    I2c_Hal_StateType         CurrentState;
    /** \brief  Current Mode of I2C Instance */
    I2c_Hal_OperatingModeType CurrentMode;
    /** \brief Holds pointer to the Transfer buffer */
    uint8_t                  *BufferPtr;
    /** \brief Size of data that needs to be Transfered */
    uint32_t                  SizeInBytes;
    /** \brief Holds the remaining bytes to be Transfered  */
    uint32_t                  RemainingSizeInBytes;

} I2c_Hal_TargetInstanceType;

/**
 * @brief  I2C Target transaction parameters
 */
typedef struct
{
    uint32_t TransferOptions; /**< Target Transfer options, see I2c_Hal_TransferOptionsType */
    uint8_t *BufferPtr;       /**< Pointer to data buffer */
    uint32_t SizeInBytes;     /**< Size of the buffer in bytes */
} I2c_Hal_TargetTransactionType;

/**
 * @brief Transfer status, used by both controller and target APIs
 */
typedef struct
{
    /** \brief Active Buffer */
    uint8_t  *BufferPtr;
    /** \brief Size of data to transfer */
    uint32_t  SizeInBytes;
    /** \brief Size of data pending to be transferred */
    uint32_t  RemainingSizeInBytes;
    /** \brief Transfer Complete Status */
    boolean   IsTransferComplete;
    /** \brief Error code, this is a bit mask of errors defined by \ref I2c_Hal_TransferErrorType */
    uint32_t  ErrorCodeMask;
} I2c_Hal_TransactionStatusType;

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

/**
 * @brief Initializes the I2C Controller Parameters with default values
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */

sint32 I2c_Hal_ControllerInitParamsSetDefault(I2c_Hal_ControllerInitParamsType *paramsPtr);

/**
 * @brief Initializes the I2C Target Parameters with default values
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */

sint32 I2c_Hal_TargetInitParamsSetDefault(I2c_Hal_TargetInitParamsType *paramsPtr);

/**
 * @brief Initializes the I2C controller hardware
 *
 * This function initializes the I2C controller mode hardware instance with the
 * provided configuration parameters. The function configures clock settings,
 * operating parameters, and prepares the hardware for data transfer operations.
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_ControllerInit(I2c_Hal_ControllerInstanceType         *controllerInstancePtr,
                              const I2c_Hal_ControllerInitParamsType *controllerInitParams);

/**
 * @brief Performs a transfer operation in controller mode using polling
 *
 * This function executes a transfer operation to an I2C target device in controller mode.
 * Depending on the transfer options, it can perform:
 * - Writing data to the target
 * - Reading data from the target
 * - Combining write and read operations
 *
 * The function:
 * - Generates START condition
 * - Transmits target address
 * - Performs data transfer (TX and/or RX)
 * - Optionally generates STOP condition
 *
 * @return I2C_HAL_ERROR_NONE if operation completed successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_ControllerTransferPolling(I2c_Hal_ControllerInstanceType          *controllerInstancePtr,
                                         const I2c_Hal_ControllerTransactionType *controllerTransactionPtr);

/**
 * @brief Performs a transfer operation in controller mode using interrupts
 *
 * This function initiates a transfer operation to an I2C target device in controller mode.
 * Depending on the transfer options, it can perform:
 * - Writing data to the target
 * - Reading data from the target
 * - Combining write and read operations
 *
 * The function configures the controller to:
 * - Generate START condition
 * - Transmit target address
 * - Perform data transfer (TX and/or RX)
 * - Optionally generate STOP condition
 *
 * The function returns immediately, and the transfer is handled asynchronously
 * through interrupts. When the transfer is complete, the configured callback
 * function will be invoked.
 *
 * See also @ref I2c_Hal_ControllerTransferCompleteCallback
 *
 * @return I2C_HAL_ERROR_NONE if operation initiated successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_ControllerTransferInterrupt(I2c_Hal_ControllerInstanceType          *controllerInstancePtr,
                                           const I2c_Hal_ControllerTransactionType *controllerTransactionPtr);

/**
 * @brief Get I2C Controller Transaction Status. Only valid in polling mode.
 *
 * This function retrieves the current status of an ongoing or completed
 * Controller transfer operation.
 *
 * @return I2C_HAL_ERROR_NONE if operation completed successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_ControllerTransferContinuePolling(I2c_Hal_ControllerInstanceType *controllerInstancePtr,
                                                 I2c_Hal_TransactionStatusType  *transferStatusPtr);

/**
 * @brief Get I2C Controller Current State.
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_GetControllerCurrentState(const I2c_Hal_ControllerInstanceType *controllerInstancePtr,
                                         I2c_Hal_StateType                    *statePtr);

/**
 * @brief I2C Controller transmit complete callback. Only valid in interrupt mode.
 */
void I2c_Hal_ControllerTransferCompleteCallback(I2c_Hal_ControllerInstanceType *controllerInstancePtr,
                                                I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr);

/**
 * @brief Callback for any error in controller mode. Only valid in interrupt mode.
 */
void I2c_Hal_ControllerErrorCallback(I2c_Hal_ControllerInstanceType *controllerInstancePtr,
                                     I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr);

/** @brief I2C Controller interrupt handler
 *
 * Interrupt handler. Needs to be called from the ISR registered in the device's interrupt controller
 * Application MUST register an ISR against the appropriate CPU interrupt number after \ref I2c_Hal_ControllerInit
 *
 * In controller mode, if controller is initialized in polling mode, then interrupt handler should not be
 * registered.
 *
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
 *
 * - @ref I2c_Hal_ControllerTransferCompleteCallback
 * - @ref I2c_Hal_ControllerErrorCallback
 *
 */
void I2c_Hal_ControllerInterruptHandler(I2c_Hal_ControllerInstanceType *controllerInstancePtr);

/**
 * @brief De-initializes an I2C controller instance
 *
 * This function disables the I2C controller hardware and releases any resources
 * associated with it. After this call, the I2C controller hardware will be in
 * a reset state and must be re-initialized before being used again.
 *
 * @param[in] controllerInstancePtr Pointer to the I2C HAL Controller instance
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_ControllerDeInit(I2c_Hal_ControllerInstanceType *controllerInstancePtr);

/**
 * @brief Aborts an ongoing Controller transaction
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_ControllerTransferAbort(I2c_Hal_ControllerInstanceType *targetInstancePtr, uint32 timeoutCount,
                                       I2c_Hal_TransactionStatusType *transferStatusPtr);

/**
 * @brief Initializes the I2C target hardware
 *
 * This function initializes the I2C target mode hardware instance with the
 * provided configuration parameters. The function sets up the target address,
 * clock settings, and prepares the hardware to respond to controller requests.
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_TargetInit(I2c_Hal_TargetInstanceType         *targetInstancePtr,
                          const I2c_Hal_TargetInitParamsType *targetInitParams);

/**
 * @brief Get I2C Target Current State.
 *
 * @return I2C_HAL_ERROR_NONE if operation completed successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_GetTargetCurrentState(const I2c_Hal_TargetInstanceType *targetInstancePtr, I2c_Hal_StateType *statePtr);

/**
 * @brief Performs a transfer operation in target mode using polling
 *
 * This function prepares the I2C target to perform a transfer operation using polling:
 * - Receiving data from a controller or
 * - Sending data to a controller
 *
 * @return I2C_HAL_ERROR_NONE if operation completed successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_TargetTransferPolling(I2c_Hal_TargetInstanceType    *targetInstancePtr,
                                     I2c_Hal_TargetTransactionType *targetTransactionPtr);

/**
 * @brief Performs a transfer operation in target mode using interrupts
 *
 * This function prepares the I2C target to perform a transfer operation:
 * - Receiving data from a controller
 * - Sending data to a controller
 *
 * It configures the appropriate buffer and sets up the hardware to handle
 * the incoming or outgoing transfer through interrupts. The function returns
 * immediately, and the transfer is processed asynchronously when the target
 * is addressed by a controller.
 *
 * When the transfer is complete (STOP condition received), the configured
 * callback function will be invoked.
 *
 * @ref I2c_Hal_TargetTransferCompleteCallback
 *
 * @return I2C_HAL_ERROR_NONE if operation setup successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_TargetTransferInterrupt(I2c_Hal_TargetInstanceType    *targetInstancePtr,
                                       I2c_Hal_TargetTransactionType *targetTransactionPtr);

/**
 * @brief Get Target Transaction Status. Only valid in polling mode.
 *
 * This function retrieves the current status of an ongoing or completed
 * Target transfer operation.
 *
 * @return I2C_HAL_ERROR_NONE if operation completed successfully, otherwise an appropriate error code
 */
sint32 I2c_Hal_TargetTransferContinuePolling(I2c_Hal_TargetInstanceType    *targetInstancePtr,
                                             I2c_Hal_TransactionStatusType *transferStatusPtr);

/**
 * @brief I2C Target address match callback handler
 *
 * This function is called internally by the I2C HAL when the target receives an address match event.
 * It handles the address match condition by determining the requested transfer direction
 * and preparing the target hardware for the subsequent data transfer phase.
 * This callback is essential for target mode operation as it initiates the appropriate
 * response based on whether the controller wants to read from or write to the target.
 *
 * @param[in] targetInstancePtr Pointer to the I2C target instance handle
 * @param[in] isTransferDirectionRead Indicates whether the controller wants to read from (1) or write to (0) the target
 * @param[in] matchedAddress The matched address value, useful for multi-address targets
 * @param[in] userArgsPtr User-provided data pointer passed during callback registration
 *
 * @return I2C_HAL_ERROR_NONE if address match handled successfully, otherwise an appropriate error code
 */

sint32 I2c_Hal_TargetAddressMatchCallback(I2c_Hal_TargetInstanceType *targetInstancePtr,
                                          boolean isTransferDirectionRead, uint16_t matchedAddress, void *userArgsPtr);

/**
 * @brief I2C Target transfer complete callback. Only valid in interrupt mode.
 *
 */
void I2c_Hal_TargetTransferCompleteCallback(I2c_Hal_TargetInstanceType    *targetInstancePtr,
                                            I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr);

/**
 * @brief Callback for any error in target mode. Only valid in interrupt.
 */
void I2c_Hal_TargetErrorCallback(I2c_Hal_TargetInstanceType    *targetInstancePtr,
                                 I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr);

/**
 * @brief I2C Target interrupt handler
 *
 * Interrupt handler. Needs to be called from the ISR registered in the device's interrupt controller
 * Application MUST register an ISR against the appropriate CPU interrupt number after \ref I2c_Hal_TargetInit
 * This function must be called as part  of the ISR with the right params
 *
 * In target mode, interrupt handler MUST always be registered.
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
 * The Supported callbacks are,
 *
 * - @ref I2c_Hal_TargetAddressMatchCallback
 * - @ref I2c_Hal_TargetTransferCompleteCallback
 * - @ref I2c_Hal_TargetErrorCallback
 *
 */
void I2c_Hal_TargetInterruptHandler(I2c_Hal_TargetInstanceType *targetInstancePtr);

/**
 * @brief De-initializes an I2C target instance
 *
 * This function disables the I2C target hardware and releases any resources
 * associated with it. After this call, the I2C target hardware will be in
 * a reset state and must be re-initialized before being used again.
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_TargetDeInit(I2c_Hal_TargetInstanceType *targetInstancePtr);

/**
 * @brief Aborts an ongoing Target transaction
 *
 *
 * @return I2C_HAL_ERROR_NONE if successful, otherwise an error code
 */
sint32 I2c_Hal_TargetTransferAbort(I2c_Hal_TargetInstanceType    *targetInstancePtr,
                                   I2c_Hal_TransactionStatusType *transferStatusPtr);

#ifdef __cplusplus
}
#endif

#endif /* I2C_HAL_H */

/** @} */

