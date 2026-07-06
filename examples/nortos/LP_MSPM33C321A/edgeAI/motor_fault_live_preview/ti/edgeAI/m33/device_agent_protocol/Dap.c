/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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

#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/edgeAI/m33/device_agent_protocol/dap_link/Dap_Link.h>
#include <ti/edgeAI/m33/device_agent_protocol/dap_core/Dap_Core.h>
#include <ti/edgeAI/m33/device_agent_protocol/dap_interface/Dap_Interface_Cfg.h>
#include <string.h>

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

uint32 Dap_InitParamsSetDefault(Dap_InitParamsType *paramsPtr)
{
    uint32 retVal;

    retVal = DAP_ERROR_NONE;

    if (paramsPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }

    if (retVal == DAP_ERROR_NONE)
    {
        paramsPtr->InterfaceConfigPtr = NULL_PTR;
        retVal                        = Dap_Link_InitParamsSetDefault(&paramsPtr->LinkParams);
    }

    return retVal;
}

uint32 Dap_Init(Dap_InstanceType *instancePtr, const Dap_InitParamsType *paramsPtr)
{
    uint32 retVal;

    retVal = DAP_ERROR_NONE;

    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (paramsPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }
    else if (paramsPtr->InterfaceConfigPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }

    if (retVal == DAP_ERROR_NONE)
    {
        instancePtr->IsInitialized = FALSE;

        retVal = Dap_Link_Init(&instancePtr->LinkInstance, &paramsPtr->LinkParams);

        if (retVal == DAP_ERROR_NONE)
        {
            retVal = Dap_PipelineConfigSetDefault(&instancePtr->PipelineConfig);
        }

        if (retVal == DAP_ERROR_NONE)
        {
            uint32 i;

            instancePtr->InterfaceConfig           = *(paramsPtr->InterfaceConfigPtr);
            instancePtr->StreamingContext.IsActive = FALSE;

            for (i = 0U; i < DAP_INTERFACE_MAX_SENSORS; i++)
            {
                instancePtr->StreamingContext.SequenceNumber[i]  = 0U;
                instancePtr->StreamingContext.SampleSizeBytes[i] = 0U;
            }
            instancePtr->StreamingContext.TotalSampleCount   = 0U;
            instancePtr->StreamingContext.CurrentSampleCount = 0U;
            instancePtr->StreamingContext.HeaderSent         = FALSE;

            instancePtr->IsInitialized = TRUE;
        }
    }

    return retVal;
}

uint32 Dap_Open(Dap_InstanceType *instancePtr)
{
    uint32 retVal;

    retVal = DAP_ERROR_NONE;

    /* Parameter validation */
    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Instance valid and initialized */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        retVal = Dap_Link_StartReceive(&instancePtr->LinkInstance);
    }

    return retVal;
}

uint32 Dap_PipelineConfigSetDefault(Dap_PipelineConfigType *configPtr)
{
    uint32 retVal;
    uint32 i;

    retVal = DAP_ERROR_NONE;

    if (configPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }

    if (retVal == DAP_ERROR_NONE)
    {
        configPtr->Mode        = DAP_PIPELINE_MODE_DATA_ACQUISITION;
        configPtr->SensorCount = 0U;
        configPtr->ModelIndex  = 0U;

        for (i = 0U; i < DAP_INTERFACE_MAX_SENSORS; i++)
        {
            configPtr->SensorIndex[i] = 0U;
        }
    }

    return retVal;
}

uint32 Dap_Process(Dap_InstanceType *instancePtr)
{
    uint32  retVal;
    boolean frameReady = FALSE;
    boolean isRxIdle   = FALSE;

    retVal = DAP_ERROR_NONE;

    /* Parameter validation */
    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Instance valid and initialized */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        retVal = Dap_IsFrameReady(instancePtr, &frameReady);

        if (frameReady == TRUE)
        {
            retVal = Dap_Core_ProcessFrame(instancePtr);

            /* Restart receive */
            retVal = Dap_Link_StartReceive(&instancePtr->LinkInstance);
        }
        else
        {
            (void)Dap_Link_IsRxIdle(&instancePtr->LinkInstance, &isRxIdle);

            if (isRxIdle == TRUE)
            {
                retVal = Dap_Link_StartReceive(&instancePtr->LinkInstance);
            }
        }
    }

    return retVal;
}

