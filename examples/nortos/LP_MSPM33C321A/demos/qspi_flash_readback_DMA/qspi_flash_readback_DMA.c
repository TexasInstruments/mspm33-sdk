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

#include <ti/external_flash/QSPI_FLASH.h>
#include "ti_msp_dl_config.h"

/*******************************************************************************
 * ERROR FLAG DEFINITIONS
 ******************************************************************************/

/** @defgroup EXAMPLE_ERROR_FLAGS Error Flag Definitions
 *  @brief Error codes to identify which operation failed
 *  @{
 */

#define ERROR_NONE 0x00U
#define ERROR_INIT 0x01U
#define ERROR_READID 0x02U
#define ERROR_SET_QEBIT 0x03U
#define ERROR_ENABLE_QPI 0x04U
#define ERROR_CHIP_ERASE 0x05U
#define ERROR_PAGE_PROGRAM 0x06U
#define ERROR_READ 0x07U
#define ERROR_DISABLE_QPI 0x08U
#define ERROR_DEINIT_QSPI 0x09U
#define ERROR_DATA_MISMATCH 0x0AU

/*******************************************************************************
 * TEST DATA SIZE DEFINITIONS
 ******************************************************************************/

/*! Size of test data in bytes (512 bytes = 2 pages to demonstrate multi-page DMA write) */
#define TEST_DATA_SIZE_BYTES 512U
#define TEST_DATA_SIZE_WORDS (TEST_DATA_SIZE_BYTES / 2U)

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/*! QSPIFLASH driver handle */
QSPIFLASH_Handle handle;

/*! QSPIFLASH initialization parameters */
QSPIFLASH_InitParams params;

/*! Status return from QSPIFLASH APIs */
QSPIFLASH_Status status;

/*! Command configuration for QSPI operations */
QSPIFLASH_CommandConfig commandConfig;

/*! Starting address for flash operations */
uint32_t ADDRESS = 0x00001000U;

/*! Overall test pass/fail status */
bool gStatus = true;

/*! Error flag indicating which operation failed */
uint8_t gErrorFlag = ERROR_NONE;

/*! Buffer to store JEDEC ID (manufacturer ID, memory type, capacity) */
uint8_t id[3];

/*! Buffer to store data read back from flash */
uint16_t readback_data[TEST_DATA_SIZE_WORDS];

volatile bool gDMARXDataTransferred = false;
volatile bool gDMATXDataTransferred = false;

/*! DMA configuration for writeDMA API */
QSPIFLASH_DMAConfig dmaConfig;

/*******************************************************************************
 * DMA CALLBACK FUNCTION
 ******************************************************************************/

/*!
 *  @brief DMA start callback for QSPIFLASH_writeDMA (TX)
 *
 *  This function is called by QSPIFLASH_writeDMA to start each page transfer.
 *  It configures and enables the DMA channel to transfer data to QSPI TX FIFO.
 *
 *  @param[in]  srcAddr     Source address of data to transfer
 *  @param[in]  sizeBytes   Number of bytes to transfer
 */
void startDMATxTransfer(const void *srcAddr, uint32_t sizeBytes)
{
    /* Reset DMA completion flag */
    *(dmaConfig.dmaCompleteFlag) = false;

    /* Re-enable the TX DMA done interrupt for this transfer */
    DL_QSPI_enableInterrupt(QSPI_0_INST, DL_QSPI_INTERRUPT_DMA_DONE_TX);

    /* Configure DMA source, destination, and size */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t) srcAddr);
    DL_DMA_setDestAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t)(&(QSPI->TXDATA)));
    DL_DMA_setTransferSize(
        DMA0, DMA0_CH1_CHAN_ID, sizeBytes / sizeof(uint16_t));

    /* Enable DMA channel to start transfer */
    DL_DMA_enableChannel(DMA0, DMA0_CH1_CHAN_ID);
}

