/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "uart_console.h"

/* Temperature calibration constants */
#define TEMP_COEFF                                                      (130.0f)
#define TEMP_OFFSET                                                      (20.0f)
/*
 * Temperature coefficient parameters:
 * 
 * For crystal FC-135 (LFXT):
 *  Parabolic coefficient (B) = -0.04 ppm/C^2
 *  Turnover point (T0) = 25C
 *  Formula: drift = B * (T - T0)^2
 */
#define LFXT_B                                                          (-0.04f)
#define LFXT_T0                                                          (25.0f)
#define ADC_BIT_RESOLUTION                                                  (12)
#define ADC_VREF_IN_mV                                                    (2500)
#define RTC_A_MAX_DRIFT_PPM                                                (240)
/* Global variables */
volatile bool gDynamicTempCalibrationEnabled = false;
volatile bool gStaticOffsetCalibrationEnabled = false;
volatile bool gtimerTriggered = false;
volatile bool gCheckADC = false;
volatile bool enableUTC = false;
volatile bool gCalibrationInProgress = false;
DL_RTC_A_Calendar calendarTime;
char userInput1='n';
char userInput2='y'; /* Set to 'y' by default to enable dynamic calibration */

/* Function prototypes */
void dynamicTemperatureCalibration(void);
void staticOffsetCalibration(void);
/**
 * Timer Interrupt Handler
 * 
 * This function is called when the timer interrupt occurs (every 5 minutes).
 * It calls the dynamic temperature calibration function if enabled.
 */
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            /* Set the timer enabled flag to trigger dynamic calibration in main loop */
            gtimerTriggered = true;
            break;
        default:
            break;
    }
}
/**
 * RTC_A Interrupt Handler
 * 
 * This function is called when an RTC_A interrupt occurs.
 * It checks for the RTC_A ready interrupt and prints UTC timestamp when it occurs.
 * UTC format YYYY-MM-DD DW HH:MM:SS
 */
void RTC_A_INST_IRQHandler(void)
{
    switch (DL_RTC_A_getPendingInterrupt(RTC_A)) {
        case DL_RTC_A_IIDX_PRESCALER1:
            /* RTC_A PS1(1Hz)interrupt occurred Set flag to print UTC time in main loop */
            enableUTC = true;
            break;
        default:
            break;
    }
}
/**
 * HSADC Interrupt Handler
 * 
 * This function is called when an HSADC interrupt occurs.
 * It sets a flag to indicate that ADC data is ready to be processed.
 */
void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
        gCheckADC = true;
    }
}

/**
 * Static Offset Calibration API
 * 
 * This function implements static offset-based calibration for the RTC_A.
 * It calculates referenced frequency using DL_RTC_A_getOffsetCalibrationFrequency(RTC_A) API,
 * calculates gCrystalDriftPPM and gCrystalDriftPPMInt, and performs calibration in rounds
 * if drift exceeds ±240 PPM until all calculated drift is calibrated within limits.
 * According to the spec offset compensation requires 60 seconds (one minute) to take effect as a part of the next calibration cycle.
 */
