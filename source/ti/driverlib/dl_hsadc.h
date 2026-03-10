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
 *  @file       dl_hsadc.h
 *  @brief      High Speed Analog to Digital Converter (HSADC)
 *  @defgroup   HSADC Analog to Digital Converter (HSADC)
 *
 *  @anchor ti_dl_dl_hsadc12_Overview
 *  # Overview
 *
 *  The Analog to Digital Converter Driver Library allows full configuration of
 *  the MSPM33 ADC module.
 *  The ADC is a high-performance successive-approximation-register (SAR)
 *  analog-to-digital converter.
 *
 *  <hr>
 ******************************************************************************/
/** @addtogroup HSADC
* @{
*/
#ifndef ti_dl_dl_hsadc__include
#define ti_dl_dl_hsadc__include

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __MSP_HAS_HSADC__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interrupt pulse position.
 * set interrupt pulse position to 0 for pulse generation to occur when ADC begins conversation + a number of sysclk cycles controlled by DL_HSADC_setEarlyInterruptGenerationDelay
 * set interrupt pulse position to 1 for pulse dgeneration to occur at the end of conversion 1 cycle prior to the ADC result latching into its result register
 */
typedef enum{
    DL_HSADC_INT_PULSE_POS_SOC = 0,
    DL_HSADC_INT_PULSE_POS_EOC = 1
}DL_HSADC_INT_PULSE_POS;


/**
 * @brief ADC clock divider.
 * ADCCLK = (input clock) / divider
 * available divider = 1.0, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5
 */
typedef enum
{
    DL_HSADC_CLOCK_DIVIDE_1_0 = 0U,                //!< ADCCLK = (input clock) / 1.0
    DL_HSADC_CLOCK_DIVIDE_1_5 = 1U,                //!< ADCCLK = (input clock) / 1.5
    DL_HSADC_CLOCK_DIVIDE_2_0 = 2U,                //!< ADCCLK = (input clock) / 2.0
    DL_HSADC_CLOCK_DIVIDE_2_5 = 3U,                //!< ADCCLK = (input clock) / 2.5
    DL_HSADC_CLOCK_DIVIDE_3_0 = 4U,                //!< ADCCLK = (input clock) / 3.0
    DL_HSADC_CLOCK_DIVIDE_3_5 = 5U,                //!< ADCCLK = (input clock) / 3.5
    DL_HSADC_CLOCK_DIVIDE_4_0 = 6U,                //!< ADCCLK = (input clock) / 4.0
    DL_HSADC_CLOCK_DIVIDE_4_5 = 7U,                //!< ADCCLK = (input clock) / 4.5
    DL_HSADC_CLOCK_DIVIDE_5_0 = 8U,                //!< ADCCLK = (input clock) / 5.0
    DL_HSADC_CLOCK_DIVIDE_5_5 = 9U,                //!< ADCCLK = (input clock) / 5.5
    DL_HSADC_CLOCK_DIVIDE_6_0 = 10U,               //!< ADCCLK = (input clock) / 6.0
    DL_HSADC_CLOCK_DIVIDE_6_5 = 11U,               //!< ADCCLK = (input clock) / 6.5
    DL_HSADC_CLOCK_DIVIDE_7_0 = 12U,               //!< ADCCLK = (input clock) / 7.0
    DL_HSADC_CLOCK_DIVIDE_7_5 = 13U,               //!< ADCCLK = (input clock) / 7.5
    DL_HSADC_CLOCK_DIVIDE_8_0 = 14U,               //!< ADCCLK = (input clock) / 8.0
    DL_HSADC_CLOCK_DIVIDE_8_5 = 15U                //!< ADCCLK = (input clock) / 8.5
} DL_HSADC_CLOCK_DIVIDE;

/**
 * @brief Interrupt number.
 * Interrupt 1, Interrupt 2, Interrupt 3, Interrupt 4
 */
typedef enum
{
    DL_HSADC_INT_1   = 0U,                      //Interrupt 1
    DL_HSADC_INT_2   = 1U,                      //Interrupt 2
    DL_HSADC_INT_3   = 2U,                      //Interrupt 3
    DL_HSADC_INT_4   = 3U                       //Interrupt 4
}DL_HSADC_INT;

/**
 * @brief SOC/EOC number.
 */
typedef enum
{
    DL_HSADC_SOC_NUMBER0 = 0U,        //!< SOC/EOC number 0
    DL_HSADC_SOC_NUMBER1 = 1U,        //!< SOC/EOC number 1
    DL_HSADC_SOC_NUMBER2 = 2U,        //!< SOC/EOC number 2
    DL_HSADC_SOC_NUMBER3 = 3U,        //!< SOC/EOC number 3
    DL_HSADC_SOC_NUMBER4 = 4U,        //!< SOC/EOC number 4
    DL_HSADC_SOC_NUMBER5 = 5U,        //!< SOC/EOC number 5
    DL_HSADC_SOC_NUMBER6 = 6U,        //!< SOC/EOC number 6
    DL_HSADC_SOC_NUMBER7 = 7U,        //!< SOC/EOC number 7
    DL_HSADC_SOC_NUMBER8 = 8U,        //!< SOC/EOC number 8
    DL_HSADC_SOC_NUMBER9 = 9U,        //!< SOC/EOC number 9
    DL_HSADC_SOC_NUMBER10 = 10U,      //!< SOC/EOC number 10
    DL_HSADC_SOC_NUMBER11 = 11U,      //!< SOC/EOC number 11
    DL_HSADC_SOC_NUMBER12 = 12U,      //!< SOC/EOC number 12
    DL_HSADC_SOC_NUMBER13 = 13U,      //!< SOC/EOC number 13
    DL_HSADC_SOC_NUMBER14 = 14U,      //!< SOC/EOC number 14
    DL_HSADC_SOC_NUMBER15 = 15U       //!< SOC/EOC number 15
} DL_HSADC_SOC_NUMBER;

/**
 * @brief DMA Interrupt number.
 */
typedef enum
{
    DL_HSADC_DMA_INT_1   = 0U,                      //DMA Interrupt 1 
    DL_HSADC_DMA_INT_2   = 1U,                      //DMA Interrupt 2
    DL_HSADC_DMA_INT_3   = 2U,                      //DMA Interrupt 3
    DL_HSADC_DMA_INT_4   = 3U                       //DMA Interrupt 4
}DL_HSADC_DMA_INT;

/**
 * @brief ADC input number
 */
typedef enum{
    DL_HSADC_ADCIN_0 = 0U,
    DL_HSADC_ADCIN_1 = 1U,
    DL_HSADC_ADCIN_2 = 2U,
    DL_HSADC_ADCIN_3 = 3U,
    DL_HSADC_ADCIN_4 = 4U,
    DL_HSADC_ADCIN_5 = 5U,
    DL_HSADC_ADCIN_6 = 6U,
    DL_HSADC_ADCIN_7 = 7U,
    DL_HSADC_ADCIN_8 = 8U,
    DL_HSADC_ADCIN_9 = 9U,
    DL_HSADC_ADCIN_10 = 10U,
    DL_HSADC_ADCIN_11 = 11U,
    DL_HSADC_ADCIN_12 = 12U,
    DL_HSADC_ADCIN_13 = 13U,
    DL_HSADC_ADCIN_14 = 14U,
    DL_HSADC_ADCIN_15 = 15U,
    DL_HSADC_ADCIN_16 = 16U,
    DL_HSADC_ADCIN_17 = 17U,
    DL_HSADC_ADCIN_18 = 18U,
    DL_HSADC_ADCIN_19 = 19U,
    DL_HSADC_ADCIN_20 = 20U,
    DL_HSADC_ADCIN_21 = 21U,
    DL_HSADC_ADCIN_22 = 22U,
    DL_HSADC_ADCIN_23 = 23U,
    DL_HSADC_ADCIN_24 = 24U,
    DL_HSADC_ADCIN_25 = 25U,
    DL_HSADC_ADCIN_26 = 26U,
    DL_HSADC_ADCIN_27 = 27U,
    DL_HSADC_ADCIN_28 = 28U,
    DL_HSADC_ADCIN_29 = 29U,
    DL_HSADC_ADCIN_30 = 30U,
    DL_HSADC_ADCIN_31 = 31U
} DL_HSADC_ADCIN;

/**
 * @brief SEQ/PPB/EVT number
 */
typedef enum
{
    DL_HSADC_SEQ_NUMBER1 = 0U,        //!< SEQ/PPB/EVT number 1
    DL_HSADC_SEQ_NUMBER2 = 1U,        //!< SEQ/PPB/EVT number 2
    DL_HSADC_SEQ_NUMBER3 = 2U,        //!< SEQ/PPB/EVT number 3
    DL_HSADC_SEQ_NUMBER4 = 3U         //!< SEQ/PPB/EVT number 4
} DL_HSADC_SEQ_NUMBER;

/**
 * @brief Offset trim value
 */
#define OFFSET_TRIM_VALUE(offset) ((offset) >= 0 ? (offset) * 16 : 256 - (-(offset)) * 16)

/**
 * @brief Offset trim
 * Offset can be corrected in the range of +7 to -8 LSBs
 */
