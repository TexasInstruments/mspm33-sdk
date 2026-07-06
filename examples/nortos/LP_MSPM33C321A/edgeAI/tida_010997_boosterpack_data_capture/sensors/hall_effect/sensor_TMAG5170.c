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

#include "sensor_TMAG5170.h"
#include "comm/spi_comm.h"
#include "ti_msp_dl_config.h"
#include <string.h>

/* Number of CPU cycles for 1ms delay (CPUCLK = 160 MHz) */
#define CYCLES_PER_MS          (160000U)
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

#define TMAG5170_POWERUP_DELAY_MS    50
#define TMAG5170_RESET_DELAY_MS       2
#define TMAG5170_MODE_TRANSITION_MS   1

static int tmag5170_read_reg(uint8_t regAddr, uint16_t *data);
static int tmag5170_write_reg(uint8_t regAddr, uint16_t data);

static tmag5170_config_t gCurrentConfig;

/* CS pin: PA30 — SPI_CS_PORT (GPIOA), SPI_CS_HALL_PIN (DL_GPIO_PIN_30) */
const chipSelect TMAG5170_SPI_CS = {SPI_CS_PORT, SPI_CS_HALL_PIN};

int tmag5170_init(const tmag5170_config_t *config)
{
    int ret;
    uint16_t regValue;
    uint8_t txBuffer[4];
    uint8_t rxBuffer[4];

    ret = TMAG5170_OK;

    /* Power supply settling delay  */
    delay_cycles(MS_TO_CYCLES(TMAG5170_POWERUP_DELAY_MS));

    /* Store configuration */
    memcpy(&gCurrentConfig, config, sizeof(tmag5170_config_t));

    /* Disable CRC if needed (must be done before other operations) */
    if (!config->crcEn) {
        /* Format: {0x0F, 0x00, 0x04, 0x07} writes TEST_CONFIG to disable CRC */
        txBuffer[0] = 0x0F;
        txBuffer[1] = 0x00;
        txBuffer[2] = 0x04;
        txBuffer[3] = 0x07;
        rxBuffer[0] = 0;
        rxBuffer[1] = 0;
        rxBuffer[2] = 0;
        rxBuffer[3] = 0;
        SPI_transfer(txBuffer, rxBuffer, 4, &TMAG5170_SPI_CS);
        delay_cycles(MS_TO_CYCLES(TMAG5170_MODE_TRANSITION_MS));
    }

    /* Reset the sensor to ensure clean state */
    if (ret == TMAG5170_OK) {
        ret = tmag5170_reset();
    }

    /* Disable CRC again after reset (reset re-enables default settings) */
    if (ret == TMAG5170_OK && !config->crcEn) {
        txBuffer[0] = 0x0F;
        txBuffer[1] = 0x00;
        txBuffer[2] = 0x04;
        txBuffer[3] = 0x07;
        rxBuffer[0] = 0;
        rxBuffer[1] = 0;
        rxBuffer[2] = 0;
        rxBuffer[3] = 0;
        SPI_transfer(txBuffer, rxBuffer, 4, &TMAG5170_SPI_CS);
        delay_cycles(MS_TO_CYCLES(TMAG5170_MODE_TRANSITION_MS));
    }

    /* Configure DEVICE_CONFIG register (0x00) */
    if (ret == TMAG5170_OK) {
        regValue = 0;
        regValue |= (config->operatingMode & 0x07) << 4;   /* Bits 6-4: Operating mode */
        regValue |= (config->convAvg & 0x07) << 12;        /* Bits 14-12: Conversion averaging */

        ret = tmag5170_write_reg(TMAG5170_REG_DEVICE_CONFIG, regValue);
    }

    /* Configure SENSOR CONFIG Register */
    if (ret == TMAG5170_OK) {
        regValue = 0;
        regValue |= (config->chEn & 0x0F) << 6;            /* Bits 9-6: Channel enable */
        regValue |= (config->magRange & 0x03) << 4;        /* Bits 5-4: Z range */
        regValue |= (config->magRange & 0x03) << 2;        /* Bits 3-2: Y range */
        regValue |= (config->magRange & 0x03) << 0;        /* Bits 1-0: X range */

        ret = tmag5170_write_reg(TMAG5170_REG_SENSOR_CONFIG, regValue);
    }

    /* Configure SYSTEM_CONFIG register (0x02) with default settings */
    if (ret == TMAG5170_OK) {
        regValue = 0;  /* Use defaults */
        ret = tmag5170_write_reg(TMAG5170_REG_SYSTEM_CONFIG, regValue);
    }

    return ret;
}

/**
 * @brief Reset the TMAG5170 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int tmag5170_reset(void)
{
    uint16_t regValue;
    int ret;

    /* Reset via Deep Sleep mode entry/exit
     * Operating mode bits 6-4: 110b = Deep Sleep (0x6 << 4 = 0x60)
     */
    regValue = (TMAG5170_MODE_DEEP_SLEEP << 4);

    /* Enter Deep Sleep mode */
    ret = tmag5170_write_reg(TMAG5170_REG_DEVICE_CONFIG, regValue);

    if (ret == TMAG5170_OK) {
        /* Wait for deep sleep entry */
        delay_cycles(MS_TO_CYCLES(TMAG5170_MODE_TRANSITION_MS));

        /* Exit Deep Sleep mode via CS pulse
         * Per vendor reference: toggle CS to wake from deep sleep
         */
        SPI_toggleCS(&TMAG5170_SPI_CS);

        /* Wait for startup from deep sleep  */
        delay_cycles(MS_TO_CYCLES(TMAG5170_RESET_DELAY_MS));
    }

    return ret;
}

