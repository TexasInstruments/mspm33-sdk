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
/*                 I2C Instance Configuration                                */
/* ========================================================================== */

/*
 * Controller: UC1_0, SDA=PA0, SCL=PA1
 * Target:     UC13_0, SDA=PB12, SCL=PB13
 * Wire connections required:
 *   PA0 (UC1_0 SDA) <--> PB12 (UC13_0 SDA)
 *   PA1 (UC1_0 SCL) <--> PB13 (UC13_0 SCL)
 * NOTE: PA0 has a Red LED connected on PCB - as controller,
 *       it can drive SDA strongly enough despite the LED load.
 */
#define APP_I2C_CONTROLLER_INSTANCE UC1_0
#define APP_I2C_TARGET_INSTANCE UC13_0

#define APP_I2C_TARGET_ADDRESS (0x50U)

#define APP_I2C_CONTROLLER_IRQ UC1_0_INT_IRQn
#define APP_I2C_TARGET_IRQ UC13_0_INT_IRQn

#define App_I2c_ControllerIRQHandler(void) UC1_0_IRQHandler(void)
#define App_I2c_TargetIRQHandler(void) UC13_0_IRQHandler(void)

/* GPIO Configuration */
#define GPIO_I2CC_SDA_PORT GPIOA
#define GPIO_I2CC_SDA_PIN DL_GPIO_PIN_0
#define GPIO_I2CC_SDA_IOMUX IOMUX_PINCM1
#define GPIO_I2CC_SDA_FUNC IOMUX_PINCM1_PF_UC1_0_SDA

#define GPIO_I2CC_SCL_PORT GPIOA
#define GPIO_I2CC_SCL_PIN DL_GPIO_PIN_1
#define GPIO_I2CC_SCL_IOMUX IOMUX_PINCM2
#define GPIO_I2CC_SCL_FUNC IOMUX_PINCM2_PF_UC1_0_SCL

#define GPIO_I2CT_SDA_PORT GPIOB
#define GPIO_I2CT_SDA_PIN DL_GPIO_PIN_12
#define GPIO_I2CT_SDA_IOMUX IOMUX_PINCM29
#define GPIO_I2CT_SDA_FUNC IOMUX_PINCM29_PF_UC13_0_SDA

#define GPIO_I2CT_SCL_PORT GPIOB
#define GPIO_I2CT_SCL_PIN DL_GPIO_PIN_13
#define GPIO_I2CT_SCL_IOMUX IOMUX_PINCM30
#define GPIO_I2CT_SCL_FUNC IOMUX_PINCM30_PF_UC13_0_SCL

/* Sysconfig-generated symbol mappings */
#define I2CC_INST APP_I2C_CONTROLLER_INSTANCE
#define I2CC_INST_INT_IRQN APP_I2C_CONTROLLER_IRQ
#define I2CC_INST_IRQHandler UC1_0_IRQHandler
#define I2CC_BUS_SPEED_HZ (100000U)
#define I2CC_INST_CLOCK_SOURCE DL_I2CC_CLOCK_BUSCLK
#define I2CC_INST_CLOCK_DIVIDE DL_I2CC_CLOCK_DIVIDE_1

#define I2CT_INST APP_I2C_TARGET_INSTANCE
#define I2CT_INST_INT_IRQN APP_I2C_TARGET_IRQ
#define I2CT_INST_IRQHandler UC13_0_IRQHandler
#define I2CT_TARGET_OWN_ADDR APP_I2C_TARGET_ADDRESS
#define I2CT_INST_CLOCK_SOURCE DL_I2CT_CLOCK_BUSCLK
#define I2CT_INST_CLOCK_DIVIDE DL_I2CT_CLOCK_DIVIDE_1

/* ========================================================================== */
/*                 GPIO and I2C Peripheral Init Functions                    */
/* ========================================================================== */

static inline void App_I2c_InitGPIO(void)
{
    /* Enable GPIO port power */
    DL_GPIO_enablePower(GPIO_I2CC_SDA_PORT);
    DL_GPIO_enablePower(GPIO_I2CT_SDA_PORT);

    /* Configure I2C Controller pins with pull-ups and HiZ */
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2CC_SDA_IOMUX,
        GPIO_I2CC_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2CC_SDA_IOMUX);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2CC_SCL_IOMUX,
        GPIO_I2CC_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2CC_SCL_IOMUX);

    /* Configure I2C Target pins with pull-ups and HiZ */
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2CT_SDA_IOMUX,
        GPIO_I2CT_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2CT_SDA_IOMUX);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2CT_SCL_IOMUX,
        GPIO_I2CT_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2CT_SCL_IOMUX);
}

/* Physical loopback via jumper wires required */
static inline void App_I2c_ExternalLoopbackInit(void)
{
    /* Nothing to do — loopback is done via physical wire connections */
}

static inline void App_I2c_ExternalLoopbackDeInit(void)
{
    /* Nothing to do */
}

#endif /* APP_CFG_H */
