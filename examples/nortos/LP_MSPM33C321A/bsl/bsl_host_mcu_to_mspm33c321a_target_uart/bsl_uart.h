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

#include "stdint.h"

#define BSL_DELAY (1000000)

#define BSL_CRC (CRC)
#define CRC_Regs (CRCP0)
#define BSL_CRC_SEED ((uint32_t) 0xFFFFFFFFU)

#define MAX_PAYLOAD_DATA_SIZE (128)
/* MAX_PACKET_SIZE = MAX_PAYLOAD_DATA_SIZE + HDR_LEN_CMD_BYTES + CRC_BYTES = 128 + 8 = 136 */
#define MAX_PACKET_SIZE (136)

uint8_t BSL_TX_buffer[MAX_PACKET_SIZE + 2];
uint8_t BSL_RX_buffer[MAX_PACKET_SIZE + 2];
/*  Define BSL CORE commands */
#define CMD_CONNECTION (0x12)
#define CMD_GET_ID (0x19)
#define CMD_RX_PASSWORD (0x21)
#define CMD_MASS_ERASE (0x15)
#define CMD_PROGRAMDATA (0x20)
#define CMD_START_APP (0x40)
#define CMD_CHANGE_BAUDRATE (0x52)
#define CMD_MEMORY_READBACK ((uint8_t) 0x29)

#define DELAY_BSL_OP (50000)

/*  Other useful macros */
#define PACKET_HEADER (0x80)

#define CMD_BYTE (1)
#define HDR_LEN_CMD_BYTES (4)
#define CRC_BYTES (4)
#define PASSWORD_SIZE (uint8_t)(32)
#define ACK_BYTE (1)
#define ID_BACK (28)
#define ADDRS_BYTES (4)

/*  Conversion MACROS */
#define LSB(x) (x & 0x00FF)
#define MSB(x) ((x & 0xFF00) >> 8)

enum {
    /* No Error Occurred! The operation was successful. (BSL_ACK - Packet received successfully) */
    eBSL_success = 0x00,

    /* Header incorrect */
    eBSL_errorHeaderIncorrect = 0x51,

    /* Checksum incorrect */
    eBSL_errorChecksumIncorrect = 0x52,

    /* Packet size zero */
    eBSL_errorPacketSizeZero = 0x53,

    /* Packet size exceeds buffer */
    eBSL_errorPacketSizeTooBig = 0x54,

    /* Unknown error */
    eBSL_errorUnknown = 0x55,

    /* Unknown baud rate */
    eBSL_errorUnknownBaudRate = 0x56

};
typedef uint8_t BSL_error_t;

typedef enum {

    BSL_UARTBaudRates4800    = 0x1,
    BSL_UARTBaudRates9600    = 0x2,
    BSL_UARTBaudRates19200   = 0x3,
    BSL_UARTBaudRates38400   = 0x4,
    BSL_UARTBaudRates57600   = 0x5,
    BSL_UARTBaudRates115200  = 0x6,
    BSL_UARTBaudRates1000000 = 0x7,
    BSL_UARTBaudRates2000000 = 0x8,
    BSL_UARTBaudRates3000000 = 0x9,
    BSL_UARTBaudRates4000000 = 0xA,
} BSL_UARTBaudRates;

uint16_t BSL_MAX_BUFFER_SIZE;

void TurnOnErrorLED(void);

BSL_error_t Host_BSL_Connection(void);
BSL_error_t Host_BSL_GetID(void);
BSL_error_t Host_BSL_loadPassword(uint8_t* pPassword);
BSL_error_t Host_BSL_MassErase(void);
BSL_error_t Host_BSL_writeMemory(
    uint32_t addr, const uint8_t* data, uint32_t len);
BSL_error_t Host_BSL_StartApp(void);
BSL_error_t Host_BSL_Change_Bitrate(BSL_UARTBaudRates baudrate);

uint32_t calculateCRC(const uint8_t* data, uint8_t length);
BSL_error_t Host_BSL_getResponse(void);
