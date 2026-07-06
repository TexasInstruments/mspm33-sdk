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

#include "ti_msp_dl_config.h"
#include "app_cfg.h"

#include "comm/i2c_comm.h"
#include "comm/spi_comm.h"
#include "dap/dap_interface.h"
#include "sensors/sensor.h"

#include <ti/driverlib/driverlib.h>
#include <ti/hal/mspm33/include/I2c_Hal.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>
#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <stdbool.h>

int main(void)
{
    SYSCFG_DL_init();

    App_Uart_InitGpio();

    /* I2C init */
    App_I2c_InitGPIO();
    if (I2c_Comm_Init() != 0) {
        while (1) {}
    }

    /* SPI init for BMI270/TMAG5170 */
    App_Spi_InitGPIO();
    SPI_init();

    /* Sensor init (all sensor initialization) */
    if (Sensor_Init() != 0) { while (1) {} }

    /* DAP init (Uart_Hal + Dap_Init + Dap_Open + NVIC for UART) */
    if (Dap_Interface_Init() != 0) { while (1) {} }

    volatile bool gStreamStarted = false;
    
    while (1) {
        Dap_Interface_ProcessCommands();

        if (Dap_Interface_IsStreaming()) {
            if (!gStreamStarted) {
                if (Dap_Interface_StartStream() == 0) {
                    gStreamStarted = true;
                    uint8_t  sel        = Dap_Interface_GetSelectedSensor();
                    uint32_t numSamples = Dap_Interface_GetSampleCount();
                    Sensor_AcquireSamples(sel, numSamples);
                    gStreamStarted = false;
                }
            }
        } else {
            gStreamStarted = false;
        }
    }
}

/* ========================================================================== */
/*                 Interrupt Handlers                                         */
/* ========================================================================== */

/* UART — new SDK DAP stack via Uart_Hal */
void UART_0_INST_IRQHandler(void)
{
    Uart_Hal_InterruptHandler((Uart_Hal_InstanceType *)Dap_Interface_GetUartInstance());
}

/* I2C — drives interrupt-based transfers for HDC3020, OPT4001, BMP384 */
void I2C_INST_IRQHandler(void)
{
    I2c_Hal_ControllerInterruptHandler(&gI2cInstance);
}

/* HSADC — PIR sensor ADC result ready */
void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1)) {
        if (DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST, DL_HSADC_INT_1)) {
            uint16_t result = DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);
            Sensor_Pir_SetADCResult(result);
        }
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
    }
}

/* ========================================================================== */
/*                 UART HAL callbacks (required by Uart_Hal.c)               */
/* ========================================================================== */

void Uart_Hal_TxCompleteCallback(Uart_Hal_InstanceType *instancePtr,
                                 Uart_Hal_TxStatusType *statusPtr,
                                 void *userArgsPtr)
{
    if (instancePtr == (Uart_Hal_InstanceType *)Dap_Interface_GetUartInstance()) {
        Dap_TransmitCallback(instancePtr, statusPtr, userArgsPtr);
    }
}

void Uart_Hal_RxCompleteCallback(Uart_Hal_InstanceType *instancePtr,
                                 Uart_Hal_RxStatusType *statusPtr,
                                 void *userArgsPtr)
{
    if (instancePtr == (Uart_Hal_InstanceType *)Dap_Interface_GetUartInstance()) {
        Dap_ReceiveCallback(instancePtr, statusPtr, userArgsPtr);
    }
}

/* ========================================================================== */
/*                 I2C HAL callbacks                                          */
/* ========================================================================== */

void I2c_Hal_ControllerTransferCompleteCallback(
    I2c_Hal_ControllerInstanceType *instancePtr,
    I2c_Hal_TransactionStatusType  *statusPtr,
    void                           *userArgsPtr)
{
    (void)instancePtr;
    (void)userArgsPtr;
    gI2cTransferErrorCode = 0U;
    gI2cTransferComplete  = true;
}

void I2c_Hal_ControllerErrorCallback(
    I2c_Hal_ControllerInstanceType *instancePtr,
    I2c_Hal_TransactionStatusType  *statusPtr,
    void                           *userArgsPtr)
{
    (void)instancePtr;
    (void)userArgsPtr;
    gI2cTransferErrorCode = statusPtr->ErrorCodeMask;
    gI2cTransferComplete  = true;
}

/* GPIOB — BMI270 data-ready (PB2) and TMAG5170 alert (PB3) */
void GPIOB_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(SENSOR_INTR_PORT)) {
        case SENSOR_INTR_ACC_GYRO_IIDX:
            DL_GPIO_clearInterruptStatus(SENSOR_INTR_PORT, SENSOR_INTR_ACC_GYRO_PIN);
            gDataReady_Acc_Gyro = 1;
            break;
        case SENSOR_INTR_MAGNET_IIDX:
            DL_GPIO_clearInterruptStatus(SENSOR_INTR_PORT, SENSOR_INTR_MAGNET_PIN);
            gDataReady_HallEffect = 1;
            break;
        default:
            break;
    }
}