typedef enum {
    DL_HSADC_OFFSET_TRIM_NEG_8LSB = OFFSET_TRIM_VALUE(-8),
    DL_HSADC_OFFSET_TRIM_NEG_7LSB = OFFSET_TRIM_VALUE(-7),
    DL_HSADC_OFFSET_TRIM_NEG_6LSB = OFFSET_TRIM_VALUE(-6),
    DL_HSADC_OFFSET_TRIM_NEG_5LSB = OFFSET_TRIM_VALUE(-5),
    DL_HSADC_OFFSET_TRIM_NEG_4LSB = OFFSET_TRIM_VALUE(-4),
    DL_HSADC_OFFSET_TRIM_NEG_3LSB = OFFSET_TRIM_VALUE(-3),
    DL_HSADC_OFFSET_TRIM_NEG_2LSB = OFFSET_TRIM_VALUE(-2),
    DL_HSADC_OFFSET_TRIM_NEG_1LSB = OFFSET_TRIM_VALUE(-1),
    DL_HSADC_OFFSET_TRIM_0LSB = OFFSET_TRIM_VALUE(0),
    DL_HSADC_OFFSET_TRIM_1LSB = OFFSET_TRIM_VALUE(1),
    DL_HSADC_OFFSET_TRIM_2LSB = OFFSET_TRIM_VALUE(2),
    DL_HSADC_OFFSET_TRIM_3LSB = OFFSET_TRIM_VALUE(3),
    DL_HSADC_OFFSET_TRIM_4LSB = OFFSET_TRIM_VALUE(4),
    DL_HSADC_OFFSET_TRIM_5LSB = OFFSET_TRIM_VALUE(5),
    DL_HSADC_OFFSET_TRIM_6LSB = OFFSET_TRIM_VALUE(6),
    DL_HSADC_OFFSET_TRIM_7LSB = OFFSET_TRIM_VALUE(7)
} DL_HSADC_OFFSET_TRIM;

/**
 * @brief Oversampling limit
 */
typedef enum{
    DL_HSADC_OVERSAMPLING_LIMIT_NULL = 0U,
    DL_HSADC_OVERSAMPLING_LIMIT_2 = 1U,
    DL_HSADC_OVERSAMPLING_LIMIT_4 = 2U,
    DL_HSADC_OVERSAMPLING_LIMIT_8 = 3U
} DL_HSADC_OVERSAMPLING_LIMIT;

/**
 * @brief PPB right shift
 */
typedef enum{
    DL_HSADC_PPB_RIGHTSHIFT_0 = 0U,
    DL_HSADC_PPB_RIGHTSHIFT_1 = 1U,
    DL_HSADC_PPB_RIGHTSHIFT_2 = 2U,
    DL_HSADC_PPB_RIGHTSHIFT_3 = 3U,
    DL_HSADC_PPB_RIGHTSHIFT_4 = 4U,
    DL_HSADC_PPB_RIGHTSHIFT_5 = 5U,
    DL_HSADC_PPB_RIGHTSHIFT_6 = 6U,
    DL_HSADC_PPB_RIGHTSHIFT_7 = 7U
} DL_HSADC_PPB_RIGHTSHIFT;

/**
 * @brief Sequence preempt
 */
typedef enum{
    DL_HSADC_PREEMPT_DISABLE = 0,
    DL_HSADC_PREEMPT_NO_RESTART = 2,
    DL_HSADC_PREEMPT_RESTART = 3
} DL_HSADC_PREEMPT;

/**
 * @brief Trigger
 * used to select which trigger is used to initiate a conversion to start once priority is given to it.
 */
typedef enum{
    DL_HSADC_TRIGGER_TIELOW_SW = 0,
    DL_HSADC_TRIGGER_GEN_SUB_0 = 1,
    DL_HSADC_TRIGGER_GEN_SUB_1 = 2,
    DL_HSADC_TRIGGER_GEN_SUB_2 = 3,
    DL_HSADC_TRIGGER_GEN_SUB_3 = 4,
    DL_HSADC_TRIGGER_TIELOW = 7
} DL_HSADC_TRIGGER;

/**
 * @brief sample cap reset select
 * used to select sample cap reset to either vrefhi/2 or vreflo
 */
typedef enum{
    DL_HSADC_SAMPCAPRESET_VREFLO = 0,
    DL_HSADC_SAMPCAPRESET_HALF_VREFHI = 1
} DL_HSADC_SAMPCAPRESET;

/**
 * @brief publisher index
 */
typedef enum {
    DL_HSADC_PUBLISHER_INDEX_0 = 0,
    DL_HSADC_PUBLISHER_INDEX_1 = 1,
    DL_HSADC_PUBLISHER_INDEX_2 = 2,
    DL_HSADC_PUBLISHER_INDEX_3 = 3
} DL_HSADC_PUBLISHER_INDEX;

/**
 * @brief subscriber index
 */
typedef enum {
    DL_HSADC_SUBSCRIBER_INDEX_0 = 0,
    DL_HSADC_SUBSCRIBER_INDEX_1 = 1,
    DL_HSADC_SUBSCRIBER_INDEX_2 = 2,
    DL_HSADC_SUBSCRIBER_INDEX_3 = 3
} DL_HSADC_SUBSCRIBER_INDEX;

/**
 * @brief Step size between SOC CTL registers
 */
#define ADCSOCxCTL_STEP 1U

/**
 * @brief Step size between ADCPPB CONFIG registers
 */
#define ADCPPBxCONFIG_STEP 8U

/**
 * @brief Step size between ADCPPB LIMIT registers
 */
#define ADCPPBxLIMIT_STEP 16U

/**
 * @brief Step size between ADCPPB COUNT registers
 */
#define ADCPPBxPCOUNT_STEP 16U

/**
 * @brief Step size between ADCPPB RESULT registers
 */
#define ADCPPBxCONFIG2_STEP 16U

/**
 * @brief Step size between ADCSEQ CONFIG registers
 */
#define ADCSEQxCONFIG_STEP 1U

/**
 * @brief Step size between ADCSEQ RESULT registers
 */
#define ADCRESULTx_STEP 1U

/**
 * @brief Step size between ADCSEQ STATUS registers
 */
#define ADCPPBxRESULT_STEP 1U

/**
 * @brief Step size between ADCSEQ FIFO registers=
 */
#define ADCPPBxSUM_STEP 8U

/**
 * @brief Step size between ADCPPB COUNT registers
 */
#define ADCPPBxCOUNT_STEP 8U

/**
 * @brief Step size between ADCSEQ FIFO Result registers
 */
#define ADCSEQxFIFORESULT_STEP 1U

/**
 * @brief Step size between ADCSEQ FIFO Status registers
 */
#define ADCSEQxFIFOSTATUS_STEP 1U

/**
 * @brief maximum value of sample window
 */
#define maxSampleWindow 1472

/**
 * @brief minimum value of sample window 
 */
#define minSampleWindow 1

/**
 * @brief maximum value of delay for early interrupt generation
 */
#define maxDelayNumCycles 63

/**
 * @brief Asserts expression
 */
#define ASSERT(expr) do { if (!(expr)) { while (1); } } while (0)

/**
 * @brief Enables power in gprcm for hsadc module
 * 
 * @param adc is the base address of the ADC module.
 */
__STATIC_INLINE void DL_HSADC_enablePower(hsadc_ADC_LITE_REGS_Regs *adc)
{
    adc->GPRCM.PWREN = (HSADC_PWREN_KEY_UNLOCK_W | HSADC_PWREN_ENABLE_ENABLE);
}

/**
 * @brief Disables power in gprcm for hsadc module
 * 
 * @param adc is the base address of the ADC module.
 */
__STATIC_INLINE void DL_HSADC_disablePower(hsadc_ADC_LITE_REGS_Regs *adc)
{
    adc->GPRCM.PWREN = (HSADC_PWREN_KEY_UNLOCK_W | HSADC_PWREN_ENABLE_DISABLE);
}

/**
 * @brief Returns if power is enabled or not
 * 
 * @param adc is the base address of the ADC module.
 * @return true: if power is enabled, false: if power is disabled 
 */
__STATIC_INLINE bool DL_HSADC_isPowerEnabled(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return((adc->GPRCM.PWREN & HSADC_PWREN_ENABLE_MASK) == HSADC_PWREN_ENABLE_ENABLE);
}

/**
 * @brief Asserts reset to the hasdc peripheral
 * 
 * @param adc is the base address of the ADC module.
 */
__STATIC_INLINE void DL_HSADC_reset(hsadc_ADC_LITE_REGS_Regs *adc)
{
    adc->GPRCM.RSTCTL = (HSADC_RSTCTL_RESETASSERT_ASSERT | HSADC_RSTCTL_RESETSTKYCLR_CLR | HSADC_RSTCTL_KEY_UNLOCK_W);
}

/**
 * @brief Returns if hsadc peripheral was reset
 * 
 * @param adc is the base address of the ADC module.
 * @return true: if peripheral was reset, false: if peripheral was not reset
 */
__STATIC_INLINE bool DL_HSADC_isReset(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return ((adc->GPRCM.STAT & HSADC_STAT_RESETSTKYCLR_MASK) == HSADC_STAT_RESETSTKYCLR_RESET);
}

/**
 *  @brief Sets the event publisher channel id
 *
 *  @param  adc is the base address of the ADC module.
 *  @param  index   Specifies the register event index to be configured
 *  @param  chanID  Channel ID number. Valid range 0-15. If ChanID == 0
 *                      publisher is disconnected.
 */
