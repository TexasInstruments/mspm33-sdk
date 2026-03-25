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

#include "tvmgen_default.h"
#include "ADXL355.h"
#include <ti/edgeAI/m33/feature_extract/feature_extract.h>

/* ARM CMSIS-DSP Header files */
#include "arm_const_structs.h"
#include "arm_math.h"
#include "stdlib.h"
#include "string.h"


#include "ADXL355.h"

/* ARM CMSIS-DSP Header files */
#include "stdlib.h"
#include "string.h"

/* DAP includes */
#include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>



/** \brief Sensor configuration structure */
sensorStruct sensor;

/** \brief DAP specific variables */

/* Sensor JSON strings */
static const char DAP_SensorJson[] = "{\"name\":\"Accelerometer\",\"type\":5,\"rows\":3, \"dataFormat\":5, \"sequenceNumbers\":true, \"labels\":\"value\"}";

/* Property structures */
static Dap_Interface_PropertyInfoType DAP_Property_Samples = {
    .NamePtr = "samples", .Type = DAP_DATA_FORMAT_UINT16, .Value = {.U16 = 0U}};

/* Model JSON strings */
static const char DAP_Model0Json[] =
    "{\"name\":\"Motor_Fault\",\"task\":\"classification\",\"classIdToName\": { \"0\": \"Blade Damage\", \"1\": \"Blade Imbalance\", \"2\": \"Blade Obstruction\", \"3\": \"Normal\" },\"projectId\":\"DAP\"}";

