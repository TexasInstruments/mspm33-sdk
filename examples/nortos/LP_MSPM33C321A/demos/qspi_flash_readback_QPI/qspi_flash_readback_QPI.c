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
#define ERROR_DATA_MISMATCH 0x09U

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
uint32_t ADDRESS = 0x00000000U;

/*! Overall test pass/fail status */
bool gStatus = true;

/*! Error flag indicating which operation failed */
uint8_t gErrorFlag = ERROR_NONE;

/*! Buffer to store JEDEC ID (manufacturer ID, memory type, capacity) */
uint8_t id[3];

/*! Buffer to store data read back from flash */
uint8_t readback_data[256];

/*! Test data pattern to program into flash (256 bytes as 128 x 16-bit words) */
uint16_t page_program_data[128] = {0xCFC0U, 0x0BFCU, 0x2A53U, 0xD38FU, 0x4FF9U,
    0x264FU, 0x8B82U, 0x32E9U, 0x0338U, 0x539EU, 0x6EFEU, 0x2988U, 0x054DU,
    0x3C6DU, 0x14AFU, 0xA297U, 0x8011U, 0x08CAU, 0xB5FFU, 0xE468U, 0xCD7AU,
    0x3CF7U, 0xAA69U, 0x6527U, 0x5322U, 0x87B7U, 0x9795U, 0x1088U, 0xFC75U,
    0xD339U, 0x29DFU, 0x9A1AU, 0x1936U, 0xE93CU, 0xDE0EU, 0xB235U, 0x6666U,
    0x9A6EU, 0xE783U, 0x1B39U, 0xD26FU, 0x4DA1U, 0xE8E0U, 0xAE68U, 0x4773U,
    0xE765U, 0x8D39U, 0xBA1EU, 0x4EA5U, 0x32C2U, 0x8A36U, 0x30BBU, 0xCA1CU,
    0x0761U, 0x22D0U, 0x7650U, 0xE91BU, 0x1AA2U, 0xFE86U, 0x5173U, 0x7DF3U,
    0x32D8U, 0xCA88U, 0xD33EU, 0x1A40U, 0x83B3U, 0x54EEU, 0x0414U, 0x183DU,
    0x2D5CU, 0x57ABU, 0xBC6BU, 0x2E35U, 0x705EU, 0x2CD7U, 0x36E8U, 0xBD49U,
    0x55B9U, 0xE38EU, 0xFAE8U, 0xBCF5U, 0xF3C9U, 0x513AU, 0xECBAU, 0x0BE4U,
    0x8D0CU, 0x86CBU, 0x7A00U, 0xCDA8U, 0x38C1U, 0x95E6U, 0x262AU, 0x7389U,
    0x7BF1U, 0xA804U, 0x873EU, 0x2FC4U, 0x1E52U, 0x9508U, 0xB2CAU, 0x20B6U,
    0xA7E5U, 0x2A4EU, 0xF205U, 0x0D5FU, 0x3F2BU, 0x11DDU, 0xD6B1U, 0x261BU,
    0xA450U, 0x54F2U, 0xCED8U, 0x9795U, 0x5EC1U, 0x93E0U, 0xCA7FU, 0xDBEBU,
    0x1FEAU, 0xEA03U, 0xA28BU, 0x1E80U, 0xB6AFU, 0x506FU, 0xEFB3U, 0x69DFU,
    0x8C52U, 0x1234U, 0x5678U};

/*******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/

int main(void)
{
    /* Initialize device peripherals */
    SYSCFG_DL_init();

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

    /* Chip Erase in QPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = CHIP_ERASE_CMD;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX;
        status = QSPIFLASH_eraseChip(&handle, &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_CHIP_ERASE;
            gStatus    = false;
        }
    }

    /* Page Program in QPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = PAGE_PROGRAM_CMD;
        commandConfig.addressSize  = QSPIFLASH_ADDRESS_SIZE_3BYTE;
        commandConfig.address      = ADDRESS;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX;
        status = QSPIFLASH_write(&handle, &page_program_data,
            sizeof(page_program_data), &commandConfig);
        if (status != QSPIFLASH_STATUS_SUCCESS) {
            gErrorFlag = ERROR_PAGE_PROGRAM;
            gStatus    = false;
        }
    }

    /* Read data back in QPI mode */
    if (gErrorFlag == ERROR_NONE) {
        commandConfig.instruction  = QUAD_IO_READ_CMD;
        commandConfig.addressSize  = QSPIFLASH_ADDRESS_SIZE_3BYTE;
        commandConfig.address      = ADDRESS;
        commandConfig.dummyCycles  = DUMMY_CYCLES_QUAD_IO_READ;
        commandConfig.transferMode = QSPIFLASH_TRANSFER_MODE_QPI;
        status                     = QSPIFLASH_read(&handle, &readback_data,
            sizeof(page_program_data), &commandConfig);
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

    /* Verify programmed data matches read data */
    if (gErrorFlag == ERROR_NONE) {
        for (uint32_t i = 0U; i < 255U; i++) {
            uint8_t byte =
                (uint8_t)((page_program_data[i / 2U]) >> (8U * (i % 2U)));
            if (byte != readback_data[i]) {
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
