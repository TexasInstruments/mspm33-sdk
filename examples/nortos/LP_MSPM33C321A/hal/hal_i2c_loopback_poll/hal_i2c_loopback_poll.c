/*
 *  Copyright (C) 2026 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * I2C HAL Polling Mode Loopback Example for MSPM33
 * Demonstrates controller-target I2C communication with polling transfers
 * Two instances on one board: UC1_0 (controller), UC13_0 (target)
 * Wiring: PA0↔PB12 (SDA), PA1↔PB13 (SCL) via physical jumper wires
 *
 * Part 1: Controller writes 16 bytes to Target — target verifies receipt.
 * Part 2: Target applies cipher transform and sends data back to Controller
 *         using repeated start — controller verifies the transformed data.
 */

/* STD includes */
#include <string.h>

/* SDK includes */
#include <ti/hal/mspm33/include/I2c_Hal.h>

/* Syscfg includes (includes driverlib) */
#include "ti_msp_dl_config.h"

/* Device specific configuration */
#include "app_cfg.h"

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

#define APP_I2C_BUFFER_SIZE_BYTES (16U)
#define APP_I2C_TIMEOUT_COUNT (100000U)
#define APP_I2C_MODULO_BASE (256U)
#define APP_I2C_HW_BUSY_TIMEOUT_COUNT (10000U)

#define APP_I2C_CIPHER_MODE_COMPARISON_TRUE (1U)
#define APP_I2C_CIPHER_MODE_COMPARISON_FALSE (0U)

#define APP_I2C_SHIFT_DATA(dataPtr, index, baseValue) \
    ((((dataPtr[index]) + (index)) % (baseValue)))

/* ========================================================================== */
/*                 Private Variable Declarations                              */
/* ========================================================================== */

static I2c_Hal_TargetInstanceType App_I2c_TargetHandle;
static I2c_Hal_ControllerInstanceType App_I2c_ControllerHandle;

static uint8 App_I2c_ControllerTxBuffer[APP_I2C_BUFFER_SIZE_BYTES];
static uint8 App_I2c_ControllerRxBuffer[APP_I2C_BUFFER_SIZE_BYTES];
static uint8 App_I2c_TargetRxBuffer[APP_I2C_BUFFER_SIZE_BYTES];
static uint8 App_I2c_TargetTxBuffer[APP_I2C_BUFFER_SIZE_BYTES];

/* ========================================================================== */
/*                 Private Function Prototypes                                */
/* ========================================================================== */

static void App_I2c_InitializeBuffers(uint8 *txBufferPtr, uint8 *rxBufferPtr);
static void App_I2c_CipherShiftData(uint8 *dataPtr, uint32 sizeBytes);
static sint32 App_I2c_WaitForTransferCompletion(uint32 timeoutCount);
static boolean App_I2c_VerifyTransferData(uint8 *txBufferPtr,
    uint8 *rxBufferPtr, uint32 sizeBytes, boolean isCipherMode);
static sint32 App_I2c_Controller_Init(void);
static sint32 App_I2c_Target_Init(void);
static sint32 App_I2c_DeInit(void);
static void App_I2c_ExampleInitialization(void);

/* ========================================================================== */
/*                 Main Function                                              */
/* ========================================================================== */