__STATIC_INLINE void DL_HSADC_setPublisherChanID(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_PUBLISHER_INDEX index, uint8_t chanID)
{
    volatile uint32_t *pReg = &adc->FPUB_0;

    *(pReg + (uint32_t) index) = (chanID & HSADC_FPUB_1_CHANID_MAXIMUM);
}

/**
 *  @brief Gets the event publisher channel id
 *
 *  @param  adc is the base address of the ADC module.
 *  @param  index   Specifies the register event index to be configured
 *
 *  @return     Event publisher channel ID
 */
__STATIC_INLINE uint8_t DL_HSADC_getPublisherChanID(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_PUBLISHER_INDEX index)
{
    volatile uint32_t *pReg = &adc->FPUB_0;

    return ((uint8_t)(*(pReg + (uint32_t) index) & HSADC_FPUB_1_CHANID_MASK));
}

/**
 * @brief Sets the event subscriber channel id
 * 
 * @param adc is the base address of the ADC module.
 * @param index is the index of Subscriber.
 * @param chanID  Channel ID number. Valid range 0-15. If ChanID == 0
 *                      subscriber is disconnected.
 */
__STATIC_INLINE void DL_HSADC_setSubscriberChanID(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SUBSCRIBER_INDEX index, uint8_t chanID)
{
    volatile uint32_t *pReg = &adc->FSUB_0;

    *(pReg + (uint32_t) index) = (chanID & HSADC_FSUB_0_CHANID_MAXIMUM);
}

/**
 * @brief Gets the event subscriber channel id
 * 
 * @param adc is the base address of the ADC module.
 * @param index is the index of Subscriber.
 * @return Event subscriber channel ID
 */
__STATIC_INLINE uint8_t DL_HSADC_getSubscriberChanID(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SUBSCRIBER_INDEX index)
{
    volatile uint32_t *pReg = &adc->FSUB_0;

    return ((uint8_t)(*(pReg + (uint32_t) index) & HSADC_FSUB_0_CHANID_MASK));
}

/**
 * @brief Returns if ADC is busy
 * 
 * @param adc is the base address of the ADC module.
 * @return true: if ADC is busy, false: if ADC is available to sample next channel
 */
__STATIC_INLINE bool DL_HSADC_isBusy(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return ((adc->ADC_LITE_REGS.ADCCTL1 & HSADC_ADCCTL1_ADCBSY_MASK) == HSADC_ADCCTL1_ADCBSY_MASK);
}

/**
 * @brief returns SOC number of last converted SOC when SOC is free, returns SOC number of current SOC being converted when SOC is busy
 * 
 * @param adc is the base address of the ADC module.
 * @return SOC number of the last busy or current channel (4 bit value)
 */
__STATIC_INLINE uint32_t DL_HSADC_lastBusyChannel(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return (((adc->ADC_LITE_REGS.ADCCTL1) & (HSADC_ADCCTL1_ADCBSYCHN_MASK)) >> HSADC_ADCCTL1_ADCBSYCHN_OFS);
}

/**
 * @brief turns on power of all the analog circuitry inside the analog core
 * 
 * @param adc is the base address of the ADC module.
 */
__STATIC_INLINE void DL_HSADC_PowerUp(hsadc_ADC_LITE_REGS_Regs *adc)
{
    adc->ADC_LITE_REGS.ADCCTL1 |= HSADC_ADCCTL1_ADCPWDNZ_MASK;
}

/**
 * @brief turns off power of all the analog circuitry inside the analog core
 * 
 * @param adc is the base address of the ADC module.
 */
__STATIC_INLINE void DL_HSADC_PowerDown(hsadc_ADC_LITE_REGS_Regs *adc)
{
    adc->ADC_LITE_REGS.ADCCTL1 &= ~(HSADC_ADCCTL1_ADCPWDNZ_MASK);
}
/**
 * @brief set interrupt pulse position
 *  set interrupt pulse position to 0 for pulse generation to occur when ADC begins conversation + a number of sysclk cycles controlled by DL_HSADC_setEarlyInterruptGenerationDelay
 * set interrupt pulse position to 1 for pulse dgeneration to occur at the end of conversion 1 cycle prior to the ADC result latching into its result register
 * 
 * @param adc is the base address of the ADC module.
 * @param pulsePosition is the pulse position you want to set, can be 0 or 1
 */
__STATIC_INLINE void DL_HSADC_SetInterruptPulsePosition(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT_PULSE_POS pulsePosition)
{
    adc->ADC_LITE_REGS.ADCCTL1 = (adc->ADC_LITE_REGS.ADCCTL1 & ~(HSADC_ADCCTL1_INTPULSEPOS_MASK)) |(pulsePosition << HSADC_ADCCTL1_INTPULSEPOS_OFS);
}

/**
 * @brief sets clock prescaler value or clock divide ratio value
 * 
 * @param adc is the base address of the ADC module.
 * @param divideRatio is the valid clock prescaler value you want to set
 */
__STATIC_INLINE void DL_HSADC_setClockDivideRatio(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_CLOCK_DIVIDE divideRatio)
{
    adc->ADC_LITE_REGS.ADCCTL2 = (adc->ADC_LITE_REGS.ADCCTL2 & ~(HSADC_ADCCTL2_PRESCALE_MASK)) | divideRatio;
}
 
/**
 * @brief enable continue to interrupt mode. ADCINT pulse are generated whenever an EOC pulse is generated irrespective of whetehr the flag bit is cleared or not.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_enableContinueToInterruptMode(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTSEL |= HSADC_ADCINTSEL_INT1CONT_MASK << (interrupt * (HSADC_ADCINTSEL_INT2CONT_OFS - HSADC_ADCINTSEL_INT1CONT_OFS));
}

/**
 * @brief disable continue to interrupt mode. no further ADCINT pulse are generated until the ADCINT flag in cleared by user with DL_HSADC_InterruptStatusClear().
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_disableContinueToInterruptMode(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTSEL &= ~(HSADC_ADCINTSEL_INT1CONT_MASK << (interrupt * (HSADC_ADCINTSEL_INT2CONT_OFS - HSADC_ADCINTSEL_INT1CONT_OFS)));
}

/**
 * @brief enables ADCINT Interrupt
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_enableInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTSEL |= HSADC_ADCINTSEL_INT1E_MASK << (interrupt * HSADC_ADCINTSEL_INT2SEL_OFS);
}

/**
 * @brief disable ADCINT Interrupt
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 */
__STATIC_INLINE void DL_HSADC_disableInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTSEL &= ~(HSADC_ADCINTSEL_INT1E_MASK << (interrupt * HSADC_ADCINTSEL_INT2SEL_OFS));
}

/**
 * @brief Returns if interrupt is enabled
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 * @return true: if interrupt is enabled , false: if interrupt is disabled 
 */
__STATIC_INLINE bool DL_HSADC_checkInterruptEnable(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCINTSEL >> (interrupt * HSADC_ADCINTSEL_INT2SEL_OFS)) & HSADC_ADCINTSEL_INT1E_MASK)  == HSADC_ADCINTSEL_INT1E_MASK);
}

/**
 * @brief selects which EOC to be selected as a source for the interrupt
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @param eoc is the EOC number to be selected as a source for interrupt
 */
__STATIC_INLINE void DL_HSADC_InterruptSourceSelect(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt, DL_HSADC_SOC_NUMBER eoc)
{
    adc->ADC_LITE_REGS.ADCINTSEL = (adc->ADC_LITE_REGS.ADCINTSEL & ~(HSADC_ADCINTSEL_INT1SEL_MASK << (interrupt * HSADC_ADCINTSEL_INT2SEL_OFS))) | (eoc << (interrupt * HSADC_ADCINTSEL_INT2SEL_OFS));
}

/**
 * @brief enable DMA continue to interrupt mode. ADCINT pulse are generated whenever an EOC pulse is generated irrespective of whetehr the flag bit is cleared or not.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_enableDMAContinueToInterruptMode(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCDMAINTSEL |= HSADC_ADCDMAINTSEL_DMAINT1CONT_MASK << (interrupt * HSADC_ADCDMAINTSEL_DMAINT2SEL_OFS);
}

/**
 * @brief disable DMA continue to interrupt mode. no further ADCINT pulse are generated until the ADCINT flag in cleared by user with DL_HSADC_DMAInterruptStatusClear().
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_disableDMAContinueToInterruptMode(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCDMAINTSEL &= ~(HSADC_ADCDMAINTSEL_DMAINT1CONT_MASK << (interrupt * HSADC_ADCDMAINTSEL_DMAINT2SEL_OFS));
}

/**
 * @brief enables ADCINT DMA Interrupt
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_enableDMAInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCDMAINTSEL |= HSADC_ADCDMAINTSEL_DMAINT1E_MASK << (interrupt * HSADC_ADCDMAINTSEL_DMAINT2SEL_OFS);
}

/**
 * @brief disables ADCINT DMA Interrupt
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used
 */
__STATIC_INLINE void DL_HSADC_disableDMAInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCDMAINTSEL &= ~(HSADC_ADCDMAINTSEL_DMAINT1E_MASK << (interrupt * HSADC_ADCDMAINTSEL_DMAINT2SEL_OFS));
}

/**
 * @brief selects which EOC to be selected as a source for the interrupt
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @param eoc is the EOC number to be selected as a source for interrupt
 */
