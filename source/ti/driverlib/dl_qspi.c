/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

#include <ti/driverlib/dl_qspi.h>

#ifdef __MSP_HAS_QSPI__

void DL_QSPI_init(QSPI_Regs *qspi, DL_QSPI_Config *config)
{
    DL_Common_updateReg(&qspi->CTL0,
        (uint32_t) config->chipSelectPin | (uint32_t) config->frameFormat |
            (uint32_t) config->dataSize,
        QSPI_CTL0_CSSEL_MASK | QSPI_CTL0_FRF_MASK | QSPI_CTL0_SPO_MASK |
            QSPI_CTL0_SPH_MASK | QSPI_CTL0_DSS_MASK);

    DL_Common_updateReg(&qspi->CTL1,
        (uint32_t) config->parity | (uint32_t) config->bitOrder |
            (uint32_t) config->mode,
        QSPI_CTL1_PES_MASK | QSPI_CTL1_PREN_MASK | QSPI_CTL1_PTEN_MASK |
            QSPI_CTL1_MSB_MASK | QSPI_CTL1_CP_MASK);
}

void DL_QSPI_setClockConfig(QSPI_Regs *qspi, DL_QSPI_ClockConfig *config)
{
    qspi->CLKSEL = (uint32_t) config->clockSel;

    qspi->CLKDIV = (uint32_t) config->divideRatio;
}

void DL_QSPI_getClockConfig(QSPI_Regs *qspi, DL_QSPI_ClockConfig *config)
{
    config->clockSel = (DL_QSPI_CLOCK) qspi->CLKSEL;

    config->divideRatio = (DL_QSPI_CLOCK_DIVIDE_RATIO) qspi->CLKDIV;
}

uint8_t DL_QSPI_receiveDataBlocking8(QSPI_Regs *qspi)
{
    while (DL_QSPI_isRXFIFOEmpty(qspi)) {
    };

    return DL_QSPI_receiveData8(qspi);
}

uint16_t DL_QSPI_receiveDataBlocking16(QSPI_Regs *qspi)
{
    while (DL_QSPI_isRXFIFOEmpty(qspi)) {
    };
    return DL_QSPI_receiveData16(qspi);
}

uint32_t DL_QSPI_receiveDataBlocking32(QSPI_Regs *qspi)
{
    while (DL_QSPI_isRXFIFOEmpty(qspi)) {
    };
    return DL_QSPI_receiveData32(qspi);
}

void DL_QSPI_transmitDataBlocking8(QSPI_Regs *qspi, uint8_t data)
{
    while (DL_QSPI_isTXFIFOFull(qspi)) {
    };
    DL_QSPI_transmitData8(qspi, data);
}

void DL_QSPI_transmitDataBlocking16(QSPI_Regs *qspi, uint16_t data)
{
    while (DL_QSPI_isTXFIFOFull(qspi)) {
    };
    DL_QSPI_transmitData16(qspi, data);
}

void DL_QSPI_transmitDataBlocking32(QSPI_Regs *qspi, uint32_t data)
{
    while (DL_QSPI_isTXFIFOFull(qspi)) {
    };
    DL_QSPI_transmitData32(qspi, data);
}

bool DL_QSPI_receiveDataCheck8(QSPI_Regs *qspi, uint8_t *buffer)
{
    bool status;
    if (DL_QSPI_isRXFIFOEmpty(qspi)) {
        status = false;
    } else {
        *buffer = DL_QSPI_receiveData8(qspi);
        status  = true;
    }

    return status;
}

bool DL_QSPI_receiveDataCheck16(QSPI_Regs *qspi, uint16_t *buffer)
{
    bool status;
    if (DL_QSPI_isRXFIFOEmpty(qspi)) {
        status = false;
    } else {
        *buffer = DL_QSPI_receiveData16(qspi);
        status  = true;
    }

    return status;
}

bool DL_QSPI_receiveDataCheck32(QSPI_Regs *qspi, uint32_t *buffer)
{
    bool status;
    if (DL_QSPI_isRXFIFOEmpty(qspi)) {
        status = false;
    } else {
        *buffer = DL_QSPI_receiveData32(qspi);
        status  = true;
    }

    return status;
}

bool DL_QSPI_transmitDataCheck8(QSPI_Regs *qspi, uint8_t data)
{
    bool status;
    if (DL_QSPI_isTXFIFOFull(qspi)) {
        status = false;
    } else {
        DL_QSPI_transmitData8(qspi, data);
        status = true;
    }

    return status;
}

bool DL_QSPI_transmitDataCheck16(QSPI_Regs *qspi, uint16_t data)
{
    bool status;
    if (DL_QSPI_isTXFIFOFull(qspi)) {
        status = false;
    } else {
        DL_QSPI_transmitData16(qspi, data);
        status = true;
    }

    return status;
}

