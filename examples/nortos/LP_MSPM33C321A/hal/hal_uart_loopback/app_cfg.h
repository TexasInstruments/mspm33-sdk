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

#ifndef APP_UART_CFG_H
#define APP_UART_CFG_H

#include <ti/driverlib/dl_gpio.h>

/* ========================================================================== */
/*                 UART Instance Configuration                               */
/* ========================================================================== */

/* TX Instance: UC1_0 */
#define UART_0_INST                                                        UC1_0
#define UART_0_INST_INT_IRQN                                      UC1_0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                         DL_GPIO_PIN_1
#define GPIO_UART_0_TX_PIN                                         DL_GPIO_PIN_0
#define GPIO_UART_0_IOMUX_RX                                      (IOMUX_PINCM2)
#define GPIO_UART_0_IOMUX_TX                                      (IOMUX_PINCM1)
#define GPIO_UART_0_IOMUX_RX_FUNC                       IOMUX_PINCM2_PF_UC1_0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                       IOMUX_PINCM1_PF_UC1_0_TX

/* RX Instance: UC1_1 */
#define UART_1_INST                                                        UC1_1
#define UART_1_INST_INT_IRQN                                      UC1_1_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOA
#define GPIO_UART_1_RX_PIN                                        DL_GPIO_PIN_15
#define GPIO_UART_1_TX_PIN                                        DL_GPIO_PIN_16
#define GPIO_UART_1_IOMUX_RX                                     (IOMUX_PINCM37)
#define GPIO_UART_1_IOMUX_TX                                     (IOMUX_PINCM38)
#define GPIO_UART_1_IOMUX_RX_FUNC                      IOMUX_PINCM37_PF_UC1_1_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                      IOMUX_PINCM38_PF_UC1_1_TX

/* Application macros for HAL */
#define APP_UART_TX_INST_PTR        (UNICOMM_Inst_Regs *)UART_0_INST
#define APP_UART_RX_INST_PTR        (UNICOMM_Inst_Regs *)UART_1_INST
#define APP_UART_TX_IRQ             UART_0_INST_INT_IRQN
#define APP_UART_RX_IRQ             UART_1_INST_INT_IRQN
#define APP_UART_BAUDRATE_BPS       (9600U)
#define APP_UART_INPUT_CLOCK_HZ     (32000000U)  /* 32 MHz CPU clock */

/* ========================================================================== */
/*                 GPIO Initialization Functions                             */
/* ========================================================================== */

static inline void App_Uart_InitGPIO(void)
{
    /* Enable GPIO power for GPIOA */
    DL_GPIO_enablePower(GPIO_UART_0_TX_PORT);

    /* Configure UC1_0 (TX Instance): PA0=TX, PA1=RX */
    DL_GPIO_initPeripheralOutputFunction(GPIO_UART_0_IOMUX_TX, GPIO_UART_0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_UART_0_IOMUX_RX, GPIO_UART_0_IOMUX_RX_FUNC);

    /* Configure UC1_1 (RX Instance): PA16=TX, PA15=RX */
    DL_GPIO_initPeripheralOutputFunction(GPIO_UART_1_IOMUX_TX, GPIO_UART_1_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_UART_1_IOMUX_RX, GPIO_UART_1_IOMUX_RX_FUNC);
}

static inline void App_Uart_DeInitGPIO(void)
{
    /* GPIO power remains enabled for other peripherals */
}

#endif  /* APP_UART_CFG_H */
