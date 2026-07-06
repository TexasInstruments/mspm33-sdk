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
/*!****************************************************************************
*  @file       feature_extract.h
*  @brief      <b>PRELIMINARY</b> Feature Extraction for EdgeAI
*
*  <b>WARNING</b> These set of functions are <b>PRELIMINARY</b>, and subject to
*  change in the next few months.
*
*  To use the Feature Extraction library, include this header file in the
*  application as follows:

*  @code
*  #include <ti/ai/edge_ai/fe/feature_extract.h>
*  @endcode
*
*  @anchor ti_edgeai_feature_extract_Overview
*  # Overview #
*  This library will serve as a pre-processing stage where raw signal inputs
*  will be fed to the feature extraction library and the resulting output will
*  be fed into the pre-compiled NN model. The function that have been implemented
*  so far are the following:
*
*  - FE_applyWindow(): extracts windows of data based on a window size, stride size
*    and an input stream.
*
*  - FE_fftPool(): calculates the Real FFT magnitude for each window and average
*    pool the resulting bins. The window undergoes a symmetrical modification first,
*    doubling its size.
*
*  - FE_kurtosis(): calculates the fisher or excess Kurtorsis for each segment of
*    a given window, without bias (look for exact equation in the previous graph).
*    Kurtosis describes the "peakedness" of a probability distribution.
*
*  - FE_zeroCrossingRate(): calculates the zero crossing rate for each window. ZCR measures
*    of how often a signal crosses the zero axis, indicating the frequency or noisiness
*    of a signal.
*
*  - FE_slopeChanges(): counts the sign changes of signal differences.
*
*  - FE_topFrequencies(): calculates the top N dominant frequencies of a FFT result per window.
*    The window doubles its size first, padded with zeros to the right.
*
*  - FE_spectralEntropy(): calculates the Shannon spectral entropy of a FFT result per
*    window. The window doubles its size first, padded with zeros to the right. The
*    Spectral Entropy acts as an indicator of how spread out the power is across different
*    frequencies.
*
*  - FE_concatenateFeatures(): Concatenate many feature arrays into a big feature vector.
*    The offsets are determined by NUM_FEAT_X and NUM_WINDOWS macros.
*
*  <hr>
*  # Usage
*  To use the Feature Extraction Library call the previous APIs accordingly.
*
******************************************************************************
*/

#ifndef ti_edgeai_feature_extract__include
#define ti_edgeai_feature_extract__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#ifndef FE_PARAMETERS_IS_DEFINE

    /* Sampling */
    #define ADCSAMPLESIZE 125
    #define SAMPLING_FREQ 25

    /* Windowing */
    #define PADDING_SIZE_LEFT  4
    #define PADDING_SIZE_RIGHT 3
    #define PADDED_INPUT_SIZE  (ADCSAMPLESIZE + PADDING_SIZE_LEFT + PADDING_SIZE_RIGHT)
    #define WINDOW_SIZE        32
    #define WINDOW_STRIDE_SIZE 4
    #define NUM_WINDOWS        25

    /* FFT */
    #define FFT_SIZE    64
    #define NUM_BINS    (FFT_SIZE / 2)
    #define POOLED_BINS (NUM_BINS / 2)

    /* Top Dominant Frequencies */
    #define TOP_N_FREQ 2

    /* Kurtosis */
    #define KURTOSIS_CHUNK_SIZE  8
    #define KURTOSIS_STRIDE_SIZE 8

    /* Total Features */
    #define NUM_FEAT_FFT       16
    #define NUM_FEAT_TOPFREQ   TOP_N_FREQ
    #define NUM_FEAT_KURTOSIS  4
    #define NUM_FEAT_ZCR       1
    #define NUM_FEAT_SLOPE     1
    #define NUM_FEAT_PW_SPEC   1
    #define NUM_TOTAL_FEATURES 25

#endif

extern uint32_t samplingfreq;

/*!
 *  @brief    Specifies the FE status codes.
 *
 *  This enum defines the return states of the FE functions.
 *
 */
typedef enum
{
    /*!< Process ended successfully. */
    FE_OK            = 0x00,
    /*!< The pointer to one of the function arguments has a NULL value. */
    FE_ERR_NULLPTR   = 0x01,
    /*!< One of the Feature Extraction configuration parameters is incorrect or not supported. */
    FE_ERR_BAD_PARAM = 0x02,
    /*!< DSP operation has failed. */
    FE_ERR_DSP       = 0x03,
} fe_status_t;

/*!
 * @brief                       Builds padded/sliding windows from an input stream
 *                              considering a stride size.
 *
 * @param[in]  data_stream      pointer to input data stream [ADCSAMPLESIZE] for time series.
 * @param[out] output_windows   preallocated 2D-array [NUM_WINDOWS][WINDOW_SIZE].
 *
 * @retval  FE_OK               Process ended successfully.
 * @retval  FE_ERR_NULLPTR      The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM    One of the Feature Extraction configuration parameters is incorrect or not supported.
 * @retval  FE_ERR_DSP          DSP operation has failed.
 *
 */

fe_status_t FE_applyWindow(const uint8_t *data_stream, uint8_t output_windows[NUM_WINDOWS][WINDOW_SIZE]);