/*!
 *  @brief DMA start callback for QSPIFLASH_readDMA (RX)
 *
 *  This function is called by QSPIFLASH_readDMA to start the DMA transfer.
 *  It configures and enables the DMA channel to transfer data from QSPI RX FIFO.
 *
 *  @param[in]  destAddr    Destination address to store received data
 *  @param[in]  sizeBytes   Number of bytes to transfer
 */
void startDMARxTransfer(const void *destAddr, uint32_t sizeBytes)
{
    /* Reset DMA completion flag */
    *(dmaConfig.dmaCompleteFlag) = false;

    /* Re-enable the RX DMA done interrupt for this transfer */
    DL_QSPI_enableInterrupt(QSPI_0_INST, DL_QSPI_INTERRUPT_DMA_DONE_RX);

    /* Configure DMA source, destination, and size */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(&(QSPI->RXDATA)));
    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) destAddr);
    DL_DMA_setTransferSize(
        DMA0, DMA0_CH0_CHAN_ID, sizeBytes / sizeof(uint16_t));

    /* Enable DMA channel to start transfer */
    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);
}

/*! Test data pattern to program into flash (512 bytes as 256 x 16-bit words) */
uint16_t page_program_data[TEST_DATA_SIZE_WORDS] = {
    /* Page 1 (256 bytes) */
    0xCFC0U, 0x0BFCU, 0x2A53U, 0xD38FU, 0x4FF9U, 0x264FU, 0x8B82U, 0x32E9U,
    0x0338U, 0x539EU, 0x6EFEU, 0x2988U, 0x054DU, 0x3C6DU, 0x14AFU, 0xA297U,
    0x8011U, 0x08CAU, 0xB5FFU, 0xE468U, 0xCD7AU, 0x3CF7U, 0xAA69U, 0x6527U,
    0x5322U, 0x87B7U, 0x9795U, 0x1088U, 0xFC75U, 0xD339U, 0x29DFU, 0x9A1AU,
    0x1936U, 0xE93CU, 0xDE0EU, 0xB235U, 0x6666U, 0x9A6EU, 0xE783U, 0x1B39U,
    0xD26FU, 0x4DA1U, 0xE8E0U, 0xAE68U, 0x4773U, 0xE765U, 0x8D39U, 0xBA1EU,
    0x4EA5U, 0x32C2U, 0x8A36U, 0x30BBU, 0xCA1CU, 0x0761U, 0x22D0U, 0x7650U,
    0xE91BU, 0x1AA2U, 0xFE86U, 0x5173U, 0x7DF3U, 0x32D8U, 0xCA88U, 0xD33EU,
    0x1A40U, 0x83B3U, 0x54EEU, 0x0414U, 0x183DU, 0x2D5CU, 0x57ABU, 0xBC6BU,
    0x2E35U, 0x705EU, 0x2CD7U, 0x36E8U, 0xBD49U, 0x55B9U, 0xE38EU, 0xFAE8U,
    0xBCF5U, 0xF3C9U, 0x513AU, 0xECBAU, 0x0BE4U, 0x8D0CU, 0x86CBU, 0x7A00U,
    0xCDA8U, 0x38C1U, 0x95E6U, 0x262AU, 0x7389U, 0x7BF1U, 0xA804U, 0x873EU,
    0x2FC4U, 0x1E52U, 0x9508U, 0xB2CAU, 0x20B6U, 0xA7E5U, 0x2A4EU, 0xF205U,
    0x0D5FU, 0x3F2BU, 0x11DDU, 0xD6B1U, 0x261BU, 0xA450U, 0x54F2U, 0xCED8U,
    0x9795U, 0x5EC1U, 0x93E0U, 0xCA7FU, 0xDBEBU, 0x1FEAU, 0xEA03U, 0xA28BU,
    0x1E80U, 0xB6AFU, 0x506FU, 0xEFB3U, 0x69DFU, 0x8C52U, 0x1234U, 0x5678U,
    /* Page 2 (256 bytes) */
    0xAAAAU, 0xBBBBU, 0xCCCCU, 0xDDDDU, 0xEEEEU, 0xFFFFU, 0x1111U, 0x2222U,
    0x3333U, 0x4444U, 0x5555U, 0x6666U, 0x7777U, 0x8888U, 0x9999U, 0x0000U,
    0xABCDU, 0xDCBAU, 0x1234U, 0x4321U, 0x5678U, 0x8765U, 0x9ABCU, 0xCBA9U,
    0xDEF0U, 0x0FEDU, 0x1357U, 0x7531U, 0x2468U, 0x8642U, 0xACEBU, 0xBECAU,
    0xFACEU, 0xCAFEU, 0xBABEU, 0xBEADU, 0xDEADU, 0xBEEFU, 0xFEEDU, 0xF00DU,
    0xC0DEU, 0xD0C5U, 0xB0A7U, 0xA0B5U, 0x90C3U, 0x80D1U, 0x70E2U, 0x60F0U,
    0x50A1U, 0x40B2U, 0x30C3U, 0x20D4U, 0x10E5U, 0x00F6U, 0xF1E2U, 0xE2D3U,
    0xD3C4U, 0xC4B5U, 0xB5A6U, 0xA697U, 0x9788U, 0x8879U, 0x796AU, 0x6A5BU,
    0x5B4CU, 0x4C3DU, 0x3D2EU, 0x2E1FU, 0x1F00U, 0x00FFU, 0xFF00U, 0x00AAU,
    0xAA55U, 0x55AAU, 0xA5A5U, 0x5A5AU, 0xF0F0U, 0x0F0FU, 0xC3C3U, 0x3C3CU,
    0x9696U, 0x6969U, 0xA9A9U, 0x9A9AU, 0xB8B8U, 0x8B8BU, 0xC7C7U, 0x7C7CU,
    0xD6D6U, 0x6D6DU, 0xE5E5U, 0x5E5EU, 0xF4F4U, 0x4F4FU, 0x0303U, 0x3030U,
    0x1212U, 0x2121U, 0x4545U, 0x5454U, 0x7878U, 0x8787U, 0xABABU, 0xBABAU,
    0xCDCDU, 0xDCDCU, 0xEFEFU, 0xFEFEU, 0x0101U, 0x1010U, 0x2323U, 0x3232U,
    0x4545U, 0x5454U, 0x6767U, 0x7676U, 0x8989U, 0x9898U, 0xABCDU, 0xEF01U,
    0x2345U, 0x6789U, 0xABCDU, 0xEF01U, 0xFEDCU, 0xBA98U, 0x7654U, 0x3210U};

