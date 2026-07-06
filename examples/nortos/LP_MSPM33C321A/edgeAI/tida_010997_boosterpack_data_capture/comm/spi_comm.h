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
 
#ifndef SPI_COMM_H_
#define SPI_COMM_H_

#include <stdint.h>
#include <stdbool.h>
#include "ti_msp_dl_config.h"
#include "app_cfg.h"
#include <ti/hal/mspm33/include/Spi_Hal.h>

/**
 * @brief SPI status codes
 */
#define SPI_COMM_OK                 0    /**< Success */
#define SPI_COMM_ERR               -1    /**< Error   */

typedef struct {
    GPIO_Regs *csPort;
    uint32_t   csPin;
} chipSelect;

extern Spi_Hal_InstanceType  gSpiInstance;
extern volatile bool         gSpiTransferComplete;
extern volatile uint32_t     gSpiTransferErrorCode;

/**
 * @brief Initialize the SPI interface (configures Spi_Hal, enables SPI and NVIC)
 *
 * @return int 0 on success, error code otherwise
 */
int SPI_init(void);

/**
 * @brief Toggle CS low then high (used to wake device from deep sleep)
 */
void SPI_toggleCS(const chipSelect *cs);

/**
 * @brief Write/Read data through SPI Interface
 *
 * @param txData  Pointer to data to be written
 * @param rxData  Pointer to rx buffer
 * @param len     Read/Write size in bytes
 * @param cs      Pointer to chip select struct
 *
 * @return int 0 on success, error code otherwise
 */
int SPI_transfer(uint8_t *txData, uint8_t *rxData, uint16_t len, const chipSelect *cs);


#endif /* SPI_COMM_H_ */