void staticOffsetCalibration(void)
{
    int16_t calibVal;
    bool needsMoreRounds;
    int totalRounds = 0;
    float referencedRTC_A_Frequency = 0.0;
    float measuredRTC_A_Frequency = 0.0;
    float gCrystalDriftPPM;
    int16_t gCrystalDriftPPMInt;
    /* Get reference frequency using DL_RTC_A_getOffsetCalibrationFrequency API */
    DL_RTC_COMMON_OFFSET_CALIBRATION_FREQUENCY freqEnum = DL_RTC_A_getOffsetCalibrationFrequency(RTC_A);
    
    /* Convert enum to actual frequency value */
    switch (freqEnum) {
        case DL_RTC_COMMON_OFFSET_CALIBRATION_FREQUENCY_512:
            referencedRTC_A_Frequency = 512;
            break;
        case DL_RTC_COMMON_OFFSET_CALIBRATION_FREQUENCY_256:
            referencedRTC_A_Frequency = 256;
            break;
        case DL_RTC_COMMON_OFFSET_CALIBRATION_FREQUENCY_1:
            referencedRTC_A_Frequency = 1;
            break;
        default:
            break;
    }
    uart_printf(UART_0_INST,"Reference RTC_A frequency: %.2f Hz\n", referencedRTC_A_Frequency);
    uart_printf(UART_0_INST,"Enter measured RTC_A frequency: ");
    uart_scanf(UART_0_INST, 1, "%f", &measuredRTC_A_Frequency);
    uart_printf(UART_0_INST,"%.4f \n",measuredRTC_A_Frequency);

    /** Calculate crystal offset drift in PPM using formula 
    *
    *          (60 ×16384×(1 −fRTCCLK/512))
    * 60 is the calibration cycle length in seconds 
    * 16384 is the frequency of the Q0 output of RT0PS
    * fRTCCLK is the measured output frequency 
    * 512 is the expected frequency of the RTCCLK
    *
    */
    
    gCrystalDriftPPM = (60 * 16384 * (1 - measuredRTC_A_Frequency/referencedRTC_A_Frequency));
    gCrystalDriftPPMInt = (int16_t) round(gCrystalDriftPPM);
    uart_printf(UART_0_INST,"Crystal Offset Drift: %d PPM\n", gCrystalDriftPPMInt);
    do {
        /* Set calibration value based on drift limits */
        if (gCrystalDriftPPMInt > RTC_A_MAX_DRIFT_PPM) {
            gCrystalDriftPPMInt -= RTC_A_MAX_DRIFT_PPM;
            calibVal = RTC_A_MAX_DRIFT_PPM;
            needsMoreRounds = true;
        } else if (gCrystalDriftPPMInt < -RTC_A_MAX_DRIFT_PPM) {
            gCrystalDriftPPMInt += RTC_A_MAX_DRIFT_PPM;
            calibVal = -RTC_A_MAX_DRIFT_PPM;
            needsMoreRounds = true;
        } else {
            calibVal = gCrystalDriftPPMInt;
            needsMoreRounds = false;
        }
        
        /* Apply calibration */
        totalRounds++;
        uart_printf(UART_0_INST,"Round %d: Applying %d PPM calibration\n", totalRounds, calibVal);
        
        /* Wait until RTC_A is ready for calibration */
        while (DL_RTC_A_isReadyToCalibrate(RTC_A) != true);
        
        /* Set offset calibration value with appropriate sign */
        if (calibVal >= 0) {
            DL_RTC_A_setOffsetCalibrationAdjValue(RTC_A, DL_RTC_A_OFFSET_CALIBRATION_SIGN_UP, calibVal);
        } else {
            DL_RTC_A_setOffsetCalibrationAdjValue(RTC_A, DL_RTC_A_OFFSET_CALIBRATION_SIGN_DOWN, -calibVal);
        }
        
        /* Wait for calibration to complete */
        while (DL_RTC_A_isCalibrationWriteResultOK(RTC_A) != true);
        
        if (needsMoreRounds) {
            /* With a 32 MHz system clock, 1 second corresponds to 32,000,000 clock cycles (ticks),
            60 seconds corresponds to 1,920,000,000 clock cycles*/
            uart_printf(UART_0_INST,"Waiting for 60 seconds before starting the next offset calibration round...\n");
            delay_cycles(1920000000); /* 1 minute delay */
        } else {
            uart_printf(UART_0_INST,"Waiting for 60 seconds after the Offset Calibration is completed in %d rounds\n", totalRounds);
            delay_cycles(1920000000); /* 1 minute delay */
        }
        
    } while (needsMoreRounds);
}
/**
 * Dynamic Temperature Calibration API
 * 
 * This function implements temperature-based calibration for the RTC_A.
 * It reads temperature from the ADC once, calculates drift, and applies compensation
 * in rounds if drift exceeds ±240 PPM until all calculated drift is calibrated within limits.
 * Sets a flag to prevent multiple calibration processes from running simultaneously.
 */
void dynamicTemperatureCalibration(void)
{
    int16_t calibVal;
    bool needsMoreRounds;
    int totalRounds = 0;
    int16_t totalDrift;
    uint16_t adcResult;
    uint32_t adcResultmV;
    float gTemperatureDegC;
    float gTemperatureDriftPPM=0;
    int16_t gTemperatureDriftPPMInt =0;
    
    /* Set flag to indicate calibration is in progress */
    gCalibrationInProgress = true;
    
    /* Wait for ADC conversion to complete */
    while (false == gCheckADC) {
        __WFI();
    }
    
    /* Get ADC result */
    adcResult = DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);
    
    /* Convert ADC result to millivolts */
    adcResultmV = (((uint32_t) ADC_VREF_IN_mV * (uint32_t) adcResult) >> ADC_BIT_RESOLUTION);
    
    /* Calculate temperature in Celsius */
    gTemperatureDegC = ((float) adcResultmV - TEMP_OFFSET) / TEMP_COEFF;
    
    /* Calculate frequency drift in PPM based on temperature */
    gTemperatureDriftPPM = (LFXT_B * (gTemperatureDegC - LFXT_T0) * (gTemperatureDegC - LFXT_T0));
    
    /* Round to integer */
    gTemperatureDriftPPMInt = (int16_t) round(gTemperatureDriftPPM);
    
    /* Reset ADC flag */
    gCheckADC = false;
    
    totalDrift = gTemperatureDriftPPMInt;
    
    /* Account for crystal drift that was already compensated */
    DL_RTC_A_setOffsetCalibrationAdjValue(RTC_A, DL_RTC_A_OFFSET_CALIBRATION_SIGN_UP, 0);
    
    uart_printf(UART_0_INST,"Temperature: %.2f°C, Drift: %d PPM\n", gTemperatureDegC, totalDrift);
    
    do {
        
        /* Set calibration value */
        if (totalDrift > RTC_A_MAX_DRIFT_PPM) {
            totalDrift -= RTC_A_MAX_DRIFT_PPM;
            calibVal = RTC_A_MAX_DRIFT_PPM;
            needsMoreRounds = true;
        } else if (totalDrift < -RTC_A_MAX_DRIFT_PPM) {
            totalDrift += RTC_A_MAX_DRIFT_PPM;
            calibVal = -RTC_A_MAX_DRIFT_PPM;
            needsMoreRounds = true;
        } else {
            calibVal = totalDrift;
            needsMoreRounds = false;
        }
        
        /* Apply calibration */
        totalRounds++;
        uart_printf(UART_0_INST,"Round %d: Applying %d PPM temperature compensation\n", totalRounds, calibVal);
        
        while (DL_RTC_A_isReadyToCalibrate(RTC_A) != true);
        
        if (calibVal >= 0) {
            DL_RTC_A_setTemperatureCompensation(
                RTC_A, DL_RTC_A_TEMP_CALIBRATION_UP1PPM, calibVal);
        } else {
            DL_RTC_A_setTemperatureCompensation(
                RTC_A, DL_RTC_A_TEMP_CALIBRATION_DOWN1PPM, -calibVal);
        }
        
        while (DL_RTC_A_isCalibrationWriteResultOK(RTC_A) != true);
                
        if (needsMoreRounds) {
            /* With a 32 MHz system clock, 1 second corresponds to 32,000,000 clock cycles (ticks),
            60 seconds corresponds to 1,920,000,000 clock cycles*/
            uart_printf(UART_0_INST,"Waiting for 60 seconds before starting the next temperature calibration round...\n");
            delay_cycles(1920000000); 
        } else {
            uart_printf(UART_0_INST,"Waiting for 60 seconds after the temperature Calibration is completed in %d rounds\n", totalRounds);
            delay_cycles(1920000000); /* 1 minute delay */
        }
        
    } while (needsMoreRounds);
    
    /* Clear flag to indicate calibration is complete */
    gCalibrationInProgress = false;
}
/**
 * Main function
 */
