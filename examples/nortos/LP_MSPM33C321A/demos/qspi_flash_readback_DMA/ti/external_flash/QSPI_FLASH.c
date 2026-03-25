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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "QSPI_FLASH.h"
#include <string.h>
#include <ti/driverlib/dl_qspi.h>

/*******************************************************************************
 * PRIVATE DEFINES
 ******************************************************************************/

/*! Maximum transfer size for indefinite read */
#define MAX_INDEFINITE_READ_SIZE 256U

/*! Maximum number of address bytes */
#define MAX_ADDR_BYTES 4U

/*! Maximum page size for program operations */
#define MAX_PAGE_SIZE 256U

/*! Maximum combined size for address and data buffer */
#define MAX_COMBINED_SIZE (MAX_ADDR_BYTES + MAX_PAGE_SIZE)

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

static QSPIFLASH_Status QSPIFLASH_waitForIdle(QSPI_Regs *qspi);
static QSPIFLASH_Status QSPIFLASH_waitForStall(QSPI_Regs *qspi);

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 *  @brief      Wait for QSPI peripheral to become idle
 *
 *  Polls the QSPI status until the peripheral is idle and ready for
 *  a new operation.
 *
 *  @param[in]  qspi    Pointer to the QSPI peripheral registers
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS when QSPI becomes idle
 */
static QSPIFLASH_Status QSPIFLASH_waitForIdle(QSPI_Regs *qspi)
{
    do {
        if (DL_QSPI_isQSPIIdle(qspi)) {
            return QSPIFLASH_STATUS_SUCCESS;
        }
    } while (1);
}

/*!
 *  @brief      Wait for QSPI peripheral to stall
 *
 *  Polls the QSPI status until the peripheral stalls, indicating
 *  the current transfer has completed.
 *
 *  @param[in]  qspi    Pointer to the QSPI peripheral registers
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS when QSPI stalls
 */
