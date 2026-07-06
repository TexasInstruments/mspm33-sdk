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

#include "sensor_BMP384.h"
#include "comm/i2c_comm.h"
#include <string.h>
#include <ti/driverlib/driverlib.h>


/* Number of CPU cycles for 1ms delay (MSPM33 @ 160 MHz) */
#define CYCLES_PER_MS          (160000)

/* Macro to convert duration (ms) to hardware clock ticks */
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

/* Calibration parameter NVM register base address — starts at T1 */
#define BMP384_REG_NVM_CALIB_START  (0x31)
#define BMP384_NVM_CALIB_LEN        (21U)   /* 0x31 to 0x45 inclusive */

/* Internal state */
static struct {
    bmp384_config_t config;
    bmp384_calib_t calib;
} gBmp384State;

/* Measurement time in milliseconds for each OSR setting */
static const uint16_t bmp384_meas_time_ms[] = {
    1,   /* OSR x1 */
    2,   /* OSR x2 */
    4,   /* OSR x4 */
    8,   /* OSR x8 */
    16,  /* OSR x16 */
    32   /* OSR x32 */
};

/**
 * @brief Write data to a BMP384 register
 *
 * @param reg Register address
 * @param data Data to write
 * @return int 0 on success, error code otherwise
 */
static int bmp384_write_reg(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    return I2c_Comm_Transfer(I2C_TARGET_ADDR_BMP384, buf, 2, I2C_HAL_TRANSFER_OPTION_CONTROLLER_TX_DEFAULT);
}

/**
 * @brief Read data from a BMP384 register
 *
 * @param reg Register address
 * @param data Pointer to store the data
 * @param len Number of bytes to read
 * @return int 0 on success, error code otherwise
 */
static int bmp384_read_reg(uint8_t reg, uint8_t *data, uint8_t len) {
    int ret;

    /* Set register pointer */
    ret = I2c_Comm_Transfer(I2C_TARGET_ADDR_BMP384, &reg, 1, I2C_HAL_TRANSFER_OPTION_CONTROLLER_TX_DEFAULT);
    if (ret == BMP384_OK) {
        /* Read register data */
        ret = I2c_Comm_Transfer(I2C_TARGET_ADDR_BMP384, data, len, I2C_HAL_TRANSFER_OPTION_CONTROLLER_RX_DEFAULT);
        if (ret != BMP384_OK) {
            ret = BMP384_ERR_COMM;
        }
    } else {
        ret = BMP384_ERR_COMM;
    }

    return ret;
}

/**
 * @brief Read BMP384 calibration coefficients from NVM
 *
 * @return int 0 on success, error code otherwise
 */
static int bmp384_read_calibration(void) {
    uint8_t calib_data[BMP384_NVM_CALIB_LEN];
    int ret;

    /* Read all trim registers 0x31–0x45 in one burst */
    ret = bmp384_read_reg(BMP384_REG_NVM_CALIB_START, calib_data, BMP384_NVM_CALIB_LEN);
    if (ret == BMP384_OK) {
        /* Temperature trim: 0x31–0x35 (offsets 0–4) */
        gBmp384State.calib.nvm_par_t1  = (uint16_t)calib_data[1] << 8 | calib_data[0];
        gBmp384State.calib.nvm_par_t2  = (uint16_t)calib_data[3] << 8 | calib_data[2];
        gBmp384State.calib.nvm_par_t3  = (int8_t)calib_data[4];
        /* Pressure trim: 0x36–0x45 (offsets 5–20) */
        gBmp384State.calib.nvm_par_p1  = (int16_t)((uint16_t)calib_data[6]  << 8 | calib_data[5]);
        gBmp384State.calib.nvm_par_p2  = (int16_t)((uint16_t)calib_data[8]  << 8 | calib_data[7]);
        gBmp384State.calib.nvm_par_p3  = (int8_t)calib_data[9];
        gBmp384State.calib.nvm_par_p4  = (int8_t)calib_data[10];
        gBmp384State.calib.nvm_par_p5  = (uint16_t)calib_data[12] << 8 | calib_data[11];
        gBmp384State.calib.nvm_par_p6  = (uint16_t)calib_data[14] << 8 | calib_data[13];
        gBmp384State.calib.nvm_par_p7  = (int8_t)calib_data[15];
        gBmp384State.calib.nvm_par_p8  = (int8_t)calib_data[16];
        gBmp384State.calib.nvm_par_p9  = (int16_t)((uint16_t)calib_data[18] << 8 | calib_data[17]);
        gBmp384State.calib.nvm_par_p10 = (int8_t)calib_data[19];
        gBmp384State.calib.nvm_par_p11 = (int8_t)calib_data[20];
        gBmp384State.calib.t_lin       = 0.0;
    }

    return ret;
}