int main(void)
{  
    /* Initialize system configuration */
    SYSCFG_DL_init();
    /* Enable the RTC_A, HSADC, and Timer interrupts at NVIC */
    NVIC_EnableIRQ(RTC_A_INST_INT_IRQN);
    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    /* Confirm VREF has settled before triggering HSADC conversion */
    while (DL_VREF_CTL1_READY_NOTRDY == DL_VREF_getStatus(VREF)); 
    /* Start RTC_A clock */
    DL_RTC_A_enableClockControl(RTC_A);
    /* Ask user if they want to enable static offset calibration */
    uart_printf(UART_0_INST,"Do you want to perform static offset calibration? (y/n): ");
    uart_scanf(UART_0_INST, 1, "%c", &userInput1); 
    uart_printf(UART_0_INST,"%c \n",userInput1);  
    if (userInput1 == 'y' || userInput1 == 'Y') {
        uart_printf(UART_0_INST,"Static offset calibration enabled\n");
        gStaticOffsetCalibrationEnabled = true;
        /* Perform static offset calibration */
        staticOffsetCalibration();
    } else {
        uart_printf(UART_0_INST,"Static offset calibration disabled\n");
        gStaticOffsetCalibrationEnabled = false;
    }
    
    /* Ask user if they want to enable dynamic temperature calibration */
    uart_printf(UART_0_INST,"Do you want to enable dynamic temperature calibration? (y/n): ");
    uart_scanf(UART_0_INST, 1, "%c", &userInput2);  
    uart_printf(UART_0_INST,"%c \n",userInput2);  
    if (userInput2 == 'y' || userInput2 == 'Y') {
        uart_printf(UART_0_INST,"Dynamic temperature calibration enabled\n");
        gDynamicTempCalibrationEnabled = true;
        /* Start the timer for periodic temperature calibration (every 5 minutes) */
        DL_TimerG_startCounter(TIMER_0_INST);
    } else {
        uart_printf(UART_0_INST,"Dynamic temperature calibration disabled\n");
        gDynamicTempCalibrationEnabled = false;
    }
    
    /* Main loop */
    while (1) {
        /* Process both dynamic calibration and UTC time printing */
        
        /* Check if timer event occurred, dynamic calibration is enabled, and no calibration is in progress */
        if (gtimerTriggered && gDynamicTempCalibrationEnabled && !gCalibrationInProgress) {
            uart_printf(UART_0_INST,"Processing dynamic calibration after timer event : 5 minutes elapsed\n");
            dynamicTemperatureCalibration();
            /* Reset the timer flag after processing */
            gtimerTriggered = false;
        }
        
        /* Check if UTC time should be printed */
        if (enableUTC) {
            if (DL_RTC_A_isSafeToRead(RTC_A)) {
                calendarTime = DL_RTC_A_getCalendarTime(RTC_A);
                uart_printf(UART_0_INST,"UTC : %d-%d-%d %d %d:%d:%d\n", calendarTime.year, calendarTime.month, calendarTime.dayOfMonth, calendarTime.dayOfWeek, calendarTime.hours, calendarTime.minutes, calendarTime.seconds);
            }
            enableUTC = false;
        }
        
        /* Wait for interrupt if no events to process */
        if (!gtimerTriggered && !enableUTC) {
            __WFI();
        }
    }
}