uint32 Dap_IsStreaming(const Dap_InstanceType *instancePtr, boolean *isStreamingPtr)
{
    uint32 retVal = DAP_ERROR_NONE;

    /* Parameter validation */
    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (isStreamingPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Parameters valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        *isStreamingPtr = instancePtr->StreamingContext.IsActive;
    }

    return retVal;
}

uint32 Dap_IsFrameReady(const Dap_InstanceType *instancePtr, boolean *isFrameReadyPtr)
{
    uint32 retVal = DAP_ERROR_NONE;

    /* Parameter validation */
    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (isFrameReadyPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Parameters valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        (void)Dap_Link_IsFrameReady(&instancePtr->LinkInstance, isFrameReadyPtr);
    }

    return retVal;
}

uint32 Dap_GetPipelineConfig(const Dap_InstanceType *instancePtr, Dap_PipelineConfigType *configPtr)
{
    uint32 retVal = DAP_ERROR_NONE;

    /* Parameter validation */
    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (configPtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Parameters valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        *configPtr = instancePtr->PipelineConfig;
    }

    return retVal;
}

uint32 Dap_StartSensorStream(Dap_InstanceType *instancePtr, const uint16 *sampleSizes, uint32 totalSamples)
{
    uint32                    retVal = DAP_ERROR_NONE;
    uint32                    i;
    uint32                    totalPayloadLen;
    Dap_StreamingContextType *ctxPtr;

    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else if (instancePtr->StreamingContext.IsActive == FALSE)
    {
        retVal = DAP_ERROR_NOT_STREAMING;
    }
    else if (sampleSizes == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }
    else
    {
        /* All parameters valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        ctxPtr = &instancePtr->StreamingContext;

        for (i = 0U; i < DAP_INTERFACE_MAX_SENSORS; i++)
        {
            ctxPtr->SequenceNumber[i]  = 0U;
            ctxPtr->SampleSizeBytes[i] = sampleSizes[i];
        }

        ctxPtr->TotalSampleCount   = totalSamples;
        ctxPtr->CurrentSampleCount = 0U;
        totalPayloadLen            = 0U;

        if (totalSamples > 0U)
        {
            for (i = 0U; i < instancePtr->PipelineConfig.SensorCount; i++)
            {
                /* Per sample: sensor_id (1) + seq (1) + data (sampleSize) */
                totalPayloadLen += (2U + (uint32)sampleSizes[i]) * totalSamples;
            }
        }
        else
        {
            /* Unlimited streaming */
            totalPayloadLen = 0U;
        }

        retVal = Dap_Core_SendStreamHeader(&instancePtr->LinkInstance, DAP_CHANNEL_SENSOR_SIGNAL, totalPayloadLen);

        if (retVal == DAP_ERROR_NONE)
        {
            ctxPtr->HeaderSent = TRUE;
        }
    }

    return retVal;
}