/**
 * @brief Get magnetic field raw data
 *
 * @param data Pointer to structure to store sensor data
 * @return int 0 on success, error code otherwise
 */
int tmag5170_get_data(tmag5170_data_t *data)
{
    int ret;
    uint16_t regValue;

    ret = TMAG5170_OK;

    /* Initialize data structure */
    memset(data, 0, sizeof(tmag5170_data_t));

    /* Read X channel if enabled */
    if (ret == TMAG5170_OK && (gCurrentConfig.chEn & TMAG5170_CH_X)) {
        ret = tmag5170_read_reg(TMAG5170_REG_X_CH_RESULT, &regValue);
        if (ret == TMAG5170_OK) {
            data->xRaw = regValue;  /* Extract 16-bit result */
        }
    }

    /* Read Y channel if enabled */
    if (ret == TMAG5170_OK && (gCurrentConfig.chEn & TMAG5170_CH_Y)) {
        ret = tmag5170_read_reg(TMAG5170_REG_Y_CH_RESULT, &regValue);
        if (ret == TMAG5170_OK) {
            data->yRaw = regValue;  /* Extract 16-bit result */
        }
    }

    /* Read Z channel if enabled */
    if (ret == TMAG5170_OK && (gCurrentConfig.chEn & TMAG5170_CH_Z)) {
        ret = tmag5170_read_reg(TMAG5170_REG_Z_CH_RESULT, &regValue);
        if (ret == TMAG5170_OK) {
            data->zRaw = regValue;  /* Extract 16-bit result */
        }
    }

    /* Clear data ready flag if it's configured as an interrupt */
    if (ret == TMAG5170_OK) {
        tmag5170_clear_data_ready();
        data->valid = true;
    }

    return ret;
}

/**
 * @brief Enable or disable the data ready interrupt
 *
 * @param enable true to enable data ready interrupt, false to disable
 * @return int 0 on success, error code otherwise
 */
int tmag5170_set_data_ready_interrupt(bool enable)
{
    uint16_t regValue;
    int ret;

    regValue = 0;
    if (enable) {
        regValue = (0x1) << 8;
    }

    /* Write back to ALERT_CONFIG register */
    ret = tmag5170_write_reg(TMAG5170_REG_ALERT_CONFIG, regValue);

    return ret;
}

/**
 * @brief Clear data ready interrupt status
 *
 * @return int 0 on success, error code otherwise
 */
int tmag5170_clear_data_ready(void)
{
    uint16_t regValue;
    int ret;

    /* Reading the CONV_STATUS register clears the data ready flag */
    ret = tmag5170_read_reg(TMAG5170_REG_CONV_STATUS, &regValue);

    return ret;
}

/**
 * @brief Read from a TMAG5170 register
 *
 * @param regAddr Register address
 * @param data Pointer to store the read data
 * @return int 0 on success, error code otherwise
 */
static int tmag5170_read_reg(uint8_t regAddr, uint16_t *data)
{
    uint8_t txBuffer[4];
    uint8_t rxBuffer[4];
    int ret;

    txBuffer[0] = 0;
    txBuffer[1] = 0;
    txBuffer[2] = 0;
    txBuffer[3] = 0;
    rxBuffer[0] = 0;
    rxBuffer[1] = 0;
    rxBuffer[2] = 0;
    rxBuffer[3] = 0;

    /* Set read bit and register address */
    txBuffer[0] = (regAddr & 0x7F) | TMAG5170_SPI_READ;  /* R/W bit = 1 for read */
    txBuffer[1] = 0x00;
    txBuffer[2] = 0x00;
    txBuffer[3] = 0x00;

    /* Perform SPI transfer */
    if (SPI_transfer(txBuffer, rxBuffer, 4, &TMAG5170_SPI_CS) != 0) {
        ret = TMAG5170_ERR_SPI;
    } else {
        *data = ((uint16_t)rxBuffer[1] << 8) | ((uint16_t)rxBuffer[2]);
        ret = TMAG5170_OK;
    }

    return ret;
}

/**
 * @brief Write to a TMAG5170 register
 *
 * @param regAddr Register address
 * @param data Data to write
 * @return int 0 on success, error code otherwise
 */
static int tmag5170_write_reg(uint8_t regAddr, uint16_t data)
{
    uint8_t txBuffer[4];
    uint8_t rxBuffer[4];
    int ret;

    txBuffer[0] = 0;
    txBuffer[1] = 0;
    txBuffer[2] = 0;
    txBuffer[3] = 0;
    rxBuffer[0] = 0;
    rxBuffer[1] = 0;
    rxBuffer[2] = 0;
    rxBuffer[3] = 0;

    /* Set write bit and register address (per vendor reference code) */
    txBuffer[0] = (regAddr & 0x7F) | TMAG5170_SPI_WRITE;  /* R/W bit = 0 for write */

    /* Set 16-bit data in bytes 1-2 */
    txBuffer[1] = (data >> 8) & 0xFF;   /* Data MSB */
    txBuffer[2] = data & 0xFF;          /* Data LSB */
    txBuffer[3] = 0x00;                 /* CMD bits and CRC (not used when CRC disabled) */

    /* Perform SPI transfer */
    if (SPI_transfer(txBuffer, rxBuffer, 4, &TMAG5170_SPI_CS) != 0) {
        ret = TMAG5170_ERR_SPI;
    } else {
        ret = TMAG5170_OK;
    }

    return ret;
}
