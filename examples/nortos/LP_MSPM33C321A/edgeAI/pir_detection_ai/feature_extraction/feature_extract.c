/*
 * Copyright (c) 2025-2026, Texas Instruments Incorporated
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

/*
 *  ======== feature_extract.c ========
 */

#include <string.h>

#include <feature_extraction/feature_extract.h>
#include "arm_math.h"


/* Generic float buffer */
static float generic_float_buf[FFT_SIZE];
/* Complex-output buffer for rfft */
static float fft_complex_buf[FFT_SIZE];
/* Magnitude bins (only half + DC bins) */
static float fft_magnitud_buf[FFT_SIZE / 2 + 1];

/*!
 * @fn                          FE_sign_f
 *
 * @brief                       Inline helper function to calculate sign of input
 *                              word.
 */

static inline float FE_sign_f(float x)
{
    if (x > 0.0f)
    {

        return 1.0f;
    }
    if (x < 0.0f)
    {

        return -1.0f;
    }

    return 0.0f;
}

/*!
 * @fn                          FE_applyWindow
 *
 * @brief                       Builds padded/sliding windows from an input stream
 *
 */

fe_status_t FE_applyWindow(const uint8_t *data_stream, uint8_t output_windows[NUM_WINDOWS][WINDOW_SIZE])
{
    if (data_stream == NULL || output_windows == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    uint8_t padded_window[PADDED_INPUT_SIZE];
    /* Clear input */
    memset(padded_window, 0, sizeof(padded_window));
    /* Build padded window */
    memcpy(&padded_window[PADDING_SIZE_LEFT], data_stream, ADCSAMPLESIZE);
    /* Extract windows */
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        for (int j = 0; j < WINDOW_SIZE; j++)
        {
            output_windows[i][j] = padded_window[i * WINDOW_STRIDE_SIZE + j];
        }
    }

    return FE_OK;
}

/*!
 * @fn                          FE_slopeChanges
 *
 * @brief                       Counts the sign changes of signal differences.
 *
 */

