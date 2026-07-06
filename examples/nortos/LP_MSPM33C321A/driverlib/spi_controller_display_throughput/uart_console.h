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
/*!****************************************************************************
 *  @file       uart_console.h
 *  @brief      UART Utilities For Logging
 *
 ******************************************************************************
 */
/** @addtogroup UART_LOG_UTILS
 * @{
 */

#include <stdint.h>
#include <ti/devices/msp/msp.h>

/**
 * @brief Maximum size of the UART buffer for formatted output in bytes
 */
#define UART_BUFFER_SIZE_BYTES 3000

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/* Function Declarations                                                      */
/* ========================================================================== */

/**
 *  @brief      Print formatted output to console via UART
 *
 *  This function provides printf-like functionality for logging.
 *  It accepts a format string and variable arguments, formats the
 *  output using vsnprintf, and sends the resulting string to the console.
 *
 *  @param[in]  unicomm  Pointer to the register overlay for the UART peripheral
 *  @param[in]  pStr     Format string for the output
 *  @param[in]  ...      Variable arguments to be formatted according to the format string
 */
void uart_printf(UNICOMM_Inst_Regs *unicomm, const char *pStr, ...);

/**
 *  @brief      Output a character string to console
 *
 *  The function calculates the length of the string and sends it
 *  using the uartSend function. A temporary pointer (tempStr) is used
 *  to traverse the string for length calculation while preserving the
 *  original string pointer for the actual data transmission.
 *
 *  @param[in]  unicomm  Pointer to the register overlay for the UART peripheral
 *  @param[in]  str      Pointer to the null-terminated string to be printed
 */

void message(UNICOMM_Inst_Regs *unicomm, const uint8_t *str);

/**
 *  @brief      Send a block of data through UART
 *
 *  Sends a block of data pointed to by 'data' and 'length' bytes
 *  long. Uses blocking mode to transmit each byte of data.
 *
 *  @param[in]  unicomm  Pointer to the register overlay for the UART peripheral
 *  @param[in]  length   Number of data bytes to transfer
 *  @param[in]  data     Pointer to data to send
 *
 *  @note       If data word is less than 8 bits, then the data must be left
 *              aligned in the data byte.
 */

void uart_send(UNICOMM_Inst_Regs *unicomm, uint32_t length, uint8_t *data);

/**
 *  @brief      Read formatted input from console via UART
 *
 *  This function provides scanf-like functionality for input parsing.
 *  It reads a string from console, then parses it according to the format
 *  string using vsscanf, storing the results in the provided variables.
 *
 *  @param[in]  unicomm  Pointer to the register overlay for the UART peripheral
 *  @param[in]  maxBytes Maximum number of bytes to read from UART
 *  @param[in]  format   Format string for parsing the input
 *  @param[out] ...      Variable arguments to store the parsed values
 *
 *  @return     The number of input items successfully matched and assigned
 */
int uart_scanf(UNICOMM_Inst_Regs *unicomm, uint32_t maxBytes, const char *format, ...);


#ifdef __cplusplus
}
#endif

/** @}*/