/**
 * @brief Compensate raw 24-bit temperature ADC and store t_lin
 *
 * Must be called before bmp384_compensate_pressure(). t_lin is the
 * linearised temperature used internally in the pressure formula.
 * Per BMP384 datasheet Section 9.2 (double-precision reference code).
 */

 /*
 
 static void bmp384_compensate_temperature(uint32_t adc_t) {
    double par_t1 = (double)gBmp384State.calib.nvm_par_t1 * 256.0;
    double par_t2 = (double)gBmp384State.calib.nvm_par_t2 / 1073741824.0;
    double par_t3 = (double)gBmp384State.calib.nvm_par_t3 / 281474976710656.0;
    double pd1    = (double)adc_t - par_t1;
    double pd2    = pd1 * par_t2;

    gBmp384State.calib.t_lin = pd2 + (pd1 * pd1) * par_t3;
 }
 */




/**
 * @brief Compensate raw 24-bit pressure ADC to Pascals
 *
 * @param adc_p Raw 24-bit pressure ADC value
 * @return uint32_t Compensated pressure in Pa (~30000–120000 at sea level)
 *
 * Per BMP384 datasheet Section 9.3 (double-precision reference code).
 * bmp384_compensate_temperature() must be called first to populate t_lin.
 */

 // USED IF COMPENSATED VALUES ARE REQUIRED
 
 /*
 static uint32_t bmp384_compensate_pressure(uint32_t adc_p) {
    double t_lin    = gBmp384State.calib.t_lin;
    double par_p1   = ((double)gBmp384State.calib.nvm_par_p1 - 16384.0) / 1048576.0;
    double par_p2   = ((double)gBmp384State.calib.nvm_par_p2 - 16384.0) / 536870912.0;
    double par_p3   = (double)gBmp384State.calib.nvm_par_p3  / 4294967296.0;
    double par_p4   = (double)gBmp384State.calib.nvm_par_p4  / 137438953472.0;
    double par_p5   = (double)gBmp384State.calib.nvm_par_p5  * 8.0;
    double par_p6   = (double)gBmp384State.calib.nvm_par_p6  / 64.0;
    double par_p7   = (double)gBmp384State.calib.nvm_par_p7  / 256.0;
    double par_p8   = (double)gBmp384State.calib.nvm_par_p8  / 32768.0;
    double par_p9   = (double)gBmp384State.calib.nvm_par_p9  / 281474976710656.0;
    double par_p10  = (double)gBmp384State.calib.nvm_par_p10 / 281474976710656.0;
    double par_p11  = (double)gBmp384State.calib.nvm_par_p11 / 36893488147419103232.0;
    double uncomp   = (double)adc_p;
    double pd1, pd2, pd3, pd4, pout1, pout2;

    pd1   = par_p6 * t_lin;
    pd2   = par_p7 * (t_lin * t_lin);
    pd3   = par_p8 * (t_lin * t_lin * t_lin);
    pout1 = par_p5 + pd1 + pd2 + pd3;

    pd1   = par_p2 * t_lin;
    pd2   = par_p3 * (t_lin * t_lin);
    pd3   = par_p4 * (t_lin * t_lin * t_lin);
    pout2 = uncomp * (par_p1 + pd1 + pd2 + pd3);

    pd1 = uncomp * uncomp;
    pd2 = par_p9 + par_p10 * t_lin;
    pd3 = pd1 * pd2;
    pd4 = pd3 + (uncomp * uncomp * uncomp) * par_p11;

    return (uint32_t)(pout1 + pout2 + pd4);
}


 */

/**
 * @brief Initialize the BMP384 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmp384_init(const bmp384_config_t *config) {
    uint8_t chip_id;
    int ret;

    /* Read chip ID to verify communication */
    ret = bmp384_read_reg(BMP384_REG_CHIP_ID, &chip_id, 1);
    if (ret == BMP384_OK) {
        /* Verify chip ID */
        if (chip_id == BMP384_CHIP_ID) {
            /* Soft reset the sensor */
            ret = bmp384_reset();
            if (ret == BMP384_OK) {
                /* Wait 10ms for reset to complete */
                DL_Common_delayCycles(MS_TO_CYCLES(10));

                /* Read calibration coefficients from NVM */
                ret = bmp384_read_calibration();
            }

            /* Configure the sensor */
            if (ret == BMP384_OK) {
                ret = bmp384_configure(config);
            }
        } else {
            ret = BMP384_ERR_INVALID_ID;
        }
    }

    return ret;
}

/**
 * @brief Reset the BMP384 sensor
 *
 * @return int 0 on success, error code otherwise
 */
int bmp384_reset(void) {
    return bmp384_write_reg(BMP384_REG_CMD, BMP384_CMD_SOFT_RESET);
}

/**
 * @brief Configure the BMP384 sensor
 *
 * @param config Pointer to configuration structure
 * @return int 0 on success, error code otherwise
 */
