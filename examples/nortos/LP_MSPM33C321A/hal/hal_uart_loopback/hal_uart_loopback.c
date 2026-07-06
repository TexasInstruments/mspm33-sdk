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
   UART HAL Loopback Example for MSPM33
   Two UART instances: UC1_0 (TX) and UC1_1 (RX)
   TX: Polling mode via UC1_0 (PA0=TX, PA1=RX)
   RX: Interrupt mode via UC1_1 (PA16=TX, PA15=RX)
   Loopback: Physical jumper wires
     Jumper 1: PA0 (UC1_0 TX) → PA15 (UC1_1 RX)
     Jumper 2: PA1 (UC1_0 RX) ← PA16 (UC1_1 TX)
*/

/* STD includes */
#include <string.h>

/* SDK includes */
#include <ti/driverlib/driverlib.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>

/* Syscfg includes */
#include "ti_msp_dl_config.h"

/* Device specific configuration */
#include "app_cfg.h"

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

#define APP_UART_READ_STRING_MAX_BYTES (100U)
#define APP_UART_FNX_FAIL (0U)
#define APP_UART_FNX_PASS (1U)
#define APP_UART_RX_TIMEOUT_COUNT (1000000U)

/* ========================================================================== */
/*                 Private Variable Declarations                              */
/* ========================================================================== */

static Uart_Hal_InstanceType App_Uart_TxInstance;
static Uart_Hal_InstanceType App_Uart_RxInstance;
static volatile bool App_Uart_IsRxDone;
static volatile uint32_t App_Uart_RxError;

/* ========================================================================== */
/*                 Private Function Prototypes                                */
/* ========================================================================== */

static sint32 App_Uart_Init(Uart_Hal_InstanceType *txInstancePtr,
    Uart_Hal_InstanceType *rxInstancePtr);
static sint32 App_Uart_DeInit(Uart_Hal_InstanceType *txInstancePtr,
    Uart_Hal_InstanceType *rxInstancePtr);
static sint32 App_Uart_SendString(Uart_Hal_InstanceType *instancePtr,
    const char *txString, uint16_t sizeBytes);
static sint32 App_Uart_ReadString(
    Uart_Hal_InstanceType *instancePtr, char *rxString, uint16_t sizeBytes);

/* ========================================================================== */
/*                 Main Function                                              */
/* ========================================================================== */

int main(void)
{
    volatile sint32 apiStatus;
    const char txString[]                         = "Hello World !";
    char rxString[APP_UART_READ_STRING_MAX_BYTES] = {0};

    /* Initialize device (clocks, interrupts, etc.) */
    SYSCFG_DL_init();
    /* Initialize UART GPIO pins */
    App_Uart_InitGPIO();

    apiStatus = App_Uart_Init(&App_Uart_TxInstance, &App_Uart_RxInstance);
    if (APP_UART_FNX_PASS != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
        return 1;
    }

    apiStatus = App_Uart_SendString(
        &App_Uart_TxInstance, txString, strlen(txString) + 1);
    if (APP_UART_FNX_PASS != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
        return 1;
    }

    apiStatus = App_Uart_ReadString(
        &App_Uart_RxInstance, rxString, strlen(txString) + 1);
    if (APP_UART_FNX_PASS != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
        return 1;
    }

    apiStatus = App_Uart_DeInit(&App_Uart_TxInstance, &App_Uart_RxInstance);
    if (APP_UART_FNX_PASS != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
        return 1;
    }

    /* Verify received data matches transmitted data */
    if (0 != App_Uart_RxError) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
        return 1;
    }

    if (0 != strcmp(rxString, txString)) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
        return 1;
    }

    /* All tests passed - light green LED */
    DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    __BKPT(0);

    return 0;
}

/* ========================================================================== */
/*                 Private Function Implementations                           */
/* ========================================================================== */

static sint32 App_Uart_Init(
    Uart_Hal_InstanceType *txInstancePtr, Uart_Hal_InstanceType *rxInstancePtr)
{
    sint32 retVal;
    sint32 status;
    Uart_Hal_InitParamsType txInitParams;
    Uart_Hal_InitParamsType rxInitParams;

    /* Set default parameters for TX instance (UC1_0) */
    Uart_Hal_InitParamsSetDefault(&txInitParams);
    txInitParams.BaudRateBps  = APP_UART_BAUDRATE_BPS;
    txInitParams.HwRegsPtr    = APP_UART_TX_INST_PTR;
    txInitParams.InputClockHz = APP_UART_INPUT_CLOCK_HZ;

    /* Set default parameters for RX instance (UC1_1) */
    Uart_Hal_InitParamsSetDefault(&rxInitParams);
    rxInitParams.BaudRateBps  = APP_UART_BAUDRATE_BPS;
    rxInitParams.HwRegsPtr    = APP_UART_RX_INST_PTR;
    rxInitParams.InputClockHz = APP_UART_INPUT_CLOCK_HZ;

    /* Initialize TX instance first */
    status = Uart_Hal_Init(txInstancePtr, &txInitParams);
    if (UART_HAL_ERROR_NONE == status) {
        /* Initialize RX instance only if TX succeeded */
        status = Uart_Hal_Init(rxInstancePtr, &rxInitParams);
    }

    if (UART_HAL_ERROR_NONE == status) {
        /* Enable interrupts for RX UART instance (UC1_1) */
        NVIC_ClearPendingIRQ(APP_UART_RX_IRQ);
        NVIC_EnableIRQ(APP_UART_RX_IRQ);
        retVal = APP_UART_FNX_PASS;
    } else {
        retVal = APP_UART_FNX_FAIL;
    }

    return retVal;
}

