/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM33 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM33C321
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM33C321

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m33/dl_core.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */

/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     160000000

/* Defines for TIMER_0 */
#define TIMER_0_INST                                                    (TIMG8_0)
#define TIMER_0_INST_IRQHandler                                 TIMG8_0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG8_0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                          (2499U)

/* Defines for I2C */
#define I2C_INST                                                          UC15_0
#define I2C_INST_IRQHandler                                      UC15_0_IRQHandler
#define I2C_INST_INT_IRQN                                        UC15_0_INT_IRQn
#define I2C_BUS_SPEED_HZ                                                  400000
#define GPIO_I2C_SDA_PORT                                                  GPIOA
#define GPIO_I2C_SDA_PIN                                           DL_GPIO_PIN_0
#define GPIO_I2C_IOMUX_SDA                                        (IOMUX_PINCM1)
#define GPIO_I2C_IOMUX_SDA_FUNC                          IOMUX_PINCM1_PF_UC15_0_SDA
#define GPIO_I2C_SCL_PORT                                                  GPIOA
#define GPIO_I2C_SCL_PIN                                           DL_GPIO_PIN_1
#define GPIO_I2C_IOMUX_SCL                                        (IOMUX_PINCM2)
#define GPIO_I2C_IOMUX_SCL_FUNC                          IOMUX_PINCM2_PF_UC15_0_SCL

/* Defines for SPI_0 */
#define SPI_0_INST                                                          UC2
#define SPI_0_INST_IRQHandler                                    UC2_IRQHandler
#define SPI_0_INST_INT_IRQN                                        UC2_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOA
#define GPIO_SPI_0_PICO_PIN                                      DL_GPIO_PIN_9
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM20)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                    IOMUX_PINCM20_PF_UC2_PICO
#define GPIO_SPI_0_POCI_PORT                                              GPIOB
#define GPIO_SPI_0_POCI_PIN                                      DL_GPIO_PIN_19
#define GPIO_SPI_0_IOMUX_POCI                                   (IOMUX_PINCM45)
#define GPIO_SPI_0_IOMUX_POCI_FUNC                    IOMUX_PINCM45_PF_UC2_POCI
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOA
#define GPIO_SPI_0_SCLK_PIN                                      DL_GPIO_PIN_8
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM19)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                    IOMUX_PINCM19_PF_UC2_SCLK
/* Removed these as CS will be controlled via GPIO. SPI will work in 3 wire mode.*/
// #define GPIO_SPI_0_CS0_PORT                                               GPIOB
// #define GPIO_SPI_0_CS0_PIN                                        DL_GPIO_PIN_6
// #define GPIO_SPI_0_IOMUX_CS1                                     (IOMUX_PINCM23)
// #define GPIO_SPI_0_IOMUX_CS1_FUNC                       IOMUX_PINCM23_PF_UC2_CS1

/* Port definition for Pin Group GPIO_LEDS */
#define GPIO_LEDS_PORT                                                   (GPIOA)

/* Defines for USER_LED_1: GPIOB.22 with pinCMx 50 on package pin 84 */
#define GPIO_LEDS_USER_LED_1_PIN                                (DL_GPIO_PIN_2)
#define GPIO_LEDS_USER_LED_1_IOMUX                               (IOMUX_PINCM7)
/* Defines for SDC: GPIOB.30 with pinCMx 67 on package pin 31 */
#define LCD_SDC_PORT                                                     (GPIOB)
#define LCD_SDC_PIN                                             (DL_GPIO_PIN_30)
#define LCD_SDC_IOMUX                                            (IOMUX_PINCM67)
/* Defines for RESET: GPIOA.10 with pinCMx 21 on package pin 33 */
#define LCD_RESET_PORT                                                   (GPIOA)
#define LCD_RESET_PIN                                           (DL_GPIO_PIN_10)
#define LCD_RESET_IOMUX                                          (IOMUX_PINCM21)

/* Port definition for Pin Group LCD_CS */
#define LCD_CS_PORT                                                      (GPIOB)
/* Defines for PIN_0: GPIOB.6 with pinCMx 23 on package pin 40 */
#define LCD_CS_PIN_0_PIN                                         (DL_GPIO_PIN_6)
#define LCD_CS_PIN_0_IOMUX                                       (IOMUX_PINCM23)

/* Defines for PWM: GPIOB.3 with pinCMx 16 on package pin 24 */
#define LCD_PWM_PORT                                                     (GPIOB)
#define LCD_PWM_PIN                                              (DL_GPIO_PIN_3)
#define LCD_PWM_IOMUX                                            (IOMUX_PINCM16)
/* Port definition for Pin Group CTP */
#define CTP_PORT                                                         (GPIOC)

/* Defines for RST: GPIOC.4 with pinCMx 78 on package pin 67 */
#define CTP_RST_PIN                                              (DL_GPIO_PIN_4)
#define CTP_RST_IOMUX                                            (IOMUX_PINCM78)
/* Defines for INT: GPIOC.5 with pinCMx 79 on package pin 68 */
#define CTP_INT_PIN                                              (DL_GPIO_PIN_5)
#define CTP_INT_IOMUX                                            (IOMUX_PINCM79)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_SYSCTL_CLK_init(void);
void SYSCFG_DL_I2C_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_DMA0_init(void);

/* Defines for DMA0_CH1 */
#define DMA0_CH1_CHAN_ID                                                     (1)
#define SPI_0_INST_DMA_TRIGGER                                (DMA0_UC2_TX_TRIG)


#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