__STATIC_INLINE void DL_HSADC_DMAInterruptSourceSelect(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt, DL_HSADC_SOC_NUMBER eoc)
{
    adc->ADC_LITE_REGS.ADCDMAINTSEL = (adc->ADC_LITE_REGS.ADCDMAINTSEL & ~(HSADC_ADCDMAINTSEL_DMAINT1SEL_MASK << (interrupt * HSADC_ADCDMAINTSEL_DMAINT2SEL_OFS))) | (eoc << (interrupt * HSADC_ADCDMAINTSEL_DMAINT2SEL_OFS));
}

/**
 * @brief Returns any ADC Interrupt Flag Status. 
 * 
 * Reading these flags indicates if the associated INT condition occurred. This flag will be set irrespective of corresponding interrupt enable settings.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @return true: Selected EOC/OSINT event occured, false: Selected EOC/OSINT event did not occur
 */
__STATIC_INLINE bool DL_HSADC_getRawInterruptStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCRAWINTFLG >> interrupt) & HSADC_ADCRAWINTFLG_ADCRAWINT1_MASK) == HSADC_ADCRAWINTFLG_ADCRAWINT1_MASK);
}

 /**
 * @brief Returns any ADC DMA Interrupt Flag Status. 
 * 
 * Reading these flags indicates if the associated ADC DMA INT condition occurred. This flag will be set irrespective of corresponding DMA interrupt enable settings.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @return true: Selected EOC/OSINT event occured, false: Selected EOC/OSINT event did not occur
 */
__STATIC_INLINE bool DL_HSADC_getRawDMAInterruptStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCRAWINTFLG >> interrupt) & HSADC_ADCRAWINTFLG_ADCDMARAWINT1_MASK) == HSADC_ADCRAWINTFLG_ADCDMARAWINT1_MASK);
}

/**
 * @brief Returns Interrupt Flag Status indicating if the associated ADCINT pulse was generated since the last clear or not.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @return true: ADCINT pulse generated, false: ADCINT pulse not generated
 */
__STATIC_INLINE bool DL_HSADC_getInterruptStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCINTFLG >> interrupt) & HSADC_ADCINTFLG_ADCINT1_MASK) == HSADC_ADCINTFLG_ADCINT1_MASK);
}

/**
 * @brief Returns Interrupt Result Ready Flag Status, which is set when the conversions results associated with ADCINT latch into the corresponding results register. This flag can be used in an ISR that is entered in early interrupt mode to ensure that the corresponding results are ready before proceeding to read the result register
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @return true: Conversion results have latched, false: Conversion results have not latched 
 */
__STATIC_INLINE bool DL_HSADC_getInterruptResultReadyStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCINTFLG >> interrupt) & HSADC_ADCINTFLG_ADCINT1RESULT_MASK) == HSADC_ADCINTFLG_ADCINT1RESULT_MASK);
}

/**
 * @brief Returns ADC DMA Interrupt 3 Flag status. Reading these flags indicates if the associated ADCDMAINT pulse was generated since the last clear.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 * @return true: ADCDMAINT pulse generated, false: ADCDMAINT pulse not generated 
 */
__STATIC_INLINE bool DL_HSADC_getDMAInterruptStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCINTFLG >> interrupt) & HSADC_ADCINTFLG_ADCDMAINT1_MASK) == HSADC_ADCINTFLG_ADCDMAINT1_MASK);
}

/**
 * @brief sets ADCINT flags status in the adc interrupt flag and adc raw interrupt flag registers
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 */
__STATIC_INLINE void DL_HSADC_InterruptStatusForce(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTFLGFRC |= (HSADC_ADCINTFLGFRC_ADCINT1_MASK << interrupt);
}

/**
 * @brief sets ADCDMAINT flags status in the adc interrupt flag and adc raw interrupt flag registers
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used  
 */
__STATIC_INLINE void DL_HSADC_DMAInterruptStatusForce(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTFLGFRC |= (HSADC_ADCINTFLGFRC_ADCINT1_MASK << (interrupt + HSADC_ADCINTFLGFRC_ADCDMAINT1_OFS));
}

/**
 * @brief clears ADCINT flags status in the adc interrupt flag and adc raw interrupt flag registers. If software sets the clear bit on the same cycle that hardware is trying to set the flag it, then hardware has priority and the overflow bit will not be set.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used 
 */
__STATIC_INLINE void DL_HSADC_InterruptStatusClear(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTFLGCLR |= (HSADC_ADCINTFLGCLR_ADCINT1_MASK << interrupt);
}

/**
 * @brief clears ADCDMAINT flags status in the adc interrupt flag and adc raw interrupt flag registers. If software sets the clear bit on the same cycle that hardware is trying to set the flag it, then hardware has priority and the overflow bit will not be set.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used  
 */
__STATIC_INLINE void DL_HSADC_DMAInterruptStatusClear(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTFLGCLR |= (HSADC_ADCINTFLGCLR_ADCINT1_MASK << (interrupt + HSADC_ADCINTFLGCLR_ADCDMAINT1_OFS));
}

/**
 * @brief Returns Interrupt Overflow Status. flag is set, if an overflow occurred when generating ADCINT pulses. If the prespective ADCINTFLG bit is set and a selected additional EOC trigger is generated, then an overflow condition occurs.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used  
 * @return true: if ADC interrupt overflow event detected ,
 * false: if No ADC interrupt overflow event detected 
 */
__STATIC_INLINE bool DL_HSADC_InterruptOverflowStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCINTOVF >> interrupt) & HSADC_ADCINTOVF_ADCINT1OVF_MASK) == HSADC_ADCINTOVF_ADCINT1OVF_MASK);
}

/**
 * @brief Returns DMA Interrupt Overflow Status. flag is set, if an overflow occurred when generating ADCINT pulses. If the prespective ADCINTFLG bit is set and a selected additional EOC trigger is generated, then an overflow condition occurs.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used  
 * @return true: if ADC DMA interrupt overflow event detected ,
 * false: if No ADC DMA interrupt overflow event detected 
 */
__STATIC_INLINE bool DL_HSADC_DMAInterruptOverflowStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    return (((adc->ADC_LITE_REGS.ADCINTOVF >>interrupt) & HSADC_ADCINTOVF_ADCDMAINT1OVF_MASK) == HSADC_ADCINTOVF_ADCDMAINT1OVF_MASK);
}

/**
 * @brief clears the respective overflow bit in the adc interrupt overflow register. If software tries to set this bit on the same clock cycle that hardware tries to set the overflow bit in the register, then hardware has the priority and the register will be set.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used  
 */
__STATIC_INLINE void DL_HSADC_InterruptOverflowStatusClear(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTOVFCLR |= (HSADC_ADCINTOVFCLR_ADCINT1OVF_MASK << interrupt);
}

/**
 * @brief clears the respective overflow bit in the adc interrupt overflow register. If software tries to set this bit on the same clock cycle that hardware tries to set the overflow bit in the register, then hardware has the priority and the register will be set.
 * 
 * @param adc is the base address of the ADC module.
 * @param interrupt is the interrupt number to be used  
 */
__STATIC_INLINE void DL_HSADC_DMAInterruptOverflowStatusClear(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_DMA_INT interrupt)
{
    adc->ADC_LITE_REGS.ADCINTOVFCLR |= (HSADC_ADCINTOVFCLR_ADCINT1OVF_MASK << (interrupt + HSADC_ADCINTOVFCLR_ADCDMAINT1OVF_OFS));
}

/**
 * @brief Returns Start of Conversion Status. Flag is set, if Trigger has been received and sample is pending for SOC{#}. Flag will be automatically cleared when the SOC{#} conversion is started. {#} is the soc number.
 * 
 * @param adc is the base address of the ADC module.
 * @param soc is the SOC number
 * @return true: if No sample pending for SOC, false: if Trigger has been received and sample is pending for SOC 
 */
__STATIC_INLINE bool DL_HSADC_getStartOfConversationStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER soc)
{
    return (((adc->ADC_LITE_REGS.ADCSOCFLG1 >> soc) & HSADC_ADCSOCFLG1_SOC0_MASK) == HSADC_ADCSOCFLG1_SOC0_MASK);
}

/**
 * @brief Returns Start of Conversion Overflow Status. Flag is set, if an SOC{#} event was generated in hardware while an existing SOC{#} event was already penidng. {#} is the soc number.
 * 
 * @param adc is the base address of the ADC module.
 * @param soc is the SOC number
 * @return true: if No soc event overflow, false: if SOC event overflow
 */
__STATIC_INLINE bool DL_HSADC_getStartOfConversationOverflowStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER soc)
{
    return (((adc->ADC_LITE_REGS.ADCSOCOVF1 >> soc) & HSADC_ADCSOCOVF1_SOC0OVF_MASK) == HSADC_ADCSOCOVF1_SOC0OVF_MASK);
}

/**
 * @brief clears the SOC{#} overflow flag in the SOC{#} overflow register. If software tries to set this bit on the same clock cycle that hardware tries to set the overflow bit in the register, then hardware has the priority and the register will be set.
 * 
 * @param adc is the base address of the ADC module.
 * @param soc is the SOC number
 */
__STATIC_INLINE void DL_HSADC_clearStartOfConversationOverflowStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER soc)
{
    adc->ADC_LITE_REGS.ADCSOCOVFCLR1 |= HSADC_ADCSOCOVFCLR1_SOC0OVF_MASK << soc;
}