static sint32 App_Uart_DeInit(
    Uart_Hal_InstanceType *txInstancePtr, Uart_Hal_InstanceType *rxInstancePtr)
{
    sint32 retVal;
    sint32 status;

    /* Disable interrupts for RX UART instance (UC1_1) */
    NVIC_DisableIRQ(APP_UART_RX_IRQ);
    NVIC_ClearPendingIRQ(APP_UART_RX_IRQ);

    /* De-initialize both UART instances */
    status = Uart_Hal_DeInit(txInstancePtr);
    status |= Uart_Hal_DeInit(rxInstancePtr);

    if (UART_HAL_ERROR_NONE == status) {
        retVal = APP_UART_FNX_PASS;
    } else {
        retVal = APP_UART_FNX_FAIL;
    }

    return retVal;
}

static sint32 App_Uart_SendString(Uart_Hal_InstanceType *instancePtr,
    const char *txString, uint16_t sizeBytes)
{
    sint32 retVal;
    sint32 apiStatus;
    Uart_Hal_TxStatusType txStatus;
    uint32_t timeoutCount;

    txStatus.IsTxComplete  = false;
    txStatus.ErrorCodeMask = 0U;
    retVal                 = APP_UART_FNX_PASS;

    /* Start TX in polling mode */
    apiStatus =
        Uart_Hal_TxPolling(instancePtr, (uint8_t *) txString, sizeBytes);
    if (UART_HAL_ERROR_NONE == apiStatus) {
        /* Poll for completion with timeout */
        timeoutCount = APP_UART_RX_TIMEOUT_COUNT;
        do {
            apiStatus = Uart_Hal_TxContinuePolling(instancePtr, &txStatus);
            timeoutCount--;
        } while ((UART_HAL_ERROR_NONE == apiStatus) &&
                 (!txStatus.IsTxComplete) && (txStatus.ErrorCodeMask == 0U) &&
                 (timeoutCount > 0));
    }

    if ((txStatus.ErrorCodeMask != 0U) || (UART_HAL_ERROR_NONE != apiStatus) ||
        (timeoutCount == 0)) {
        retVal = APP_UART_FNX_FAIL;
    }

    return retVal;
}

static sint32 App_Uart_ReadString(
    Uart_Hal_InstanceType *instancePtr, char *rxString, uint16_t sizeBytes)
{
    sint32 status;
    sint32 retVal;
    uint32_t timeoutCount;

    App_Uart_IsRxDone = false;
    App_Uart_RxError  = 0x0;

    /* Start RX in interrupt mode */
    status =
        Uart_Hal_RxInterrupt(instancePtr, (uint8_t *) rxString, sizeBytes);

    if (UART_HAL_ERROR_NONE == status) {
        /* Wait for interrupt-driven completion with timeout */
        timeoutCount = APP_UART_RX_TIMEOUT_COUNT;
        while ((false == App_Uart_IsRxDone) && (0x0 == App_Uart_RxError) &&
               (timeoutCount > 0)) {
            timeoutCount--;
        }

        if (timeoutCount == 0) {
            retVal = APP_UART_FNX_FAIL;
        } else if (App_Uart_RxError != 0x0) {
            retVal = APP_UART_FNX_FAIL;
        } else {
            retVal = APP_UART_FNX_PASS;
        }
    } else {
        retVal = APP_UART_FNX_FAIL;
    }

    return retVal;
}

/* ========================================================================== */
/*                 Interrupt Handlers and Callbacks                           */
/* ========================================================================== */

void App_Uart_RxIRQHandler(void)
{
    Uart_Hal_InterruptHandler(&App_Uart_RxInstance);
}

void Uart_Hal_TxCompleteCallback(Uart_Hal_InstanceType *instancePtr,
    Uart_Hal_TxStatusType *statusPtr, void *userArgsPtr)
{
    /* TX complete callback - not used in polling mode */
}

void Uart_Hal_RxCompleteCallback(Uart_Hal_InstanceType *instancePtr,
    Uart_Hal_RxStatusType *statusPtr, void *userArgsPtr)
{
    if ((&App_Uart_RxInstance == instancePtr) && statusPtr->IsRxComplete) {
        if (statusPtr->ErrorCodeMask == 0) {
            App_Uart_IsRxDone = true;
        } else {
            App_Uart_RxError = statusPtr->ErrorCodeMask;
        }
    }
}

/* Interrupt handler forwarding for UC1_1 */
void UC1_1_IRQHandler(void)
{
    App_Uart_RxIRQHandler();
}
