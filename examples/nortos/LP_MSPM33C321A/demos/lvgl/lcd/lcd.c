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

#include "lcd.h"
#include "../spi/spi.h"
#include "../spi/delay.h"
#include "../lvgl.h"
#include "stdlib.h"

uint16_t gLcdWidth = LCD_Height;
uint16_t gLcdHeight = LCD_Width;

void LCD_Command_Byte_send(uint8_t data)
{
    DL_GPIO_clearPins(LCD_SDC_PORT, LCD_SDC_PIN);
    SPI_WriteByte(data);
}

void LCD_Data_Byte_send(uint8_t data)
{
    DL_GPIO_setPins(LCD_SDC_PORT, LCD_SDC_PIN);
    SPI_WriteByte(data);
}

uint8_t LCD_Data_Byte_receive(void)
{
    uint8_t data;
    DL_GPIO_setPins(LCD_SDC_PORT, LCD_SDC_PIN);
    data = SPI_WriteByte(0xFF);
    return data;
}

void LCD_Register_Write(uint8_t Reg, uint16_t RegValue)
{
    LCD_Command_Byte_send(Reg);
    LCD_Data_Byte_send(RegValue);
}

uint8_t LCD_Register_Read(uint8_t Reg)
{
    LCD_Command_Byte_send(Reg);
    return LCD_Data_Byte_receive();
}

void LCD_Reset(void)
{
    DL_GPIO_clearPins(LCD_RESET_PORT, LCD_RESET_PIN);
    delay_ms(200);
    DL_GPIO_setPins(LCD_RESET_PORT, LCD_RESET_PIN);
    delay_ms(200);
}

void LCD_Init(void)
{
    LCD_Reset();

    LCD_Command_Byte_send(0xF0);
    LCD_Data_Byte_send(0xC3);
    LCD_Command_Byte_send(0xF0);
    LCD_Data_Byte_send(0x96);
    LCD_Command_Byte_send(0x36);
    LCD_Data_Byte_send(0x48);
    LCD_Command_Byte_send(0x3A);
    LCD_Data_Byte_send(0x05);
    LCD_Command_Byte_send(0xB0);
    LCD_Data_Byte_send(0x80);
    LCD_Command_Byte_send(0xB6);
    LCD_Data_Byte_send(0x00);
    LCD_Data_Byte_send(0x02);
    LCD_Command_Byte_send(0xB5);
    LCD_Data_Byte_send(0x02);
    LCD_Data_Byte_send(0x03);
    LCD_Data_Byte_send(0x00);
    LCD_Data_Byte_send(0x04);
    LCD_Command_Byte_send(0xB1);
    LCD_Data_Byte_send(0x80);
    LCD_Data_Byte_send(0x10);
    LCD_Command_Byte_send(0xB4);
    LCD_Data_Byte_send(0x00);
    LCD_Command_Byte_send(0xB7);
    LCD_Data_Byte_send(0xC6);
    LCD_Command_Byte_send(0xC5);
    LCD_Data_Byte_send(0x1C);
    LCD_Command_Byte_send(0xE4);
    LCD_Data_Byte_send(0x31);
    LCD_Command_Byte_send(0xE8);
    LCD_Data_Byte_send(0x40);
    LCD_Data_Byte_send(0x8A);
    LCD_Data_Byte_send(0x00);
    LCD_Data_Byte_send(0x00);
    LCD_Data_Byte_send(0x29);
    LCD_Data_Byte_send(0x19);
    LCD_Data_Byte_send(0xA5);
    LCD_Data_Byte_send(0x33);
    LCD_Command_Byte_send(0xC2);
    LCD_Command_Byte_send(0xA7);

    LCD_Command_Byte_send(0xE0);
    LCD_Data_Byte_send(0xF0);
    LCD_Data_Byte_send(0x09);
    LCD_Data_Byte_send(0x13);
    LCD_Data_Byte_send(0x12);
    LCD_Data_Byte_send(0x12);
    LCD_Data_Byte_send(0x2B);
    LCD_Data_Byte_send(0x3C);
    LCD_Data_Byte_send(0x44);
    LCD_Data_Byte_send(0x4B);
    LCD_Data_Byte_send(0x1B);
    LCD_Data_Byte_send(0x18);
    LCD_Data_Byte_send(0x17);
    LCD_Data_Byte_send(0x1D);
    LCD_Data_Byte_send(0x21);

    LCD_Command_Byte_send(0XE1);
    LCD_Data_Byte_send(0xF0);
    LCD_Data_Byte_send(0x09);
    LCD_Data_Byte_send(0x13);
    LCD_Data_Byte_send(0x0C);
    LCD_Data_Byte_send(0x0D);
    LCD_Data_Byte_send(0x27);
    LCD_Data_Byte_send(0x3B);
    LCD_Data_Byte_send(0x44);
    LCD_Data_Byte_send(0x4D);
    LCD_Data_Byte_send(0x0B);
    LCD_Data_Byte_send(0x17);
    LCD_Data_Byte_send(0x17);
    LCD_Data_Byte_send(0x1D);
    LCD_Data_Byte_send(0x21);

    LCD_Command_Byte_send(0xF0);
    LCD_Data_Byte_send(0x3C);
    LCD_Command_Byte_send(0xF0);
    LCD_Data_Byte_send(0x69);
    LCD_Command_Byte_send(0X13);
    LCD_Command_Byte_send(0X11);
    LCD_Command_Byte_send(0X29);

    //LCD direction setting
    LCD_Register_Write(0x36, 0x0028);

    LCD_Clear(0xFFFF);
}