static QSPIFLASH_Status QSPIFLASH_waitForStall(QSPI_Regs *qspi)
{
    do {
        if (DL_QSPI_isQSPIStalled(qspi)) {
            return QSPIFLASH_STATUS_SUCCESS;
        }
    } while (1);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

QSPIFLASH_Status QSPIFLASH_init(
    QSPIFLASH_Handle *handle, const QSPIFLASH_InitParams *params)
{
    if (handle == NULL) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    /* Set initialized to false first */
    handle->initialized = false;

    if (params == NULL || params->qspiRegs == NULL) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    /* Store QSPI register pointer */
    handle->qspiRegs    = params->qspiRegs;
    handle->initialized = true;

    return QSPIFLASH_STATUS_SUCCESS;
}

QSPIFLASH_Status QSPIFLASH_sendCommand(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPI_Regs *qspi             = handle->qspiRegs;
    uint8_t cmd                 = commandConfig->instruction;
    QSPIFLASH_TransferMode mode = (QSPIFLASH_TransferMode)commandConfig->transferMode;
    QSPIFLASH_Status status     = QSPIFLASH_STATUS_SUCCESS;

    /* Wait for QSPI to be idle */
    if (!DL_QSPI_isQSPIIdle(qspi)) {
        status = QSPIFLASH_waitForIdle(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* For Configuration frame, only SPI or QPI half-duplex modes are supported */
    if (mode != QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX &&
        mode != QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX) {
        return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Disable QSPI before reconfiguring */
    DL_QSPI_disable(qspi);

    /* Set bus format based on frame type */
    DL_QSPI_BUS_FORMAT busFormat =
        (mode == QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX)
            ? DL_QSPI_BUS_FORMAT_SPI_HALF_DUPLEX
            : DL_QSPI_BUS_FORMAT_QPI_HALF_DUPLEX;

    /* Configure bus format */
    DL_QSPI_setBusFormat(qspi, busFormat);

    /* Set word size */
    DL_QSPI_setWordSize(qspi, DL_QSPI_WORD_SIZE_8);

    /* Calculate total TX bytes */
    uint32_t totalTxBytes = 1U; /* Command byte */

    /* Set TX counts in QSPICTL1 register */
    DL_QSPI_setTXCount(qspi, totalTxBytes);

    /* For configuration frame, RXCOUNT=0 and disable Prefetch */
    DL_QSPI_setRXCount(qspi, 0U);
    DL_QSPI_disablePreFetch(qspi);

    /* Enable QSPI */
    DL_QSPI_enable(qspi);

    /* Flush FIFOs */
    DL_QSPI_flushRXFIFO(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Send command */
    DL_QSPI_transmitDataBlocking8(qspi, cmd);

    /* Check stall */
    status = QSPIFLASH_waitForStall(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Deassert chip select */
    DL_QSPI_deAssertChipSelect(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Wait for QSPI to be idle */
    status = QSPIFLASH_waitForIdle(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Release chip select for subsequent frame */
    DL_QSPI_releaseChipSelect(qspi);

    return status;
}

QSPIFLASH_Status QSPIFLASH_executeConfigurationFrame(QSPIFLASH_Handle *handle,
    const uint16_t *txData, uint32_t txDataLen,
    const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPI_Regs *qspi                   = handle->qspiRegs;
    uint8_t cmd                       = commandConfig->instruction;
    QSPIFLASH_TransferMode mode       = (QSPIFLASH_TransferMode)commandConfig->transferMode;
    QSPIFLASH_AddressSize addressSize = (QSPIFLASH_AddressSize)commandConfig->addressSize;
    QSPIFLASH_Status status           = QSPIFLASH_STATUS_SUCCESS;

    /* Wait for QSPI to be idle */
    if (!DL_QSPI_isQSPIIdle(qspi)) {
        status = QSPIFLASH_waitForIdle(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* For Configuration frame, only SPI or QPI half-duplex modes are supported */
    if (mode != QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX &&
        mode != QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX) {
        return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Disable QSPI before reconfiguring */
    DL_QSPI_disable(qspi);

    /* Set bus format based on frame type */
    DL_QSPI_BUS_FORMAT busFormat =
        (mode == QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX)
            ? DL_QSPI_BUS_FORMAT_SPI_HALF_DUPLEX
            : DL_QSPI_BUS_FORMAT_QPI_HALF_DUPLEX;

    /* Configure bus format */
    DL_QSPI_setBusFormat(qspi, busFormat);

    /* Set address mode */
    DL_QSPI_ADDRESS_MODE addressMode =
        (addressSize == QSPIFLASH_ADDRESS_SIZE_3BYTE)
            ? DL_QSPI_ADDRESS_MODE_3BYTE
            : DL_QSPI_ADDRESS_MODE_4BYTE;
    DL_QSPI_setAddressMode(qspi, addressMode);

    /* Set word size to 16-bit for entire operation (no switching mid-transfer) */
    DL_QSPI_setWordSize(qspi, DL_QSPI_WORD_SIZE_16);

    /* Calculate total TX bytes */
    uint32_t totalTxBytes = 1U; /* Command byte */
    if (txData != NULL) {
        totalTxBytes += txDataLen;
    }

    /* Set TX counts in QSPICTL1 register */
    DL_QSPI_setTXCount(qspi, totalTxBytes);

    /* For configuration frame, RXCOUNT=0 and disable Prefetch */
    DL_QSPI_setRXCount(qspi, 0U);
    DL_QSPI_disablePreFetch(qspi);

    /* Enable QSPI */
    DL_QSPI_enable(qspi);

    /* Flush FIFOs */
    DL_QSPI_flushRXFIFO(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Send command + data as 16-bit words */
    if (txData == NULL || txDataLen == 0U) {
        /* Only command byte - pack with padding */
        uint16_t word = cmd;
        DL_QSPI_transmitDataBlocking16(qspi, word);
    } else {
        /* Pack command with first byte of txData */
        uint8_t *txData8 = (uint8_t *) txData;
        uint16_t word    = ((uint16_t) txData8[0] << 8) | cmd;
        DL_QSPI_transmitDataBlocking16(qspi, word);

        /* Send remaining txData bytes as 16-bit words */
        for (uint32_t i = 1U; i < txDataLen; i += 2U) {
            uint8_t b0 = txData8[i];
            uint8_t b1 = ((i + 1U) < txDataLen) ? txData8[i + 1U] : 0U;
            word       = ((uint16_t) b1 << 8) | b0;
            DL_QSPI_transmitDataBlocking16(qspi, word);
        }
    }

    /* Check stall */
    status = QSPIFLASH_waitForStall(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Deassert chip select */
    DL_QSPI_deAssertChipSelect(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Wait for QSPI to be idle */
    status = QSPIFLASH_waitForIdle(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Release chip select for subsequent frame */
    DL_QSPI_releaseChipSelect(qspi);

    return status;
}

QSPIFLASH_Status QSPIFLASH_executeStatusFrame(QSPIFLASH_Handle *handle,
    uint8_t *rxData, uint32_t rxDataLen,
    const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPI_Regs *qspi             = handle->qspiRegs;
    uint8_t cmd                 = commandConfig->instruction;
    QSPIFLASH_TransferMode mode = (QSPIFLASH_TransferMode)commandConfig->transferMode;
    QSPIFLASH_Status status     = QSPIFLASH_STATUS_SUCCESS;

    /* Wait for QSPI to be idle */
    if (!DL_QSPI_isQSPIIdle(qspi)) {
        status = QSPIFLASH_waitForIdle(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* For Status frame, only SPI or QPI half-duplex modes are supported */
    if (mode != QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX &&
        mode != QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX) {
        return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Disable QSPI before reconfiguring */
    DL_QSPI_disable(qspi);

    /* Set bus format based on frame type */
    DL_QSPI_BUS_FORMAT busFormat =
        (mode == QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX)
            ? DL_QSPI_BUS_FORMAT_SPI_HALF_DUPLEX
            : DL_QSPI_BUS_FORMAT_QPI_HALF_DUPLEX;

    /* Configure bus format */
    DL_QSPI_setBusFormat(qspi, busFormat);

    /* Set word size to 1 Byte */
    DL_QSPI_setWordSize(qspi, DL_QSPI_WORD_SIZE_8);

    /* Calculate total TX bytes */
    uint32_t totalTxBytes = 1U; /* Command byte */

    /* Set TX counts in QSPICTL1 register */
    DL_QSPI_setTXCount(qspi, totalTxBytes);

    /* Set RX counts in QSPICTL1 register and enable Prefetch */
    DL_QSPI_setRXCount(qspi, rxDataLen);
    DL_QSPI_enablePreFetch(qspi);

    /* Enable QSPI */
    DL_QSPI_enable(qspi);

    /* Flush FIFOs */
    DL_QSPI_flushRXFIFO(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Send command */
    DL_QSPI_transmitDataBlocking8(qspi, cmd);

    /* Read RX data */
    if (rxData != NULL && rxDataLen > 0U) {
        for (uint32_t i = 0U; i < rxDataLen; i++) {
            rxData[i] = DL_QSPI_receiveDataBlocking8(qspi);
        }
    }

    /* Deassert chip select */
    DL_QSPI_deAssertChipSelect(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Wait for QSPI to be idle */
    status = QSPIFLASH_waitForIdle(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Release chip select for subsequent frame */
    DL_QSPI_releaseChipSelect(qspi);

    return status;
}

QSPIFLASH_Status QSPIFLASH_executeDataFrame(QSPIFLASH_Handle *handle,
    const uint8_t *txData, uint32_t txDataLen, uint16_t *rxData,
    uint32_t rxDataLen, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPI_Regs *qspi                   = handle->qspiRegs;
    uint8_t cmd                       = commandConfig->instruction;
    QSPIFLASH_TransferMode mode       = (QSPIFLASH_TransferMode)commandConfig->transferMode;
    uint32_t dummyCycles              = commandConfig->dummyCycles;
    QSPIFLASH_AddressSize addressSize = (QSPIFLASH_AddressSize)commandConfig->addressSize;
    QSPIFLASH_Status status           = QSPIFLASH_STATUS_SUCCESS;

    /* Wait for QSPI to be idle as required by all frame types */
    if (!DL_QSPI_isQSPIIdle(qspi)) {
        status = QSPIFLASH_waitForIdle(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* Disable QSPI before reconfiguring */
    DL_QSPI_disable(qspi);

    /* Set bus format based on frame type */
    DL_QSPI_BUS_FORMAT busFormat;

    /* Data frames must use format with QSPIFORMAT[3] as '1' */
    switch (mode) {
        case QSPIFLASH_TRANSFER_MODE_SPI:
            busFormat = DL_QSPI_BUS_FORMAT_SSS_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_DUAL_OUT:
            busFormat = DL_QSPI_BUS_FORMAT_SSD_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_DUAL_IO:
            busFormat = DL_QSPI_BUS_FORMAT_SDD_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_QUAD_OUT:
            busFormat = DL_QSPI_BUS_FORMAT_SSQ_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_QUAD_IO:
            busFormat = DL_QSPI_BUS_FORMAT_SQQ_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_QPI:
            busFormat = DL_QSPI_BUS_FORMAT_QQQ_MODE;
            break;
        default:
            return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Configure bus format */
    DL_QSPI_setBusFormat(qspi, busFormat);

    /* Set command byte */
    DL_QSPI_setCommandByte(qspi, cmd);

    /* Set address mode */
    DL_QSPI_ADDRESS_MODE addressMode =
        (addressSize == QSPIFLASH_ADDRESS_SIZE_3BYTE)
            ? DL_QSPI_ADDRESS_MODE_3BYTE
            : DL_QSPI_ADDRESS_MODE_4BYTE;
    DL_QSPI_setAddressMode(qspi, addressMode);

    /* Performance Mode */
    DL_QSPI_disablePerformanceMode(qspi);

    /* Set dummy cycles */
    DL_QSPI_setDummyClock(qspi, dummyCycles);

    /* Set word size to 16-bit for entire operation */
    DL_QSPI_setWordSize(qspi, DL_QSPI_WORD_SIZE_16);

    /* Set TX count as size of address (in bytes) */
    DL_QSPI_setTXCount(qspi, txDataLen);

    /* Configure RX count based on size */
    if (rxDataLen < 256U) {
        DL_QSPI_setRXCount(qspi, rxDataLen);
    } else {
        DL_QSPI_setRXCount(qspi, QSPI_QSPICTL1_RXCOUNT_RXFIFOMON);
    }
    DL_QSPI_enablePreFetch(qspi);

    /* Enable QSPI */
    DL_QSPI_enable(qspi);

    /* Flush FIFOs */
    DL_QSPI_flushRXFIFO(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Send Address as 16-bit words */
    if (txData != NULL && txDataLen > 0U) {
        for (uint32_t i = 0U; i < txDataLen; i += 2U) {
            uint8_t b0    = txData[i];
            uint8_t b1    = ((i + 1U) < txDataLen) ? txData[i + 1U] : 0U;
            uint16_t word = ((uint16_t) b1 << 8) | b0;
            DL_QSPI_transmitDataBlocking16(qspi, word);
        }
    }

    /* Read RX data */
    if (rxData != NULL && rxDataLen > 0U) {
        for (uint32_t i = 0U; i < rxDataLen / 2U; i++) {
            rxData[i] = DL_QSPI_receiveDataBlocking16(qspi);
        }
        /* Handle last odd byte */
        if ((rxDataLen & 1U) != 0U) {
            uint8_t *rxData8        = (uint8_t *) rxData;
            rxData8[rxDataLen - 1U] = DL_QSPI_receiveDataBlocking8(qspi);
        }
    }

    /* Check stall for large transfers */
    if (rxDataLen >= 256U) {
        status = QSPIFLASH_waitForStall(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* Deassert chip select */
    DL_QSPI_deAssertChipSelect(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Wait for QSPI to be idle */
    status = QSPIFLASH_waitForIdle(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    if (rxDataLen >= 256U) {
        DL_QSPI_flushRXFIFO(qspi);
        while (DL_QSPI_isQSPIStalled(qspi)) {
            /* Wait for stall to clear */
        }
    }

    /* Release chip select for subsequent frame */
    DL_QSPI_releaseChipSelect(qspi);

    return status;
}

QSPIFLASH_Status QSPIFLASH_readID(QSPIFLASH_Handle *handle, uint8_t *id,
    uint32_t idLen, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || id == NULL || commandConfig == NULL || idLen < 3U) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    /* Read ID is a status frame */
    return QSPIFLASH_executeStatusFrame(handle, id, idLen, commandConfig);
}

QSPIFLASH_Status QSPIFLASH_readSecurityRegister(QSPIFLASH_Handle *handle,
    uint8_t *status, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || status == NULL || commandConfig == NULL ||
        !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    return QSPIFLASH_executeStatusFrame(
        handle, status, sizeof(*status), commandConfig);
}

QSPIFLASH_Status QSPIFLASH_readStatusRegister(QSPIFLASH_Handle *handle,
    uint8_t *status, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || status == NULL || commandConfig == NULL ||
        !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    return QSPIFLASH_executeStatusFrame(
        handle, status, sizeof(*status), commandConfig);
}

QSPIFLASH_Status QSPIFLASH_writeStatusRegister(QSPIFLASH_Handle *handle,
    uint8_t statusValue, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;
    QSPIFLASH_CommandConfig writeConfig = {0};
    writeConfig.transferMode = commandConfig->transferMode;

    /* Enable write operations */
    writeConfig.instruction = WRITE_ENABLE_CMD;
    result                  = QSPIFLASH_writeEnable(handle, &writeConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Prepare status byte as 16-bit buffer */
    uint16_t statusBuffer[1];
    statusBuffer[0] = statusValue;

    /* Write status register */
    writeConfig.instruction = WRITE_STATUS_CFG_REG_CMD;
    result                  = QSPIFLASH_executeConfigurationFrame(
        handle, statusBuffer, sizeof(statusValue), &writeConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Wait for write to complete */
    writeConfig.instruction = READ_STATUS_REG_CMD;
    result                  = QSPIFLASH_waitForReady(handle, &writeConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    return result;
}

QSPIFLASH_Status QSPIFLASH_setQEBit(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;
    QSPIFLASH_CommandConfig qeConfig = {0};
    qeConfig.transferMode = commandConfig->transferMode;

    /* Read current status register */
    uint8_t status;
    qeConfig.instruction = READ_STATUS_REG_CMD;
    result = QSPIFLASH_readStatusRegister(handle, &status, &qeConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Check if QE bit is already set */
    if ((status & STATUS_QE_BIT_MASK) != 0U) {
        return result;
    }

    /* Set QE bit and write back */
    status |= STATUS_QE_BIT_MASK;
    return QSPIFLASH_writeStatusRegister(handle, status, commandConfig);
}

QSPIFLASH_Status QSPIFLASH_resetQEBit(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;
    QSPIFLASH_CommandConfig qeConfig = {0};
    qeConfig.transferMode = commandConfig->transferMode;

    /* Read current status register */
    uint8_t status;
    qeConfig.instruction = READ_STATUS_REG_CMD;
    result = QSPIFLASH_readStatusRegister(handle, &status, &qeConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Check if QE bit is already cleared */
    if ((status & STATUS_QE_BIT_MASK) == 0U) {
        return result;
    }

    /* Clear QE bit and write back */
    status &= ~STATUS_QE_BIT_MASK;
    return QSPIFLASH_writeStatusRegister(handle, status, commandConfig);
}

QSPIFLASH_Status QSPIFLASH_waitForReady(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    uint8_t status;
    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;

    do {
        result = QSPIFLASH_readStatusRegister(handle, &status, commandConfig);
        if (result != QSPIFLASH_STATUS_SUCCESS) {
            return result;
        }

        /* Check if WIP (write in progress) bit is cleared */
        if ((status & STATUS_WIP) == 0U) {
            return result;
        }
    } while (1);
}

QSPIFLASH_Status QSPIFLASH_writeEnable(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    /* Write enable is a configuration frame */
    return QSPIFLASH_executeConfigurationFrame(
        handle, NULL, 0U, commandConfig);
}

QSPIFLASH_Status QSPIFLASH_reset(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;
    QSPIFLASH_CommandConfig resetConfig = {0};

    /* Reset enable command is required before Reset */
    resetConfig.instruction  = RESET_ENABLE_CMD;
    resetConfig.transferMode = commandConfig->transferMode;
    result =
        QSPIFLASH_executeConfigurationFrame(handle, NULL, 0U, &resetConfig);

    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Reset is a configuration frame */
    return QSPIFLASH_executeConfigurationFrame(
        handle, NULL, 0U, commandConfig);
}

QSPIFLASH_Status QSPIFLASH_enableQuadMode(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    /* Send enable quad mode command */
    QSPIFLASH_Status result =
        QSPIFLASH_executeConfigurationFrame(handle, NULL, 0U, commandConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    return result;
}

QSPIFLASH_Status QSPIFLASH_disableQuadMode(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    /* Send disable quad mode command */
    QSPIFLASH_Status result =
        QSPIFLASH_executeConfigurationFrame(handle, NULL, 0U, commandConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    return result;
}

QSPIFLASH_Status QSPIFLASH_read(QSPIFLASH_Handle *handle, void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || data == NULL || size == 0U ||
        commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    uint32_t address = commandConfig->address;

    uint8_t addrBytes[4];
    uint32_t addrLen;

    if (commandConfig->addressSize == QSPIFLASH_ADDRESS_SIZE_4BYTE) {
        addrBytes[0] = (uint8_t)((address >> 24) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[2] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[3] = (uint8_t)(address & 0xFFU);
        addrLen      = 4U;
    } else {
        addrBytes[0] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[2] = (uint8_t)(address & 0xFFU);
        addrLen      = 3U;
    }

    return QSPIFLASH_executeDataFrame(
        handle, addrBytes, addrLen, data, size, commandConfig);
}

QSPIFLASH_Status QSPIFLASH_write(QSPIFLASH_Handle *handle, const void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || data == NULL || size == 0U ||
        commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_CommandConfig writeConfig = {0};
    uint32_t address            = commandConfig->address;
    QSPIFLASH_TransferMode mode = (QSPIFLASH_TransferMode)commandConfig->transferMode;

    /* For write operations, only SSS and QQQ modes are supported */
    if (mode != QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX &&
        mode != QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX) {
        return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Calculate page boundaries */
    uint32_t pageSize     = PAGE_SIZE;
    uint32_t pageOffset   = address & (pageSize - 1U);
    uint32_t bytesInPage  = pageSize - pageOffset;
    uint32_t bytesToWrite = (size < bytesInPage) ? size : bytesInPage;

    /* Write data page by page */
    const uint8_t *dataPtr  = (const uint8_t *) data;
    uint32_t remainingBytes = size;
    uint32_t currentAddress = address;
    QSPIFLASH_Status status = QSPIFLASH_STATUS_SUCCESS;

    while (remainingBytes > 0U) {
        writeConfig.instruction  = WRITE_ENABLE_CMD;
        writeConfig.transferMode = mode;

        /* Enable write operations */
        status = QSPIFLASH_writeEnable(handle, &writeConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }

        /* Prepare address bytes */
        uint8_t addrBytes[4];
        uint32_t addrLen;

        if (commandConfig->addressSize == QSPIFLASH_ADDRESS_SIZE_4BYTE) {
            addrBytes[0] = (uint8_t)((currentAddress >> 24) & 0xFFU);
            addrBytes[1] = (uint8_t)((currentAddress >> 16) & 0xFFU);
            addrBytes[2] = (uint8_t)((currentAddress >> 8) & 0xFFU);
            addrBytes[3] = (uint8_t)(currentAddress & 0xFFU);
            addrLen      = 4U;
        } else {
            addrBytes[0] = (uint8_t)((currentAddress >> 16) & 0xFFU);
            addrBytes[1] = (uint8_t)((currentAddress >> 8) & 0xFFU);
            addrBytes[2] = (uint8_t)(currentAddress & 0xFFU);
            addrLen      = 3U;
        }

        /* Create combined buffer for address and data */
        uint32_t totalBytes = addrLen + bytesToWrite;

        uint16_t combinedBuffer[MAX_COMBINED_SIZE / 2U];

        uint32_t byteIndex = 0U;
        uint32_t wordIndex = 0U;

        while (byteIndex < totalBytes) {
            uint8_t b0;
            uint8_t b1;

            /* Get first byte (LSB) */
            if (byteIndex < addrLen) {
                b0 = addrBytes[byteIndex];
            } else {
                b0 = dataPtr[byteIndex - addrLen];
            }
            byteIndex++;

            /* Get second byte (MSB), if exists */
            if (byteIndex < totalBytes) {
                if (byteIndex < addrLen) {
                    b1 = addrBytes[byteIndex];
                } else {
                    b1 = dataPtr[byteIndex - addrLen];
                }
                byteIndex++;
            } else {
                /* Padding for odd byte count */
                b1 = 0U;
            }

            combinedBuffer[wordIndex++] = ((uint16_t) b1 << 8) | b0;
        }

        /* Execute page program command */
        status = QSPIFLASH_executeConfigurationFrame(
            handle, combinedBuffer, addrLen + bytesToWrite, commandConfig);

        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }

        writeConfig.instruction  = READ_STATUS_REG_CMD;
        writeConfig.transferMode = mode;

        /* Wait for programming to complete */
        status = QSPIFLASH_waitForReady(handle, &writeConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }

        /* Update counters and pointers */
        remainingBytes -= bytesToWrite;
        dataPtr += bytesToWrite;
        currentAddress += bytesToWrite;

        /* Calculate bytes to write for next iteration */
        bytesToWrite = (remainingBytes > pageSize) ? pageSize : remainingBytes;
    }

    return status;
}

QSPIFLASH_Status QSPIFLASH_eraseSector(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;

    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPIFLASH_CommandConfig eraseConfig = {0};
    uint32_t address = commandConfig->address;

    /* Align address to sector boundary */
    address &= ~(SECTOR_SIZE - 1U);

    /* Enable write operations */
    eraseConfig.instruction  = WRITE_ENABLE_CMD;
    eraseConfig.transferMode = commandConfig->transferMode;
    result                   = QSPIFLASH_writeEnable(handle, &eraseConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Prepare address bytes */
    uint8_t addrBytes[4];
    uint32_t addrLen;

    if (commandConfig->addressSize == QSPIFLASH_ADDRESS_SIZE_4BYTE) {
        addrBytes[0] = (uint8_t)((address >> 24) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[2] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[3] = (uint8_t)(address & 0xFFU);
        addrLen      = 4U;
    } else {
        addrBytes[0] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[2] = (uint8_t)(address & 0xFFU);
        addrLen      = 3U;
    }

    /* Pack address into 16-bit buffer for executeConfigurationFrame */
    uint16_t addrBuffer[2];
    uint32_t byteIndex = 0U;
    uint32_t wordIndex = 0U;

    while (byteIndex < addrLen) {
        uint8_t b0 = addrBytes[byteIndex++];
        uint8_t b1 = (byteIndex < addrLen) ? addrBytes[byteIndex++] : 0U;
        addrBuffer[wordIndex++] = ((uint16_t) b1 << 8) | b0;
    }

    /* Send erase command and address */
    result = QSPIFLASH_executeConfigurationFrame(
        handle, addrBuffer, addrLen, commandConfig);

    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Wait for erase to complete */
    eraseConfig.instruction  = READ_STATUS_REG_CMD;
    eraseConfig.transferMode = commandConfig->transferMode;
    return QSPIFLASH_waitForReady(handle, &eraseConfig);
}

QSPIFLASH_Status QSPIFLASH_eraseBlock(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;

    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPIFLASH_CommandConfig eraseConfig = {0};
    uint32_t address = commandConfig->address;

    /* Enable write operations */
    eraseConfig.instruction  = WRITE_ENABLE_CMD;
    eraseConfig.transferMode = commandConfig->transferMode;
    result                   = QSPIFLASH_writeEnable(handle, &eraseConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Prepare address bytes */
    uint8_t addrBytes[4];
    uint32_t addrLen;

    if (commandConfig->addressSize == QSPIFLASH_ADDRESS_SIZE_4BYTE) {
        addrBytes[0] = (uint8_t)((address >> 24) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[2] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[3] = (uint8_t)(address & 0xFFU);
        addrLen      = 4U;
    } else {
        addrBytes[0] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[2] = (uint8_t)(address & 0xFFU);
        addrLen      = 3U;
    }

    /* Pack address into 16-bit buffer for executeConfigurationFrame */
    uint16_t addrBuffer[2];
    uint32_t byteIndex = 0U;
    uint32_t wordIndex = 0U;

    while (byteIndex < addrLen) {
        uint8_t b0 = addrBytes[byteIndex++];
        uint8_t b1 = (byteIndex < addrLen) ? addrBytes[byteIndex++] : 0U;
        addrBuffer[wordIndex++] = ((uint16_t) b1 << 8) | b0;
    }

    /* Send erase command and address */
    result = QSPIFLASH_executeConfigurationFrame(
        handle, addrBuffer, addrLen, commandConfig);

    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Wait for erase to complete */
    eraseConfig.instruction  = READ_STATUS_REG_CMD;
    eraseConfig.transferMode = commandConfig->transferMode;
    return QSPIFLASH_waitForReady(handle, &eraseConfig);
}

QSPIFLASH_Status QSPIFLASH_eraseChip(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    QSPIFLASH_Status result = QSPIFLASH_STATUS_SUCCESS;

    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    QSPIFLASH_CommandConfig eraseConfig = {0};

    /* Enable write operations */
    eraseConfig.instruction  = WRITE_ENABLE_CMD;
    eraseConfig.transferMode = commandConfig->transferMode;
    result                   = QSPIFLASH_writeEnable(handle, &eraseConfig);
    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Send chip erase command */
    result =
        QSPIFLASH_executeConfigurationFrame(handle, NULL, 0U, commandConfig);

    if (result != QSPIFLASH_STATUS_SUCCESS) {
        return result;
    }

    /* Wait for erase to complete - this can take a long time */
    eraseConfig.instruction  = READ_STATUS_REG_CMD;
    eraseConfig.transferMode = commandConfig->transferMode;
    return QSPIFLASH_waitForReady(handle, &eraseConfig);
}

QSPIFLASH_Status QSPIFLASH_deinit(QSPIFLASH_Handle *handle)
{
    if (handle == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_ERROR;
    }

    /* Disable QSPI */
    DL_QSPI_disable(handle->qspiRegs);

    /* Disable power to QSPI peripheral */
    DL_QSPI_disablePower(handle->qspiRegs);

    /* Mark as uninitialized */
    handle->initialized = false;

    return QSPIFLASH_STATUS_SUCCESS;
}

QSPIFLASH_Status QSPIFLASH_preReadTransfer(QSPIFLASH_Handle *handle,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    uint32_t address                  = commandConfig->address;
    QSPI_Regs *qspi                   = handle->qspiRegs;
    uint8_t cmd                       = commandConfig->instruction;
    QSPIFLASH_TransferMode mode       = (QSPIFLASH_TransferMode)commandConfig->transferMode;
    uint32_t dummyCycles              = commandConfig->dummyCycles;
    QSPIFLASH_AddressSize addressSize = (QSPIFLASH_AddressSize)commandConfig->addressSize;
    QSPIFLASH_Status status           = QSPIFLASH_STATUS_SUCCESS;

    uint8_t addrBytes[4];
    uint32_t addrLen;

    if (commandConfig->addressSize == QSPIFLASH_ADDRESS_SIZE_4BYTE) {
        addrBytes[0] = (uint8_t)((address >> 24) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[2] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[3] = (uint8_t)(address & 0xFFU);
        addrLen      = 4U;
    } else {
        addrBytes[0] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[2] = (uint8_t)(address & 0xFFU);
        addrLen      = 3U;
    }

    /* Wait for QSPI to be idle as required by all frame types */
    if (!DL_QSPI_isQSPIIdle(qspi)) {
        status = QSPIFLASH_waitForIdle(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* Disable QSPI before reconfiguring */
    DL_QSPI_disable(qspi);

    /* Set bus format based on frame type */
    DL_QSPI_BUS_FORMAT busFormat;

    /* Data frames must use format with QSPIFORMAT[3] as '1' */
    switch (mode) {
        case QSPIFLASH_TRANSFER_MODE_SPI:
            busFormat = DL_QSPI_BUS_FORMAT_SSS_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_DUAL_OUT:
            busFormat = DL_QSPI_BUS_FORMAT_SSD_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_DUAL_IO:
            busFormat = DL_QSPI_BUS_FORMAT_SDD_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_QUAD_OUT:
            busFormat = DL_QSPI_BUS_FORMAT_SSQ_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_QUAD_IO:
            busFormat = DL_QSPI_BUS_FORMAT_SQQ_MODE;
            break;
        case QSPIFLASH_TRANSFER_MODE_QPI:
            busFormat = DL_QSPI_BUS_FORMAT_QQQ_MODE;
            break;
        default:
            return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Configure bus format */
    DL_QSPI_setBusFormat(qspi, busFormat);

    /* Set command byte */
    DL_QSPI_setCommandByte(qspi, cmd);

    /* Set address mode */
    DL_QSPI_ADDRESS_MODE addressMode =
        (addressSize == QSPIFLASH_ADDRESS_SIZE_3BYTE)
            ? DL_QSPI_ADDRESS_MODE_3BYTE
            : DL_QSPI_ADDRESS_MODE_4BYTE;
    DL_QSPI_setAddressMode(qspi, addressMode);

    /* Performance Mode */
    DL_QSPI_disablePerformanceMode(qspi);

    /* Set dummy cycles */
    DL_QSPI_setDummyClock(qspi, dummyCycles);

    DL_QSPI_WORD_SIZE wordSize = DL_QSPI_getWordSize(qspi);

    /* Set word size to 16-bit for Address transfer */
    DL_QSPI_setWordSize(qspi, DL_QSPI_WORD_SIZE_16);

    /* Set TX count as size of address (in bytes) */
    DL_QSPI_setTXCount(qspi, addrLen);

    /* Configure RX count based on size */
    if (size < 256U) {
        DL_QSPI_setRXCount(qspi, size);
    } else {
        DL_QSPI_setRXCount(qspi, QSPI_QSPICTL1_RXCOUNT_RXFIFOMON);
    }
    DL_QSPI_enablePreFetch(qspi);

    /* Enable QSPI */
    DL_QSPI_enable(qspi);

    /* Flush FIFOs */
    DL_QSPI_flushRXFIFO(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Send Address as 16-bit words */
    if (addrLen > 0U) {
        for (uint32_t i = 0U; i < addrLen; i += 2U) {
            uint8_t b0    = addrBytes[i];
            uint8_t b1    = ((i + 1U) < addrLen) ? addrBytes[i + 1U] : 0U;
            uint16_t word = ((uint16_t) b1 << 8) | b0;
            DL_QSPI_transmitDataBlocking16(qspi, word);
        }
    }

    /* Switch back to selected word size for DMA data transfer */
    DL_QSPI_setWordSize(qspi, wordSize);

    return status;
}

QSPIFLASH_Status QSPIFLASH_postReadTransfer(QSPIFLASH_Handle *handle)
{
    if (handle == NULL) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_Status status = QSPIFLASH_STATUS_SUCCESS;
    QSPI_Regs *qspi = handle->qspiRegs;

    /* Deassert chip select */
    DL_QSPI_deAssertChipSelect(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Wait for QSPI to be idle */
    status = QSPIFLASH_waitForIdle(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Release chip select for subsequent frame */
    DL_QSPI_releaseChipSelect(qspi);

    return status;
}

QSPIFLASH_Status QSPIFLASH_preWriteTransfer(QSPIFLASH_Handle *handle,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL || !handle->initialized ||
        size == 0U) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    uint32_t address                  = commandConfig->address;
    QSPI_Regs *qspi                   = handle->qspiRegs;
    uint8_t cmd                       = commandConfig->instruction;
    QSPIFLASH_TransferMode mode       = (QSPIFLASH_TransferMode)commandConfig->transferMode;
    QSPIFLASH_AddressSize addressSize = (QSPIFLASH_AddressSize)commandConfig->addressSize;
    QSPIFLASH_Status status           = QSPIFLASH_STATUS_SUCCESS;

    /* For write operations, only half-duplex modes are supported */
    if (mode != QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX &&
        mode != QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX) {
        return QSPIFLASH_STATUS_INVALID_MODE;
    }

    /* Prepare address bytes */
    uint8_t addrBytes[4];
    uint32_t addrLen;

    if (addressSize == QSPIFLASH_ADDRESS_SIZE_4BYTE) {
        addrBytes[0] = (uint8_t)((address >> 24) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[2] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[3] = (uint8_t)(address & 0xFFU);
        addrLen      = 4U;
    } else {
        addrBytes[0] = (uint8_t)((address >> 16) & 0xFFU);
        addrBytes[1] = (uint8_t)((address >> 8) & 0xFFU);
        addrBytes[2] = (uint8_t)(address & 0xFFU);
        addrLen      = 3U;
    }

    /* Enable write operations */
    QSPIFLASH_CommandConfig writeConfig = {0};
    writeConfig.instruction  = WRITE_ENABLE_CMD;
    writeConfig.transferMode = mode;
    status                   = QSPIFLASH_writeEnable(handle, &writeConfig);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Wait for QSPI to be idle */
    if (!DL_QSPI_isQSPIIdle(qspi)) {
        status = QSPIFLASH_waitForIdle(qspi);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }
    }

    /* Disable QSPI before reconfiguring */
    DL_QSPI_disable(qspi);

    /* Set bus format based on half-duplex mode */
    DL_QSPI_BUS_FORMAT busFormat =
        (mode == QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX)
            ? DL_QSPI_BUS_FORMAT_SPI_HALF_DUPLEX
            : DL_QSPI_BUS_FORMAT_QPI_HALF_DUPLEX;

    /* Configure bus format */
    DL_QSPI_setBusFormat(qspi, busFormat);

    /* Set address mode */
    DL_QSPI_ADDRESS_MODE addressMode =
        (addressSize == QSPIFLASH_ADDRESS_SIZE_3BYTE)
            ? DL_QSPI_ADDRESS_MODE_3BYTE
            : DL_QSPI_ADDRESS_MODE_4BYTE;
    DL_QSPI_setAddressMode(qspi, addressMode);

    /* Calculate total TX bytes: 1 (command) + address length + data size */
    uint32_t totalTxBytes = 1U + addrLen + size;

    /* Set TX counts in QSPICTL1 register */
    DL_QSPI_setTXCount(qspi, totalTxBytes);

    /* For configuration frame (write), RXCOUNT=0 and disable Prefetch */
    DL_QSPI_setRXCount(qspi, 0U);
    DL_QSPI_disablePreFetch(qspi);

    /* Enable QSPI */
    DL_QSPI_enable(qspi);

    /* Flush FIFOs */
    DL_QSPI_flushRXFIFO(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    DL_QSPI_WORD_SIZE wordSize = DL_QSPI_getWordSize(qspi);
    /*
     * Send command + address as individual bytes to avoid padding issues.
     * For 4-byte address (5 bytes total with cmd), using 16-bit words would
     * add a padding byte that corrupts DMA data. Using 8-bit transmits ensures
     * exact byte count in FIFO.
     */
    DL_QSPI_setWordSize(qspi, DL_QSPI_WORD_SIZE_8);

    /* Send command byte */
    DL_QSPI_transmitDataBlocking8(qspi, cmd);

    /* Send address bytes */
    for (uint32_t i = 0U; i < addrLen; i++) {
        DL_QSPI_transmitDataBlocking8(qspi, addrBytes[i]);
    }

    /* Switch back to selected word size for DMA data transfer */
    DL_QSPI_setWordSize(qspi, wordSize);

    /* QSPI is now ready for DMA to write data to TX FIFO */
    return QSPIFLASH_STATUS_SUCCESS;
}

QSPIFLASH_Status QSPIFLASH_postWriteTransfer(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig)
{
    if (handle == NULL || commandConfig == NULL) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_Status status = QSPIFLASH_STATUS_SUCCESS;
    QSPI_Regs *qspi = handle->qspiRegs;

    /* Check stall */
    status = QSPIFLASH_waitForStall(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Deassert chip select */
    DL_QSPI_deAssertChipSelect(qspi);
    DL_QSPI_flushTXFIFO(qspi);

    /* Wait for QSPI to be idle */
    status = QSPIFLASH_waitForIdle(qspi);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Release chip select for subsequent frame */
    DL_QSPI_releaseChipSelect(qspi);

    /* Wait for flash programming to complete */
    QSPIFLASH_CommandConfig waitConfig = {0};
    waitConfig.instruction  = READ_STATUS_REG_CMD;
    waitConfig.transferMode = commandConfig->transferMode;
    status                  = QSPIFLASH_waitForReady(handle, &waitConfig);

    return status;
}

QSPIFLASH_Status QSPIFLASH_writeDMA(QSPIFLASH_Handle *handle, const void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig,
    const QSPIFLASH_DMAConfig *dmaConfig)
{
    if (handle == NULL || data == NULL || size == 0U ||
        commandConfig == NULL || dmaConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    if (dmaConfig->startDMA == NULL || dmaConfig->dmaCompleteFlag == NULL) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_TransferMode mode = (QSPIFLASH_TransferMode)commandConfig->transferMode;

    /* For write operations, only half-duplex modes are supported */
    if (mode != QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX &&
        mode != QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX) {
        return QSPIFLASH_STATUS_INVALID_MODE;
    }

    QSPIFLASH_Status status = QSPIFLASH_STATUS_SUCCESS;
    QSPIFLASH_CommandConfig pageConfig = {0};
    const uint8_t *dataPtr  = (const uint8_t *) data;
    uint32_t currentAddress = commandConfig->address;
    uint32_t remainingBytes = size;

    /* Copy command config for page operations */
    pageConfig.instruction  = commandConfig->instruction;
    pageConfig.addressSize  = commandConfig->addressSize;
    pageConfig.transferMode = commandConfig->transferMode;
    pageConfig.dummyCycles  = commandConfig->dummyCycles;

    /* Process data page by page */
    while (remainingBytes > 0U) {
        /* Calculate bytes for this page (handle page boundary) */
        uint32_t pageOffset  = currentAddress & (PAGE_SIZE - 1U);
        uint32_t bytesInPage = PAGE_SIZE - pageOffset;
        uint32_t bytesToWrite =
            (remainingBytes < bytesInPage) ? remainingBytes : bytesInPage;

        /* Update address for this page */
        pageConfig.address = currentAddress;

        /* Setup QSPI for this page write (includes write enable) */
        status = QSPIFLASH_preWriteTransfer(handle, bytesToWrite, &pageConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }

        /* Start DMA transfer for this page */
        dmaConfig->startDMA(dataPtr, bytesToWrite);

        /* Wait for DMA to complete */
        while (*(dmaConfig->dmaCompleteFlag) == false) {
            /* Optionally could add timeout here */
        }

        /* Complete the page write (wait for flash programming) */
        status = QSPIFLASH_postWriteTransfer(handle, &pageConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            return status;
        }

        /* Update pointers and counters for next page */
        dataPtr += bytesToWrite;
        currentAddress += bytesToWrite;
        remainingBytes -= bytesToWrite;
    }

    return QSPIFLASH_STATUS_SUCCESS;
}

QSPIFLASH_Status QSPIFLASH_readDMA(QSPIFLASH_Handle *handle, void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig,
    const QSPIFLASH_DMAConfig *dmaConfig)
{
    if (handle == NULL || data == NULL || size == 0U ||
        commandConfig == NULL || dmaConfig == NULL || !handle->initialized) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    if (dmaConfig->startDMA == NULL || dmaConfig->dmaCompleteFlag == NULL) {
        return QSPIFLASH_STATUS_INVALID_PARAMS;
    }

    QSPIFLASH_Status status = QSPIFLASH_STATUS_SUCCESS;

    /* Setup QSPI for read operation (sends command + address) */
    status = QSPIFLASH_preReadTransfer(handle, size, commandConfig);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        return status;
    }

    /* Start DMA transfer (from QSPI RX FIFO to user buffer) */
    dmaConfig->startDMA(data, size);

    /* Wait for DMA to complete */
    while (*(dmaConfig->dmaCompleteFlag) == false) {
        /* Optionally could add timeout here */
    }

    /* Complete the read transfer */
    status = QSPIFLASH_postReadTransfer(handle);

    return status;
}