/* Inference value structures */
static const Dap_Interface_InfValueInfoType DAP_InfValue0 = {.NamePtr = "inference_result",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};
static const Dap_Interface_InfValueInfoType DAP_InfValue1 = {.NamePtr = "class0_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

static const Dap_Interface_InfValueInfoType DAP_InfValue2 = {.NamePtr = "class1_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

static const Dap_Interface_InfValueInfoType DAP_InfValue3 = {.NamePtr = "class2_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};
static const Dap_Interface_InfValueInfoType DAP_InfValue4 = {.NamePtr = "class3_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

/* DAP Interface Configuration */
static const Dap_InterfaceConfigType DAP_InterfaceConfig = {
    /* Sensors */
    .SensorList    = {DAP_SensorJson, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
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
    .InfValueList  = {&DAP_InfValue0, &DAP_InfValue1, &DAP_InfValue2, &DAP_InfValue3, &DAP_InfValue4, NULL_PTR, NULL_PTR,
                      NULL_PTR},
    .InfValueCount = 5U};

/* ========================================================================== */
/*                 Public Variables                                           */
/* ========================================================================== */

Dap_InstanceType      DAP_Instance;
Uart_Hal_InstanceType DAP_UartInstance;


int8_t DAP_OutBuff[5];

/** \brief Size of the class buffer */
#define SAMPLE_BUFFER_SIZE (10)

/** \brief Number of input channels */
#define INPUT_CHANNELS     (3)

/** \brief Model input buffer */
float32_t if_map[1][3][128][1];

/** \brief Model output buffer */
float of_map[1][4];

/** \brief Data points of a single frame */
float input_data[FE_FRAME_SIZE*INPUT_CHANNELS];

/** \brief Extracted features of a single frame */
int8_t featuresPerFrame[FE_FEATURE_SIZE_PER_FRAME * INPUT_CHANNELS];

/** \brief Complete feature set for model input */
int8_t totalFeatures[FE_FEATURE_SIZE_PER_FRAME*FE_NUM_FRAME_CONCAT*INPUT_CHANNELS];

/** \brief Circular class buffer to determine which class LED has to glow */
uint8_t class_buffer[SAMPLE_BUFFER_SIZE];
/** \brief Index for the class buffer */
uint8_t class_buffer_index = 0;

/** \brief Sensor configuration structure */
sensorStruct sensor;

/* Feature Extraction Handle defintions*/

feature_extraction fe;
feature_extraction_handle fe_handle = &fe;
float scratch_buffer[FE_FRAME_SIZE * 4];
float history_buffer[FE_FEATURE_SIZE_PER_FRAME*FE_NUM_FRAME_CONCAT*INPUT_CHANNELS];

#define TEST_FEATURE_EXTRACTION 0

#ifdef TEST_FEATURE_EXTRACTION
    extern float raw_input_test[];
    extern float32_t model_test_input[];
    extern model_output_t golden_output[];
#endif

/** \brief Initialises the sensor */
void sensor_init()
{
    sensor.spiReg      = SPI_0_INST;
    sensor.csPort      = SPI_PORT;
    sensor.csPin       = SPI_CS_0_PIN;
    sensor.readyPort   = SENSOR_PORT;
    sensor.readyPin    = SENSOR_RDY_1_PIN;
    sensor.sensorMode  = SENSOR_MODE_IDLE;

    DL_GPIO_disableInterrupt(SENSOR_PORT, SENSOR_RDY_1_PIN);
}


/** \brief Feature extraction from the vibration data */
void motor_FE()
{
    int i,j;
    int index =0;
    fe_handle->input_buffer = &input_data[0];
    fe_handle->output_buffer = &totalFeatures[0];
    for(i=0;i< FE_NUM_FRAME_CONCAT ;i++)
    {
        for(j = 0 ; j < FE_FRAME_SIZE ; j++)
        {
            input_data[j]                     = (float)dataBuff.xData[i*FE_FRAME_SIZE + j];
            input_data[(1*FE_FRAME_SIZE + j)] = (float)dataBuff.yData[i*FE_FRAME_SIZE + j];
            input_data[(2*FE_FRAME_SIZE + j)] = (float)dataBuff.zData[i*FE_FRAME_SIZE + j];
        }
        FE_runFeatureExtract(fe_handle);
    }

    for(i=0;i<INPUT_CHANNELS;i++)
    {
        for(j=0;j<FE_NUM_FRAME_CONCAT*FE_FEATURE_SIZE_PER_FRAME;j++)
        {
            if_map[0][i][j][0] = totalFeatures[index];
            index++;
        }
    }
}

void softmax(const float *input, float *output, int length)
{
    float max = input[0];
    float sum = 0.0f;
    int i;

    // Find max value for numerical stability
    for (i = 1; i < length; i++) {
        if (input[i] > max) {
            max = input[i];
        }
    }

    // Compute exp(x - max) and sum
    for (i = 0; i < length; i++) {
        output[i] = expf(input[i] - max);
        sum += output[i];
    }

    // Normalize
    for (i = 0; i < length; i++) {
        output[i] /= sum;
    }
}

/** \brief Glow LED based on which class has the maximum frequency */
void post_processing()
{
    uint8_t freq[4] = {0, 0, 0, 0};
    int i;
    uint8_t maximum_freq_class = 0;
    float class_prob[4];

    /* Calculate the frequency of the classes */
    for(i=0;i<SAMPLE_BUFFER_SIZE;i++)
    {
        if(class_buffer[i])
        {
            freq[class_buffer[i]-1]++;
        }
    }

    /* Find class with maximum frequency */
    for(i=1;i<4;i++)
    {
        if(freq[maximum_freq_class]<freq[i])
        {
            maximum_freq_class = i;
        }
    }

    /* Run only if the buffer is completely filled */
    if(freq[0]+freq[1]+freq[2]+freq[3] == SAMPLE_BUFFER_SIZE)
    {
        DL_GPIO_clearPins(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
        DL_GPIO_clearPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
        DL_GPIO_clearPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);

        switch(maximum_freq_class)
        {
            case 0:
                DL_GPIO_setPins(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
                break;
            case 1:
                DL_GPIO_setPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
                break;
            case 2:
                DL_GPIO_setPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);
                break;
            case 3:
                DL_GPIO_setPins(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
                DL_GPIO_setPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
                DL_GPIO_setPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);
                break;
            default:
                break;
        }

    }

    /* Find class with maximum frequency */
    uint8_t maximum_freq_class_DAP = 0;
    for(int i=1;i<4;i++){
        if(of_map[0][maximum_freq_class_DAP]<of_map[0][i]){
            maximum_freq_class_DAP = i;
        }
    }

    softmax(&of_map[0][0], class_prob, 4);
    DAP_OutBuff[0] = maximum_freq_class_DAP;
    DAP_OutBuff[1] = (int8_t) (class_prob[0]*100);
    DAP_OutBuff[2] = (int8_t) (class_prob[1]*100);
    DAP_OutBuff[3] = (int8_t) (class_prob[2]*100);
    DAP_OutBuff[4] = (int8_t) (class_prob[3]*100);
}

/** \brief Append output class with maximum frequency to the buffer */
void add_output_class()
{
    uint8_t output_class = 0;
    int i;
    /* Find class with maximum +ve value */
    for(i=1;i<4;i++){
        if(of_map[0][output_class]<of_map[0][i])
        {
            output_class = i;
        }
    }

    class_buffer[class_buffer_index] = output_class + 1;
    class_buffer_index = (class_buffer_index + 1)%SAMPLE_BUFFER_SIZE;
}

/* Main function */
int main(void)
{
    int status = DAP_ERROR_NONE;
    bool streamStarted = false;
    int dataCounter;
    SYSCFG_DL_init();

    DL_GPIO_clearInterruptStatus(SENSOR_PORT, SENSOR_RDY_1_PIN);
    DL_GPIO_disableInterrupt(SENSOR_PORT, SENSOR_RDY_1_PIN);

    DL_GPIO_clearPins(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
    DL_GPIO_clearPins(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN);
    DL_GPIO_clearPins(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN);

    struct tvmgen_default_inputs tvm_if_map = {(void*) &if_map[0]};
    struct tvmgen_default_outputs tvm_of_map = {(void*) &of_map[0]};

    /* Intializing feature extraction*/
    fe_handle->scratch_buffer = &scratch_buffer[0];
    FE_allocFeatureExtract(fe_handle);
    #if TEST_FEATURE_EXTRACTION
        int error = 0;
        model_input_t test_feature_extraction[1 * FE_STACKING_CHANNELS * FE_STACKING_FRAME_WIDTH * 1];
        fe_handle->input_buffer = &raw_input_test[0];
        fe_handle->history_buffer = &model_test_input[0];
        fe_handle->output_buffer = &test_feature_extraction[0];
        fe_handle->test_feature_extraction = true;
        FE_initFeatureExtract(fe_handle);

        fe_handle->output_buffer = &if_map[0];
        fe_handle->test_feature_extraction = false;
        FE_initFeatureExtract(fe_handle);

        FE_runFeatureExtract(fe_handle);

        error = FE_compareModelInput((model_input_t * )test_feature_extraction,(model_input_t * )if_map);
        printf("Feature extraction mismatches %d\n", error);
        tvmgen_default_run(&tvm_if_map, &tvm_of_map);
        error = FE_compareModelOutput((model_output_t *)golden_output,(model_output_t*) of_map);

        if (error == 0)
        {
            printf("All tests have passed.\r\n");
        }
        printf("Golden vectors matched: %d not matched: %d\n", FE_NN_OUT_SIZE - error, error);
    #endif
    fe_handle->output_buffer  = &if_map[0];
    fe_handle->history_buffer = history_buffer;

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

    /* Sensor initialization */
    sensor_init();
    ADXL355_config(sensor);
    ADXL355_enableInterrupts(&sensor);
    NVIC_EnableIRQ(SENSOR_INT_IRQN);

    /* If write and read were successful, toggle LED */
    while (1)
    {
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
                    dataCounter   = 0U;
                    streamStarted = TRUE;
                }
                sensor_init();
                ADXL355_config(sensor);
                ADXL355_enableInterrupts(&sensor);
                NVIC_EnableIRQ(SENSOR_INT_IRQN);

            }
            if(streamStarted == TRUE)
            {
                if(dataReady)
                {
                    ADXL355_disableInterrupts(&sensor);

                    /* Give data to feature extraction and model */
                    motor_FE();

                    /* Run Inference */
                    tvmgen_default_run(&tvm_if_map, &tvm_of_map);

                    /* Add the output class with maximum value to the class_buffer */
                    add_output_class();

                    /* Glow the respective led for the class with maximum frequency */
                    post_processing();

                    status = Dap_Core_SendStreamHeader(&(DAP_Instance.LinkInstance), DAP_CHANNEL_INF_VALUE, sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamSample(&(DAP_Instance.LinkInstance),(uint8_t *)DAP_OutBuff,sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamEnd(&(DAP_Instance.LinkInstance));

                    ADXL355_enableInterrupts(&sensor);

                    NVIC_EnableIRQ(SENSOR_INT_IRQN);
                    buffCount = 0;
                    dataReady = 0;
                }
            }
        }
        else
        {
            streamStarted = FALSE;
        }
    }
}



/* Interrupt handler for reading SPI data from the sensor */
void SENSOR_INT_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(SENSOR_PORT))
    {
    case SENSOR_RDY_1_IIDX:
        if(sensor.sensorMode == SENSOR_MODE_COLLECT_DATA)
        {
            SPI_readByte(sensor, ADXL355_STATUS_ADDRESS);
            ADXL355_getData(sensor);
        }
        else
        {
            /* Incorrect interrupt  */
        }
        break;
    default:
        break;
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
