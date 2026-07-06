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

#ifndef SENSOR_HDC3020_H_
#define SENSOR_HDC3020_H_

#include <stdint.h>
#include <stdbool.h>

#define I2C_TARGET_ADDR_HDC3020         (0x46)

typedef enum {
    HDC3020_LPM_0 = 0,
    HDC3020_LPM_1 = 1,
    HDC3020_LPM_2 = 2,
    HDC3020_LPM_3 = 3
} hdc3020_lpm_t;

typedef enum {
    HDC3020_RATE_0_5HZ = 0,
    HDC3020_RATE_1HZ   = 1,
    HDC3020_RATE_2HZ   = 2,
    HDC3020_RATE_4HZ   = 3,
    HDC3020_RATE_10HZ  = 4
} hdc3020_rate_t;

typedef enum {
    HDC3020_MODE_TRIGGER_ON_DEMAND = 0,
    HDC3020_MODE_AUTO_MEASUREMENT  = 1
} hdc3020_mode_t;

typedef enum {
    HDC3020_SENSOR_BOTH = 0,
    HDC3020_SENSOR_TEMPERATURE,
    HDC3020_SENSOR_HUMIDITY
} hdc3020_sensor_t;

typedef struct {
    uint16_t temperature;
    uint16_t humidity;
    bool valid;
} hdc3020_data_t;

typedef struct {
    hdc3020_mode_t mode;
    hdc3020_lpm_t lpm;
    hdc3020_rate_t rate;
} hdc3020_config_t;

#define HDC3020_CMD_SOFT_RESET                  0x30A2
#define HDC3020_CMD_READ_MEASUREMENT            0xE000
#define HDC3020_CMD_READ_RH_ONLY                0xE001
#define HDC3020_CMD_EXIT_AUTO_MODE              0x3093

#define HDC3020_OK                  0
#define HDC3020_ERR_I2C            -1
#define HDC3020_ERR_CRC            -2
#define HDC3020_ERR_INVALID_DATA   -3

int hdc3020_init(const hdc3020_config_t *config);
int hdc3020_reset(void);
int hdc3020_get_data(hdc3020_data_t *data, hdc3020_sensor_t sensor_type);
int hdc3020_trigger_measurement(hdc3020_lpm_t lpm);
int hdc3020_set_mode(hdc3020_mode_t mode, hdc3020_lpm_t lpm, hdc3020_rate_t rate);
uint8_t hdc3020_calculate_crc(const uint8_t *data, uint8_t length);

#endif /* SENSOR_HDC3020_H_ */