/**
 * @brief Selects the channel to be converted when SOC{#} event is recieved by the ADC. {#} is the soc number.
 * 
 * @param adc is the base address of the ADC module.
 * @param soc is the SOC number
 * @param adcin is the channel number
 */
__STATIC_INLINE void DL_HSADC_SOCChannelSelect(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER soc, DL_HSADC_ADCIN adcin )
{
    uint32_t *ctlRegAddr;
    ctlRegAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSOC0CTL + soc * ADCSOCxCTL_STEP);
    *ctlRegAddr = (*ctlRegAddr & ~HSADC_ADCSOC0CTL_CHSEL_MASK) | (adcin << HSADC_ADCSOC0CTL_CHSEL_OFS);
}

/**
 * @brief Enables SOC{#} thrashold comparator. {#} is the soc number
 * 
 * @param adc is the base address of the ADC module.
 * @param soc is the SOC number
 */
__STATIC_INLINE void DL_HSADC_enableSOCComparator(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER soc)
{
    uint32_t *ctlRegAddr;
    ctlRegAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSOC0CTL + soc * ADCSOCxCTL_STEP);
    *ctlRegAddr |= HSADC_ADCSOC0CTL_COMPEN_MASK;
}

/**
 * @brief Disables SOC{#} thrashold comparator. {#} is the soc number
 * 
 * @param adc is the base address of the ADC module.
 * @param soc is the SOC number
 */
__STATIC_INLINE void DL_HSADC_disableSOCComparator(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER soc)
{
    uint32_t *ctlRegAddr;
    ctlRegAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSOC0CTL + soc * ADCSOCxCTL_STEP);
    *ctlRegAddr &= ~HSADC_ADCSOC0CTL_COMPEN_MASK;
}

/**
 * @brief Returns PPB TRIP high status.Flag is set, if the digital compare trip high event has occurred  in PPB{#}. {#} is the ppb number
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @return true: if trip high event occurred, false: if trip high event not occurred 
 */
__STATIC_INLINE bool DL_HSADC_getPPBTripHighStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    return (((adc->ADC_LITE_REGS.ADCEVTSTAT >> ppbNumber * HSADC_ADCEVTSTAT_PPB2TRIPHI_OFS) & HSADC_ADCEVTSTAT_PPB1TRIPHI_MASK) == HSADC_ADCEVTSTAT_PPB1TRIPHI_MASK);
}

/**
 * @brief Returns PPB TRIP low status. Flag is set, if the digital compare trip low event has occurred  in PPB{#}. {#} is the ppb number
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @return true: if trip low event occurred, false: if trip low event not occurred 
 */
__STATIC_INLINE bool DL_HSADC_getPPBTripLowStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    return (((adc->ADC_LITE_REGS.ADCEVTSTAT >> ppbNumber * HSADC_ADCEVTSTAT_PPB2TRIPHI_OFS) & HSADC_ADCEVTSTAT_PPB1TRIPLO_MASK) == HSADC_ADCEVTSTAT_PPB1TRIPLO_MASK);
}

/**
 * @brief Returns PPB TRIP zero status. Flag is set, if the digital compare trip zero event has occurred  in PPB{#}. {#} is the ppb number
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @return true: if trip zero event occurred, false: if trip zero event not occurred 
 */
__STATIC_INLINE bool DL_HSADC_getPPBTripZeroStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    return (((adc->ADC_LITE_REGS.ADCEVTSTAT >> ppbNumber * HSADC_ADCEVTSTAT_PPB2TRIPHI_OFS) & HSADC_ADCEVTSTAT_PPB1ZERO_MASK) == HSADC_ADCEVTSTAT_PPB1ZERO_MASK);
}

/**
 * @brief Returns PPB TRIP limit status. Flag is set, if the digital compare trip limit event has occurred in PPB{#}. {#} is the ppb number
 * 
 * @param adc is the base address of the ADC module. 
 * @param ppbNumber is the PPB number 
 * @return true: if trip limit event occurred, false: if trip limit event not occurred 
 */
__STATIC_INLINE bool DL_HSADC_getPPBTripInLimitStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    return (((adc->ADC_LITE_REGS.ADCEVTSTAT >> ppbNumber * HSADC_ADCEVTSTAT_PPB2TRIPHI_OFS) & HSADC_ADCEVTSTAT_PPB1INLIMIT_MASK) == HSADC_ADCEVTSTAT_PPB1INLIMIT_MASK);
}

/**
 * @ brief Clears the corresponding trip high flag in the ADCEVTSTAT register.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_clearPPBTripHighStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTCLR |= (uint32_t)(HSADC_ADCEVTCLR_PPB1TRIPHI_MASK<<(ppbNumber * (HSADC_ADCEVTCLR_PPB2TRIPHI_OFS - HSADC_ADCEVTCLR_PPB1TRIPHI_OFS)));
}

/**
 * @brief Clears the corresponding trip low flag in the ADCEVTSTAT register.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_clearPPBTripLowStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTCLR |= (uint32_t)(HSADC_ADCEVTCLR_PPB1TRIPLO_MASK<<(ppbNumber * (HSADC_ADCEVTCLR_PPB2TRIPLO_OFS - HSADC_ADCEVTCLR_PPB1TRIPLO_OFS)));
}

/**
 * @brief Clears the corresponding trip zero flag in the ADCEVTSTAT register.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_clearPPBTripZeroStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTCLR |= (uint32_t)(HSADC_ADCEVTCLR_PPB1ZERO_MASK<<(ppbNumber * (HSADC_ADCEVTCLR_PPB2ZERO_OFS - HSADC_ADCEVTCLR_PPB1ZERO_OFS)));
}

/**
 * @brief Clears the corresponding trip limit flag in the ADCEVTSTAT register.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_clearPPBTripInLimitStatus(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTCLR |= (uint32_t)(HSADC_ADCEVTCLR_PPB1INLIMIT_MASK<<(ppbNumber * (HSADC_ADCEVTCLR_PPB2INLIMIT_OFS - HSADC_ADCEVTCLR_PPB1INLIMIT_OFS)));
}

/**
 * @brief  Allows the corresponding rising trip high flag to activate the event signal to the PWM blocks. The flag must be cleared before it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripHighEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL |= (uint32_t)(HSADC_ADCEVTSEL_PPB1TRIPHI_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2TRIPHI_OFS - HSADC_ADCEVTSEL_PPB1TRIPHI_OFS)));
}

/**
 * @brief Allows the corresponding rising trip low flag to activate the event signal to the PWM blocks. The flag must be cleared before it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripLowEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL |= (uint32_t)(HSADC_ADCEVTSEL_PPB1TRIPLO_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2TRIPLO_OFS - HSADC_ADCEVTSEL_PPB1TRIPLO_OFS)));
}

/**
 * @brief Allows the corresponding rising trip zero flag to activate the event signal to the PWM blocks. The flag must be cleared before it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripZeroEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL |= (uint32_t)(HSADC_ADCEVTSEL_PPB1ZERO_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2ZERO_OFS - HSADC_ADCEVTSEL_PPB1ZERO_OFS)));
}

/**
 * @brief Allows the corresponding rising trip limit flag to activate the event signal to the PWM blocks. The flag must be cleared before it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripInLimitEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL |= (uint32_t)(HSADC_ADCEVTSEL_PPB1INLIMIT_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2INLIMIT_OFS - HSADC_ADCEVTSEL_PPB1INLIMIT_OFS)));
}

/**
 * @brief Clears the trip high event. After this, it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripHighEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL &= ~(uint32_t)(HSADC_ADCEVTSEL_PPB1TRIPHI_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2TRIPHI_OFS- HSADC_ADCEVTSEL_PPB1TRIPHI_OFS)));
}

/**
 * @brief  Clears the trip low event. After this, it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripLowEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL &= ~(uint32_t)(HSADC_ADCEVTSEL_PPB1TRIPLO_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2TRIPLO_OFS- HSADC_ADCEVTSEL_PPB1TRIPLO_OFS)));
}

/**
 * @brief Clears the trip zero event. After this, it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripZeroEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL &= ~(uint32_t)(HSADC_ADCEVTSEL_PPB1ZERO_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2ZERO_OFS- HSADC_ADCEVTSEL_PPB1ZERO_OFS)));
}

/**
 * @brief Clears the trip limit event. After this, it can produce additional events to the PWM blocks.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripInLimitEvent(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTSEL &= ~(uint32_t)(HSADC_ADCEVTSEL_PPB1INLIMIT_MASK<<(ppbNumber * (HSADC_ADCEVTSEL_PPB2INLIMIT_OFS- HSADC_ADCEVTSEL_PPB1INLIMIT_OFS)));
}

/**
 * @brief Allows the corresponding rising trip high flag to activate the event interrupt signal to the PIE. The flag must be cleared before it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripHighInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL |= (uint32_t)(HSADC_ADCEVTINTSEL_PPB1TRIPHI_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2TRIPHI_OFS- HSADC_ADCEVTINTSEL_PPB1TRIPHI_OFS)));
}

/**
 * @brief Allows the corresponding rising trip low flag to activate the event interrupt signal to the PIE. The flag must be cleared before it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripLowInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL |= (uint32_t)(HSADC_ADCEVTINTSEL_PPB1TRIPLO_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2TRIPLO_OFS- HSADC_ADCEVTINTSEL_PPB1TRIPLO_OFS)));
}

/**
 * @brief Allows the corresponding rising trip zero flag to activate the event interrupt signal to the PIE. The flag must be cleared before it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripZeroInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL |= (uint32_t)(HSADC_ADCEVTINTSEL_PPB1ZERO_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2ZERO_OFS- HSADC_ADCEVTINTSEL_PPB1ZERO_OFS)));
}

/**
 * @brief Allows the corresponding rising trip limit flag to activate the event interrupt signal to the PIE. The flag must be cleared before it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBTripInLimitInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL |= (uint32_t)(HSADC_ADCEVTINTSEL_PPB1INLIMIT_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2INLIMIT_OFS- HSADC_ADCEVTINTSEL_PPB1INLIMIT_OFS)));
}

/**
 * @brief Clears the trip high event. After this, it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripHighInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL &= ~(uint32_t)(HSADC_ADCEVTINTSEL_PPB1TRIPHI_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2TRIPHI_OFS- HSADC_ADCEVTINTSEL_PPB1TRIPHI_OFS)));
}

/**
 * @brief Clears the trip low event. After this, it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripLowInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL &= ~(uint32_t)(HSADC_ADCEVTINTSEL_PPB1TRIPLO_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2TRIPLO_OFS- HSADC_ADCEVTINTSEL_PPB1TRIPLO_OFS)));
}

/**
 * @brief Clears the trip zero event. After this, it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripZeroInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL &= ~(uint32_t)(HSADC_ADCEVTINTSEL_PPB1ZERO_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2ZERO_OFS- HSADC_ADCEVTINTSEL_PPB1ZERO_OFS)));
}

/**
 * @brief Clears the trip limit event. After this, it can produce additional interrupts to the PIE.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number 
 */
