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

#include "sensor.h"
#include <ti/driverlib/driverlib.h>
#include "app_cfg.h"
#include "humidity_temperature/sensor_HDC3020.h"

/* CPU cycles per millisecond (CPUCLK = 160 MHz per SysConfig) */
#define CYCLES_PER_MS          (160000U)
#define MS_TO_CYCLES(ms)       ((ms) * (CYCLES_PER_MS))

/* Sensor data buffers */
static uint8_t pressDataBytes[4];

volatile uint8_t gDataReady_Acc_Gyro   = 0;
volatile uint8_t gDataReady_HallEffect = 0;

int Sensor_Init(void)
{
    /* I2S GPIO init for digital microphone */
    DL_GPIO_initPeripheralInputFunction(GPIO_I2S_0_IOMUX_AD0, GPIO_I2S_0_IOMUX_AD0_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_I2S_0_IOMUX_BCLK, GPIO_I2S_0_IOMUX_BCLK_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_I2S_0_IOMUX_WCLK, GPIO_I2S_0_IOMUX_WCLK_FUNC);
    DL_I2S_enableWBCLKGeneration(I2S_0_INST);

    /* Enable interrupts */
    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
    NVIC_ClearPendingIRQ(I2C_INST_INT_IRQN);
    NVIC_EnableIRQ(I2C_INST_INT_IRQN);

    return 0;
}

/* ------------------------------------------------------------------ */
/* Per-sensor capture functions                                         */
/* ------------------------------------------------------------------ */

static void Sensor_PIR_captureData(uint32_t numSamples)
{
    Sensor_Pir_DataType pirData;

    Sensor_Pir_Init();
    DL_Timer_startCounter(TIMER_0_INST);

    while (numSamples) {
        Sensor_Pir_GetData(&pirData);
        Dap_Interface_StreamRaw(&pirData.motion_level, 1U);
        numSamples--;
        DL_Common_delayCycles(MS_TO_CYCLES(10));
    }

    DL_Timer_stopCounter(TIMER_0_INST);
}

static void Sensor_Temperature_captureData(uint32_t numSamples)
{
    hdc3020_config_t config;
    hdc3020_data_t data;
    uint8_t buf[2];
    int ret;

    config.mode = HDC3020_MODE_TRIGGER_ON_DEMAND;
    config.lpm = HDC3020_LPM_0;
    config.rate = HDC3020_RATE_1HZ;
    ret = hdc3020_init(&config);

    if (ret == HDC3020_OK) {
        while (numSamples) {
            ret = hdc3020_get_data(&data, HDC3020_SENSOR_TEMPERATURE);
            if (ret == HDC3020_OK && data.valid) {
                buf[0] = (uint8_t)((data.temperature >> 8) & 0xFF);
                buf[1] = (uint8_t)(data.temperature & 0xFF);
                Dap_Interface_StreamRaw(buf, 2U);
                numSamples--;
            }
            DL_Common_delayCycles(MS_TO_CYCLES(100));
        }
    }
}

static void Sensor_Humidity_captureData(uint32_t numSamples)
{
    hdc3020_config_t config;
    hdc3020_data_t data;
    uint8_t buf[2];
    int ret;

    config.mode = HDC3020_MODE_TRIGGER_ON_DEMAND;
    config.lpm = HDC3020_LPM_0;
    config.rate = HDC3020_RATE_1HZ;
    ret = hdc3020_init(&config);

    if (ret == HDC3020_OK) {
        while (numSamples) {
            ret = hdc3020_get_data(&data, HDC3020_SENSOR_HUMIDITY);
            if (ret == HDC3020_OK && data.valid) {
                buf[0] = (uint8_t)((data.humidity >> 8) & 0xFF);
                buf[1] = (uint8_t)(data.humidity & 0xFF);
                Dap_Interface_StreamRaw(buf, 2U);
                numSamples--;
            }
            DL_Common_delayCycles(MS_TO_CYCLES(100));
        }
    }
}

static void Sensor_AmbientLight_captureData(uint32_t numSamples)
{
    int ret;

    ret = Sensor_Opt4001_Init();

    if (ret == 0) {
        while (numSamples) {
            if (Sensor_Opt4001_ReadAmbientLight() == 0) {
                Dap_Interface_StreamRaw((const uint8_t *)alsDataBytes, 2U);
                numSamples--;
            }
            DL_Common_delayCycles(MS_TO_CYCLES(100));
        }
    }
}