bool DL_QSPI_transmitDataCheck32(QSPI_Regs *qspi, uint32_t data)
{
    bool status;
    if (DL_QSPI_isTXFIFOFull(qspi)) {
        status = false;
    } else {
        DL_QSPI_transmitData32(qspi, data);
        status = true;
    }

    return status;
}

uint32_t DL_QSPI_drainRXFIFO8(QSPI_Regs *qspi, uint8_t *buffer, uint32_t maxCount)
{
    uint32_t i;
    for (i = 0; i < maxCount; i++) {
        if (!DL_QSPI_isRXFIFOEmpty(qspi)) {
            buffer[i] = DL_QSPI_receiveData8(qspi);
        } else {
            break;
        }
    }

    return i;
}

uint32_t DL_QSPI_drainRXFIFO16(
    QSPI_Regs *qspi, uint16_t *buffer, uint32_t maxCount)
{
    uint32_t i;
    for (i = 0; i < maxCount; i++) {
        if (!DL_QSPI_isRXFIFOEmpty(qspi)) {
            buffer[i] = DL_QSPI_receiveData16(qspi);
        } else {
            break;
        }
    }

    return i;
}

uint32_t DL_QSPI_drainRXFIFO32(
    QSPI_Regs *qspi, uint32_t *buffer, uint32_t maxCount)
{
    uint32_t i;
    for (i = 0; i < maxCount; i++) {
        if (!DL_QSPI_isRXFIFOEmpty(qspi)) {
            buffer[i] = DL_QSPI_receiveData32(qspi);
        } else {
            break;
        }
    }

    return i;
}

uint32_t DL_QSPI_fillTXFIFO8(QSPI_Regs *qspi, uint8_t *buffer, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++) {
        if (!DL_QSPI_isTXFIFOFull(qspi)) {
            DL_QSPI_transmitData8(qspi, buffer[i]);
        } else {
            break;
        }
    }

    return i;
}

uint32_t DL_QSPI_fillTXFIFO16(QSPI_Regs *qspi, uint16_t *buffer, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++) {
        if (!DL_QSPI_isTXFIFOFull(qspi)) {
            DL_QSPI_transmitData16(qspi, buffer[i]);
        } else {
            break;
        }
    }

    return i;
}

bool DL_QSPI_saveConfiguration(QSPI_Regs *qspi, DL_QSPI_backupConfig *ptr)
{
    bool stateSaved = !ptr->backupRdy;
    if (stateSaved) {
        ptr->controlWord0                 = qspi->CTL0;
        ptr->controlWord1                 = qspi->CTL1;
        ptr->clockControl                 = qspi->CLKCTL;
        ptr->clockSel                     = qspi->CLKSEL;
        ptr->divideRatio                  = qspi->CLKDIV;
        ptr->interruptFifoLevelSelectWord = qspi->IFLS;
        ptr->interruptMask0               = qspi->CPU_INT.IMASK;
        ptr->interruptMask1               = qspi->DMA_TRIG_RX.IMASK;
        ptr->interruptMask2               = qspi->DMA_TRIG_TX.IMASK;
        ptr->backupRdy                    = true;
    }
    return stateSaved;
}

bool DL_QSPI_restoreConfiguration(QSPI_Regs *qspi, DL_QSPI_backupConfig *ptr)
{
    bool stateRestored = ptr->backupRdy;
    if (stateRestored) {
        /* Set CTL1.ENABLE=0 during initialization */
        qspi->CTL1              = ptr->controlWord1 & ~(QSPI_CTL1_ENABLE_MASK);
        qspi->CTL0              = ptr->controlWord0;
        qspi->CLKCTL            = ptr->clockControl;
        qspi->CLKSEL            = ptr->clockSel;
        qspi->CLKDIV            = ptr->divideRatio;
        qspi->IFLS              = ptr->interruptFifoLevelSelectWord;
        qspi->CPU_INT.IMASK     = ptr->interruptMask0;
        qspi->DMA_TRIG_RX.IMASK = ptr->interruptMask1;
        qspi->DMA_TRIG_TX.IMASK = ptr->interruptMask2;

        /* Re-enable QSPI if it was originally enabled */
        if ((ptr->controlWord1 & QSPI_CTL1_ENABLE_MASK) ==
            QSPI_CTL1_ENABLE_MASK) {
            DL_QSPI_enable(qspi);
        }
        ptr->backupRdy = false;
    }
    return stateRestored;
}

uint32_t DL_QSPI_fillTXFIFO32(QSPI_Regs *qspi, uint32_t *buffer, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++) {
        if (!DL_QSPI_isTXFIFOFull(qspi)) {
            DL_QSPI_transmitData32(qspi, buffer[i]);
        } else {
            break;
        }
    }

    return i;
}

#endif /* __MSP_HAS_QSPI__ */
