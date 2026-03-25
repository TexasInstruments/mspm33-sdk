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

#include <string.h>

#include "feature_extract_app.h"

/*!
 *  Function to initialize FeatureExtract_Params based on a Model type
 */

fe_status_t FeatureExtract_config(FeatureExtract_Params *params)
{

    /* Initialize FeatureExtract_PIR_Params with values coming from DAP. */
    params->model_type = PIR_Model;

    return FE_OK;
}

/*!
 *  Function to execute a sequence of Feature Extraction functions for
 *  a certain model type.
 */

fe_status_t FeatureExtract_run(FeatureExtract_Params *params, uint8_t *input_stream, float *output_stream)
{

    fe_status_t status = FE_ERR_BAD_PARAM;

    static uint8_t input_adcpirbuffer[ADCSAMPLESIZE];
    static uint8_t output_windows[NUM_WINDOWS][WINDOW_SIZE];
    static uint8_t uart_window_out[WINDOW_SIZE];
    static float fft_output_pool_mag[POOLED_BINS];
    static float top_dominant_freq[TOP_N_FREQ];
    static float kurtosis_output[NUM_FEAT_KURTOSIS];
    static float concatenated_features[NUM_WINDOWS * NUM_TOTAL_FEATURES];
    static float slope_changes;
    static float zcr_out;
    static float power_spectrum;

    if (params->model_type == PIR_Model)
    {
        memcpy(input_adcpirbuffer, input_stream, ADCSAMPLESIZE);
        status = FE_applyWindow(input_adcpirbuffer, output_windows);
        for (int i = 0; i < NUM_WINDOWS; i++)
        {
            status = FE_fftPool(output_windows[i], fft_output_pool_mag);
            status = FE_slopeChanges(output_windows[i], &slope_changes);
            status = FE_topFrequencies(output_windows[i], top_dominant_freq);
            status = FE_spectralEntropy(output_windows[i], &power_spectrum);
            status = FE_kurtosis(output_windows[i], kurtosis_output);
            status = FE_zeroCrossingRate(output_windows[i], &zcr_out);
            status = FE_concatenateFeatures(fft_output_pool_mag,
                                            i,
                                            kurtosis_output,
                                            &zcr_out,
                                            &slope_changes,
                                            top_dominant_freq,
                                            &power_spectrum,
                                            output_stream);
        }
    }

    return status;
}

uint8_t FeatureExtract_findClass(float *input, uint8_t size)
{
    float maxValue = input[0];
    uint8_t maxIndex = 0;

    for (int i = 1; i < size; ++i)
    {
        if (input[i] > maxValue)
        {
            maxValue = input[i];
            maxIndex = i;
        }
    }

    float temp = 0;
    float sum_exps = 0;
    for (int i = 0; i < size; ++i)
    {
        temp = input[i];
        input[i] = (float)exp(temp);
        sum_exps += input[i];
    }
    
    for (int i = 0; i < size; ++i)
    {
        temp = input[i];
        input[i] = (uint8_t)(100*(temp/sum_exps));
    }

    return maxIndex;
}