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

#include "i2c_comm.h"
#include <ti/driverlib/driverlib.h>
#include <ti/hal/mspm33/include/I2c_Hal.h>
#include "app_cfg.h"

#define I2C_TIMEOUT_MS          (100U)
#define CYCLES_PER_MS           (160000U)
#define MS_TO_CYCLES(ms)        ((ms) * (CYCLES_PER_MS))

I2c_Hal_ControllerInstanceType gI2cInstance;
volatile bool     gI2cTransferComplete  = false;
volatile uint32_t gI2cTransferErrorCode = 0U;

int I2c_Comm_Init(void)
{
    I2c_Hal_ControllerInitParamsType params;
    sint32 status;

    status = I2c_Hal_ControllerInitParamsSetDefault(&params);
    if (status == I2C_HAL_ERROR_NONE) {
        params.ClockConfig.clockSel    = I2C_INST_CLOCK_SOURCE;
        params.ClockConfig.divideRatio = I2C_INST_CLOCK_DIVIDE;
        params.HwRegsPtr               = I2C_INST;
        params.InputClockFreqHz        = I2C_INPUT_CLOCK_HZ;
        params.Speed                   = I2C_HAL_SPEED_FAST;
        params.UserArgsPtr             = NULL_PTR;

        status = I2c_Hal_ControllerInit(&gI2cInstance, &params);
        if (status == I2C_HAL_ERROR_NONE) {
            NVIC_ClearPendingIRQ(I2C_INST_INT_IRQN);
            NVIC_EnableIRQ(I2C_INST_INT_IRQN);
            status = 0;
        } else {
            status = -3;
        }
    } else {
        status = -2;
    }

    return status;
}

int I2c_Comm_Transfer(uint8_t slave_addr, uint8_t *buf, uint8_t len, uint32_t options)
{
    I2c_Hal_ControllerTransactionType txn;
    uint32_t timeout;
    int status;

    status = -1;
    timeout = 0;

    txn.TargetAddress   = slave_addr;
    txn.AddressingMode  = I2C_HAL_ADDRESSING_MODE_7BIT;
    txn.TransferOptions = options;
    txn.BufferPtr       = buf;
    txn.SizeInBytes     = len;

    gI2cTransferComplete = false;
    gI2cTransferErrorCode = 0U;

    if (I2c_Hal_ControllerTransferInterrupt(&gI2cInstance, &txn) == I2C_HAL_ERROR_NONE) {
        status = -2;
        while (!gI2cTransferComplete) {
            DL_Common_delayCycles(MS_TO_CYCLES(1));
            if (timeout++ >= I2C_TIMEOUT_MS) {
                status = -2;
                break;
            }
        }
        if (timeout < I2C_TIMEOUT_MS) {
            status = (gI2cTransferErrorCode != I2C_HAL_TRANSFER_ERROR_NONE) ? -3 : 0;
        }
    } else {
        status = -1;
    }

    return status;
}
