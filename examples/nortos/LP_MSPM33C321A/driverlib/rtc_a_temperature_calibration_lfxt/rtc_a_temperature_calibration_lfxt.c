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

#include "ti_msp_dl_config.h"
#include <math.h>

/* clang-format off */
// TODO PG2 add IQ Math implementation
// #define ENABLE_IQ_MATH

#ifndef ENABLE_IQ_MATH
// TODO PG2 update with valid coefficient and offset values once available in
// the device datasheet. These are dummy values at the moment
#define TEMP_COEFF                                                      (130.0f)
#define TEMP_OFFSET                                                      (20.0f)
/*
 * For crystal FC-135:
 *  Parabolic coefficient (B) = -0.04 ppm/C^2
 *  Turnover point (T0) = 25C
 */
#define LFXT_B                                                          (-0.04f)
#define LFXT_T0                                                          (25.0f)

float gTemperatureDegC;
float gDriftPPM;
int16_t gDriftPPMInt;
#else
#error "IQ Math implementation is not currently supported"
#endif
#define ADC_BIT_RESOLUTION                                                  (12)
#define ADC_VREF_IN_mV                                                    (2500)
#define RTC_A_MAX_DRIFT_PPM                                                (240)
/* clang-format on */
volatile bool gCheckADC;

void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
        gCheckADC = true;
    }
}

int main(void)
{
    uint16_t adcResult;
    uint32_t adcResultmV;

    SYSCFG_DL_init();

    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);

    /* Start RTC_A clock */
    DL_RTC_A_enableClockControl(RTC_A);
    
    /* Confirm VREF has settled before triggering HSADC conversion */
    while (DL_VREF_CTL1_READY_NOTRDY == DL_VREF_getStatus(VREF))
        ;
    gCheckADC = false;

    while (1) {
        DL_HSADC_triggerSequencerSoftwareForce(
            HSADC0_INST, DL_HSADC_SEQ_NUMBER1);

        while (false == gCheckADC) {
            __WFI();
        }
        adcResult =
            DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);
        /*
         * Convert adcResultmV = (Vref / 2^(ADC_resolution)* adcResult
         * adcResultmV = (2500mV / 2^12) * adcResult
         */
        adcResultmV = (((uint32_t) ADC_VREF_IN_mV * (uint32_t) adcResult) >>
                       ADC_BIT_RESOLUTION);
#ifndef ENABLE_IQ_MATH
        /*
         * Temperature in Celsius. Is calculated in using generic linear
         * correction.
         * adcResultmV = (TEMP_COEFF * temperatureDegC) + TEMP_OFFSET
         * temperatureDegC = (adcResultmV - TEMP_OFFSET) / TEMP_COEFF
         */
        gTemperatureDegC = ((float) adcResultmV - TEMP_OFFSET) / TEMP_COEFF;

        /*
         * The frequency of the crystal can be calculated as:
         *  df/f0 (ppm) =  B (T - T0)^2)
         *      where:
         *          B   = Parabolic coefficient
         *          T   = Ambient temperature
         *          T0  = Turnover point (room temperature)
         */
        gDriftPPM = (LFXT_B * (gTemperatureDegC - LFXT_T0) *
                     (gTemperatureDegC - LFXT_T0));

        /*
         * Round to integer, each LSB in RTC_A Temperature compensation register
         * represents a +-1ppm adjustment.
         */
        gDriftPPMInt = (int16_t) round(gDriftPPM);
#else
#error "IQ Math implementation is not currently supported"
#endif  
        /* Limit drift to +-240PPM */
        if (gDriftPPMInt > RTC_A_MAX_DRIFT_PPM) {
            gDriftPPMInt = RTC_A_MAX_DRIFT_PPM;
        } else if (gDriftPPMInt < (-RTC_A_MAX_DRIFT_PPM)) {
            gDriftPPMInt = -RTC_A_MAX_DRIFT_PPM;
        }
        /* Attempt to update compensation value */
        do {
            /* Wait until RTC_A is ready for compensation */
            while (DL_RTC_A_isReadyToCalibrate(RTC_A) != true)
                ;

            if (gDriftPPMInt >= 0) {
                DL_RTC_A_setTemperatureCompensation(
                    RTC_A, DL_RTC_A_TEMP_CALIBRATION_UP1PPM, gDriftPPMInt);
                    gCheckADC = false;
            } else {
                DL_RTC_A_setTemperatureCompensation(
                    RTC_A, DL_RTC_A_TEMP_CALIBRATION_DOWN1PPM, -gDriftPPMInt);
                    gCheckADC = false;
            }
            /* Check if write was successful */
        } while (DL_RTC_A_isCalibrationWriteResultOK(RTC_A) != true);
        DL_HSADC_enableSequencer(HSADC0_INST, DL_HSADC_SEQ_NUMBER1);
    }
}                             
