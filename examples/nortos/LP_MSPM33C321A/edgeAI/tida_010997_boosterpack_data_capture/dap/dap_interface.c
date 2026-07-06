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

#include "dap_interface.h"
#include "ti_msp_dl_config.h"
#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>
#include "app_cfg.h"

/* Sensor JSON strings — index matches SensorIndex enum: PIR=0, TEMP=1, HUMID=2,
 * PRESS=3, ALS=4, ACCEL=5, GYRO=6, MAG=7, DIGMIC=8. */
static const char Dap_Sensor0Json[] =
    "{\"name\":\"PIR\",\"type\":3,\"dataFormat\":4,\"sequenceNumbers\":true,\"labels\":\"motion_level\"}";
static const char Dap_Sensor1Json[] =
    "{\"name\":\"Temperature\",\"type\":3,\"dataFormat\":2,\"sequenceNumbers\":true,\"labels\":\"celsius_x100\"}";
static const char Dap_Sensor2Json[] =
    "{\"name\":\"Humidity\",\"type\":3,\"dataFormat\":2,\"sequenceNumbers\":true,\"labels\":\"percent_x100\"}";
static const char Dap_Sensor3Json[] =
    "{\"name\":\"Pressure\",\"type\":3,\"dataFormat\":6,\"sequenceNumbers\":true,\"labels\":\"pascal\"}";
static const char Dap_Sensor4Json[] =
    "{\"name\":\"Ambient Light\",\"type\":3,\"dataFormat\":2,\"sequenceNumbers\":true,\"labels\":\"lux_x100\"}";
static const char Dap_Sensor5Json[] =
    "{\"name\":\"3-axis Accelerometer\",\"type\":4,\"dataFormat\":2,\"columns\":3,"
    "\"sequenceNumbers\":true,\"labels\":[\"x\",\"y\",\"z\"]}";
static const char Dap_Sensor6Json[] =
    "{\"name\":\"3-axis Gyroscope\",\"type\":4,\"dataFormat\":2,\"columns\":3,"
    "\"sequenceNumbers\":true,\"labels\":[\"x\",\"y\",\"z\"]}";
static const char Dap_Sensor7Json[] =
    "{\"name\":\"Magnetic Sensor\",\"type\":4,\"dataFormat\":2,\"columns\":3,"
    "\"sequenceNumbers\":true,\"labels\":[\"x\",\"y\",\"z\"]}";
static const char Dap_Sensor8Json[] =
    "{\"name\":\"Digital Microphone\",\"type\":2,\"dataFormat\":3,\"sequenceNumbers\":true,\"labels\":\"audio\"}";


static Dap_Interface_PropertyInfoType DAP_Property_Samples = {
    .NamePtr = "samples", .Type = DAP_DATA_FORMAT_UINT32, .Value = {.U32 = 256U}};

static const Dap_InterfaceConfigType Dap_InterfaceConfig = {
    .SensorList  = {Dap_Sensor0Json, Dap_Sensor1Json, Dap_Sensor2Json, Dap_Sensor3Json,
                    Dap_Sensor4Json, Dap_Sensor5Json, Dap_Sensor6Json, Dap_Sensor7Json, 
                    Dap_Sensor8Json},
    .SensorCount = 9U, 
    .ModelList   = {NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
    .ModelCount  = 0U,
    .PropertyList  = {&DAP_Property_Samples, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
    .PropertyCount = 1U,
    .InfValueList  = {NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
    .InfValueCount = 0U};

static Dap_InstanceType      gDap_Instance;
static Uart_Hal_InstanceType gDap_UartInstance;

/* Bytes per sample indexed by global sensor index 0-8 */
static const uint16_t gSensorSizeBytes[9] = {
    1U, 2U, 2U, 4U, 2U, 6U, 6U, 6U, 4U};

int Dap_Interface_Init(void)
{
    Uart_Hal_InitParamsType p;
    Dap_InitParamsType dp;
    int status;

    status = -1;
    Uart_Hal_InitParamsSetDefault(&p);
    p.BaudRateBps = UART_0_BAUD_RATE;
    p.HwRegsPtr   = (UNICOMM_Inst_Regs *)UART_0_INST;

    if (Uart_Hal_Init(&gDap_UartInstance, &p) == UART_HAL_ERROR_NONE) {
        NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
        NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

        Dap_InitParamsSetDefault(&dp);
        dp.LinkParams.UartInstancePtr = &gDap_UartInstance;
        dp.InterfaceConfigPtr         = &Dap_InterfaceConfig;
        Dap_Init(&gDap_Instance, &dp);
        Dap_Open(&gDap_Instance);

        status = 0;
    }

    return status;
}

void Dap_Interface_ProcessCommands(void)
{
    Dap_Process(&gDap_Instance);
}

bool Dap_Interface_IsStreaming(void)
{
    boolean isStreaming = FALSE;
    Dap_IsStreaming(&gDap_Instance, &isStreaming);
    return (isStreaming == TRUE);
}

uint8_t Dap_Interface_GetSelectedSensor(void)
{
    Dap_PipelineConfigType cfg;
    Dap_GetPipelineConfig(&gDap_Instance, &cfg);
    return cfg.SensorIndex[0];
}

int Dap_Interface_StartStream(void)
{
    Dap_PipelineConfigType cfg;
    uint16_t sampleSizes[DAP_INTERFACE_MAX_SENSORS];
    uint8_t sel;
    sint32 status;
    int ret;
    uint32_t i;

    ret = -1;
    for (i = 0U; i < DAP_INTERFACE_MAX_SENSORS; i++) {
        sampleSizes[i] = 0U;
    }

    Dap_GetPipelineConfig(&gDap_Instance, &cfg);
    sel = cfg.SensorIndex[0];
    sampleSizes[0] = (sel < 9U) ? gSensorSizeBytes[sel] : 2U;

    status = Dap_StartSensorStream(&gDap_Instance, sampleSizes,
                                   DAP_Property_Samples.Value.U32);
    ret = (status == DAP_ERROR_NONE) ? 0 : -1;

    return ret;
}

void Dap_Interface_StreamRaw(const uint8_t *data, uint32_t len)
{
    Dap_StreamSensorSample(&gDap_Instance, 0U, (const uint8 *)data, len);
}

bool Dap_Interface_IsStreamActive(void)
{
    return (gDap_Instance.StreamingContext.IsActive == TRUE);
}

uint32_t Dap_Interface_GetSampleCount(void)
{
    return DAP_Property_Samples.Value.U32;
}

void *Dap_Interface_GetInstance(void)
{
    return &gDap_Instance;
}

void *Dap_Interface_GetUartInstance(void)
{
    return &gDap_UartInstance;
}