static void Sensor_Pressure_captureData(uint32_t numSamples)
{
    bmp384_config_t config;
    bmp384_data_t   data;
    uint32_t pressure_pa;
    int ret;

    config.mode     = BMP384_MODE_NORMAL;
    config.pressOsr = BMP384_OSR_x4;
    config.odr      = BMP384_ODR_50HZ;
    config.filter   = BMP384_FILTER_3;
    config.pressEn  = true;
    config.intMode  = BMP384_INT_DISABLE;

    ret = bmp384_init(&config);
    if (ret == BMP384_OK) {
        while (numSamples) {
            ret = bmp384_get_data(&data);
            if (ret == BMP384_OK && data.valid) {
                pressure_pa = data.pressure;

                pressDataBytes[0] = (uint8_t)((pressure_pa >> 24) & 0xFFU);
                pressDataBytes[1] = (uint8_t)((pressure_pa >> 16) & 0xFFU);
                pressDataBytes[2] = (uint8_t)((pressure_pa >>  8) & 0xFFU);
                pressDataBytes[3] = (uint8_t)( pressure_pa        & 0xFFU);
                Dap_Interface_StreamRaw((const uint8_t *)pressDataBytes, 4U);
                numSamples--;
            }
            DL_Common_delayCycles(MS_TO_CYCLES(20));
        }
    }
}

static void Sensor_Accelerometer_captureData(uint32_t numSamples)
{
    bmi270_config_t config;
    bmi270_accel_t data;
    uint8_t buf[6];
    uint32_t timeout;
    int ret;

    config.power_mode      = BMI270_PWR_MODE_NORMAL;
    config.sensor_enable   = BMI270_SENSOR_ACC;
    config.acc_range       = BMI270_ACC_RANGE_8G;
    config.acc_odr         = BMI270_ODR_100HZ;
    config.acc_bwp         = BMI270_BWP_NORMAL;
    config.acc_filter_perf = true;

    ret = bmi270_init(&config);
    if (ret != BMI270_OK) {
        return;
    }

    ret = bmi270_configure_interrupt(BMI270_INT_DATA_RDY, BMI270_INT_PIN_1, true);
    if (ret == BMI270_OK) {
        while (numSamples) {
            gDataReady_Acc_Gyro = 0;
            timeout = 20000000U;
            while (!gDataReady_Acc_Gyro && timeout) { timeout--; }
            if (!timeout) { break; }

            if (bmi270_get_accel(&data) == BMI270_OK) {
                buf[0] = (uint8_t)((data.x >> 8) & 0xFF);
                buf[1] = (uint8_t)(data.x & 0xFF);
                buf[2] = (uint8_t)((data.y >> 8) & 0xFF);
                buf[3] = (uint8_t)(data.y & 0xFF);
                buf[4] = (uint8_t)((data.z >> 8) & 0xFF);
                buf[5] = (uint8_t)(data.z & 0xFF);
                Dap_Interface_StreamRaw(buf, 6U);
                numSamples--;
            }
        }
    }
}

static void Sensor_Gyro_captureData(uint32_t numSamples)
{
    bmi270_config_t config;
    bmi270_gyro_t data;
    uint8_t buf[6];
    uint32_t timeout;
    int ret;

    config.power_mode      = BMI270_PWR_MODE_NORMAL;
    config.sensor_enable   = BMI270_SENSOR_GYR;
    config.gyr_range       = BMI270_GYR_RANGE_2000DPS;
    config.gyr_odr         = BMI270_ODR_100HZ;
    config.gyr_bwp         = BMI270_BWP_NORMAL;
    config.gyr_filter_perf = true;
    config.gyr_noise_perf  = false;

    ret = bmi270_init(&config);
    if (ret == BMI270_OK) {
        ret = bmi270_configure_interrupt(BMI270_INT_DATA_RDY, BMI270_INT_PIN_1, true);
        if (ret == BMI270_OK) {
            while (numSamples) {
                gDataReady_Acc_Gyro = 0;
                timeout = 20000000U;
                while (!gDataReady_Acc_Gyro && timeout) { timeout--; }
                if (!timeout) { break; }

                if (bmi270_get_gyro(&data) == BMI270_OK) {
                    buf[0] = (uint8_t)((data.x >> 8) & 0xFF);
                    buf[1] = (uint8_t)(data.x & 0xFF);
                    buf[2] = (uint8_t)((data.y >> 8) & 0xFF);
                    buf[3] = (uint8_t)(data.y & 0xFF);
                    buf[4] = (uint8_t)((data.z >> 8) & 0xFF);
                    buf[5] = (uint8_t)(data.z & 0xFF);
                    Dap_Interface_StreamRaw(buf, 6U);
                    numSamples--;
                }
            }
        }
    }
}