fe_status_t FE_slopeChanges(const uint8_t *input_window, float *slope_changes)
{
    if (input_window == NULL || slope_changes == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    uint32_t count  = 0;
    /* Calculate first difference */
    float prev_diff = (float)input_window[1] - (float)input_window[0];

    for (uint32_t i = 2; i < WINDOW_SIZE; i++)
    {
        /* Calculate current difference */
        float curr_diff = (float)input_window[i] - (float)input_window[i - 1];
        /* Sign change if signs are different (ignoring zeros) */
        if (FE_sign_f(curr_diff) != FE_sign_f(prev_diff))
        {
            if (FE_sign_f(curr_diff) != 0.0f || FE_sign_f(prev_diff) != 0.0f)
            {
                count++;
            }
        }
        /* Update previous difference with current one. */
        prev_diff = curr_diff;
    }

    *slope_changes = (float)count / (float)(WINDOW_SIZE - 1);

    return FE_OK;
}

/*!
 * @fn                          FE_zeroCrossingRate
 *
 * @brief                       Measures of how often a signal crosses the zero
 *                              axis, indicating the frequency or noisiness of
 *                              a signal.
 */

fe_status_t FE_zeroCrossingRate(const uint8_t *input_window, float *zcr_out)
{
    if (input_window == NULL || zcr_out == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    float prev;
    float curr;
    uint32_t count = 0;

    /* Convert first element */
    prev = (float)input_window[0];
    for (uint32_t i = 1; i < WINDOW_SIZE; i++)
    {
        curr = (float)input_window[i];
        /* Sign change if signs are different (ignoring zeros) */
        if (FE_sign_f(curr) != FE_sign_f(prev))
        {
            if (FE_sign_f(curr) != 0.0f || FE_sign_f(prev) != 0.0f)
            {
                count++;
            }
        }
        prev = curr;
    }

    *zcr_out = (float)count / (float)WINDOW_SIZE;

    return FE_OK;
}

/*!
 * @fn                          FE_kurtosis
 *
 * @brief                       Calculates the fisher or excess kurtosis without
 *                              bias for each segment of a given series. Kurtosis
 *                              describes the "peakedness" of a probability distribution.
 *
 */

fe_status_t FE_kurtosis(const uint8_t *input_window, float *kurtosis_output)
{
    if (input_window == NULL || kurtosis_output == NULL)
    {

        return FE_ERR_NULLPTR;
    }
    if (KURTOSIS_CHUNK_SIZE == 0)
    {

        return FE_ERR_BAD_PARAM;
    }

    uint32_t k_counter = 0;
    for (uint32_t start = 0; start + KURTOSIS_CHUNK_SIZE <= WINDOW_SIZE; start += KURTOSIS_STRIDE_SIZE)
    {

        /* Calculate mean */
        float mean = 0.0f;
        for (uint32_t j = 0; j < KURTOSIS_CHUNK_SIZE; ++j)
        {
            mean += (float)input_window[start + j];
        }
        mean /= (float)KURTOSIS_CHUNK_SIZE;

        /* Calculate moments */
        float m2 = 0.0f;
        float m4 = 0.0f;
        for (uint32_t j = 0; j < KURTOSIS_CHUNK_SIZE; ++j)
        {
            float d  = (float)input_window[start + j] - mean;
            float d2 = d * d;
            /* Keep the second moment for the variance calculation later */
            m2 += d2;
            /* Calculate fourth central moment around the mean */
            m4 += d2 * d2;
        }
        /* Calculate variance squared */
        float var2 = m2 * m2;

        /* Handle zero variance case (constant signal) - kurtosis is undefined, use 0 */
        if (var2 < FLT_EPSILON)
        {
            kurtosis_output[k_counter++] = 0.0f;
            continue;
        }

        /* Calculate bias factors */
        float bias_factor1 = (((float)(KURTOSIS_CHUNK_SIZE + 1) * (float)(KURTOSIS_CHUNK_SIZE) *
                               (float)(KURTOSIS_CHUNK_SIZE - 1)) /
                              ((float)(KURTOSIS_CHUNK_SIZE - 2) * (float)(KURTOSIS_CHUNK_SIZE - 3)));
        float bias_factor2 = (((float)(KURTOSIS_CHUNK_SIZE - 1) * (float)(KURTOSIS_CHUNK_SIZE - 1)) /
                              ((float)(KURTOSIS_CHUNK_SIZE - 2) * (float)(KURTOSIS_CHUNK_SIZE - 3)));

        /* Calculate kurtosis */
        kurtosis_output[k_counter++] = (float)((bias_factor1 * (m4 / var2)) - (3.0f * bias_factor2));
    }

    return FE_OK;
}

/*!
 * @fn                          FE_spectralEntropy
 *
 * @brief                       Calculates the Shannon spectral entropy. The Spectral
 *                              Entropy acts as an indicator of how spread out the
 *                              power is.
 *
 */

fe_status_t FE_spectralEntropy(const uint8_t *input_window, float *entropy_out)
{
    if (input_window == NULL || entropy_out == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    /* Initialize cmsis-dsp fft function */
    arm_rfft_fast_instance_f32 rfft;
    if (arm_rfft_fast_init_f32(&rfft, FFT_SIZE) != ARM_MATH_SUCCESS)
    {

        return FE_ERR_DSP;
    }

    /* Zero input float buffer then copy half-window */
    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        generic_float_buf[i] = 0.0f;
    }
    for (uint32_t i = 0; i < FFT_SIZE / 2; i++)
    {
        generic_float_buf[i] = (float)input_window[i];
    }

    /* Forward real FFT (result is complex interleaved) */
    arm_rfft_fast_f32(&rfft, generic_float_buf, fft_complex_buf, 0);
    /* Calculate magnitudes: only first FFT_SIZE/2+1 bins are meaningful for real FFT */
    arm_cmplx_mag_f32(fft_complex_buf, fft_magnitud_buf, FFT_SIZE / 2 + 1);

    /* Zero out the DC bin */
    fft_magnitud_buf[0] = 0.0f;

    /* Calculate power (sum squares) with cmsis-dsp library*/
    float power_sum = 0.0f;
    arm_power_f32(fft_magnitud_buf, (uint32_t)(NUM_BINS + 1), &power_sum);

    /* Handle zero power case (constant signal) - entropy is 0 */
    if (power_sum < FLT_EPSILON)
    {
        *entropy_out = 0.0f;
        return FE_OK;
    }

    /* Calculate Shannon entropy based on normalized power spectrum */
    float entropy = 0.0f;
    for (uint32_t i = 0; i < (uint32_t)(NUM_BINS + 1); i++)
    {
        /* Calculate normalized power spectrum (probability) */
        float p_norm = (fft_magnitud_buf[i] * fft_magnitud_buf[i]) / power_sum;
        /* Evaluate if probability is significantly greater than zero */
        if (p_norm > FLT_EPSILON)
        {
            /* Calculate log */
            float p_norm_log;
            arm_vlog_f32(&p_norm, &p_norm_log, 1);
            /* Calculate entropy */
            entropy -= p_norm * p_norm_log;
        }
    }
    *entropy_out = entropy;

    return FE_OK;
}

/*!
 * @fn                          FE_topFrequencies
 *
 * @brief                       Extract the index of the dominant frequencies
 *                              (Hz) per window.
 *
 */

fe_status_t FE_topFrequencies(const uint8_t *input_window, float *top_freqs)
{
    if (input_window == NULL || top_freqs == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    /* Initialize cmsis-dsp fft function */
    arm_rfft_fast_instance_f32 rfft;
    if (arm_rfft_fast_init_f32(&rfft, FFT_SIZE) != ARM_MATH_SUCCESS)
    {

        return FE_ERR_DSP;
    }

    /* Zero input float buffer then copy half-window */
    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        generic_float_buf[i] = 0.0f;
    }
    for (uint32_t i = 0; i < FFT_SIZE / 2; i++)
    {
        generic_float_buf[i] = (float)input_window[i];
    }

    /* Forward real FFT (result is complex interleaved) */
    arm_rfft_fast_f32(&rfft, generic_float_buf, fft_complex_buf, 0);
    /* Calculate magnitudes: only first FFT_SIZE/2+1 bins are meaningful for real FFT */
    arm_cmplx_mag_f32(fft_complex_buf, fft_magnitud_buf, FFT_SIZE / 2 + 1);

    /* Zero DC to avoid evaluating it */
    fft_magnitud_buf[0] = 0.0f;
    static float fft_tmp_buf[FFT_SIZE / 2 + 1];
    /* Calculate first TOP_N_FREQ dominant frequencies */
    for (int i = 0; i < TOP_N_FREQ; i++)
    {
        float max_val;
        uint32_t max_idx;
        // Find max magnitud and corresponding FFT bin index of only FFT_SIZE/2 number of bins.
        arm_abs_f32(fft_magnitud_buf,fft_tmp_buf,FFT_SIZE/2 + 1);
        arm_max_f32(fft_tmp_buf, NUM_BINS, &max_val, &max_idx);
        /* Eliminate the found frequency peak */
        fft_magnitud_buf[max_idx]     = 0.0f;
        /* Convert bin index to frequency (Hz) */
        float freq                    = ((float)samplingfreq / (float)FFT_SIZE) * (float)max_idx;
        top_freqs[TOP_N_FREQ - i - 1] = freq;
    }

    return FE_OK;
}

/*!
 * @fn                            FE_calculateSymmetricMirror
 *
 * @brief                         Inline helper function to calculate symmetric mirror
 *                                of input (half window) into full FFT-size buffer.
 *
 */

static inline fe_status_t FE_calculateSymmetricMirror(const uint8_t *input_half_window, uint8_t *symmetric_window)
{
    if (input_half_window == NULL || symmetric_window == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    /* Window size of the input stream has to be half of the FFT size */
    uint32_t middle = FFT_SIZE / 2;
    /* Clear output */
    memset(symmetric_window, 0, FFT_SIZE);
    /* Copy first half */
    memcpy(symmetric_window, input_half_window, middle);
    /* Mirror (symmetric) - copy again starting at the middle */
    memcpy(&symmetric_window[middle], input_half_window, middle);

    return FE_OK;
}

/*!
 * @fn                            FE_fftBinsAveragePooling
 *
 * @brief                         Inline helper function to average bins ignoring
 *                                DC (index 0).
 *
 */

static inline fe_status_t FE_fftBinsAveragePooling(const float *fft_mag, uint32_t pooling_stride, float *out_pool)
{
    if (fft_mag == NULL || out_pool == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    int counter = 0;
    /* Start at i=1 to skip DC component (index 0), pool pairs of adjacent bins */
    for (int i = 1; i < NUM_BINS; i += pooling_stride)
    {
        out_pool[counter] = (fft_mag[i] + fft_mag[i + 1]) / (float)pooling_stride;
        counter++;
    }
    /* Zero out the last pooled bin (Nyquist frequency area) */
    out_pool[counter - 1] = 0.0f;

    return FE_OK;
}

/*!
 * @fn                              FE_fftPool
 *
 * @brief                           calculate FFT magnitudes and pooling (averaging)
 *                                  to produce NUM_FEAT_FFT features.
 *
 */

fe_status_t FE_fftPool(const uint8_t *input_window, float *fft_output_pool_mag)
{
    if (input_window == NULL || fft_output_pool_mag == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    /* Initialize cmsis-dsp fft function */
    arm_rfft_fast_instance_f32 rfft;
    arm_status status = arm_rfft_fast_init_f32(&rfft, FFT_SIZE);
    if (status != ARM_MATH_SUCCESS)
    {
        return FE_ERR_DSP;
    }

    /* Symmetric mirrored input */
    static uint8_t symm_window[FFT_SIZE];
    // Symmetrically mirror the window
    FE_calculateSymmetricMirror(input_window, symm_window);

    /* Convert to float */
    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        generic_float_buf[i] = (float)symm_window[i];
    }

    /* Forward real FFT (result is complex interleaved) */
    arm_rfft_fast_f32(&rfft, generic_float_buf, fft_complex_buf, 0);

    /* Calculate magnitudes: only first FFT_SIZE/2+1 bins are meaningful for real FFT */
    arm_cmplx_mag_f32(fft_complex_buf, fft_magnitud_buf, FFT_SIZE / 2 + 1);

    // Average Pool FFT magnitud with stride and remove DC
    FE_fftBinsAveragePooling(fft_magnitud_buf, 2, fft_output_pool_mag);

    return FE_OK;
}

/*!
 * @fn                                  FE_concatenateFeatures
 *
 * @brief                               Concatenate many feature arrays into a big
 *                                      feature vector. The offsets are determined
 *                                      by NUM_FEAT_X and NUM_WINDOWS macros.
 *
 */

fe_status_t FE_concatenateFeatures(const float *fft_output_pool_mag,
                                   uint32_t window_index,
                                   const float *kurtosis_output,
                                   const float *zcr,
                                   const float *slope_changes,
                                   const float *top_dominant_freq,
                                   const float *power_spectrum,
                                   float *concatenated_features)
{
    if (fft_output_pool_mag == NULL || kurtosis_output == NULL || zcr == NULL || slope_changes == NULL ||
        top_dominant_freq == NULL || power_spectrum == NULL || concatenated_features == NULL)
    {

        return FE_ERR_NULLPTR;
    }

    if (window_index >= NUM_WINDOWS)
    {

        return FE_ERR_BAD_PARAM;
    }

    /* FFT features */
    size_t base_fft = (size_t)window_index * (size_t)NUM_FEAT_FFT;
    for (size_t i = 0; i < NUM_FEAT_FFT; i++)
    {
        concatenated_features[base_fft + i] = fft_output_pool_mag[i];
    }

    /* Kurtosis features */
    size_t base_kurtosis = (size_t)NUM_WINDOWS * (size_t)NUM_FEAT_FFT +
                           (size_t)window_index * (size_t)NUM_FEAT_KURTOSIS;
    for (size_t i = 0; i < NUM_FEAT_KURTOSIS; i++)
    {
        concatenated_features[base_kurtosis + i] = kurtosis_output[i];
    }

    /* ZCR features*/
    size_t base_zcr = (size_t)NUM_WINDOWS * ((size_t)NUM_FEAT_FFT + (size_t)NUM_FEAT_KURTOSIS) +
                      (size_t)window_index * (size_t)NUM_FEAT_ZCR;
    for (size_t i = 0; i < NUM_FEAT_ZCR; i++)
    {
        concatenated_features[base_zcr + i] = *zcr;
    }

    /* Slope changes features */
    size_t base_slope = (size_t)NUM_WINDOWS *
                            ((size_t)NUM_FEAT_FFT + (size_t)NUM_FEAT_KURTOSIS + (size_t)NUM_FEAT_ZCR) +
                        (size_t)window_index * (size_t)NUM_FEAT_SLOPE;
    for (size_t i = 0; i < NUM_FEAT_SLOPE; i++)
    {
        concatenated_features[base_slope + i] = *slope_changes;
    }

    /* Top frequencies features */
    size_t base_top = (size_t)NUM_WINDOWS * ((size_t)NUM_FEAT_FFT + (size_t)NUM_FEAT_KURTOSIS + (size_t)NUM_FEAT_ZCR +
                                             (size_t)NUM_FEAT_SLOPE) +
                      (size_t)window_index * (size_t)NUM_FEAT_TOPFREQ;
    for (size_t i = 0; i < NUM_FEAT_TOPFREQ; i++)
    {
        concatenated_features[base_top + i] = top_dominant_freq[i];
    }

    /* Spectral entropy features */
    size_t base_pw = (size_t)NUM_WINDOWS * ((size_t)NUM_FEAT_FFT + (size_t)NUM_FEAT_KURTOSIS + (size_t)NUM_FEAT_ZCR +
                                            (size_t)NUM_FEAT_SLOPE + (size_t)NUM_FEAT_TOPFREQ) +
                     (size_t)window_index * (size_t)NUM_FEAT_PW_SPEC;
    for (size_t i = 0; i < NUM_FEAT_PW_SPEC; i++)
    {
        concatenated_features[base_pw + i] = *power_spectrum;
    }

    return FE_OK;
}