__STATIC_INLINE void DL_HSADC_disablePPBTripInLimitInterrupt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    adc->ADC_LITE_REGS.ADCEVTINTSEL &= ~(uint32_t)(HSADC_ADCEVTINTSEL_PPB1INLIMIT_MASK<<(ppbNumber * (HSADC_ADCEVTINTSEL_PPB2INLIMIT_OFS- HSADC_ADCEVTINTSEL_PPB1INLIMIT_OFS)));
}

/**
 * @brief returns ADC Type. Always set to 6 for HSADC-12b.
 * 
 * @param adc is the base address of the ADC module.
 * @return ADC Type number (8 bit value)
 */
__STATIC_INLINE uint32_t DL_HSADC_getADCType(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return (adc->ADC_LITE_REGS.ADCREV & HSADC_ADCREV_TYPE_MASK);
}

/**
 * @brief returns ADC Revision. To allow documentation of differences between revisions. First version is labeled as 00h.
 * 
 * @param adc is the base address of the ADC module.
 * @return ADC Revision number (24 bit value) 
 */
__STATIC_INLINE uint32_t DL_HSADC_getADCRevision(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return (uint32_t) (((adc->ADC_LITE_REGS.ADCREV & HSADC_ADCREV_REV_MASK) >> HSADC_ADCREV_REV_OFS));
}

/**
 * @brief sets ADC Offset Trim
 * 
 * @param adc is the base address of the ADC module.
 * @param offsetTrim is the offset trim
 */
__STATIC_INLINE void DL_HSADC_setOffsetTrim(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_OFFSET_TRIM offsetTrim)
{
    adc->ADC_LITE_REGS.ADCOFFTRIM = (adc->ADC_LITE_REGS.ADCOFFTRIM & ~(HSADC_ADCOFFTRIM_OFFTRIM_MASK)) | offsetTrim;
}

/**
 * @brief enables the post conversion hardware processing circuit to automatically clear the ADCEVTSTAT on a conversion if the event condition is no longer present.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_enablePPBCycleByCycle(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* PPBConfigAddr;
    PPBConfigAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1CONFIG + ppbNumber * ADCPPBxCONFIG_STEP);
    *PPBConfigAddr |= HSADC_ADCPPB1CONFIG_CBCEN_MASK;
}

/**
 * @brief disables the post conversion hardware processing circuit to automatically clear the ADCEVTSTAT on a conversion if the event condition is no longer present.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_disablePPBCycleByCycle(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* PPBConfigAddr;
    PPBConfigAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1CONFIG + ppbNumber * ADCPPBxCONFIG_STEP);
    *PPBConfigAddr &= ~HSADC_ADCPPB1CONFIG_CBCEN_MASK;
}

/**
 * @brief sets the digital comparator trip high limit. When comparing to an ADCPPBxRESULT register, the upper bits will be ignored:
 * TRIPHI[15:13] will be ignored in 12 bit mode
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @param highLimit is the high limit
 */
__STATIC_INLINE void DL_HSADC_setPPBTripHighLimit(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber, uint16_t highLimit)
{
    uint32_t* PPBConfigAddr;
    PPBConfigAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1TRIPHI + ppbNumber * ADCPPBxCONFIG_STEP);
    *PPBConfigAddr = (*PPBConfigAddr & ~(HSADC_ADCPPB1TRIPHI_LIMITHI_MASK)) | highLimit;
}

/**
 * @brief sets the digital comparator trip low limit if ADCPPB{#}TRIPLO.LIMITLO2EN = 1. When comparing to an ADCPPBxRESULT register, the upper bits will be ignored:
 * TRIPLO[15:13] will be ignored in 12 bit mode
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @param lowLimit is the low limit
 */
__STATIC_INLINE void DL_HSADC_setPPBTripLowLimit(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber, uint16_t lowLimit)
{
    uint32_t* PPBConfigAddr;
    PPBConfigAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1TRIPLO + ppbNumber * ADCPPBxCONFIG_STEP);
    *PPBConfigAddr = (*PPBConfigAddr & ~(HSADC_ADCPPB1TRIPLO_LIMITLO_MASK)) | lowLimit;
}

/**
 * @brief Defines the delay from the fall edge of ADCSOC in terms of system clock cycles, for the interrupt to be generated.
 * 
 * @param adc is the base address of the ADC module.
 * @param delayNumCycles is the delay in terms of system clock cycles
 */
__STATIC_INLINE void DL_HSADC_setEarlyInterruptGenerationDelay(hsadc_ADC_LITE_REGS_Regs *adc, uint8_t delayNumCycles)
{
    ASSERT(delayNumCycles <= maxDelayNumCycles);
    adc->ADC_LITE_REGS.ADCINTCYCLE = (adc->ADC_LITE_REGS.ADCINTCYCLE & ~(HSADC_ADCINTCYCLE_DELAY_MASK)) | delayNumCycles;
}

/**
 * @brief returns the ADC wrapper type. Always set to 6 for this ADC
 * 
 * @param adc is the base address of the ADC module.
 * @return ADC Wrapper Type (8 bit value) 
 */
__STATIC_INLINE uint32_t DL_HSADC_getADCWrapperType(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return (adc->ADC_LITE_REGS.ADCREV2 & HSADC_ADCREV2_WRAPPERTYPE_MASK);
}

/**
 * @brief returns the ADC wrapper revision. To allow documentation of differences between revisions. First version is labeled as 00h.
 * 
 * @param adc is the base address of the ADC module.
 * @return ADC Wrapper Revision version (24 bit value) 
 */
__STATIC_INLINE uint32_t DL_HSADC_getADCWrapperRevision(hsadc_ADC_LITE_REGS_Regs *adc)
{
    return ((adc->ADC_LITE_REGS.ADCREV2  & HSADC_ADCREV2_WRAPPERREV_MASK)>> HSADC_ADCREV2_WRAPPERREV_OFS);
}

/**
 * @brief sets the oversampling limit
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number 
 * @param limit is the oversampling limit
 */
__STATIC_INLINE void DL_HSADC_setPPBOversamplingLimit(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber, DL_HSADC_OVERSAMPLING_LIMIT limit)
{
    uint32_t* PPBLimitAddr;
    PPBLimitAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1LIMIT + ppbNumber * ADCPPBxLIMIT_STEP);
    *PPBLimitAddr = (*PPBLimitAddr & ~(HSADC_ADCPPB1LIMIT_LIMIT_MASK)) | limit;
}

/**
 * @brief Oversampling Partial Count.  Each time a new result propagates through the PPB signal chain and accumulates into ADCPPB{#}PSUM this register is incremented by 1.  
 * This register is reset when either a count-match event occurs (PCOUNT = LIMIT) or PPB{#} receives a sync. event.
 * This result is available 1 SYSCLK cycle after the associated ADCPPB{#}RESULT is available. 
 * This will be 2 SYSCLK cycles after the associated ADCRESULT is available, unless multiple PPBs point to the same SOC (refer to the ADCPPB{#}RESULT timing information). 
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @return Oversampling Partial Count (3 bit value)  
 */
__STATIC_INLINE uint32_t DL_HSADC_readPPBPartialCount(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* PPBPcountAddr;
    PPBPcountAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1PCOUNT + ppbNumber * ADCPPBxPCOUNT_STEP);
    return (*PPBPcountAddr & HSADC_ADCPPB1PCOUNT_PCOUNT_MASK);
}

/**
 * @brief sets the number of bits to right shift PSUM before loading into the final SUM.  
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @param shift is the number of bits to shift
 */
