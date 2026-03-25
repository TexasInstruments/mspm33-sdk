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

#include <ti/driverlib/m33/dl_interrupt.h>
#include "ti_msp_dl_config.h"
#include "arm_math.h"
/* Artifacts */
#include "tvmgen_default.h"
#include "user_input_config.h"
#include "ti_edgeai_fe_config.h"
#include "feature_extract.h"
#include "feature_extract_app.h"

/* SysConfig generated includes */
#include "ti_msp_dl_config.h"

/* DAP includes */
#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>

#define HUMAN_MOTION_INDEX          (0)
#define BACKGROUND_MOTION_INDEX     (1)
#define DOG_MOTION_INDEX            (2)



/* ========================================================================== */
/*                 DAP Variables                                           */
/* ========================================================================== */

/* Sensor JSON strings */
static const char Dap_Sensor0Json[] = "{\"name\":\"PIR\",\"type\":3,\"dataFormat\":5,\"sequenceNumbers\":true,\"labels\":\"value\"}";
/* Property structures */
static Dap_Interface_PropertyInfoType DAP_Property_Samples = {
    .NamePtr = "samples", .Type = DAP_DATA_FORMAT_UINT16, .Value = {.U16 = 256U}};
    
/* Model JSON strings */
static const char DAP_Model0Json[] =
    "{\"name\":\"Model1\",\"task\":\"classification\",\"classIdToName\": { \"0\": \"Person\", \"1\": \"Background\", \"2\": \"Dog\" },\"projectId\":\"DAP\"}";

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
bool streamStarted = false;
int8_t DAP_OutBuff[NUM_CLASSES+1];

/* Defines the size of each buffer for ADC data acquisition */
#define ADC_BUFFER_SIZE         128

/* Defines the number of buffer for ADC data acquisition */
#define NUM_BUFFER              (2)

/* Global Variable to store input to the Model */
int8_t if_map_int8[1][1][25][20];

/* Global variable to store Model output */
static float pir_output_class0[1][NUM_CLASSES];
static float pir_output_class1[1][128];
float scratch_buffer[ADCSAMPLESIZE * 4];

/* Application buffers */
uint8_t sensor_buffer[125];
static float feature_buffer[NUM_WINDOWS * NUM_TOTAL_FEATURES];

/* Feature Extraction parameters */
static FeatureExtract_Params featureExtractParams;
/* ADC related Params */
volatile uint8_t ADC_buffers[ADC_BUFFER_SIZE];
volatile uint32_t dataIndex = 0;

/* Flag to indicate whether the buffer is full */
volatile bool ADC_bufferState;

/* Stores the ADC result */
volatile uint32_t gADCresult;

uint32_t samplingfreq;
uint8_t *framePtr;
/* Stores the data for each window */
float window[32];

//#define TEST_FEATURE_EXTRACT

#ifdef TEST_FEATURE_EXTRACT
    extern int8_t raw_input_test[125];
    extern float model_test_input[625];
    extern int golden_output[3];
#endif


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
    uint8_t maxIndex = HUMAN_MOTION_INDEX;

    if(pir_output_class0[0][BACKGROUND_MOTION_INDEX] > pir_output_class0[0][maxIndex])
    {
        maxIndex = BACKGROUND_MOTION_INDEX;
    }

    if(pir_output_class0[0][DOG_MOTION_INDEX] > pir_output_class0[0][maxIndex])
    {
        maxIndex = DOG_MOTION_INDEX;
    }

    DL_GPIO_clearPins(LED_RED_PORT, LED_RED_LED_RED_PIN_PIN);
    DL_GPIO_clearPins(LED_BLUE_PORT, LED_BLUE_LED_BLUE_PIN_PIN);
    DL_GPIO_clearPins(LED_GREEN_PORT, LED_GREEN_LED_GREEN_PIN_PIN);

    /* Based on the model output, Turn on the corresponding LED */
    switch(maxIndex)
    {
        case HUMAN_MOTION_INDEX:
                    DL_GPIO_setPins(LED_RED_PORT, LED_RED_LED_RED_PIN_PIN);
                    break;

        case BACKGROUND_MOTION_INDEX:
                    DL_GPIO_setPins(LED_BLUE_PORT, LED_BLUE_LED_BLUE_PIN_PIN);
                    break;

        case DOG_MOTION_INDEX:
                    DL_GPIO_setPins(LED_GREEN_PORT, LED_GREEN_LED_GREEN_PIN_PIN);
                    break;

        default:
            break;
    }
    softmax(pir_output_class0[0], Classification_prob, 3);

    DAP_OutBuff[0] = maxIndex;
    DAP_OutBuff[1] = (int8_t) (Classification_prob[0] * 100);
    DAP_OutBuff[2] = (int8_t) (Classification_prob[1] * 100);
    DAP_OutBuff[3] = (int8_t) (Classification_prob[2] * 100); 
}


int main(void)
{
    int status = DAP_ERROR_NONE;
    SYSCFG_DL_init();
    FeatureExtract_config(&featureExtractParams);
    samplingfreq = (float)33;
    struct tvmgen_default_inputs tvm_if_map = {(void*) &feature_buffer[0]};
    struct tvmgen_default_outputs tvm_of_map = {(void*) &pir_output_class0[0]};
    int i = 0;
    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);
    #ifdef TEST_FEATURE_EXTRACT
        FeatureExtract_config(&featureExtractParams);
        FeatureExtract_run(&featureExtractParams, (uint8_t *)raw_input_test, feature_buffer);
        tvmgen_default_run(&tvm_if_map, &tvm_of_map);
        __BKPT(0);
    #endif
    DL_HSADC_triggerSequencerSoftwareForce(
            HSADC0_INST, DL_HSADC_SEQ_NUMBER1);


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

    tvmgen_default_run(&tvm_if_map,&tvm_of_map);
    while (1) {

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
    
                /* Check if the current ADC buffer is full and ready for processing */
                if(ADC_bufferState)
                {
                    
                    FeatureExtract_config(&featureExtractParams);
                    FeatureExtract_run(&featureExtractParams, (uint8_t *)ADC_buffers, feature_buffer);

                    /* Setting the current ADC Buffer state to empty */
                    ADC_bufferState = false;

                    /* Run Inference */
                    tvmgen_default_run(&tvm_if_map, &tvm_of_map);

                    postProcessing();
                    status = Dap_Core_SendStreamHeader(&(DAP_Instance.LinkInstance), DAP_CHANNEL_INF_VALUE, sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamSample(&(DAP_Instance.LinkInstance),(uint8 *)DAP_OutBuff,sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamEnd(&(DAP_Instance.LinkInstance));
                }
            }
        }
    }
}

void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1))
    {
        if ( DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST,DL_HSADC_INT_1)) {
            gADCresult = DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);
            if(ADC_bufferState != true)
            {
                /* Check if whole ADC Buffer data is captured */
                if(dataIndex < ADC_BUFFER_SIZE)
                {
                    /* Storing the ADC data */
                    ADC_buffers[dataIndex++] = (uint8_t)(gADCresult>>4);
                }

                if(dataIndex >= ADC_BUFFER_SIZE)
                {
                    /* Set flag that the buffer is full */
                    ADC_bufferState = true;

                    dataIndex = 0;
                }     
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


