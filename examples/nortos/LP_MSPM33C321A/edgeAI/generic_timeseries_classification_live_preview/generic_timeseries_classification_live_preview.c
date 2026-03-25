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

#include <ti/driverlib/m33/dl_interrupt.h>
#include "ti_msp_dl_config.h"
/* Artifacts */
#include "tvmgen_default.h"

#include <ti/edgeAI/m33/feature_extract/feature_extract.h>

/* DAP includes */
#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>

#define SAWTOOTH_WAVE_INDEX     (0)
#define SINE_WAVE_INDEX         (1)
#define SQUARE_WAVE_INDEX       (2)
#define SLIDING_WINDOW_SIZE     4

/* Defines the size of each buffer for ADC data acquisition */
#define ADC_BUFFER_SIZE (FE_FRAME_SIZE)*(FE_NUM_FRAME_CONCAT)
/* Defines the number of buffer for ADC data acquisition */
#define NUM_BUFFER              (2)
#define CPU_CLK_FREQ_KHZ        160000


/* ========================================================================== */
/*                 DAP Variables                                           */
/* ========================================================================== */

/* Sensor JSON strings */
static const char Dap_Sensor0Json[] = "{\"name\":\"ADC\",\"type\":3,\"dataFormat\":5,\"sequenceNumbers\":true,\"labels\":\"value\"}";
/* Property structures */
static Dap_Interface_PropertyInfoType DAP_Property_Samples = {
    .NamePtr = "samples", .Type = DAP_DATA_FORMAT_UINT16, .Value = {.U16 = 256U}};
    
/* Model JSON strings */
static const char DAP_Model0Json[] =
    "{\"name\":\"Model1\",\"task\":\"classification\",\"classIdToName\": { \"0\": \"Sawtooth\", \"1\": \"Sine\", \"2\": \"Square\" },\"projectId\":\"DAP\"}";

