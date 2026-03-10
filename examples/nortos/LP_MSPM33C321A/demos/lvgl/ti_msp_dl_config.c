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

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM33 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM33C321
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_SYSCTL_CLK_init();
    SYSCFG_DL_TIMER_0_init();
    SYSCFG_DL_I2C_init();
    SYSCFG_DL_SPI_0_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_GPIO_reset(GPIOC);
    DL_I2CC_reset(I2C_INST);
    DL_SPI_reset(SPI_0_INST);
    DL_TimerG_reset(TIMER_0_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_GPIO_enablePower(GPIOC);
    DL_I2CC_enablePower(I2C_INST);
    DL_SPI_enablePower(SPI_0_INST);
    DL_TimerG_enablePower(TIMER_0_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SDA,
        GPIO_I2C_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SCL,
        GPIO_I2C_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_0_IOMUX_SCLK, GPIO_SPI_0_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_0_IOMUX_PICO, GPIO_SPI_0_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_SPI_0_IOMUX_POCI,
        GPIO_SPI_0_IOMUX_POCI_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_0_IOMUX_CS1, GPIO_SPI_0_IOMUX_CS1_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_LEDS_USER_LED_1_IOMUX);

    DL_GPIO_initDigitalOutput(LCD_SDC_IOMUX);

    DL_GPIO_initDigitalOutput(LCD_RESET_IOMUX);

    DL_GPIO_initDigitalOutput(LCD_PWM_IOMUX);

    DL_GPIO_initDigitalOutput(CTP_RST_IOMUX);

    DL_GPIO_initDigitalInputFeatures(CTP_INT_IOMUX, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_setPins(GPIOA, LCD_RESET_PIN);
    DL_GPIO_enableOutput(GPIOA, LCD_RESET_PIN);
    DL_GPIO_clearPins(GPIOB, LCD_SDC_PIN | LCD_PWM_PIN);
    DL_GPIO_setPins(GPIOB, GPIO_LEDS_USER_LED_1_PIN);
    DL_GPIO_enableOutput(
        GPIOB, GPIO_LEDS_USER_LED_1_PIN | LCD_SDC_PIN | LCD_PWM_PIN);
    DL_GPIO_setPins(CTP_PORT, CTP_RST_PIN);
    DL_GPIO_enableOutput(CTP_PORT, CTP_RST_PIN);
}

static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {.rDivClk2x = 0x0,
    .rDivClk1                                                   = 0x0,
    .rDivClk0                                                   = 0x0,
    .enableCLK2x = DL_SYSCTL_SYSPLL_CLK2X_DISABLE,
    .enableCLK1  = DL_SYSCTL_SYSPLL_CLK1_ENABLE,
    .enableCLK0  = DL_SYSCTL_SYSPLL_CLK0_ENABLE,
    .sysPLLMCLK  = DL_SYSCTL_SYSPLL_MCLK_CLK0,
    .sysPLLRef   = DL_SYSCTL_SYSPLL_REF_SYSOSC,
    .qDiv        = 0x09,
    .pDiv        = DL_SYSCTL_SYSPLL_PDIV_2,
    .inputFreq   = DL_SYSCTL_SYSPLL_INPUT_FREQ_32_48_MHZ};

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{
    // //Low Power Mode is configured to be SLEEP
    // DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    // DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    // /* Set default configuration */
    // DL_SYSCTL_disableHFXT();
    // DL_SYSCTL_disableSYSPLL();

    //Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    /* Set default configuration */
    DL_SYSCTL_disableHFXT();
    DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1_DIV_1_DIV_2);
    //DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);
    //DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1_DIV_2_DIV_2);
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_CLK_init(void)
{
    while (
        (DL_SYSCTL_getClockStatus() & (DL_SYSCTL_CLK_STATUS_SYSPLL_GOOD |
                                          DL_SYSCTL_CLK_STATUS_HSCLK_GOOD)) !=
        (DL_SYSCTL_CLK_STATUS_SYSPLL_GOOD | DL_SYSCTL_CLK_STATUS_HSCLK_GOOD)) {
        /* Ensure that clocks are in default POR configuration before initialization.
            * Additionally once LFXT is enabled, the internal LFOSC is disabled, and cannot
            * be re-enabled other than by executing a BOOTRST. */
        ;
    }
}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 31U,
};

/*
  * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
  * TIMER_0_INST_LOAD_VALUE = (500 ms * 125000 Hz) - 1
  */
static const DL_TimerG_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIMER_0_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_0_init(void)
{
    DL_TimerG_setClockConfig(
        TIMER_0_INST, (DL_TimerG_ClockConfig *) &gTIMER_0ClockConfig);

    DL_TimerG_initTimerMode(
        TIMER_0_INST, (DL_TimerG_TimerConfig *) &gTIMER_0TimerConfig);
    DL_TimerG_enableInterrupt(TIMER_0_INST, DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_0_INST);
}

static const DL_I2CC_ClockConfig gI2CClockConfig = {
    .clockSel    = DL_I2CC_CLOCK_BUSCLK,
    .divideRatio = DL_I2CC_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C_init(void)
{
    DL_I2CC_setClockConfig(I2C_INST, (DL_I2CC_ClockConfig *) &gI2CClockConfig);

    /* Configure Controller Mode */
    DL_I2CC_resetTransfer(I2C_INST);
    /* Set frequency to 400000 Hz*/
    DL_I2CC_setTimerPeriod(I2C_INST, 15);
    DL_I2CC_setTXFIFOThreshold(I2C_INST, DL_I2CC_TX_FIFO_LEVEL_EMPTY);
    DL_I2CC_setRXFIFOThreshold(I2C_INST, DL_I2CC_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_I2CC_enableClockStretching(I2C_INST);

    /* Enable module */
    DL_I2CC_enable(I2C_INST);
}

static const DL_SPI_Config gSPI_0_config = {
    .mode          = DL_SPI_MODE_CONTROLLER,
    .frameFormat   = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity        = DL_SPI_PARITY_NONE,
    .dataSize      = DL_SPI_DATA_SIZE_8,
    .bitOrder      = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_1,
};

static const DL_SPI_ClockConfig gSPI_0_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_0_init(void)
{
    DL_SPI_setClockConfig(
        SPI_0_INST, (DL_SPI_ClockConfig *) &gSPI_0_clockConfig);

    DL_SPI_init(SPI_0_INST, (DL_SPI_Config *) &gSPI_0_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     500000 = (32000000)/((1 + 31) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_0_INST, 0);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_0_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL,
        DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(SPI_0_INST);
}