int bmp384_configure(const bmp384_config_t *config) {
    int ret;
    uint8_t pwr_ctrl;
    uint8_t osr;
    uint8_t odr;
    uint8_t config_reg;
    uint8_t int_ctrl;

    /* Store configuration */
    memcpy(&gBmp384State.config, config, sizeof(bmp384_config_t));
    ret = BMP384_OK;

    /* Configure power control — always enable temperature (bit 1) because
     * t_lin from temperature ADC is required for pressure compensation. */
    pwr_ctrl = ((config->pressEn ? 1 : 0) << 0) |
               (1U << 1) |
               ((config->mode & 0x03) << 4);
    ret = bmp384_write_reg(BMP384_REG_PWR_CTRL, pwr_ctrl);

    /* Configure oversampling */
    if (ret == BMP384_OK) {
        osr = ((config->pressOsr & 0x07) << 0);
        ret = bmp384_write_reg(BMP384_REG_OSR, osr);
    }

    /* Configure output data rate */
    if (ret == BMP384_OK) {
        odr = config->odr & 0x1F;
        ret = bmp384_write_reg(BMP384_REG_ODR, odr);
    }

    /* Configure filter */
    if (ret == BMP384_OK) {
        config_reg = (config->filter & 0x07) << 1;
        ret = bmp384_write_reg(BMP384_REG_CONFIG, config_reg);
    }

    /* Configure interrupts if needed */
    if (ret == BMP384_OK && config->intMode != BMP384_INT_DISABLE) {
        int_ctrl = 0;

        /* Configure interrupt sources */
        if (config->intMode & BMP384_INT_DRDY) {
            int_ctrl |= (1 << 6);  /* drdy_en */
        }
        if (config->intMode & BMP384_INT_FIFO_WM) {
            int_ctrl |= (1 << 3);  /* fwtm_en */
        }
        if (config->intMode & BMP384_INT_FIFO_FULL) {
            int_ctrl |= (1 << 4);  /* ffull_en */
        }

        /* Configure interrupt pin properties */
        int_ctrl |= (1 << 2);  /* int_level (active high) */

        ret = bmp384_write_reg(BMP384_REG_INT_CTRL, int_ctrl);
    }

    return ret;
}

/**
 * @brief Get pressure data
 *
 * @param data Pointer to data structure to store the readings
 * @return int 0 on success, error code otherwise
 */
int bmp384_get_data(bmp384_data_t *data) {
    uint8_t raw_data[6];
    uint8_t status;
    uint16_t wait_time;
    uint32_t adc_p;
    uint32_t adc_t;
    int ret;

    /* Initialize data as invalid */
    data->valid = false;
    ret = BMP384_OK;

    /* If in forced mode, trigger a measurement */
    if (gBmp384State.config.mode == BMP384_MODE_FORCED) {
        ret = bmp384_trigger_measurement();
        if (ret == BMP384_OK) {
            /* Wait for measurement to complete */
            wait_time = bmp384_meas_time_ms[gBmp384State.config.pressOsr];
            DL_Common_delayCycles(MS_TO_CYCLES(wait_time));
        }
    }

    /* Check if data is ready */
    if (ret == BMP384_OK) {
        ret = bmp384_get_status(&status);
    }

    /* Verify data ready status */
    if (ret == BMP384_OK) {
        if (!(status & 0x60)) {  /* Check drdy_press and drdy_temp bits */
            ret = BMP384_ERR_INVALID_DATA;
        }
    }

    /* Read raw pressure and temperature data */
    if (ret == BMP384_OK) {
        ret = bmp384_read_reg(BMP384_REG_DATA, raw_data, 6);
    }

    /* Extract 24-bit pressure and temperature ADC values per BMP384 datasheet.
     * 0x04 PRESS_XLSB = bits [7:0], 0x05 PRESS_LSB = bits [15:8], 0x06 PRESS_MSB = bits [23:16]
     * 0x07 TEMP_XLSB  = bits [7:0], 0x08 TEMP_LSB  = bits [15:8], 0x09 TEMP_MSB  = bits [23:16]
     */
    if (ret == BMP384_OK) {
        adc_p = ((uint32_t)raw_data[2] << 16) | ((uint32_t)raw_data[1] << 8) | raw_data[0];
        adc_t = ((uint32_t)raw_data[5] << 16) | ((uint32_t)raw_data[4] << 8) | raw_data[3];
        (void)adc_t;

        // USED IF COMPENSATED VALUES ARE REQUIRED
        //bmp384_compensate_temperature(adc_t);
        //data->pressure = bmp384_compensate_pressure(adc_p);
        data->pressure = adc_p;
        data->valid = true;
    }

    return ret;
}

/**
 * @brief Trigger a single measurement (only in forced mode)
 *
 * @return int 0 on success, error code otherwise
 */
int bmp384_trigger_measurement(void) {
    uint8_t pwr_ctrl;
    int ret;

    /* Read current power control register */
    ret = bmp384_read_reg(BMP384_REG_PWR_CTRL, &pwr_ctrl, 1);

    /* Set mode to forced mode while preserving other bits */
    if (ret == BMP384_OK) {
        pwr_ctrl &= ~(0x03 << 4);  /* Clear mode bits */
        pwr_ctrl |= (BMP384_MODE_FORCED & 0x03) << 4;  /* Set forced mode */

        /* Write updated power control register */
        ret = bmp384_write_reg(BMP384_REG_PWR_CTRL, pwr_ctrl);
    }

    return ret;
}

/**
 * @brief Get sensor status
 *
 * @param status Pointer to store the status
 * @return int 0 on success, error code otherwise
 */
int bmp384_get_status(uint8_t *status) {
    return bmp384_read_reg(BMP384_REG_STATUS, status, 1);
}