int main(void)
{
    sint32 status = I2C_HAL_ERROR_NONE;
    I2c_Hal_ControllerTransactionType controllerTxConfig, controllerRxConfig;
    I2c_Hal_TargetTransactionType targetRxConfig, targetTxConfig;
    boolean transferSuccess = FALSE;

    SYSCFG_DL_init();

    App_I2c_InitGPIO();

    status = App_I2c_Controller_Init();
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    status = App_I2c_Target_Init();
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    App_I2c_ExampleInitialization();

    /*
     * PART 1: Controller writes data to Target
     */
    targetRxConfig.BufferPtr   = App_I2c_TargetRxBuffer;
    targetRxConfig.SizeInBytes = APP_I2C_BUFFER_SIZE_BYTES;
    targetRxConfig.TransferOptions =
        I2C_HAL_TRANSFER_OPTION_READ |
        I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE;

    status =
        I2c_Hal_TargetTransferPolling(&App_I2c_TargetHandle, &targetRxConfig);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    controllerTxConfig.BufferPtr      = App_I2c_ControllerTxBuffer;
    controllerTxConfig.SizeInBytes    = APP_I2C_BUFFER_SIZE_BYTES;
    controllerTxConfig.TargetAddress  = APP_I2C_TARGET_ADDRESS;
    controllerTxConfig.AddressingMode = I2C_HAL_ADDRESSING_MODE_7BIT;
    controllerTxConfig.TransferOptions =
        I2C_HAL_TRANSFER_OPTION_WRITE | I2C_HAL_TRANSFER_OPTION_START_ENABLE;

    status = I2c_Hal_ControllerTransferPolling(
        &App_I2c_ControllerHandle, &controllerTxConfig);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    status = App_I2c_WaitForTransferCompletion(APP_I2C_TIMEOUT_COUNT);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    transferSuccess = App_I2c_VerifyTransferData(App_I2c_ControllerTxBuffer,
        App_I2c_TargetRxBuffer, APP_I2C_BUFFER_SIZE_BYTES,
        APP_I2C_CIPHER_MODE_COMPARISON_FALSE);
    if (!transferSuccess) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    App_I2c_CipherShiftData(App_I2c_TargetRxBuffer, APP_I2C_BUFFER_SIZE_BYTES);

    for (uint32 i = 0; i < APP_I2C_BUFFER_SIZE_BYTES; i++) {
        App_I2c_TargetTxBuffer[i] = App_I2c_TargetRxBuffer[i];
    }

    /*
     * PART 2: Target sends processed data back to Controller (repeated start)
     */
    targetTxConfig.BufferPtr   = App_I2c_TargetTxBuffer;
    targetTxConfig.SizeInBytes = APP_I2C_BUFFER_SIZE_BYTES;
    targetTxConfig.TransferOptions =
        I2C_HAL_TRANSFER_OPTION_WRITE |
        I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE;

    status =
        I2c_Hal_TargetTransferPolling(&App_I2c_TargetHandle, &targetTxConfig);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    controllerRxConfig.BufferPtr       = App_I2c_ControllerRxBuffer;
    controllerRxConfig.SizeInBytes     = APP_I2C_BUFFER_SIZE_BYTES;
    controllerRxConfig.TargetAddress   = APP_I2C_TARGET_ADDRESS;
    controllerRxConfig.AddressingMode  = I2C_HAL_ADDRESSING_MODE_7BIT;
    controllerRxConfig.TransferOptions = I2C_HAL_TRANSFER_OPTION_READ |
                                         I2C_HAL_TRANSFER_OPTION_START_ENABLE |
                                         I2C_HAL_TRANSFER_OPTION_STOP_ENABLE;

    status = I2c_Hal_ControllerTransferPolling(
        &App_I2c_ControllerHandle, &controllerRxConfig);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    status = App_I2c_WaitForTransferCompletion(APP_I2C_TIMEOUT_COUNT);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    App_I2c_DeInit();

    transferSuccess = App_I2c_VerifyTransferData(App_I2c_ControllerTxBuffer,
        App_I2c_ControllerRxBuffer, APP_I2C_BUFFER_SIZE_BYTES,
        APP_I2C_CIPHER_MODE_COMPARISON_TRUE);
    if (!transferSuccess) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    /* All tests pass */
    DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    __BKPT(0);

    return 0;
}

/* ========================================================================== */
/*                 Private Function Implementations                           */
/* ========================================================================== */

static sint32 App_I2c_Controller_Init(void)
{
    I2c_Hal_ControllerInitParamsType controllerInitParams;

    I2c_Hal_ControllerInitParamsSetDefault(&controllerInitParams);
    controllerInitParams.HwRegsPtr   = APP_I2C_CONTROLLER_INSTANCE;
    controllerInitParams.UserArgsPtr = APP_I2C_CONTROLLER_INSTANCE;
    controllerInitParams.Speed       = I2C_HAL_SPEED_STANDARD;

    return I2c_Hal_ControllerInit(
        &App_I2c_ControllerHandle, &controllerInitParams);
}