uint32 Dap_StreamSensorSample(Dap_InstanceType *instancePtr, uint8 sensorIndex, const uint8 *dataPtr,
                              uint32 sizeInBytes)
{
    uint32                    retVal = DAP_ERROR_NONE;
    uint8                     samplePacket[DAP_LINK_BUFFER_SIZE_BYTES];
    uint32                    packetIdx;
    uint8                     sensorId;

    Dap_StreamingContextType *ctxPtr;

    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else if (instancePtr->StreamingContext.IsActive == FALSE)
    {
        retVal = DAP_ERROR_NOT_STREAMING;
    }
    else if ((dataPtr == NULL_PTR) || (sizeInBytes == 0U))
    {
        retVal = DAP_ERROR_INVALID_PARAMS;
    }
    else if (sensorIndex >= instancePtr->PipelineConfig.SensorCount)
    {
        retVal = DAP_ERROR_INVALID_SENSOR_INDEX;
    }
    else
    {
        /* All parameters valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        ctxPtr = &instancePtr->StreamingContext;

        if (ctxPtr->HeaderSent == FALSE)
        {
            retVal = DAP_ERROR_STREAM_NOT_STARTED;
        }
    }

    if (retVal == DAP_ERROR_NONE)
    {
        sensorId = instancePtr->PipelineConfig.SensorIndex[sensorIndex];
        (void)sensorId;

        /* Sample packet: [sensor_id] + [seq] + [data] */
        packetIdx                 = 0U;
        samplePacket[packetIdx++] = (((sensorIndex + 1U) << 5 )+ (ctxPtr->SequenceNumber[sensorIndex] >> 11));
        samplePacket[packetIdx++] = ctxPtr->SequenceNumber[sensorIndex] & 0xFF;

        if ((packetIdx + sizeInBytes) < DAP_LINK_BUFFER_SIZE_BYTES)
        {
            (void)memcpy(&samplePacket[packetIdx], dataPtr, sizeInBytes);
            packetIdx += sizeInBytes;
        }
        else
        {
            retVal = DAP_ERROR_BUFFER_OVERFLOW;
        }
    }

    if (retVal == DAP_ERROR_NONE)
    {
        retVal = Dap_Core_SendStreamSample(&instancePtr->LinkInstance, samplePacket, packetIdx);
    }

    if (retVal == DAP_ERROR_NONE)
    {
        ctxPtr->SequenceNumber[sensorIndex]++;
        ctxPtr->CurrentSampleCount++;

        if ((ctxPtr->TotalSampleCount > 0U) && (ctxPtr->CurrentSampleCount >= ctxPtr->TotalSampleCount))
        {
            (void)Dap_StopSensorStream(instancePtr);
        }
    }

    return retVal;
}

uint32 Dap_StopSensorStream(Dap_InstanceType *instancePtr)
{
    uint32                    retVal = DAP_ERROR_NONE;
    Dap_StreamingContextType *ctxPtr;

    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Instance valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        ctxPtr = &instancePtr->StreamingContext;

        if (ctxPtr->HeaderSent == TRUE)
        {
            retVal = Dap_Core_SendStreamEnd(&instancePtr->LinkInstance);

            ctxPtr->HeaderSent         = FALSE;
            ctxPtr->CurrentSampleCount = 0U;
            ctxPtr->TotalSampleCount   = 0U;
        }

        instancePtr->StreamingContext.IsActive = FALSE;
    }

    return retVal;
}

uint32 Dap_DeInit(Dap_InstanceType *instancePtr)
{
    uint32 retVal = DAP_ERROR_NONE;

    /* Parameter validation */
    if (instancePtr == NULL_PTR)
    {
        retVal = DAP_ERROR_INVALID_INSTANCE;
    }
    else if (instancePtr->IsInitialized == FALSE)
    {
        retVal = DAP_ERROR_NOT_INITIALIZED;
    }
    else
    {
        /* Instance valid */
    }

    if (retVal == DAP_ERROR_NONE)
    {
        instancePtr->StreamingContext.IsActive = FALSE;

        (void)Dap_Link_DeInit(&instancePtr->LinkInstance);

        instancePtr->IsInitialized = FALSE;
    }

    return retVal;
}

void Dap_ReceiveCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_RxStatusType *statusPtr, void *userArgsPtr)
{
    Dap_Link_ContinueReceive((Dap_Link_InstanceType *)userArgsPtr, instancePtr, statusPtr);
}

void Dap_TransmitCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_TxStatusType *statusPtr, void *userArgsPtr)
{
    Dap_Link_ContinueTransmit((Dap_Link_InstanceType *)userArgsPtr, instancePtr, statusPtr);
}
