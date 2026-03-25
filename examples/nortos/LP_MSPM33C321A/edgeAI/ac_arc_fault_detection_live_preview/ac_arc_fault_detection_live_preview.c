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

/* Includes */
#include <ti/driverlib/m33/dl_interrupt.h>
#include "ti_msp_dl_config.h"
#include <ti/edgeAI/m33/feature_extract/feature_extract.h>

/* Artifacts */
#include "tvmgen_default.h"

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
/* Model JSON strings */
static const char DAP_Model0Json[] =
    "{\"name\":\"Model1\",\"task\":\"classification\",\"classIdToName\": { \"0\": \"Arc\", \"1\": \"Normal\"},\"projectId\":\"DAP\"}";


    /* Inference value structures */
static const Dap_Interface_InfValueInfoType DAP_InfValue0 = {.NamePtr = "inference_result",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};
static const Dap_Interface_InfValueInfoType DAP_InfValue1 = {.NamePtr = "class0_probability",
                                                                  .Format  = DAP_DATA_FORMAT_INT8};

static const Dap_Interface_InfValueInfoType DAP_InfValue2 = {.NamePtr = "class1_probability",
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
    .InfValueList  = {&DAP_InfValue0, &DAP_InfValue1, &DAP_InfValue2, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                      NULL_PTR},
    .InfValueCount = 3U};


/* ========================================================================== */
/*                 Public Variables                                           */
/* ========================================================================== */

volatile Dap_InstanceType      DAP_Instance;
volatile Uart_Hal_InstanceType DAP_UartInstance;

volatile int8_t DAP_OutBuff[3];
volatile int maxIndex;
/* ========================================================================== */
/*                 Private Function Prototypes                                */
/* ========================================================================== */

/* Macro Definitions */
#define ADC_SAMPLE_SIZE (FE_FRAME_SIZE)
#define FE_FEATURE_SIZE (FE_FRAME_SIZE)
#define FE_FRAME_SKIP   (1U)

/* When FIFO is enabled 2 samples are compacted in a single word */
#define ADC_FIFO_SAMPLES (ADC_SAMPLE_SIZE >> 1)
#define NUM_FRAME_CONNCAT (8)
#define INFERENCE_FRAME_THRESHOLD_FOR_ARC 5

/* Global Variables */

/* ADC Ping-Pong Buffers */
volatile uint32_t gADCSamplesPing[ADC_FIFO_SAMPLES];
volatile uint32_t gADCSamplesPong[ADC_FIFO_SAMPLES];

/* Global Variable to store input to the Model */
volatile int8_t gIF_Map[1 * FE_STACKING_CHANNELS * FE_STACKING_FRAME_WIDTH * 1];

/* Global variable to store Model output */
volatile int8_t gOF_Map[1][2] = {0, 0};

struct tvmgen_default_inputs tvm_if_map = { (void*) &gIF_Map[0] };
struct tvmgen_default_outputs tvm_of_map = { (void*) &gOF_Map[0] };

/* Buffer to hold newly extracted features in 8-bit integer format.*/
volatile int8_t gNewFeatures[FE_FEATURE_SIZE_PER_FRAME];

/*Flag set by an ADC interrupt when new sample data is ready to be processed.*/
volatile bool gCheckADC;

/*Flag used to manage a ping-pong buffer system, typically toggled by an ISR*/
volatile bool gPing;

/*Circular buffer to store the last 8 predictions from the neural network.
  Each prediction is 0 (normal) or 1 (arc fault).*/