__STATIC_INLINE void DL_HSADC_setPPBRightShift(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber, DL_HSADC_PPB_RIGHTSHIFT shift )
{
    uint32_t* PPBConfig2Addr;
    PPBConfig2Addr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1CONFIG2 + ppbNumber * ADCPPBxCONFIG2_STEP);
    *PPBConfig2Addr = (*PPBConfig2Addr & ~(HSADC_ADCPPB1CONFIG2_SHIFT_MASK)) | shift;
}

/**
 * @brief select ADCPPB{#}SUM as Post Processing Block {#} Compare Source Select.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_selectCompareSourceAsPPBSum(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* PPBConfig2Addr;
    PPBConfig2Addr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1CONFIG2 + ppbNumber * ADCPPBxCONFIG2_STEP);
    *PPBConfig2Addr |= HSADC_ADCPPB1CONFIG2_COMPSEL_MASK;
}

/**
 * @brief select ADCPPB{#}RESULT as Post Processing Block {#} Compare Source Select.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 */
__STATIC_INLINE void DL_HSADC_selectCompareSourceAsPPBResult(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* PPBConfig2Addr;
    PPBConfig2Addr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1CONFIG2 + ppbNumber * ADCPPBxCONFIG2_STEP);
    *PPBConfig2Addr &= ~HSADC_ADCPPB1CONFIG2_COMPSEL_MASK;
}

/**
 * @brief returns Post Processing Block {#} Oversampling Partial Sum.  
 * Each time a new result propagates through the PPB signal chain and latches into ADCPPB{#}RESULT the result is subsequently accumulated into this register.  
 * This register is reset when either a count-match event occurs (PCOUNT = LIMIT) or PPB{#} receives a sync. event.
 * 
 * @param adc is the base address of the ADC module.
 * @param ppbNumber is the PPB number
 * @return Oversampling Partial Sum (16 bit value)  
 */
__STATIC_INLINE uint32_t DL_HSADC_getOversamplingPartialSum(hsadc_ADC_LITE_REGS_Regs *adc,  DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* PPBPsumAddr;
    PPBPsumAddr = (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCPPB1PSUM + ppbNumber * ADCPPBxCONFIG2_STEP);
    return (*PPBPsumAddr & HSADC_ADCPPB1PSUM_PSUM_MASK);
}

/**
 * @brief sets END SOC of last enabled sequence
 * 
 * @param adc is the base address of the ADC module.
 * @param seqEndSOC is the end of SOC sequence number
 */
__STATIC_INLINE void DL_HSADC_setEndOfSequencer(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SOC_NUMBER seqEndSOC)
{
    
    adc->ADC_LITE_REGS.ADCSEQCTL = (adc->ADC_LITE_REGS.ADCSEQCTL & ~(HSADC_ADCSEQCTL_SEQEND_MASK)) | seqEndSOC;
}

/**
 * @brief sets SOC sequencer preempt
 * 0X: Pre-empt disabled 
 * 10: Pre-empt enabled and will not restart aborted Sequence
 * 11: Pre-empt enabled and will restart aborted Sequence
 * 
 * @param adc is the base address of the ADC module.
 * @param seqPreemptNumber is the seq preempt number
 */
__STATIC_INLINE void DL_HSADC_setSequencerPreempt(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_PREEMPT seqPreemptNumber)
{
    adc->ADC_LITE_REGS.ADCSEQCTL = (adc->ADC_LITE_REGS.ADCSEQCTL & ~(HSADC_ADCSEQCTL_SEQPREEMPT_MASK)) | (seqPreemptNumber << HSADC_ADCSEQCTL_SEQPREEMPT_OFS);
}

/**
 * @brief sets up the sequencer configurations. 
 * SOC{#} Acquisition Prescale. Controls the sample and hold window for each SOC in this Seqeunce. 
 * The configured acquisition time must be at least as long as one ADCCLK cycle for correct ADC operation and atmost 1473 ADCCLK cycles.
 * SOC{#} Trigger Source Select. Along with the SOC{#} field in the ADCINTSOCSEL1 register, this bit field configures which trigger will set the SOC{#} flag in the ADCSOCFLG1 register to initiate a conversion to start once priority is given to it.
 * 
 * @param adc is the base address of the ADC module.
 * @param seqNumber is the sequencer number
 * @param sampleWindow is the sample window.
 * @param trigger is the trigger
 * @param socStartNumber is the Beginning SOC of this sequencer 
 */
__STATIC_INLINE void DL_HSADC_setupSequencer(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER seqNumber, uint32_t sampleWindow,  DL_HSADC_TRIGGER trigger, DL_HSADC_SOC_NUMBER socStartNumber)
{
    ASSERT(sampleWindow >= minSampleWindow && sampleWindow <= maxSampleWindow);
    uint32_t ACQPS;
    uint32_t mask;
    uint32_t* seqConfigAddr;
    seqConfigAddr =  (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSEQ1CONFIG + seqNumber * ADCSEQxCONFIG_STEP);
    mask = (HSADC_ADCSEQ1CONFIG_ACQPS_MASK | HSADC_ADCSEQ1CONFIG_SEQSTART_MASK | HSADC_ADCSEQ1CONFIG_TRIGSEL_MASK);
    
    if (sampleWindow > 448UL){
        ACQPS = (3<<6UL) | (((sampleWindow - 448UL) >> 4UL) - 1UL);
    }
    else if (sampleWindow > 192UL){
        ACQPS = (2<<6UL) | (((sampleWindow - 192UL) >> 2UL) - 1UL);
    }
    else if (sampleWindow > 64UL){
        ACQPS = (1<<6UL) | (((sampleWindow - 64UL) >> 1UL) - 1UL);
    }
    else{
        ACQPS = sampleWindow - 1UL;
    }
    *seqConfigAddr = (*seqConfigAddr & ~(mask) ) | ACQPS| socStartNumber << HSADC_ADCSEQ1CONFIG_SEQSTART_OFS | trigger << HSADC_ADCSEQ1CONFIG_TRIGSEL_OFS;
}

/**
 * @brief enables the sequencer
 * 
 * @param adc is the base address of the ADC module.
 * @param seqNumber is the sequencer number
 */
__STATIC_INLINE void DL_HSADC_enableSequencer(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqConfigAddr;
    seqConfigAddr =  (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSEQ1CONFIG + seqNumber * ADCSEQxCONFIG_STEP);
    *seqConfigAddr |= HSADC_ADCSEQ1CONFIG_SEQENABLE_MASK;
}

/**
 * @brief disables the sequencer
 * 
 * @param adc is the base address of the ADC module.
 * @param seqNumber is the sequencer number
 */
__STATIC_INLINE void DL_HSADC_disableSequencer(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqConfigAddr;
    seqConfigAddr =  (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSEQ1CONFIG + seqNumber * ADCSEQxCONFIG_STEP);
    *seqConfigAddr &= ~HSADC_ADCSEQ1CONFIG_SEQENABLE_MASK;
}

/**
 * @brief enables sample cap reset and sets the sample cap reset. Resets sample cap after conversion to either vrefhi/2 or vreflo
 * sampleCapResetSelect_vreflo – The sample cap is reset to Vreflo after each conversion
 * sampleCapResetSelect_half_vrefhi – The sample cap is reset to Vrefhi/2 after each conversion
 * 
 * @param adc is the base address of the ADC module.
 * @param seqNumber is the sequencer number
 * @param sampleCapResetSelect is used to select sample cap reset to either vrefhi/2 or vreflo
 */
__STATIC_INLINE void DL_HSADC_setSampleCapReset(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER seqNumber, DL_HSADC_SAMPCAPRESET sampleCapResetSelect)
{
    uint32_t* seqConfigAddr;
    seqConfigAddr =  (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSEQ1CONFIG + seqNumber * ADCSEQxCONFIG_STEP);
    *seqConfigAddr = (*seqConfigAddr & ~(HSADC_ADCSEQ1CONFIG_SAMPCAPRESETDISABLE_MASK | HSADC_ADCSEQ1CONFIG_SAMPCAPRESETSEL_MASK)) | ((uint32_t)sampleCapResetSelect << HSADC_ADCSEQ1CONFIG_SAMPCAPRESETSEL_OFS);
}

/**
 * @brief Disables the sample cap reset. The sample cap is not reset after each conversion.
 * 
 * @param adc is the base address of the ADC module.
 * @param seqNumber is the sequencer number
 */
__STATIC_INLINE void DL_HSADC_disableSampleCapReset(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqConfigAddr;
    seqConfigAddr =  (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSEQ1CONFIG + seqNumber * ADCSEQxCONFIG_STEP);
    *seqConfigAddr |= HSADC_ADCSEQ1CONFIG_SAMPCAPRESETDISABLE_MASK; 
}

/**
 * @brief enables the sequencer software force
 * 
 * @param adc is the base address of the ADC module.
 * @param seqNumber is the sequencer number
 */
__STATIC_INLINE void DL_HSADC_triggerSequencerSoftwareForce(hsadc_ADC_LITE_REGS_Regs *adc, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqConfigAddr;
    seqConfigAddr =  (uint32_t*)((uint32_t*)&adc->ADC_LITE_REGS.ADCSEQ1CONFIG + seqNumber * ADCSEQxCONFIG_STEP);
    *seqConfigAddr |= HSADC_ADCSEQ1CONFIG_SEQSWFRC_MASK; 
}