/* Inference value structures */
static const Dap_Interface_InfValueInfoType DAP_InfValue0 = {.NamePtr = "inference_result",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};
static const Dap_Interface_InfValueInfoType DAP_InfValue1 = {.NamePtr = "class0_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

static const Dap_Interface_InfValueInfoType DAP_InfValue2 = {.NamePtr = "class1_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

static const Dap_Interface_InfValueInfoType DAP_InfValue3 = {.NamePtr = "class2_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

/* DAP Interface Configuration */
static const Dap_InterfaceConfigType DAP_InterfaceConfig = {
    /* Sensors */
    .SensorList    = {Dap_Sensor0Json, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR},
    .SensorCount   = 1U,
    /* Models */
    .ModelList     = {DAP_Model0Json, NULL_PTR, NULL_PTR, NULL_PTR},
    .ModelCount    = 1U,
    /* Properties */
    .PropertyList  = {&DAP_Property_Samples, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
    .PropertyCount = 1U,
    /* Inference values */
    .InfValueList  = {&DAP_InfValue0, &DAP_InfValue1, &DAP_InfValue2, &DAP_InfValue3, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR},
    .InfValueCount = 4U};


/* ========================================================================== */
/*                 Public Variables                                           */
/* ========================================================================== */

Dap_InstanceType      DAP_Instance;
Uart_Hal_InstanceType DAP_UartInstance;


float scratch_buffer[FE_FRAME_SIZE * 4];
/* Global Variable to store input to the Model */
model_input_t   model_input[1][FE_FEATURE_SIZE_PER_FRAME*FE_NUM_FRAME_CONCAT][1][1];
/* Global variable to store Model output */
model_output_t  model_output[1][3]={0,0,0};

/* Stores extracted features of a single frame */
volatile int8_t featuresPerFrame[FE_FEATURE_SIZE_PER_FRAME];

/* Stores features for all frames within the current sliding window */
volatile int8_t newFeatures[(SLIDING_WINDOW_SIZE * FE_FEATURE_SIZE_PER_FRAME)];

/* Stores the complete feature set for model input */
volatile int8_t totalFeatures[FE_FEATURE_SIZE_PER_FRAME*FE_NUM_FRAME_CONCAT];


/* ADC related Params */
/* Ping pong buffers for storing data from the ADC */
volatile float ADC_buffers[NUM_BUFFER][ADC_BUFFER_SIZE];
float* framePtr;
volatile uint32_t dataIndex = 0;
volatile int prodBufferIndex = 0, conBufferIndex = 0;

/* Flag to indicate whether the buffer is full */
volatile bool ADC_bufferState[NUM_BUFFER];

volatile uint32_t gADCresult;

feature_extraction fe;
feature_extraction_handle fe_handle = &fe;

struct tvmgen_default_inputs tvm_model_input     = {(void *)&model_input};
struct tvmgen_default_outputs tvm_model_output   = {(void *)&model_output};


extern float raw_input_test[];
extern float model_test_input[];
extern model_output_t golden_output[];

volatile uint32_t time1;
volatile uint32_t time2;

#define FAIL 0
#define PASS 1
int test_result = FAIL;

bool streamStarted = false;

int8_t DAP_OutBuff[4];

float Classification_prob[3];

void softmax(const float *input, float *output, int length) {
    float max = input[0];
    float sum = 0.0f;
    
    // Find max value for numerical stability
    for (int i = 1; i < length; i++) {
        if (input[i] > max) {
            max = input[i];
        }
    }
    
    // Compute exp(x - max) and sum
    for (int i = 0; i < length; i++) {
        output[i] = expf(input[i] - max);
        sum += output[i];
    }
    
    // Normalize
    for (int i = 0; i < length; i++) {
        output[i] /= sum;
    }
}

void postProcessing(void)
{
    /* Index with the highest value corresponds to the model's predicted class */
    uint8_t maxIndex = SAWTOOTH_WAVE_INDEX;


    if(model_output[0][SINE_WAVE_INDEX] > model_output[0][maxIndex])
    {
        maxIndex = SINE_WAVE_INDEX;
    }

    if(model_output[0][SQUARE_WAVE_INDEX] > model_output[0][maxIndex])
    {
        maxIndex = SQUARE_WAVE_INDEX;
    }

    DL_GPIO_clearPins(LED_RED_PORT, LED_RED_PIN);
    DL_GPIO_clearPins(LED_BLUE_PORT, LED_BLUE_PIN);
    DL_GPIO_clearPins(LED_GREEN_PORT, LED_GREEN_PIN);

    /* Based on the model output, Turn on the corresponding LED */
    switch(maxIndex)
    {
        case SAWTOOTH_WAVE_INDEX:
                    DL_GPIO_setPins(LED_RED_PORT,LED_RED_PIN);
                    break;

        case SINE_WAVE_INDEX:
                    DL_GPIO_setPins(LED_BLUE_PORT,LED_BLUE_PIN);
                    break;

        case SQUARE_WAVE_INDEX:
                    DL_GPIO_setPins(LED_GREEN_PORT,LED_GREEN_PIN);
                    break;

        default:
            break;
    }
    float Classification_tmp[3] = {(float)model_output[0][0],(float)model_output[0][1],(float)model_output[0][2]};
    softmax(Classification_tmp, Classification_prob, 3);

    DAP_OutBuff[0] = maxIndex;
    DAP_OutBuff[1] = (int8_t) (Classification_prob[0] * 100);
    DAP_OutBuff[2] = (int8_t) (Classification_prob[1] * 100);
    DAP_OutBuff[3] = (int8_t) (Classification_prob[2] * 100); 
}

void FE_init(feature_extraction_handle fe_handle)
{

    fe_handle->scratch_buffer = &scratch_buffer[0];
    FE_allocFeatureExtract(fe_handle);
    fe_handle->input_buffer  = framePtr;
    fe_handle->output_buffer = &model_input[0];
    fe_handle->test_feature_extraction = false;
    FE_initFeatureExtract(fe_handle);

}


void processing()
{
    fe_handle->input_buffer = &(ADC_buffers[conBufferIndex][0]);
    FE_runFeatureExtract(fe_handle);
    /* Setting the current ADC Buffer state to empty */
    ADC_bufferState[conBufferIndex] = 0;

    conBufferIndex = (conBufferIndex + 1) % NUM_BUFFER;
    
    /* Run Inference */
    tvmgen_default_run(&tvm_model_input, &tvm_model_output);
    postProcessing();
}


int main(void)
{

    int status = DAP_ERROR_NONE;
    SYSCFG_DL_init();

    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);
    
    FE_init(fe_handle);
    /* Initialize UART HAL */    
    Uart_Hal_InitParamsType initParams;

    Uart_Hal_InitParamsSetDefault(&initParams);
    initParams.BaudRateBps = UART_0_BAUD_RATE;
    initParams.HwRegsPtr   = (UNICOMM_Inst_Regs *)UART_0_INST;
    status = Uart_Hal_Init(&DAP_UartInstance, &initParams);    
    
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

    Dap_InitParamsType dapParams;
    Dap_InitParamsSetDefault(&dapParams);

    dapParams.LinkParams.UartInstancePtr = &DAP_UartInstance;
    dapParams.InterfaceConfigPtr         = &DAP_InterfaceConfig;

    status = Dap_Init(&DAP_Instance, &dapParams);
    if (status == DAP_ERROR_NONE)
    {
        printf("DAP initialized successfully\n");
    }

    status = Dap_Open(&DAP_Instance);
    if (status == DAP_ERROR_NONE)
    {
        printf("DAP opened successfully\n");
    }    


    while (1) {
        /* Check if the current ADC buffer is full and ready for processing */

        status = Dap_Process(&DAP_Instance);        

        boolean isStreaming = FALSE;
        Dap_IsStreaming(&DAP_Instance, &isStreaming);
        if (isStreaming == TRUE)
        {
            if (streamStarted == FALSE)
            {
                /* sampleSizes and totalSamples args would be calculated here in end application */
                if (status == DAP_ERROR_NONE)
                {
                    streamStarted = TRUE;
                }
                DL_Timer_startCounter(TIMER_0_INST);
                
            }

            if (streamStarted == TRUE)
            {
                
                if(ADC_bufferState[conBufferIndex])
                {
                    processing();
                    status = Dap_Core_SendStreamHeader(&(DAP_Instance.LinkInstance), DAP_CHANNEL_INF_VALUE, sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamSample(&(DAP_Instance.LinkInstance),(uint8 *)DAP_OutBuff,sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamEnd(&(DAP_Instance.LinkInstance));
                }
            }
        }
        else
        {
            streamStarted = FALSE;
        }
    }
}


void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1))
    {
        if ( DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST,DL_HSADC_INT_1)) {
            gADCresult = DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);

            /* Check if whole ADC Buffer data is captured */
            if(dataIndex < ADC_BUFFER_SIZE)
            {
                /* Storing the ADC data */
                ADC_buffers[prodBufferIndex][dataIndex++] = (gADCresult) ;
            }

            if(dataIndex >= ADC_BUFFER_SIZE)
            {
                /* Set flag that the buffer is full */
                ADC_bufferState[prodBufferIndex] = true;

                prodBufferIndex = (prodBufferIndex + 1) % NUM_BUFFER;

                /* Reset the flag */
                ADC_bufferState[prodBufferIndex] = false;
                dataIndex = 0;
            }            
        }
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
    }
    
}


/* ========================================================================== */
/*                 Interrupt Handlers and Callbacks                           */
/* ========================================================================== */

void UART_0_INST_IRQHandler(void)
{       
    Uart_Hal_InterruptHandler(&DAP_UartInstance);
}

void Uart_Hal_TxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_TxStatusType *statusPtr,
                                 void *userArgsPtr)
{
    if (instancePtr == &DAP_UartInstance)
    {
        Dap_TransmitCallback(instancePtr, statusPtr, userArgsPtr);
    }
}

void Uart_Hal_RxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_RxStatusType *statusPtr,
                                 void *userArgsPtr)
{
    if (instancePtr == &DAP_UartInstance)
    {
        Dap_ReceiveCallback(instancePtr, statusPtr, userArgsPtr);
    }
}