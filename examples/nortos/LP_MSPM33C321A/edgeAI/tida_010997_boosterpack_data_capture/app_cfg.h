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

#ifndef APP_CFG_H
#define APP_CFG_H


/* ========================================================================== */
/*                 UART Controller Instance Configuration                     */
/* ========================================================================== */

#define UART_0_INST                                                         UC12
#define UART_0_INST_FREQUENCY                                           80000000
#define UART_0_INST_IRQHandler                                   UC12_IRQHandler
#define UART_0_INST_INT_IRQN                                       UC12_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                       IOMUX_PINCM22_PF_UC12_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                       IOMUX_PINCM21_PF_UC12_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_80_MHZ_115200_BAUD                                      (43)
#define UART_0_FBRD_80_MHZ_115200_BAUD                                      (26)


/* ========================================================================== */
/*                 I2C Controller Instance Configuration                      */
/* ========================================================================== */

/*
 * I2C Controller: UC1_0
 *   SDA = PA0 (IOMUX_PINCM1)
 *   SCL = PA1 (IOMUX_PINCM2)
 * Connected to HDC3020 temperature/humidity sensor.
 */
#define I2C_INST                UC1_0
#define I2C_INST_IRQHandler     UC1_0_IRQHandler
#define I2C_INST_INT_IRQN       UC1_0_INT_IRQn
#define I2C_INST_CLOCK_SOURCE   DL_I2CC_CLOCK_BUSCLK
#define I2C_INST_CLOCK_DIVIDE   DL_I2CC_CLOCK_DIVIDE_1
#define I2C_BUS_SPEED_HZ        (400000U)
#define I2C_INPUT_CLOCK_HZ      (80000000U)

/* I2C SDA: UC1_0 SDA = PA0 */
#define GPIO_I2C_SDA_PORT       GPIOA
#define GPIO_I2C_SDA_PIN        DL_GPIO_PIN_0
#define GPIO_I2C_IOMUX_SDA      (IOMUX_PINCM1)
#define GPIO_I2C_IOMUX_SDA_FUNC IOMUX_PINCM1_PF_UC1_0_SDA

/* I2C SCL: UC1_0 SCL = PA1 */
#define GPIO_I2C_SCL_PORT       GPIOA
#define GPIO_I2C_SCL_PIN        DL_GPIO_PIN_1
#define GPIO_I2C_IOMUX_SCL      (IOMUX_PINCM2)
#define GPIO_I2C_IOMUX_SCL_FUNC IOMUX_PINCM2_PF_UC1_0_SCL


/* ========================================================================== */
/*                 SPI Controller Instance Configuration                      */
/* ========================================================================== */

#define SENSOR_INTR_PORT                                                 (GPIOB)

/* Defines for ACC_GYRO: GPIOB.2 with pinCMx 15 on package pin 23 */
// pins affected by this interrupt request:["ACC_GYRO","MAGNET","ACC_GYRO_2"]
#define SENSOR_INTR_INT_IRQN                                    (GPIOB_INT_IRQn)
#define SENSOR_INTR_INT_IRQHandler                            (GPIOB_IRQHandler)
#define SENSOR_INTR_ACC_GYRO_IIDX                            (DL_GPIO_IIDX_DIO2)
#define SENSOR_INTR_ACC_GYRO_PIN                                 (DL_GPIO_PIN_2)
#define SENSOR_INTR_ACC_GYRO_IOMUX                               (IOMUX_PINCM15)
/* Defines for MAGNET: GPIOB.3 with pinCMx 16 on package pin 24 */
#define SENSOR_INTR_MAGNET_IIDX                              (DL_GPIO_IIDX_DIO3)
#define SENSOR_INTR_MAGNET_PIN                                   (DL_GPIO_PIN_3)
#define SENSOR_INTR_MAGNET_IOMUX                                 (IOMUX_PINCM16)
/* Defines for ACC_GYRO_2: GPIOB.1 with pinCMx 13 on package pin 21 */
#define SENSOR_INTR_ACC_GYRO_2_IIDX                          (DL_GPIO_IIDX_DIO1)
#define SENSOR_INTR_ACC_GYRO_2_PIN                               (DL_GPIO_PIN_1)
#define SENSOR_INTR_ACC_GYRO_2_IOMUX                             (IOMUX_PINCM13)
/* Port definition for Pin Group SPI_CS */
#define SPI_CS_PORT                                                      (GPIOA)

/* Defines for IMU: GPIOA.29 with pinCMx 4 on package pin 4 */
#define SPI_CS_IMU_PIN                                          (DL_GPIO_PIN_29)
#define SPI_CS_IMU_IOMUX                                          (IOMUX_PINCM4)
/* Defines for HALL: GPIOA.30 with pinCMx 5 on package pin 5 */
#define SPI_CS_HALL_PIN                                         (DL_GPIO_PIN_30)
#define SPI_CS_HALL_IOMUX                                         (IOMUX_PINCM5)