volatile uint8_t gPredVector[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/*Write pointer for the circular prediction buffer.*/
volatile uint8_t gPredPtr;

/* Running sum of the values in the 'pred' buffer, used for a voting-based
   arc fault detection algorithm.*/
volatile uint16_t gPredSum;

volatile float scratch_buffer[FE_FRAME_SIZE * 4];
volatile feature_extraction fe;
volatile feature_extraction_handle fe_handle = &fe;

//#define TESTING_FEATURE_EXTRACTION
#ifdef TESTING_FEATURE_EXTRACTION
extern uint16_t raw_input_test[];
extern float model_test_input[];
extern int8_t golden_output[];
#endif
/* Function to adjust sensitivity of post AI/ML inference tripping */
int postProcessing(int inf)
{
    /* determines at what point a prediction is considered arcing */
    int inf_thresh = 0;
    int frame_thresh = INFERENCE_FRAME_THRESHOLD_FOR_ARC;

    /* circular buffer for pred */
    gPredSum -= gPredVector[gPredPtr];
    if (inf >= inf_thresh)
    {
      gPredVector[gPredPtr] = 1;
      gPredSum += 1;
    }
    else
    {
      gPredVector[gPredPtr] = 0;
    }

    gPredPtr = (gPredPtr + 1) % 8;

    if (gPredSum > frame_thresh)
    {
      return 1;
    }
    else
    {
      return 0;
    }
    return 0;
}

/* Scales log300 data samples*/
void data_scaling(int16_t *samples)
{
    for(int i = 0; i < ADC_SAMPLE_SIZE; ++i)
	{
        samples[i] = ((samples[i] )*8);
    }
}


volatile bool streamStarted = 0;
volatile int status;


int main(void)
{
	/* Initialize local and global variables*/
	volatile uint16_t *newSamples;
    volatile float newSamples_float[ADC_SAMPLE_SIZE];
	gPing = true;
	gCheckADC = false;
	gPredPtr = 0;
	gPredSum = 0;
	/* Initialize the device for 160MHz operation from SYSPLL. */
    SYSCFG_DL_init();
    NVIC_EnableIRQ(DMA0_INT_IRQn);
    fe_handle->scratch_buffer = &scratch_buffer[0];
    FE_allocFeatureExtract(fe_handle);
    fe_handle->input_buffer  = newSamples;
    fe_handle->output_buffer = &gIF_Map[0];
    fe_handle->test_feature_extraction = false;
    FE_initFeatureExtract(fe_handle);

    /* Configure DMA0 source, destination and size */
	DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) (&(HSADC0_RESULT_INST->ADC_LITE_RESULT_REGS.ADCSEQ1FIFORESULT)));
	DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)&gADCSamplesPing[0]);
	DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, ADC_FIFO_SAMPLES);
	DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

	/* Initialize the feature extaction library */

    DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);

    /* Initialize UART HAL */
    volatile Uart_Hal_InitParamsType initParams;

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

	/* Enter execution loop - acquire data, pre-process, and run inference */
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
                    streamStarted = TRUE;
                }
                DL_Timer_startCounter(TIMER_0_INST);

            }

            if(streamStarted == TRUE)
            {
                /* start of half AC cycle */
                if (true == gCheckADC)
                {
                    /* The data will be alternating between each buffer */
                    /* Switch to send ADC Results to Pong Buffer */
                    if (gPing == true)
                    {
                        newSamples = gADCSamplesPing;
                        DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)&gADCSamplesPong[0]);
                        DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, ADC_FIFO_SAMPLES);
                        DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

                        gPing = false;
                    }
                    /* Switch to send ADC Results to Ping Buffer */
                    else
                    {
                        newSamples = gADCSamplesPong;
                        DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)&gADCSamplesPing[0]);
                        DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, ADC_FIFO_SAMPLES);
                        DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

                        gPing = true;
                    }
                    DL_Timer_startCounter(TIMER_0_INST);

                    data_scaling(newSamples);


                    for(int i = 0; i < ADC_SAMPLE_SIZE; i++)
                    {
                        newSamples_float[i] = (float)(newSamples[i]);
                    }

                    #ifdef TESTING_FEATURE_EXTRACTION
                        fe_handle->output_buffer = &test_feature_extraction[0];
                        fe_handle->test_feature_extraction = true;
                        FE_initFeatureExtract(fe_handle);
                        newSamples = raw_input_test;
                    #endif
                    gCheckADC = false;
                    fe_handle->input_buffer  = newSamples_float;
                    fe_handle->output_buffer = &gIF_Map[0];
                    /* Run Feature extraction on the new captured samples */
                    FE_runFeatureExtract(fe_handle);

                    /* Copy the features to tvm_if_map */
                    /* Run Inference on the trained model using NPU */
                    tvmgen_default_run(&tvm_if_map, &tvm_of_map);

                    int norm= gOF_Map[0][1];
                    int arc= gOF_Map[0][0];

                    if(postProcessing(arc-norm))
                    {
                        /* if an arc is detected set GPIO high */
                        DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
                    }
                    else
                    {
                        /* if an arc is not detected set GPIO low */
                        DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
                    }
                    if(arc > norm)
                        DAP_OutBuff[0] = maxIndex;
                    else
                        DAP_OutBuff[0] = maxIndex;
                    DAP_OutBuff[1] = (int8_t) gOF_Map[0][0];
                    DAP_OutBuff[2] = (int8_t) gOF_Map[0][1];
                    status = Dap_Core_SendStreamHeader(&(DAP_Instance.LinkInstance), DAP_CHANNEL_INF_VALUE, sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamSample(&(DAP_Instance.LinkInstance),(uint8_t *)DAP_OutBuff,sizeof(DAP_OutBuff));
                    status = Dap_Core_SendStreamEnd(&(DAP_Instance.LinkInstance));
                }
            }
	    }
    }

	return 0;
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

void DMA0_IRQHandler(void)
{
    if (DL_HSADC_getDMAInterruptStatus(HSADC0_INST, DL_HSADC_DMA_INT_1)) {
        DL_HSADC_DMAInterruptStatusClear(HSADC0_INST, DL_HSADC_DMA_INT_1);
        gCheckADC = true;
        DL_Timer_stopCounter(TIMER_0_INST);
        DL_DMA_disableChannel(DMA0, DMA0_CH0_CHAN_ID);
    }
}