/*
 *  Copyright (C) 2026 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef APP_CFG_H
#define APP_CFG_H

/* ========================================================================== */
/*                 SPI Instance Configuration                                */
/* ========================================================================== */

/*
 * Controller: UC2 (SPI_0)
 * Pins: SCLK=PB18, PICO=PB17, POCI=PB19, CS0=PA2
 * Mode: Controller with internal hardware loopback (PICO routed to POCI)
 * No external wiring required - loopback is internal to the SPI peripheral
 */
#define APP_SPI_CONTROLLER_INSTANCE UC2
#define APP_SPI_BIT_RATE_HZ (500000U)

/* GPIO Configuration for SPI pins */
#define GPIO_SPI_SCLK_PORT GPIOB
#define GPIO_SPI_SCLK_PIN DL_GPIO_PIN_18
#define GPIO_SPI_SCLK_IOMUX IOMUX_PINCM44
#define GPIO_SPI_SCLK_FUNC IOMUX_PINCM44_PF_UC2_SCLK

#define GPIO_SPI_PICO_PORT GPIOB
#define GPIO_SPI_PICO_PIN DL_GPIO_PIN_17
#define GPIO_SPI_PICO_IOMUX IOMUX_PINCM43
#define GPIO_SPI_PICO_FUNC IOMUX_PINCM43_PF_UC2_PICO

#define GPIO_SPI_POCI_PORT GPIOB
#define GPIO_SPI_POCI_PIN DL_GPIO_PIN_19
#define GPIO_SPI_POCI_IOMUX IOMUX_PINCM45
#define GPIO_SPI_POCI_FUNC IOMUX_PINCM45_PF_UC2_POCI

#define GPIO_SPI_CS0_PORT GPIOA
#define GPIO_SPI_CS0_PIN DL_GPIO_PIN_2
#define GPIO_SPI_CS0_IOMUX IOMUX_PINCM7
#define GPIO_SPI_CS0_FUNC IOMUX_PINCM7_PF_UC2_CS0

/* ========================================================================== */
/*                 GPIO and SPI Peripheral Init Functions                    */
/* ========================================================================== */

static inline void App_Spi_InitGPIO(void)
{
    /* Enable GPIO port power */
    DL_GPIO_enablePower(GPIO_SPI_SCLK_PORT);
    DL_GPIO_enablePower(GPIO_SPI_PICO_PORT);
    DL_GPIO_enablePower(GPIO_SPI_POCI_PORT);
    DL_GPIO_enablePower(GPIO_SPI_CS0_PORT);

    /* Configure SPI pins as peripheral outputs/inputs */
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_SCLK_IOMUX, GPIO_SPI_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_PICO_IOMUX, GPIO_SPI_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_SPI_POCI_IOMUX,
        GPIO_SPI_POCI_FUNC, DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_CS0_IOMUX, GPIO_SPI_CS0_FUNC);
}

/* Internal hardware loopback - PICO routed to POCI internally */
static inline void App_Spi_LoopbackInit(void)
{
    DL_SPI_enableLoopbackMode(APP_SPI_CONTROLLER_INSTANCE);
}

static inline void App_Spi_LoopbackDeInit(void)
{
    DL_SPI_disableLoopbackMode(APP_SPI_CONTROLLER_INSTANCE);
}

#endif /* APP_CFG_H */
