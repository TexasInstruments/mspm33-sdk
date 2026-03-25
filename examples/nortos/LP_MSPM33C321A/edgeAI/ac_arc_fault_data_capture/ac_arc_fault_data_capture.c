/*
 *  Copyright (C) 2025 Texas Instruments Incorporated
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

/* STD includes */
#include <string.h>
#include <stdio.h>

/* SysConfig generated includes */
#include "ti_msp_dl_config.h"

/* DAP includes */
#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */


/* ========================================================================== */
/*                 Private Variable Declarations                              */
/* ========================================================================== */


/* -------------------------------------------------------------------------- */
/*                 DAP Interface Configuration Data                           */
/* -------------------------------------------------------------------------- */

/* Sensor JSON strings */
static const char Dap_Sensor0Json[] = "{\"name\":\"Current_Sensor\",\"type\":4,\"columns\":2,\"dataFormat\":5,\"sequenceNumbers\":true,\"labels\":[\"Current\", \"Arc Gap Voltage\"]}";

/* Property structures */
static Dap_Interface_PropertyInfoType DAP_Property_Samples = {
    .NamePtr = "samples", .Type = DAP_DATA_FORMAT_UINT16, .Value = {.U16 = 0}};

/* DAP Interface Configuration */
static const Dap_InterfaceConfigType Test_Dap_InterfaceConfig = {
    /* Sensors */
    .SensorList    = {Dap_Sensor0Json, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR},
    .SensorCount   = 1U,
    /* Models */
    .ModelList     = {NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
    .ModelCount    = 0U,
    /* Properties */
    .PropertyList  = {&DAP_Property_Samples, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
    .PropertyCount = 1U,
    /* Inference values */
    .InfValueList  = {NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR},
    .InfValueCount = 0U};

/* ========================================================================== */
/*                 Public Variables                                           */
/* ========================================================================== */

Dap_InstanceType      Test_Dap_Instance;
Uart_Hal_InstanceType Test_Dap_UartInstance;

/* ========================================================================== */
/*                 Private Function Prototypes                                */
/* ========================================================================== */

static uint32_t App_Uart_Init(Uart_Hal_InstanceType *instancePtr);

/* ========================================================================== */
/*                 Main Function                                              */
/* ========================================================================== */

volatile int ADC_BUFFER_SIZE = 0;
volatile bool streamStarted = 0;
volatile uint16_t gADCresult0;
volatile uint16_t gADCresult1;
volatile int status;
volatile bool gADCNewResult = 1;
uint8_t adc_out[4];

int main(void)
{
    sint32       status             = 0;
    uint8_t        dummySensorData[1] = {100};
    uint8_t        dataCounter        = 0U;
    uint16 sampleSizes[DAP_INTERFACE_MAX_SENSORS];
    sampleSizes[0] = 4;
    uint32_t totalSamples  = 0;
    volatile boolean   isStreaming  = FALSE;

    /* Initialize device clocks and peripherals via SysConfig */
    SYSCFG_DL_init();

    /* Initialize UART HAL */
    Uart_Hal_InitParamsType initParams;

    Uart_Hal_InitParamsSetDefault(&initParams);
    initParams.BaudRateBps = UART_0_BAUD_RATE;
    initParams.HwRegsPtr   = (UNICOMM_Inst_Regs *)UART_0_INST;
    status = Uart_Hal_Init(&Test_Dap_UartInstance, &initParams);

    if (status == UART_HAL_ERROR_NONE)
    {
        printf("UART HAL initialized successfully\n");
    }
    else
    {
        printf("ERROR: UART HAL init failed!\n");
        /* UART init failed - halt */
        while (1);
    }
    /* Enable UART interrupt in NVIC */
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);

    Dap_InitParamsType dapParams;
    Dap_InitParamsSetDefault(&dapParams);

    dapParams.LinkParams.UartInstancePtr = &Test_Dap_UartInstance;
    dapParams.InterfaceConfigPtr         = &Test_Dap_InterfaceConfig;

    status = Dap_Init(&Test_Dap_Instance, &dapParams);
    if (status == DAP_ERROR_NONE)
    {
        printf("DAP initialized successfully\n");
    }

    status = Dap_Open(&Test_Dap_Instance);
    if (status == DAP_ERROR_NONE)
    {
        printf("DAP opened successfully\n");
    }

    while (1)
    {
        status = Dap_Process(&Test_Dap_Instance);
        Dap_IsStreaming(&Test_Dap_Instance, &isStreaming);

        if (isStreaming == TRUE)
        {
            if (streamStarted == FALSE)
            {
                totalSamples = Test_Dap_InterfaceConfig.PropertyList[0]->Value.U16;
                /* sampleSizes and totalSamples args would be calculated here in end application */
                status = Dap_StartSensorStream(&Test_Dap_Instance, sampleSizes, totalSamples);
                if (status == DAP_ERROR_NONE)
                {
                    dataCounter   = 0U;
                    streamStarted = TRUE;
                }
                DL_Timer_startCounter(TIMER_0_INST);
            }

            if (streamStarted == TRUE)
            {
                if(gADCNewResult)
                {
                    adc_out[0] = (gADCresult0 >> 8);
                    adc_out[1] = (gADCresult0 & 0x00FF);
                    adc_out[2] = (gADCresult1 >> 8);
                    adc_out[3] = (gADCresult1 & 0x00FF);

                    gADCNewResult = 0;

                    status = Dap_StreamSensorSample(&Test_Dap_Instance, 0U, (const uint8_t *)adc_out,sizeof(adc_out));
                }
                if(Test_Dap_Instance.StreamingContext.IsActive == false) // Max Data Collected
                {
                    DL_Timer_stopCounter(TIMER_0_INST);
                }
            }
        }
        else
        {
            streamStarted = FALSE;
        }
    }
}

/* ========================================================================== */
/*                 Interrupt Handlers and Callbacks                           */
/* ========================================================================== */

void UART_0_INST_IRQHandler(void)
{
    Uart_Hal_InterruptHandler(&Test_Dap_UartInstance);
}

void Uart_Hal_TxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_TxStatusType *statusPtr,
                                 void *userArgsPtr)
{
    if (instancePtr == &Test_Dap_UartInstance)
    {
        Dap_TransmitCallback(instancePtr, statusPtr, userArgsPtr);
    }
}

void Uart_Hal_RxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_RxStatusType *statusPtr,
                                 void *userArgsPtr)
{
    if (instancePtr == &Test_Dap_UartInstance)
    {
        Dap_ReceiveCallback(instancePtr, statusPtr, userArgsPtr);
    }
}


void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1))
    {
        if ( DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST,DL_HSADC_INT_1)) {

            gADCresult0 = DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);
            gADCresult1 = DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER1);
            gADCNewResult = 1;
        }
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
    }
}