/*!
 * @brief                       Measures of how often a signal crosses the zero
 *                              axis, indicating the frequency or noisiness of
 *                              a signal.
 *
 * @param[in]  input_window     input window uint8.
 * @param[out] zcr_out          pointer to result float32.
 *
 * @retval  FE_OK               Process ended successfully.
 * @retval  FE_ERR_NULLPTR      The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM    One of the Feature Extraction configuration parameters is incorrect or not supported.
 * @retval  FE_ERR_DSP          DSP operation has failed.
 *
 */

fe_status_t FE_zeroCrossingRate(const uint8_t *input_window, float *zcr_out);

/*!
 * @brief                       Counts the sign changes of signal differences.
 *
 * @param[in]  input_window     input window uint8.
 * @param[out] slope_changes    pointer to result float32.
 *
 * @retval  FE_OK               Process ended successfully.
 * @retval  FE_ERR_NULLPTR      The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM    One of the Feature Extraction configuration parameters is incorrect or not supported.
 * @retval  FE_ERR_DSP          DSP operation has failed.
 *
 */
fe_status_t FE_slopeChanges(const uint8_t *input_window, float *slope_changes);

/*!
 * @brief                           calculate FFT magnitudes and pooling (averaging)
 *                                  to produce NUM_FEAT_FFT features.
 *
 * @param[in]  input_window         input window uint8.
 * @param[out] fft_output_pool_mag  preallocated array of NUM_FEAT_FFT float32.
 *
 * @retval  FE_OK                   Process ended successfully.
 * @retval  FE_ERR_NULLPTR          The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM        One of the Feature Extraction configuration parameters is incorrect or not
 * supported.
 * @retval  FE_ERR_DSP              DSP operation has failed.
 *
 */
fe_status_t FE_fftPool(const uint8_t *input_window, float *fft_output_pool_mag);

/*!
 * @brief                       Extract the index of the dominant frequencies
 *                              (Hz) per window.
 *
 * @param[in]  input_window     input window uint8.
 * @param[out] top_freqs        preallocated array of length TOP_N_FREQ.
 *
 * @retval  FE_OK               Process ended successfully.
 * @retval  FE_ERR_NULLPTR      The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM    One of the Feature Extraction configuration parameters is incorrect or not supported.
 * @retval  FE_ERR_DSP          DSP operation has failed.
 *
 */
fe_status_t FE_topFrequencies(const uint8_t *input_window, float *top_freqs);

/*!
 * @brief                       Calculates the Shannon spectral entropy. The Spectral
 *                              Entropy acts as an indicator of how spread out the
 *                              power is.
 *
 * @param[in]  input_window     input window uint8.
 * @param[out] entropy_out      pointer to float32 result.
 *
 * @retval  FE_OK               Process ended successfully.
 * @retval  FE_ERR_NULLPTR      The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM    One of the Feature Extraction configuration parameters is incorrect or not supported.
 * @retval  FE_ERR_DSP          DSP operation has failed.
 *
 */
fe_status_t FE_spectralEntropy(const uint8_t *input_window, float *entropy_out);

/*!
 * @brief                       Calculates the fisher or excess kurtosis without
 *                              bias for each segment of a given series. Kurtosis
 *                              describes the "peakedness" of a probability distribution.
 *
 * @param[in]  input_window     input window uint8.
 * @param[out] kurtosis_output  preallocated array of NUM_FEAT_KURTOSIS float32.
 *
 * @retval  FE_OK               Process ended successfully.
 * @retval  FE_ERR_NULLPTR      The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM    One of the Feature Extraction configuration parameters is incorrect or not supported.
 * @retval  FE_ERR_DSP          DSP operation has failed.
 *
 */
fe_status_t FE_kurtosis(const uint8_t *input_window, float *kurtosis_output);

/*!
 * @brief                               Concatenate many feature arrays into a big
 *                                      feature vector. The offsets are determined
 *                                      by NUM_FEAT_X and NUM_WINDOWS macros.
 *
 * @param[in]  fft_output_pool_mag      pointer to FFT features (NUM_FEAT_FFT)
 * @param[in]  window_index             which window this belongs to (0 .. NUM_WINDOWS-1)
 * @param[in]  kurtosis_output          pointer to NUM_FEAT_KURTOSIS values
 * @param[in]  zcr                      pointer to zcr value (scalar)
 * @param[in]  slope_changes            pointer to slope_changes value (scalar)
 * @param[in]  top_dominant_freq        pointer to TOP_N_FREQ values
 * @param[in]  power_spectrum           pointer to spectral entropy (scalar)
 * @param[out] concatenated_features    preallocated feature vector of size NUM_TOTAL_FEATURES * NUM_WINDOWS
 *
 * @retval  FE_OK                       Process ended successfully.
 * @retval  FE_ERR_NULLPTR              The pointer to one of the function arguments has a NULL value.
 * @retval  FE_ERR_BAD_PARAM            One of the Feature Extraction configuration parameters is incorrect or not
 * supported.
 * @retval  FE_ERR_DSP                  DSP operation has failed.
 *
 */
fe_status_t FE_concatenateFeatures(const float *fft_output_pool_mag,
                                   uint32_t window_index,
                                   const float *kurtosis_output,
                                   const float *zcr,
                                   const float *slope_changes,
                                   const float *top_dominant_freq,
                                   const float *power_spectrum,
                                   float *concatenated_features);

#endif /* fe_feature_extract_Module__include */

#ifdef __cplusplus
}
#endif
