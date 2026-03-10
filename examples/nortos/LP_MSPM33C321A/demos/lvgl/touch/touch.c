/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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

#include "touch.h"
#include "../ti_msp_dl_config.h"

#include "../spi/delay.h"
#include "../lcd/lcd.h"
#include "string.h"

uint8_t  Touch_state;
uint16_t Touch_point_x;
uint16_t Touch_point_y;

uint8_t TP_Register_Write(uint16_t Reg, uint8_t *Buffer, uint8_t Len)
{
    DL_I2CC_transmitData(I2C_INST, Reg & 0XFF);
    if (Len == DL_I2CC_fillTXFIFO(I2C_INST, &Buffer[0], Len)) {
        /* Wait for I2C to be Idle */
        while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
            ;
        DL_I2CC_startTransfer(
            I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_TX, Len + 1);
        /* Poll until the Controller writes all bytes */
        while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY)
            ;
        return 0;
    } else
        return 1;
}

void TP_Register_Read(uint16_t Reg, uint8_t *Buffer, uint8_t Len)
{
    uint8_t i;
    DL_I2CC_transmitData(I2C_INST, Reg & 0XFF);
    /* Wait for I2C to be Idle */
    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
        ;
    DL_I2CC_startTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_TX, 1);
    /* Poll until the Controller writes all bytes */
    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY)
        ;
    /* Wait for I2C to be Idle */
    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
        ;
    /* Send a read request to Target */
    DL_I2CC_startTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_RX, Len);
    /*
     * Receive all bytes from target
     */
    for (i = 0; i < Len; i++) {
        while (DL_I2CC_isRXFIFOEmpty(I2C_INST))
            ;
        Buffer[i] = DL_I2CC_receiveData(I2C_INST);
    }
}

void TP_Init(void)
{
    DL_GPIO_clearPins(CTP_PORT, CTP_RST_PIN);
    delay_ms(10);
    DL_GPIO_setPins(CTP_PORT, CTP_RST_PIN);
    delay_ms(500);
}

uint8_t TP_Scan(void)
{
    uint8_t buffer[4];
    static uint8_t t = 0;
    t++;
    if ((t % TP_DETECT_PERIOD) == 0 || t < TP_DETECT_INITIAL)
    {
        TP_Register_Read(TOUCH_STATUS_REG, &Touch_state, 1);
        if(Touch_state)
        {
            TP_Register_Read(TOUCH_LOCATION_REG, buffer, 4);
            Touch_point_y = gLcdHeight - (((uint16_t)(buffer[0] & 0x0F) << 8) + buffer[1]);
            Touch_point_x = ((uint16_t)(buffer[2] & 0x0F) << 8) + buffer[3];
            if(Touch_point_x == 0 && Touch_point_y == 0)
                return 0;
            t = 0;
            return 1;
        }
    }
    if (t > TP_DETECT_MAX) t = TP_DETECT_INITIAL;
    return 0;
}
