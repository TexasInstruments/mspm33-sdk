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

#define HUMAN_MOTION_INDEX          (0)
#define BACKGROUND_MOTION_INDEX     (1)
#define DOG_MOTION_INDEX            (2)

/* Defines the size of each buffer for ADC data acquisition */
#define ADC_BUFFER_SIZE         ADCSAMPLESIZE

/* Defines the number of buffer for ADC data acquisition */
#define NUM_BUFFER              (2)

/* Global Variable to store input to the Model */
int8_t if_map_int8[1][1][25][20];

/* Global variable to store Model output */
static float pir_output_class0[1][NUM_CLASSES];
static float pir_output_class1[1][128];
float scratch_buffer[ADCSAMPLESIZE * 4];

extern uint8_t raw_input_test[125];
extern float32_t model_test_input[625];

/* Application buffers */
uint8_t sensor_buffer[125];
static float feature_buffer[NUM_WINDOWS * NUM_TOTAL_FEATURES];

/* Feature Extraction parameters */
static FeatureExtract_Params featureExtractParams;
/* ADC related Params */
/* Ping pong buffers for storing data from the ADC */
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
}


int main(void)
{
    SYSCFG_DL_init();
    FeatureExtract_config(&featureExtractParams);
    samplingfreq = (float)33;
    struct tvmgen_default_inputs tvm_if_map = {(void*) &feature_buffer[0]};
    struct tvmgen_default_outputs tvm_of_map = {(void*) &pir_output_class0[0]};

    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);

    DL_HSADC_triggerSequencerSoftwareForce(
            HSADC0_INST, DL_HSADC_SEQ_NUMBER1);
    int i = 0;

    FeatureExtract_run(&featureExtractParams,raw_input_test, feature_buffer);

    tvmgen_default_run(&tvm_if_map,&tvm_of_map);
    while (1) {
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
                ADC_buffers[dataIndex++] = (uint8_t)(gADCresult>>4) ;
            }

            if(dataIndex >= ADC_BUFFER_SIZE)
            {
                /* Set flag that the buffer is full */
                ADC_bufferState = true;

                dataIndex = 0;
            }            
        }
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
    }
    
}