/*******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/

int main(void)
{
    /* Initialize device peripherals */
    SYSCFG_DL_init();

    NVIC_EnableIRQ(QSPI_0_INST_INT_IRQN);

    /* Initialize QSPI driver */
    params.qspiRegs = QSPI;
    status          = QSPIFLASH_init(&handle, &params);
    if (status != QSPIFLASH_STATUS_SUCCESS) {
        gErrorFlag = ERROR_INIT;
        gStatus    = false;
    }

    /* Read External Flash JEDEC ID in SPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = READ_ID_CMD;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX;
        status = QSPIFLASH_readID(&handle, id, sizeof(id), &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_READID;
            gStatus    = false;
        }
    }

    /* Set QE (Quad Enable) bit in status register */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = WRITE_STATUS_CFG_REG_CMD;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX;
        status = QSPIFLASH_setQEBit(&handle, &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_SET_QEBIT;
            gStatus    = false;
        }
    }

    /* Enable QPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = ENABLE_QPI_CMD;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX;
        status = QSPIFLASH_enableQuadMode(&handle, &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_ENABLE_QPI;
            gStatus    = false;
        }
    }

    /* Sector Erase in QPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = SECTOR_ERASE_CMD;
        commandConfig.addressSize  = QSPIFLASH_ADDRESS_SIZE_3BYTE;
        commandConfig.address      = ADDRESS;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX;
        status = QSPIFLASH_eraseSector(&handle, &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_CHIP_ERASE;
            gStatus    = false;
        }
    }

    /* Page Program in QPI mode using DMA with automatic page handling */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = PAGE_PROGRAM_CMD;
        commandConfig.addressSize  = QSPIFLASH_ADDRESS_SIZE_3BYTE;
        commandConfig.address      = ADDRESS;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX;

        /* Configure DMA callbacks for writeDMA API */
        dmaConfig.startDMA        = startDMATxTransfer;
        dmaConfig.dmaCompleteFlag = &gDMATXDataTransferred;

        /* Select the word size for packing/unpacking from FIFO to flash interface.
        * Select 16 when DMA transfer size is 16 bits and number of bytes are even.
        * Select 8 when DMA transfer size is 8 bits or number of bytes are odd */
        DL_QSPI_setWordSize(QSPI, DL_QSPI_WORD_SIZE_16);

        /* Write all data (512 bytes = 2 pages) using single API call
         * The library automatically handles page boundaries */
        status = QSPIFLASH_writeDMA(&handle, page_program_data,
            sizeof(page_program_data), &commandConfig, &dmaConfig);

        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_PAGE_PROGRAM;
            gStatus    = false;
        }
    }

    /* Read data back in QPI mode using DMA */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = QUAD_IO_READ_CMD;
        commandConfig.addressSize  = QSPIFLASH_ADDRESS_SIZE_3BYTE;
        commandConfig.address      = ADDRESS;
        commandConfig.dummyCycles  = DUMMY_CYCLES_QUAD_IO_READ;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI;

        /* Configure DMA callbacks for readDMA API */
        dmaConfig.startDMA        = startDMARxTransfer;
        dmaConfig.dmaCompleteFlag = &gDMARXDataTransferred;

        /* Select the word size for packing/unpacking from FIFO to flash interface.
         * Select 16 when DMA transfer size is 16 bits and number of bytes are even.
         * Select 8 when DMA transfer size is 8 bits or number of bytes are odd */
        DL_QSPI_setWordSize(QSPI, DL_QSPI_WORD_SIZE_16);

        /* Read all data using single API call */
        status = QSPIFLASH_readDMA(&handle, readback_data,
            sizeof(readback_data), &commandConfig, &dmaConfig);

        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_READ;
            gStatus    = false;
        }
    }

    /* Disable QPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = DISABLE_QPI_CMD;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX;
        status = QSPIFLASH_disableQuadMode(&handle, &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_DISABLE_QPI;
            gStatus    = false;
        }
    }

    /* De-init QSPI */
    if (gErrorFlag == ERROR_NONE) {
        status = QSPIFLASH_deinit(&handle);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_DEINIT_QSPI;
            gStatus    = false;
        }
    }

    /* Verify programmed data matches read data */
    if (gErrorFlag == ERROR_NONE) {
        for (uint32_t i = 0U; i < sizeof(readback_data) / sizeof(uint16_t);
             i++) {
            if (page_program_data[i] != readback_data[i]) {
                gErrorFlag = ERROR_DATA_MISMATCH;
                gStatus    = false;
                break;
            }
        }
    }

    /* Indicate pass/fail status via LED */
    if (gStatus) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    __BKPT(0);
    while (1) {
    }
}

void QSPI_0_INST_IRQHandler(void)
{
    switch (DL_QSPI_getPendingInterrupt(QSPI_0_INST)) {
        case DL_QSPI_IIDX_DMA_DONE_TX:
            /* DMA0 is done transferring data from page_program_data to TXFIFO*/
            gDMATXDataTransferred = true;
            DL_QSPI_disableInterrupt(
                QSPI_0_INST, DL_QSPI_INTERRUPT_DMA_DONE_TX);
            break;
        case DL_QSPI_IIDX_DMA_DONE_RX:
            /* DMA0 is done transferring data from RXFIFO to readback_data*/
            gDMARXDataTransferred = true;
            DL_QSPI_disableInterrupt(
                QSPI_0_INST, DL_QSPI_INTERRUPT_DMA_DONE_RX);
            break;
        default:
            break;
    }
}
