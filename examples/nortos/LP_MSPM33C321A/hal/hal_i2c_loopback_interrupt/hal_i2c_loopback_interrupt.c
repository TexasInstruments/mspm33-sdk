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
 * I2C HAL Interrupt Mode Loopback Example for MSPM33
 * Demonstrates controller-target I2C communication with interrupt-driven transfers
 * Two instances on one board: UC1_0 (controller), UC13_0 (target)
 * Wiring: PA0↔PB12 (SDA), PA1↔PB13 (SCL)
 */

#include <string.h>
#include <ti/hal/mspm33/include/I2c_Hal.h>

/* Syscfg includes (includes driverlib) */
#include "ti_msp_dl_config.h"

/* Device specific configuration */
#include "app_cfg.h"
#define APP_I2C_BUFFER_SIZE_BYTES (16U)
#define APP_I2C_TIMEOUT_COUNT (100000U)
#define APP_I2C_MODULO_BASE (256U)
#define APP_I2C_HW_BUSY_TIMEOUT_COUNT (10000U)

#define APP_I2C_CIPHER_MODE_COMPARISON_TRUE (1U)
#define APP_I2C_CIPHER_MODE_COMPARISON_FALSE (0U)

#define APP_I2C_SHIFT_DATA(dataPtr, index, baseValue) \
    ((((dataPtr[index]) + (index)) % (baseValue)))

static I2c_Hal_TargetInstanceType App_I2c_TargetHandle;
static I2c_Hal_ControllerInstanceType App_I2c_ControllerHandle;

static uint8_t App_I2c_ControllerTxBuffer[APP_I2C_BUFFER_SIZE_BYTES];
static uint8_t App_I2c_ControllerRxBuffer[APP_I2C_BUFFER_SIZE_BYTES];
static uint8_t App_I2c_TargetRxBuffer[APP_I2C_BUFFER_SIZE_BYTES];
static uint8_t App_I2c_TargetTxBuffer[APP_I2C_BUFFER_SIZE_BYTES];

static volatile uint8_t App_I2c_ControllerTransferDone  = 0;
static volatile uint8_t App_I2c_TargetTransferDone      = 0;
static volatile uint8_t App_I2c_ControllerTransferError = 0;
static volatile uint8_t App_I2c_TargetTransferError     = 0;

static void App_I2c_InitializeBuffers(
    uint8_t *txBufferPtr, uint8_t *rxBufferPtr);
static void App_I2c_CipherShiftData(uint8_t *dataPtr, uint32_t sizeBytes);
static int32_t App_I2c_WaitForTransferCompletion(uint32_t timeoutCount);
static uint8_t App_I2c_VerifyTransferData(uint8_t *txBufferPtr,
    uint8_t *rxBufferPtr, uint32_t sizeBytes, uint8_t isCipherMode);
static int32_t App_I2c_Controller_Init(void);
static int32_t App_I2c_Target_Init(void);
static int32_t App_I2c_DeInit(void);
static void App_I2c_ExampleInitialization(void);

