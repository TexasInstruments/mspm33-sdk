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

#include "sensor_OPT4001.h"
#include "comm/i2c_comm.h"
#include <ti/driverlib/driverlib.h>

#define OPT4001_I2C_ADDR         (0x44U)
#define OPT4001_REG_RESULT       (0x00U)
#define OPT4001_REG_CONFIG       (0x0AU)
#define OPT4001_REG_DEVICE_ID    (0x11U)
#define OPT4001_DEVICE_ID        (0x121U)
#define OPT4001_CONV_TIME_MS     (13U)
#define CYCLES_PER_MS            (160000U)
#define MS_TO_CYCLES(ms)         ((ms) * (CYCLES_PER_MS))

volatile uint8_t alsDataBytes[2] = {0};

static int opt4001_write_reg(uint8_t reg, uint16_t data)
{
    uint8_t buf[3];

    buf[0] = reg;
    buf[1] = (uint8_t)(data >> 8);
    buf[2] = (uint8_t)(data & 0xFFU);
    return I2c_Comm_Transfer(OPT4001_I2C_ADDR, buf, 3U, I2C_HAL_TRANSFER_OPTION_CONTROLLER_TX_DEFAULT);
}

static int opt4001_read_reg(uint8_t reg, uint16_t *data)
{
    uint8_t buf[2];
    int status;

    status = -1;

    if (I2c_Comm_Transfer(OPT4001_I2C_ADDR, &reg, 1U, I2C_HAL_TRANSFER_OPTION_CONTROLLER_TX_DEFAULT) == 0) {
        if (I2c_Comm_Transfer(OPT4001_I2C_ADDR, buf, 2U, I2C_HAL_TRANSFER_OPTION_CONTROLLER_RX_DEFAULT) == 0) {
            *data = ((uint16_t)buf[0] << 8) | buf[1];
            status = 0;
        } else {
            status = -2;
        }
    } else {
        status = -1;
    }

    return status;
}

int Sensor_Opt4001_Init(void)
{
    uint16_t device_id;
    uint16_t config;
    int status;

    status = -1;

    if (opt4001_read_reg(OPT4001_REG_DEVICE_ID, &device_id) == 0) {
        if ((device_id & 0x0FFFU) == OPT4001_DEVICE_ID) {
            config = 0;
            config |= (0xC << 10);
            config |= (0x5 << 6);
            config |= (0x3 << 4);

            if (opt4001_write_reg(OPT4001_REG_CONFIG, config) == 0) {
                status = 0;
            } else {
                status = -3;
            }
        } else {
            status = -2;
        }
    } else {
        status = -1;
    }

    return status;
}

int Sensor_Opt4001_ReadAmbientLight(void)
{
    uint16_t result;
    uint8_t exponent;
    uint16_t mantissa;
    float lux;
    uint16_t lux_scaled;
    int status;

    status = 0;

    DL_Common_delayCycles(MS_TO_CYCLES(OPT4001_CONV_TIME_MS));

    if (opt4001_read_reg(OPT4001_REG_RESULT, &result) == 0) {
        exponent = (result >> 12) & 0xFU;
        mantissa = result & 0xFFFU;

        lux = (float)mantissa * (1U << exponent) / 100.0f;
        lux_scaled = (uint16_t)(lux * 100.0f);

        alsDataBytes[0] = (uint8_t)((lux_scaled >> 8) & 0xFF);
        alsDataBytes[1] = (uint8_t)(lux_scaled & 0xFF);

        status = 0;
    } else {
        status = -1;
    }

    return status;
}
