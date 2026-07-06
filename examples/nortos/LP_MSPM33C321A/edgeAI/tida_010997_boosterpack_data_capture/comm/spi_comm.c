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

#include "spi_comm.h"
#include <ti/driverlib/driverlib.h>

Spi_Hal_InstanceType  gSpiInstance;
volatile bool         gSpiTransferComplete  = false;
volatile uint32_t     gSpiTransferErrorCode = 0U;

int SPI_init(void)
{
    Spi_Hal_InitParamsType params;
    sint32 status;

    status = Spi_Hal_InitParamsSetDefault(&params);
    if (status == SPI_HAL_ERROR_NONE) {
        params.HwRegsPtr        = SPI_INST;
        params.Mode             = SPI_HAL_MODE_CONTROLLER;
        params.DataWidth        = (Spi_Hal_DataWidthType)8U;
        params.FrameFormat      = SPI_HAL_FRAMEFORMAT_MOTO_4WIRE_POL0_PHA0;
        params.BitOrder         = SPI_HAL_BITORDER_MSB_FIRST;
        params.Parity           = SPI_HAL_PARITY_NONE;
        params.BitRateHz        = SPI_BIT_RATE_HZ;
        params.InputClockFreqHz = SPI_INPUT_CLOCK_HZ;
        params.ClockDivideRatio = SPI_CLOCK_DIVIDER;
        params.RxOnly           = FALSE;
        params.UserArgsPtr      = NULL_PTR;

        status = Spi_Hal_Init(&gSpiInstance, &params);
        if (status == SPI_HAL_ERROR_NONE) {
            DL_SPI_enable(SPI_INST);
            status = 0;
        } else {
            status = -3;
        }
    } else {
        status = -2;
    }

    return status;
}

void SPI_toggleCS(const chipSelect *cs)
{
    DL_GPIO_clearPins(cs->csPort, cs->csPin);
    delay_cycles(100000);
    DL_GPIO_setPins(cs->csPort, cs->csPin);
}

int SPI_transfer(uint8_t *txData, uint8_t *rxData, uint16_t len, const chipSelect *cs)
{
    Spi_Hal_TransferStatusType status;
    uint8_t dummyBuffer[8];
    int ret;

    DL_SPI_drainRXFIFO8(SPI_INST, dummyBuffer, 8);

    ret = -1;
    DL_GPIO_clearPins(cs->csPort, cs->csPin);

    if (Spi_Hal_TransferPolling(&gSpiInstance, txData, rxData, (uint32)len) == SPI_HAL_ERROR_NONE) {
        do {
            Spi_Hal_TransferContinuePolling(&gSpiInstance, &status);
        } while (!status.IsTransferComplete);

        if (status.ErrorCodeMask != 0U) {
            ret = -3;
        } else {
            ret = 0;
        }
    } else {
        ret = -1;
    }

    DL_GPIO_setPins(cs->csPort, cs->csPin);

    return ret;
}