static sint32 App_I2c_Target_Init(void)
{
    I2c_Hal_TargetInitParamsType targetInitParams;

    I2c_Hal_TargetInitParamsSetDefault(&targetInitParams);
    targetInitParams.HwRegsPtr  = APP_I2C_TARGET_INSTANCE;
    targetInitParams.OwnAddress = APP_I2C_TARGET_ADDRESS;

    return I2c_Hal_TargetInit(&App_I2c_TargetHandle, &targetInitParams);
}

static void App_I2c_ExampleInitialization(void)
{
    App_I2c_ExternalLoopbackInit();
    App_I2c_InitializeBuffers(
        App_I2c_ControllerTxBuffer, App_I2c_TargetRxBuffer);
    App_I2c_InitializeBuffers(
        App_I2c_TargetTxBuffer, App_I2c_ControllerRxBuffer);
}

static sint32 App_I2c_DeInit(void)
{
    sint32 status;

    App_I2c_ExternalLoopbackDeInit();

    status = I2c_Hal_TargetDeInit(&App_I2c_TargetHandle);
    if (status != I2C_HAL_ERROR_NONE) {
        return status;
    }

    return I2c_Hal_ControllerDeInit(&App_I2c_ControllerHandle);
}

static void App_I2c_InitializeBuffers(uint8 *txBufferPtr, uint8 *rxBufferPtr)
{
    for (uint32 i = 0; i < APP_I2C_BUFFER_SIZE_BYTES; i++) {
        txBufferPtr[i] = (uint8)(i + 1U);
    }
    memset(rxBufferPtr, 0, APP_I2C_BUFFER_SIZE_BYTES);
}

static void App_I2c_CipherShiftData(uint8 *dataPtr, uint32 sizeBytes)
{
    for (uint32 i = 0; i < sizeBytes; i++) {
        dataPtr[i] =
            (uint8)(APP_I2C_SHIFT_DATA(dataPtr, i, APP_I2C_MODULO_BASE));
    }
}

static sint32 App_I2c_WaitForTransferCompletion(volatile uint32 timeoutCount)
{
    I2c_Hal_TransactionStatusType controllerStatus, targetStatus;
    sint32 status = I2C_HAL_ERROR_NONE;

    do {
        volatile sint32 targetTransferStatus =
            I2c_Hal_TargetTransferContinuePolling(
                &App_I2c_TargetHandle, &targetStatus);

        volatile sint32 controllerTransferStatus =
            I2c_Hal_ControllerTransferContinuePolling(
                &App_I2c_ControllerHandle, &controllerStatus);

        if ((controllerTransferStatus != I2C_HAL_ERROR_NONE) ||
            (targetTransferStatus != I2C_HAL_ERROR_NONE)) {
            status = I2C_HAL_ERROR_TRANSFER;
            break;
        }

        timeoutCount--;
        if (timeoutCount == 0U) {
            I2c_Hal_ControllerTransferAbort(&App_I2c_ControllerHandle,
                APP_I2C_HW_BUSY_TIMEOUT_COUNT, &controllerStatus);
            I2c_Hal_TargetTransferAbort(&App_I2c_TargetHandle, &targetStatus);
            status = I2C_HAL_ERROR_TRANSFER;
            break;
        }

    } while ((!controllerStatus.IsTransferComplete) ||
             (!targetStatus.IsTransferComplete));

    return status;
}

static boolean App_I2c_VerifyTransferData(uint8 *txBufferPtr,
    uint8 *rxBufferPtr, uint32 sizeBytes, boolean isCipherMode)
{
    boolean status = TRUE;
    uint8 expected = 0U;

    for (uint32 i = 0; i < sizeBytes; i++) {
        if (isCipherMode) {
            expected = (uint8)(
                APP_I2C_SHIFT_DATA(txBufferPtr, i, APP_I2C_MODULO_BASE));
        } else {
            expected = txBufferPtr[i];
        }

        if (rxBufferPtr[i] != expected) {
            status = FALSE;
            break;
        }
    }

    return status;
}