/**
 * @brief returns the 12-bit ADC result. 
 * After the ADC completes a conversion of SOC{#}, the digital result is placed in this bit field.
 * If oversampling is enabled, Final SUM (After any Shift if enabled) will be stored in this register after completing the specified number of samples.
 * 
 * @param adcResult is the base address of the ADC result module
 * @param resultNumber is the SOC number of the result
 * @return ADC result (16 bit value)  
 */
__STATIC_INLINE uint16_t DL_HSADC_getResult(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SOC_NUMBER resultNumber)
{
    uint16_t* resultAddr;
    resultAddr =  ((uint16_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCRESULT0 + resultNumber * ADCRESULTx_STEP);
    return ((*resultAddr) & HSADC_ADCRESULT0_RESULT_MASK);
}

/**
 * @brief returs the PPB result. The result of the offset/reference subtraction post conversion processing is returned by this API.
 * This result is available 1 SYSCLK cycle after the associated ADCRESULT is available, unless multiple PPBs point to the same SOC.  
 * In the case of multiple PPBs associated with the same SOC, the lowest numbered PPB's result will be availble 1 SYSCLK cycle after the associated ADCRESULT and subsequent results (in order from lowest numbered PPB to highest) will each become available every 2-3 SYSCLK cycles (refer to the TRM for more detailed timing information). 
 * 
 * @param adcResult is the base address of the ADC result module
 * @param ppbNumber is the PPB number
 * @return the PPB result (13 bit value)
 */
__STATIC_INLINE uint32_t DL_HSADC_getPPBResult(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* ppbResultAddr;
    ppbResultAddr =  (uint32_t*)((uint32_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCPPB1RESULT + ppbNumber * ADCPPBxRESULT_STEP);
    return (*ppbResultAddr & HSADC_ADCPPB1RESULT_PPBRESULT_MASK);
}

/**
 * @brief returns the final oversampling sum result. 
 * When either a count-match event occurs (PCOUNT = LIMIT) or PPB{#} receives a sync. event, the value of PSUM is loaded into this register.  
 * In the case of a count-match event, the sum loaded into this register includes the value from the most recent conversion. 
 * The value from PSUM will be right shifted by the amount specified in the SHIFT register before being loaded into the final SUM result register.
 * This result is available 1 SYSCLK cycle after the associated ADCPPB{#}RESULT is available (only in case of a count-match event).
 * This will be 2 SYSCLK cycles after the associated ADCRESULT is available, unless multiple PPBs point to the same SOC (refer to the ADCPPB{#}RESULT timing information).
 * 
 * @param adcResult is the base address of the ADC result module
 * @param ppbNumber is the PPB number
 * @return the final oversampling sum result (16 bit value)
 */
__STATIC_INLINE uint16_t DL_HSADC_getFinalSumResult(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint32_t* ppbSumAddr;
    ppbSumAddr =  ((uint32_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCPPB1SUM + ppbNumber * ADCPPBxSUM_STEP);
    return (uint16_t)(*ppbSumAddr & HSADC_ADCPPB1SUM_SUM_MASK);
}

/**
 * @brief returns oversampling Final Count. 
 * When either a count-match event occurs (PCOUNT = LIMIT) or PPB{#} receives a sync. event, the value of PCOUNT is loaded into this register.
 * 
 * @param adcResult is the base address of the ADC result module
 * @param ppbNumber is the PPB number
 * @return oversampling Final Count 
 */
__STATIC_INLINE uint16_t DL_HSADC_getFinalCountResult(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER ppbNumber)
{
    uint16_t* ppbCountAddr;
    ppbCountAddr =  ((uint16_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCPPB1COUNT + ppbNumber * ADCPPBxCOUNT_STEP);
    return ((*ppbCountAddr) & HSADC_ADCPPB1COUNT_COUNT_MASK);
}

/**
 * @brief returns 32-bit FIFO result containing a pair of conversion results. 
 * This API can be used to read the conversion result of all SOCs associated with the sequencer in a sequential manner through FIFO mode. 
 * Since the Result registers are maximum 16-bits wide, every read will return RESULT corresponding to 2 consecutive SOCs
 * LSB 16-bits [15:0] will contain odd results {1,3,5,7...}, and MSB 16-bits [31:16] will contain even results {2,4,6,8...}. 
 * If Odd number of SOCs are associated with a sequencer, final read will have 16’h0 in the MSB 16-bits [31:16].
 * Example: if SEQ1 – Start: SOC0, End: SOC2
 * First read will return SOC0’s result in LSB 16-bits [15:0] and soc1’s result in MSB 16-bits [31:16]
 * Second read will return SOC2’s result in LSB 16-bits [15:0] and 16'h0 in MSB 16-bits [31:16].
 * 
 * @param adcResult is the base address of the ADC result module 
 * @param seqNumber is the sequencer number
 * @return FIFO result consisting of 2 consequtive conversion results (32 bit value)
 */
__STATIC_INLINE uint32_t DL_HSADC_getSeqFIFOResult32(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqFIFOresult;
    seqFIFOresult =  (uint32_t*)((uint32_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCSEQ1FIFORESULT + seqNumber * ADCSEQxFIFORESULT_STEP);
    return (*seqFIFOresult & HSADC_ADCSEQ1FIFORESULT_SEQFIFORESULT_MASK);

}

/**
 * @brief passes by refference, pair of conversion results from the FIFO for a given sequencer. odd results will be in LSB 16-bits [15:0], even results will be in MSB 16-bits [31:16]
 * This API can be used to read the conversion result of all SOCs associated with the sequencer in a sequential manner through FIFO mode. 
 * Since the Result registers are maximum 16-bits wide, every read will return RESULT corresponding to 2 consecutive SOCs
 * resultFirst will contain odd results {1,3,5,7...}, and resultSecond will contain even results {2,4,6,8...}. 
 * 
 * @param adcResult is the base address of the ADC result module
 * @param seqNumber is the sequencer number
 * @param resultFirst returns odd results {1,3,5,7...}
 * @param resultSecond returns even results {2,4,6,8...}
 */
__STATIC_INLINE void DL_HSADC_getSeqFIFOResultByReference(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER seqNumber, uint16_t *resultFirst, uint16_t *resultSecond)
{
    uint32_t* seqFIFOresult;
    seqFIFOresult =  (uint32_t*)((uint32_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCSEQ1FIFORESULT + seqNumber * ADCSEQxFIFORESULT_STEP);
    *resultFirst = (uint16_t)((*seqFIFOresult & HSADC_ADCSEQ1FIFORESULT_SEQFIFORESULT_MASK) & 0xFFFF);
    *resultSecond = (uint16_t)((*seqFIFOresult & HSADC_ADCSEQ1FIFORESULT_SEQFIFORESULT_MASK) >> 16);
}

/**
 * @brief Returns the status of FIFO conversions for a given sequencer. This API can be used to check if Conversion from all the SOCs associated with the Sequencer{#} are complete and result is available to be read. {#} is the sequencer number
 * 
 * @param adcResult is the base address of the ADC result module
 * @param seqNumber is the sequencer number
 * @return true: Conversion from all the SOCs associated with the Sequencer are complete and result is available to be read, false: All Conversions associated with the current Sequencer are NOT complete.
 */
__STATIC_INLINE bool DL_HSADC_isSeqFIFOValid(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqFIFOStatus;
    seqFIFOStatus =  (uint32_t*)((uint32_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCSEQ1FIFOSTATUS + seqNumber * ADCSEQxFIFOSTATUS_STEP);
    return ((*seqFIFOStatus & HSADC_ADCSEQ1FIFOSTATUS_SEQFIFOVALID_MASK) == HSADC_ADCSEQ1FIFOSTATUS_SEQFIFOVALID_MASK);
}

/**
 * @brief returns the number of pending reads with DL_HSADC_getSeqFIFOResult32 or DL_HSADC_getSeqFIFOResultByReference
 * Note: 
 * This field is only updated when after the last byte of ADCSEQ{#}FIFORESULT is read.
 * The Pending read information will not be updated for debug reads. 
 * Once all the data in the FIFO has been read, FIFO will wrap back. Subsequent reads will read from the beginning of the FIFO.
 * Returns half of total SOCs in SEQ{#} for even number of SOCs and Returns half of (1 + total SOCs) in SEQ{#}
 * 
 * @param adcResult 
 * @param seqNumber 
 * @return number of pending reads (4 bit value)
 */
__STATIC_INLINE uint32_t DL_HSADC_getPendingReads(hsadc_ADC_LITE_RESULT_REGS_Regs *adcResult, DL_HSADC_SEQ_NUMBER seqNumber)
{
    uint32_t* seqFIFOStatus;
    seqFIFOStatus =  (uint32_t*)((uint32_t*)&adcResult->ADC_LITE_RESULT_REGS.ADCSEQ1FIFOSTATUS + seqNumber * ADCSEQxFIFOSTATUS_STEP);
    return ((*seqFIFOStatus & HSADC_ADCSEQ1FIFOSTATUS_SEQFIFOPENDREAD_MASK) >> HSADC_ADCSEQ1FIFOSTATUS_SEQFIFOPENDREAD_OFS);
}


#ifdef __cplusplus
}
#endif

#endif /* __MSP_HAS_ADC__ */

#endif /* ti_dl_DL_HSADC__include */
/** @}*/