void LCD_SetWindow(
    uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    LCD_Command_Byte_send(SetX_CMD);
    LCD_Data_Byte_send(sx >> 8);
    LCD_Data_Byte_send(0x00FF & sx);
    LCD_Data_Byte_send(ex >> 8);
    LCD_Data_Byte_send(0x00FF & ex);

    LCD_Command_Byte_send(SetY_CMD);
    LCD_Data_Byte_send(sy >> 8);
    LCD_Data_Byte_send(0x00FF & sy);
    LCD_Data_Byte_send(ey >> 8);
    LCD_Data_Byte_send(0x00FF & ey);

    LCD_Command_Byte_send(WriteRAM_CMD);
}

void LCD_Clear(uint16_t Color)
{
    unsigned int i, m;
    LCD_SetWindow(0, 0, gLcdWidth - 1, gLcdHeight - 1);
    DL_GPIO_setPins(LCD_SDC_PORT, LCD_SDC_PIN);
    for (i = 0; i < gLcdHeight; i++) {
        for (m = 0; m < gLcdWidth; m++) {
            DL_SPI_transmitDataBlocking8(SPI_0_INST, Color >> 8);
            DL_SPI_transmitDataBlocking8(SPI_0_INST, Color);
        }
    }
    /* Wait until all bytes have been transmitted and the TX FIFO is empty */
    while (DL_SPI_isBusy(SPI_0_INST))
        ;
}

void lvgl_LCD_Color_Fill(
    uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, lv_color_t *color)
{
    uint32_t y = 0;
    uint16_t height, width;
    width  = ex - sx + 1;
    height = ey - sy + 1;

    LCD_SetWindow(sx, sy, ex, ey);
    DL_GPIO_setPins(LCD_SDC_PORT, LCD_SDC_PIN);
    for (y = 0; y < width * height; y++) {
        DL_SPI_transmitDataBlocking8(SPI_0_INST, (color->full) >> 8);
        DL_SPI_transmitDataBlocking8(SPI_0_INST, color->full);
        color++;
    }
    /* Wait until all bytes have been transmitted and the TX FIFO is empty */
    while (DL_SPI_isBusy(SPI_0_INST))
        ;
}