int main(void)
{
    int32_t status = I2C_HAL_ERROR_NONE;
    I2c_Hal_ControllerTransactionType controllerTxConfig, controllerRxConfig;
    I2c_Hal_TargetTransactionType targetRxConfig, targetTxConfig;
    uint8_t transferSuccess = 0;

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

    /* PART 1: Controller writes data to Target */
    App_I2c_ControllerTransferDone  = 0;
    App_I2c_TargetTransferDone      = 0;
    App_I2c_ControllerTransferError = 0;
    App_I2c_TargetTransferError     = 0;

    targetRxConfig.BufferPtr   = App_I2c_TargetRxBuffer;
    targetRxConfig.SizeInBytes = APP_I2C_BUFFER_SIZE_BYTES;
    targetRxConfig.TransferOptions =
        I2C_HAL_TRANSFER_OPTION_READ |
        I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE;

    status = I2c_Hal_TargetTransferInterrupt(
        &App_I2c_TargetHandle, &targetRxConfig);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    controllerTxConfig.BufferPtr      = App_I2c_ControllerTxBuffer;
    controllerTxConfig.SizeInBytes    = APP_I2C_BUFFER_SIZE_BYTES;
    controllerTxConfig.TargetAddress  = I2CT_TARGET_OWN_ADDR;
    controllerTxConfig.AddressingMode = I2C_HAL_ADDRESSING_MODE_7BIT;
    controllerTxConfig.TransferOptions =
        I2C_HAL_TRANSFER_OPTION_WRITE | I2C_HAL_TRANSFER_OPTION_START_ENABLE;

    status = I2c_Hal_ControllerTransferInterrupt(
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

    for (uint32_t i = 0; i < APP_I2C_BUFFER_SIZE_BYTES; i++) {
        App_I2c_TargetTxBuffer[i] = App_I2c_TargetRxBuffer[i];
    }

    /* PART 2: Target sends processed data back to Controller (repeated start) */
    App_I2c_ControllerTransferDone  = 0;
    App_I2c_TargetTransferDone      = 0;
    App_I2c_ControllerTransferError = 0;
    App_I2c_TargetTransferError     = 0;

    targetTxConfig.BufferPtr   = App_I2c_TargetTxBuffer;
    targetTxConfig.SizeInBytes = APP_I2C_BUFFER_SIZE_BYTES;
    targetTxConfig.TransferOptions =
        I2C_HAL_TRANSFER_OPTION_WRITE |
        I2C_HAL_TRANSFER_OPTION_CLOCK_STRETCH_ENABLE;

    status = I2c_Hal_TargetTransferInterrupt(
        &App_I2c_TargetHandle, &targetTxConfig);
    if (status != I2C_HAL_ERROR_NONE) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    controllerRxConfig.BufferPtr       = App_I2c_ControllerRxBuffer;
    controllerRxConfig.SizeInBytes     = APP_I2C_BUFFER_SIZE_BYTES;
    controllerRxConfig.TargetAddress   = I2CT_TARGET_OWN_ADDR;
    controllerRxConfig.AddressingMode  = I2C_HAL_ADDRESSING_MODE_7BIT;
    controllerRxConfig.TransferOptions = I2C_HAL_TRANSFER_OPTION_READ |
                                         I2C_HAL_TRANSFER_OPTION_START_ENABLE |
                                         I2C_HAL_TRANSFER_OPTION_STOP_ENABLE;

    status = I2c_Hal_ControllerTransferInterrupt(
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

static void App_I2c_InitializeBuffers(
    uint8_t *txBufferPtr, uint8_t *rxBufferPtr)
{
    for (uint32_t i = 0; i < APP_I2C_BUFFER_SIZE_BYTES; i++) {
        txBufferPtr[i] = (uint8_t)(i + 1U);
    }
    memset(rxBufferPtr, 0, APP_I2C_BUFFER_SIZE_BYTES);
}

static void App_I2c_CipherShiftData(uint8_t *dataPtr, uint32_t sizeBytes)
{
    for (uint32_t i = 0; i < sizeBytes; i++) {
        dataPtr[i] =
            (uint8_t)(APP_I2C_SHIFT_DATA(dataPtr, i, APP_I2C_MODULO_BASE));
    }
}

static int32_t App_I2c_WaitForTransferCompletion(uint32_t timeoutCount)
{
    int32_t status = I2C_HAL_ERROR_NONE;

    do {
        if (App_I2c_ControllerTransferError || App_I2c_TargetTransferError) {
            status = I2C_HAL_ERROR_TRANSFER;
            break;
        }

        if (timeoutCount == 0) {
            I2c_Hal_TransactionStatusType controllerStatus, targetStatus;
            I2c_Hal_ControllerTransferAbort(&App_I2c_ControllerHandle,
                APP_I2C_HW_BUSY_TIMEOUT_COUNT, &controllerStatus);
            I2c_Hal_TargetTransferAbort(&App_I2c_TargetHandle, &targetStatus);
            status = I2C_HAL_ERROR_TRANSFER;
            break;
        }
        timeoutCount--;

    } while (!App_I2c_ControllerTransferDone || !App_I2c_TargetTransferDone);

    return status;
}

static uint8_t App_I2c_VerifyTransferData(uint8_t *txBufferPtr,
    uint8_t *rxBufferPtr, uint32_t sizeBytes, uint8_t isCipherMode)
{
    uint8_t status   = 1;
    uint8_t expected = 0U;

    for (uint32_t i = 0; i < sizeBytes; i++) {
        if (isCipherMode) {
            expected = (uint8_t)(
                APP_I2C_SHIFT_DATA(txBufferPtr, i, APP_I2C_MODULO_BASE));
        } else {
            expected = txBufferPtr[i];
        }

        if (rxBufferPtr[i] != expected) {
            status = 0;
            break;
        }
    }

    return status;
}

static int32_t App_I2c_Controller_Init(void)
{
    I2c_Hal_ControllerInitParamsType controllerInitParams;

    controllerInitParams.ClockConfig.clockSel    = I2CC_INST_CLOCK_SOURCE;
    controllerInitParams.ClockConfig.divideRatio = I2CC_INST_CLOCK_DIVIDE;
    controllerInitParams.InputClockFreqHz        = CPUCLK_FREQ;
    controllerInitParams.HwRegsPtr               = I2CC_INST;
    controllerInitParams.UserArgsPtr             = NULL;

    switch (I2CC_BUS_SPEED_HZ) {
        case 100000:
            controllerInitParams.Speed = I2C_HAL_SPEED_STANDARD;
            break;
        case 400000:
            controllerInitParams.Speed = I2C_HAL_SPEED_FAST;
            break;
        case 1000000:
            controllerInitParams.Speed = I2C_HAL_SPEED_FAST_PLUS;
            break;
    }

    int32_t status = I2c_Hal_ControllerInit(
        &App_I2c_ControllerHandle, &controllerInitParams);

    NVIC_ClearPendingIRQ(I2CC_INST_INT_IRQN);
    NVIC_EnableIRQ(I2CC_INST_INT_IRQN);

    return status;
}

static int32_t App_I2c_Target_Init(void)
{
    I2c_Hal_TargetInitParamsType targetInitParams;

    targetInitParams.ClockConfig.clockSel    = I2CT_INST_CLOCK_SOURCE;
    targetInitParams.ClockConfig.divideRatio = I2CT_INST_CLOCK_DIVIDE;
    targetInitParams.HwRegsPtr               = I2CT_INST;
    targetInitParams.OwnAddress              = I2CT_TARGET_OWN_ADDR;
    targetInitParams.UserArgsPtr             = NULL;

    int32_t status =
        I2c_Hal_TargetInit(&App_I2c_TargetHandle, &targetInitParams);

    NVIC_ClearPendingIRQ(I2CT_INST_INT_IRQN);
    NVIC_EnableIRQ(I2CT_INST_INT_IRQN);

    return status;
}

static void App_I2c_ExampleInitialization(void)
{
    App_I2c_InitializeBuffers(
        App_I2c_ControllerTxBuffer, App_I2c_TargetRxBuffer);
    App_I2c_InitializeBuffers(
        App_I2c_TargetTxBuffer, App_I2c_ControllerRxBuffer);
}

static int32_t App_I2c_DeInit(void)
{
    int32_t status;

    status = I2c_Hal_TargetDeInit(&App_I2c_TargetHandle);
    if (status != I2C_HAL_ERROR_NONE) {
        return status;
    }

    status = I2c_Hal_ControllerDeInit(&App_I2c_ControllerHandle);

    NVIC_DisableIRQ(I2CC_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(I2CC_INST_INT_IRQN);

    NVIC_DisableIRQ(I2CT_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(I2CT_INST_INT_IRQN);

    return status;
}

/* ========================================================================== */
/*                 Interrupt Handlers                                         */
/* ========================================================================== */

void App_I2c_ControllerIRQHandler(void)
{
    I2c_Hal_ControllerInterruptHandler(&App_I2c_ControllerHandle);
}

void App_I2c_TargetIRQHandler(void)
{
    I2c_Hal_TargetInterruptHandler(&App_I2c_TargetHandle);
}

/* ========================================================================== */
/*                 Transfer Complete Callbacks                                */
/* ========================================================================== */

void I2c_Hal_ControllerTransferCompleteCallback(
    I2c_Hal_ControllerInstanceType *controllerInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    (void) userArgsPtr;

    if (transferStatusPtr->IsTransferComplete &&
        (controllerInstancePtr == &App_I2c_ControllerHandle)) {
        App_I2c_ControllerTransferDone = 1;
    } else {
        App_I2c_ControllerTransferError = 1;
    }
}

void I2c_Hal_TargetTransferCompleteCallback(
    I2c_Hal_TargetInstanceType *targetInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    (void) userArgsPtr;

    if (transferStatusPtr->IsTransferComplete &&
        (targetInstancePtr == &App_I2c_TargetHandle)) {
        App_I2c_TargetTransferDone = 1;
    } else {
        App_I2c_TargetTransferError = 1;
    }
}

/* ========================================================================== */
/*                 Error Callbacks                                            */
/* ========================================================================== */

void I2c_Hal_ControllerErrorCallback(
    I2c_Hal_ControllerInstanceType *controllerInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    I2c_Hal_TransactionStatusType controllerStatus;

    (void) userArgsPtr;
    (void) transferStatusPtr;

    App_I2c_ControllerTransferError = 1;
    I2c_Hal_ControllerTransferAbort(controllerInstancePtr,
        APP_I2C_HW_BUSY_TIMEOUT_COUNT, &controllerStatus);
}

void I2c_Hal_TargetErrorCallback(I2c_Hal_TargetInstanceType *targetInstancePtr,
    I2c_Hal_TransactionStatusType *transferStatusPtr, void *userArgsPtr)
{
    I2c_Hal_TransactionStatusType targetStatus;

    (void) userArgsPtr;
    (void) transferStatusPtr;

    App_I2c_TargetTransferError = 1;
    I2c_Hal_TargetTransferAbort(targetInstancePtr, &targetStatus);
}
