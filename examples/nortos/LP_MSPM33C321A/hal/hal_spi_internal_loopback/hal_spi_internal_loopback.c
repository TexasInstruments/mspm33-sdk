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
 * SPI HAL Internal Loopback Example for MSPM33
 * Demonstrates controller-mode SPI with internal hardware loopback (PICO → POCI)
 * Transmits and receives 256 bytes using polling transfers
 */

/* STD includes */
#include <string.h>

/* SDK includes */
#include <ti/hal/mspm33/include/Spi_Hal.h>

/* Syscfg includes (includes driverlib) */
#include "ti_msp_dl_config.h"

/* Device specific configuration */
#include "app_cfg.h"

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

#define APP_API_STATUS_OK (0U)
#define APP_API_STATUS_FAIL (1U)
#define APP_BUFF_MAX_SIZE_BYTES (256U)
#define APP_TIMEOUT_COUNT (1000000U)

/* ========================================================================== */
/*                 Private Variable Declarations                              */
/* ========================================================================== */

Spi_Hal_InstanceType App_Spi_ControllerHandle;
uint8_t App_Spi_ControllerOutBuff[APP_BUFF_MAX_SIZE_BYTES];
uint8_t App_Spi_ControllerInBuff[APP_BUFF_MAX_SIZE_BYTES];

/* ========================================================================== */
/*                 Private Function Prototypes                                */
/* ========================================================================== */

static sint32 App_Spi_Init(Spi_Hal_InstanceType *controllerInstancePtr);
static sint32 App_Spi_DeInit(Spi_Hal_InstanceType *controllerInstancePtr);
static sint32 App_Spi_SendBuffer(Spi_Hal_InstanceType *instancePtr,
    uint8_t *outBufferPtr, uint8_t *inBufferPtr, uint32 sizeBytes);

/* ========================================================================== */
/*                 Main Function                                              */
/* ========================================================================== */

int main(void)
{
    sint32 apiStatus;

    SYSCFG_DL_init();

    App_Spi_InitGPIO();

    for (uint16_t i = 0; i < APP_BUFF_MAX_SIZE_BYTES; i++) {
        App_Spi_ControllerOutBuff[i] = (uint8_t) i;
    }

    apiStatus = App_Spi_Init(&App_Spi_ControllerHandle);
    if (APP_API_STATUS_OK != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    apiStatus = App_Spi_SendBuffer(&App_Spi_ControllerHandle,
        App_Spi_ControllerOutBuff, App_Spi_ControllerInBuff,
        APP_BUFF_MAX_SIZE_BYTES);
    if (APP_API_STATUS_OK != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    apiStatus = App_Spi_DeInit(&App_Spi_ControllerHandle);
    if (APP_API_STATUS_OK != apiStatus) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    if (memcmp(App_Spi_ControllerOutBuff, App_Spi_ControllerInBuff,
            APP_BUFF_MAX_SIZE_BYTES) != 0) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        __BKPT(0);
    }

    /* All tests pass */
    DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);

    return 0;
}

/* ========================================================================== */
/*                 Private Function Implementations                           */
/* ========================================================================== */

static sint32 App_Spi_Init(Spi_Hal_InstanceType *controllerInstancePtr)
{
    sint32 retVal;
    sint32 status;
    Spi_Hal_InitParamsType controllerConfig;

    controllerConfig.UserArgsPtr = NULL_PTR;
    controllerConfig.RxOnly      = FALSE;
    controllerConfig.HwRegsPtr   = APP_SPI_CONTROLLER_INSTANCE;

    controllerConfig.Mode        = SPI_HAL_MODE_CONTROLLER;
    controllerConfig.FrameFormat = SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL0_PHA0;
    controllerConfig.BitOrder    = SPI_HAL_BITORDER_MSB_FIRST;
    controllerConfig.Parity      = SPI_HAL_PARITY_NONE;
    controllerConfig.DataWidth   = SPI_HAL_DATA_WIDTH_8;
    controllerConfig.BitRateHz   = APP_SPI_BIT_RATE_HZ;
    controllerConfig.InputClockFreqHz = 32000000U;
    controllerConfig.ClockDivideRatio = 64U;

    status = Spi_Hal_Init(controllerInstancePtr, &controllerConfig);
    if (SPI_HAL_ERROR_NONE != status) {
        __BKPT(0);
        retVal = APP_API_STATUS_FAIL;
    } else {
        App_Spi_LoopbackInit();
        retVal = APP_API_STATUS_OK;
    }

    return retVal;
}

static sint32 App_Spi_DeInit(Spi_Hal_InstanceType *controllerInstancePtr)
{
    App_Spi_LoopbackDeInit();
    Spi_Hal_DeInit(controllerInstancePtr);
    return APP_API_STATUS_OK;
}

static sint32 App_Spi_SendBuffer(Spi_Hal_InstanceType *instancePtr,
    uint8_t *outBufferPtr, uint8_t *inBufferPtr, uint32 sizeBytes)
{
    Spi_Hal_TransferStatusType txfStatus;
    uint32_least timeout;
    sint32 status;
    sint32 retVal;

    timeout = APP_TIMEOUT_COUNT;
    status  = Spi_Hal_TransferPolling(
        instancePtr, outBufferPtr, inBufferPtr, sizeBytes);
    if (SPI_HAL_ERROR_NONE == status) {
        do {
            status = Spi_Hal_TransferContinuePolling(instancePtr, &txfStatus);
        } while ((SPI_HAL_ERROR_NONE == status) &&
                 (txfStatus.IsTransferComplete == FALSE) &&
                 (txfStatus.ErrorCodeMask == 0U) && (--timeout > 0U));

        if ((SPI_HAL_ERROR_NONE == status) &&
            (txfStatus.IsTransferComplete == TRUE) &&
            (txfStatus.ErrorCodeMask == 0U) && (timeout > 0U)) {
            retVal = APP_API_STATUS_OK;
        } else {
            retVal = APP_API_STATUS_FAIL;
        }
    } else {
        retVal = APP_API_STATUS_FAIL;
    }

    return retVal;
}
