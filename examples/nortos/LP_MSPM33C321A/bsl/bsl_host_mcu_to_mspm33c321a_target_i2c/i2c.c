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

#include <bsl_i2c.h>
#include <i2c.h>
#include "stdint.h"
#include "ti_msp_dl_config.h"

volatile I2C_STATUS gI2cControllerStatus;
uint8_t test_d;
uint8_t I2C_writeBuffer(uint8_t *pData, uint32_t ui32Cnt)
{
    uint8_t gTxCount;

    gTxCount             = DL_I2CC_fillTXFIFO(I2C_INST, pData, ui32Cnt);
    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
        ;

    DL_I2CC_startTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_TX, ui32Cnt);

    DL_Common_updateReg(&I2C_INST->i2cc->CTR, UNICOMMI2CC_CTR_FRM_START_ENABLE,
        UNICOMMI2CC_CTR_FRM_START_MASK);

    while (gTxCount < ui32Cnt) {
        while (DL_I2CC_getRawInterruptStatus(
                   I2C_INST, DL_I2CC_INTERRUPT_TXFIFO_EMPTY) == 0)
            ;
        gTxCount +=
            DL_I2CC_fillTXFIFO(I2C_INST, pData + gTxCount, ui32Cnt - gTxCount);
    }
    /* Poll until the Controller writes all bytes */
    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY_BUS)
        ;
    /* Trap if there was an error */
    if (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_ERROR) {
        /* LED will remain high if there is an error */
        return 1;
    }
    // res = DL_UART_receiveDataBlocking(UART_0_INST);
    return 0;
}

void I2C_readBuffer(uint8_t *pData, uint32_t ui32Cnt)
{
    /* Wait for I2C to be Idle */
    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
        ;
    DL_I2CC_resetTransfer(I2C_INST);
    delay_cycles(100);
    /* Send a read request to Target */
    DL_I2CC_startTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_RX, ui32Cnt);

    DL_Common_updateReg(&I2C_INST->i2cc->CTR, UNICOMMI2CC_CTR_FRM_START_ENABLE,
        UNICOMMI2CC_CTR_FRM_START_MASK);

    /*
     * Receive all bytes from target. LED will remain high if not all bytes
     * are received
     */
    delay_cycles(100);
    // while (!(DL_UNICOMMI2CC_getStatus(I2C_INST) & DL_UNICOMMI2CC_STATUS_IDLE)){}

    for (uint32_t i = 0; i < ui32Cnt; i++) {
        while (DL_I2CC_isRXFIFOEmpty(I2C_INST))
            ;
        *pData = DL_I2CC_receiveData(I2C_INST);
        pData++;
    }
}

//uint8_t test_d;

uint8_t Status_check(void)
{
    BSL_error_t i2c_ack;
    uint8_t buf[1] = {0xBB};
    i2c_ack        = I2C_writeBuffer(buf, 1);
    I2C_readBuffer(BSL_RX_buffer, 1);
    return BSL_RX_buffer[0];
}