static void Sensor_MagneticField_captureData(uint32_t numSamples)
{
    tmag5170_config_t config;
    tmag5170_data_t data;
    uint8_t buf[6];
    uint32_t timeout;
    int ret;

    config.operatingMode = TMAG5170_MODE_ACTIVE_MEASURE;
    config.magRange      = TMAG5170_RANGE_50mT;
    config.chEn          = TMAG5170_CH_XYZ;
    config.convAvg       = TMAG5170_CONV_AVG_4;
    config.dataRate      = 2;
    config.crcEn         = false;
    config.lowPower      = false;

    ret = tmag5170_init(&config);
    if (ret == TMAG5170_OK) {
        ret = tmag5170_set_data_ready_interrupt(true);
        if (ret == TMAG5170_OK) {
            DL_Common_delayCycles(MS_TO_CYCLES(10));

            while (numSamples) {
                gDataReady_HallEffect = 0;
                timeout = 20000000U;
                while (!gDataReady_HallEffect && timeout) { timeout--; }
                if (!timeout) { break; }

                if (tmag5170_get_data(&data) == TMAG5170_OK && data.valid) {
                    buf[0] = (uint8_t)((data.xRaw >> 8) & 0xFF);
                    buf[1] = (uint8_t)(data.xRaw & 0xFF);
                    buf[2] = (uint8_t)((data.yRaw >> 8) & 0xFF);
                    buf[3] = (uint8_t)(data.yRaw & 0xFF);
                    buf[4] = (uint8_t)((data.zRaw >> 8) & 0xFF);
                    buf[5] = (uint8_t)(data.zRaw & 0xFF);
                    Dap_Interface_StreamRaw(buf, 6U);
                    numSamples--;
                }
            }
            DL_Common_delayCycles(MS_TO_CYCLES(10));
        }
    }
}

static void Sensor_DigitalMic_captureData(uint32_t numSamples)
{
    /* Enable I2S */
    DL_I2S_enable(I2S_0_INST);

    uint32_t rxData;
    uint8_t *ptr;

    while (numSamples) {
        rxData = DL_I2S_receiveDataBlocking32(I2S_0_INST);
        ptr = (uint8_t *)&rxData;
        uint8_t buf[4];
        buf[0] = ptr[3];
        buf[1] = ptr[2];
        buf[2] = ptr[1];
        buf[3] = ptr[0];
        Dap_Interface_StreamRaw(buf, 4U);
        numSamples--;
    }
}

/* ------------------------------------------------------------------ */

void Sensor_AcquireSamples(uint8_t index, uint32_t numSamples)
{
    switch ((SensorIndex)index) {
        case SENSOR_IDX_PIR:
            Sensor_PIR_captureData(numSamples);
            break;
        case SENSOR_IDX_TEMP:
            Sensor_Temperature_captureData(numSamples);
            break;
        case SENSOR_IDX_HUMID:
            Sensor_Humidity_captureData(numSamples);
            break;
        case SENSOR_IDX_PRESS:
            Sensor_Pressure_captureData(numSamples);
            break;
        case SENSOR_IDX_ALS:
            Sensor_AmbientLight_captureData(numSamples);
            break;
        case SENSOR_IDX_ACCEL:
            Sensor_Accelerometer_captureData(numSamples);
            break;
        case SENSOR_IDX_GYRO:
            Sensor_Gyro_captureData(numSamples);
            break;
        case SENSOR_IDX_MAG:
            Sensor_MagneticField_captureData(numSamples);
            break;
        case SENSOR_IDX_DIGMIC:
            Sensor_DigitalMic_captureData(numSamples);
            break;
        default:
            break;
    }
}
