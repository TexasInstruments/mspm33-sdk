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
#include <string.h>
/* Artifacts */
#include "tvmgen_default.h"

#include <ti/edgeAI/m33/feature_extract/feature_extract.h>

#define SAWTOOTH_WAVE_INDEX     (0)
#define SINE_WAVE_INDEX         (1)
#define SQUARE_WAVE_INDEX       (2)

/* Defines the number of frames to skip in each inference step (Sliding window) */
#define SLIDING_WINDOW_SIZE     (4)

/* Defines the size of each buffer for ADC data acquisition */
#define ADC_BUFFER_SIZE         ((SLIDING_WINDOW_SIZE)*(FE_FRAME_SIZE))

/* Defines the number of buffer for ADC data acquisition */
#define NUM_BUFFER              (2)
#define CPU_CLK_FREQ_KHZ        160000

float scratch_buffer[FE_FRAME_SIZE * 4];
/* Global Variable to store input to the Model */
model_input_t   if_map[1][FE_FEATURE_SIZE_PER_FRAME*FE_NUM_FRAME_CONCAT][1][1];
/* Global variable to store Model output */
model_output_t  of_map[1][3]={0,0,0};

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
volatile q15_t prodBufferIndex = 0, conBufferIndex = 0;

/* Flag to indicate whether the buffer is full */
volatile bool ADC_bufferState[NUM_BUFFER];

volatile uint32_t gADCresult;

feature_extraction fe;
feature_extraction_handle fe_handle = &fe;

struct tvmgen_default_inputs tvm_if_map = {(void*) &if_map[0]};
struct tvmgen_default_outputs tvm_of_map = {(void*) &of_map[0]};

// Variables for testing feature extraction
extern float raw_input_test[];
extern float model_test_input[];
extern model_output_t golden_output[];
#define FAIL 0
#define PASS 1
int test_result = FAIL;




void postProcessing(void)
{
    /* Index with the highest value corresponds to the model's predicted class */
    uint8_t maxIndex = SAWTOOTH_WAVE_INDEX;


    if(of_map[0][SINE_WAVE_INDEX] > of_map[0][maxIndex])
    {
        maxIndex = SINE_WAVE_INDEX;
    }

    if(of_map[0][SQUARE_WAVE_INDEX] > of_map[0][maxIndex])
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
}



void processing()
{
    fe_handle->scratch_buffer = &scratch_buffer[0];
    fe_handle->input_buffer = &ADC_buffers[conBufferIndex][0];
    fe_handle->output_buffer = &if_map[0];
    FE_runFeatureExtract(fe_handle);
    /* Setting the current ADC Buffer state to empty */
    ADC_bufferState[conBufferIndex] = 0;

    conBufferIndex = (conBufferIndex + 1) % NUM_BUFFER;

    /* Run Inference */
    tvmgen_default_run(&tvm_if_map, &tvm_of_map);
    postProcessing();
}

void FE_init(feature_extraction_handle fe_handle)
{

    fe_handle->scratch_buffer = &scratch_buffer[0];
    FE_allocFeatureExtract(fe_handle);
    fe_handle->input_buffer  = framePtr;
    fe_handle->output_buffer = &if_map[0];
    fe_handle->test_feature_extraction = false;
    FE_initFeatureExtract(fe_handle);

}


int main(void)
{


    SYSCFG_DL_init();


    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);

    FE_init(fe_handle);
    DL_Timer_startCounter(TIMER_0_INST);
    while (1) {
        /* Check if the current ADC buffer is full and ready for processing */
        if(ADC_bufferState[conBufferIndex])
        {
            /* Setting the current ADC Buffer state to empty */
            ADC_bufferState[conBufferIndex] = 0;

            conBufferIndex = (conBufferIndex + 1) % NUM_BUFFER;
            processing();
            /* Run Inference */
            tvmgen_default_run(&tvm_if_map, &tvm_of_map);
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