/* Defines for SPI */
#define SPI_INST                                                            UC2
#define SPI_INST_IRQHandler                                      UC2_IRQHandler
#define SPI_INST_INT_IRQN                                          UC2_INT_IRQn
#define GPIO_SPI_PICO_PORT                                                GPIOB
#define GPIO_SPI_PICO_PIN                                        DL_GPIO_PIN_17
#define GPIO_SPI_IOMUX_PICO                                     (IOMUX_PINCM43)
#define GPIO_SPI_IOMUX_PICO_FUNC                      IOMUX_PINCM43_PF_UC2_PICO
#define GPIO_SPI_POCI_PORT                                                GPIOB
#define GPIO_SPI_POCI_PIN                                        DL_GPIO_PIN_19
#define GPIO_SPI_IOMUX_POCI                                     (IOMUX_PINCM45)
#define GPIO_SPI_IOMUX_POCI_FUNC                      IOMUX_PINCM45_PF_UC2_POCI
/* GPIO configuration for SPI */
#define GPIO_SPI_SCLK_PORT                                                GPIOB
#define GPIO_SPI_SCLK_PIN                                        DL_GPIO_PIN_18
#define GPIO_SPI_IOMUX_SCLK                                     (IOMUX_PINCM44)
#define GPIO_SPI_IOMUX_SCLK_FUNC                      IOMUX_PINCM44_PF_UC2_SCLK
#define GPIO_SPI_CS0_PORT                                                 GPIOA
#define GPIO_SPI_CS0_PIN                                          DL_GPIO_PIN_2
#define GPIO_SPI_IOMUX_CS0                                       (IOMUX_PINCM7)
#define GPIO_SPI_IOMUX_CS0_FUNC                         IOMUX_PINCM7_PF_UC2_CS0

/* SPI Configuration Macros for SPI */
#define SPI_MODE                                     DL_SPI_MODE_CONTROLLER
#define SPI_FRAME_FORMAT                    DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0
#define SPI_PARITY                                   DL_SPI_PARITY_NONE
#define SPI_DATA_SIZE                                DL_SPI_DATA_SIZE_8
#define SPI_BIT_ORDER                       DL_SPI_BIT_ORDER_MSB_FIRST
#define SPI_CHIP_SELECT                              DL_SPI_CHIP_SELECT_0

/* SPI Clock Configuration Macros for SPI */
#define SPI_CLOCK_SOURCE                             DL_SPI_CLOCK_BUSCLK
#define SPI_CLOCK_DIVIDER                   DL_SPI_CLOCK_DIVIDE_RATIO_1

/* SPI Bit Rate Configuration for SPI */
#define SPI_BIT_RATE_HZ                              2000000U
#define SPI_INPUT_CLOCK_HZ                           80000000U
#define SPI_SCR                                      19U


/* ========================================================================== */
/*                 Uart GPIO Initialization                                   */
/* ========================================================================== */

static inline void App_Uart_InitGpio(void){

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_0_IOMUX_TX, GPIO_UART_0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_0_IOMUX_RX, GPIO_UART_0_IOMUX_RX_FUNC);

}

/* ========================================================================== */
/*                 I2C GPIO Initialization                                    */
/* ========================================================================== */

static inline void App_I2c_InitGPIO(void)
{
    DL_GPIO_enablePower(GPIO_I2C_SDA_PORT);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SDA,
        GPIO_I2C_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SDA);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SCL,
        GPIO_I2C_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SCL);
}

/* ========================================================================== */
/*                 SPI GPIO Initialization                                    */
/* ========================================================================== */

static inline void App_Spi_InitGPIO(void){

    DL_GPIO_initDigitalInputFeatures(SENSOR_INTR_ACC_GYRO_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(SENSOR_INTR_MAGNET_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(SENSOR_INTR_ACC_GYRO_2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(SPI_CS_IMU_IOMUX);

    DL_GPIO_initDigitalOutput(SPI_CS_HALL_IOMUX);

    DL_GPIO_setPins(SPI_CS_PORT, SPI_CS_IMU_PIN |
		SPI_CS_HALL_PIN);
    DL_GPIO_enableOutput(SPI_CS_PORT, SPI_CS_IMU_PIN |
		SPI_CS_HALL_PIN);
    DL_GPIO_setLowerPinsPolarity(SENSOR_INTR_PORT, DL_GPIO_PIN_2_EDGE_FALL |
		DL_GPIO_PIN_3_EDGE_FALL |
		DL_GPIO_PIN_1_EDGE_FALL);
    DL_GPIO_clearInterruptStatus(SENSOR_INTR_PORT, SENSOR_INTR_ACC_GYRO_PIN |
		SENSOR_INTR_MAGNET_PIN |
		SENSOR_INTR_ACC_GYRO_2_PIN);
    DL_GPIO_enableInterrupt(SENSOR_INTR_PORT, SENSOR_INTR_ACC_GYRO_PIN |
		SENSOR_INTR_MAGNET_PIN |
		SENSOR_INTR_ACC_GYRO_2_PIN);
        
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_IOMUX_SCLK, GPIO_SPI_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_IOMUX_PICO, GPIO_SPI_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_IOMUX_POCI, GPIO_SPI_IOMUX_POCI_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_IOMUX_CS0, GPIO_SPI_IOMUX_CS0_FUNC);

}

#endif /* APP_CFG_H */



