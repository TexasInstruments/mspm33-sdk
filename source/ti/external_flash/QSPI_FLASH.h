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
 *  @file       QSPI_FLASH.h
 *  @brief      QSPI NOR Flash Driver Library
 *  @defgroup   QSPIFLASH QSPI NOR Flash Driver
 *
 *  @anchor ti_dl_QSPI_FLASH_Overview
 *  # Overview
 *
 *  The QSPIFLASH Driver Library provides APIs to interface with external
 *  NOR Flash devices over QSPI. It supports various transfer modes including
 *  SPI, Dual, and Quad modes for read/write/erase operations.
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup QSPIFLASH
 * @{
 */

#ifndef QSPIFLASH_H_
#define QSPIFLASH_H_

#include <stdbool.h>
#include <stdint.h>
#include <ti/driverlib/dl_qspi.h>
#include "NOR_FLASH_COMMANDS.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! @enum QSPIFLASH_Status
 *  @brief Status return codes for QSPIFLASH operations
 */
typedef enum {
    /*! Operation completed successfully */
    QSPIFLASH_STATUS_SUCCESS,
    /*! General error occurred during operation */
    QSPIFLASH_STATUS_ERROR,
    /*! Operation timed out waiting for completion */
    QSPIFLASH_STATUS_TIMEOUT,
    /*! Invalid parameters passed to the function */
    QSPIFLASH_STATUS_INVALID_PARAMS,
    /*! Invalid transfer mode requested for the operation */
    QSPIFLASH_STATUS_INVALID_MODE
} QSPIFLASH_Status;

/*! @enum QSPIFLASH_TransferMode
 *  @brief Transfer modes for QSPI communication with external flash
 */
typedef enum {
    /*! SPI format half-duplex mode for configuration/status frames */
    QSPIFLASH_TRANSFER_MODE_SPI_HALF_DUPLEX,
    /*! QPI format half-duplex mode for configuration/status frames in quad mode */
    QSPIFLASH_TRANSFER_MODE_QPI_HALF_DUPLEX,
    /*! Standard SPI mode (1-1-1), SSS mode for data frames */
    QSPIFLASH_TRANSFER_MODE_SPI,
    /*! Dual output mode (1-1-2), SSD mode for data frames */
    QSPIFLASH_TRANSFER_MODE_DUAL_OUT,
    /*! Dual I/O mode (1-2-2), SDD mode for data frames */
    QSPIFLASH_TRANSFER_MODE_DUAL_IO,
    /*! Quad output mode (1-1-4), SSQ mode for data frames */
    QSPIFLASH_TRANSFER_MODE_QUAD_OUT,
    /*! Quad I/O mode (1-4-4), SQQ mode for data frames */
    QSPIFLASH_TRANSFER_MODE_QUAD_IO,
    /*! Full Quad I/O mode (4-4-4), QQQ mode for data frames */
    QSPIFLASH_TRANSFER_MODE_QPI
} QSPIFLASH_TransferMode;

/*! @enum QSPIFLASH_AddressSize
 *  @brief Address size options for flash commands
 */
typedef enum {
    /*! 24-bit (3-byte) address size for flash up to 16MB */
    QSPIFLASH_ADDRESS_SIZE_3BYTE,
    /*! 32-bit (4-byte) address size for flash larger than 16MB */
    QSPIFLASH_ADDRESS_SIZE_4BYTE
} QSPIFLASH_AddressSize;

/*! @enum QSPIFLASH_TransferType
 *  @brief Transfer type for QSPI transfers
 */
typedef enum {
    /*! Blocking (synchronous) transfer - CPU waits for completion */
    QSPIFLASH_TRANSFER_SYNC,
    /*! DMA based transfer - uses DMA for data movement */
    QSPIFLASH_TRANSFER_DMA,
    /*! CPU Interrupt based transfer - uses interrupts for non-blocking operation */
    QSPIFLASH_TRANSFER_CPU_INT
} QSPIFLASH_TransferType;

/*!
 *  @brief Command configuration structure for QSPI flash operations
 *
 *  This structure contains all parameters needed to configure a QSPI
 *  flash command including instruction, address, dummy cycles, and transfer mode.
 */
typedef struct {
    /*! 1-byte instruction/command opcode to be sent to flash */
    uint32_t instruction;
    /*! Target address in flash memory */
    uint32_t address;
    /*! Address size. One of @ref QSPIFLASH_AddressSize */
    uint32_t addressSize;
    /*! Number of dummy cycles required after address phase */
    uint32_t dummyCycles;
    /*! Transfer mode for the operation. One of @ref QSPIFLASH_TransferMode */
    uint32_t transferMode;
} QSPIFLASH_CommandConfig;

/*!
 *  @brief Handle structure for QSPIFLASH driver instance
 *
 *  This structure maintains the state and configuration of a QSPIFLASH
 *  driver instance. It must be initialized using @ref QSPIFLASH_init
 *  before use.
 */
typedef struct {
    /*! Pointer to QSPI peripheral registers */
    QSPI_Regs *qspiRegs;
    /*! Driver initialization status. true if initialized, false otherwise */
    bool initialized;
    /*! Transfer type configuration. One of @ref QSPIFLASH_TransferType */
    QSPIFLASH_TransferType transferType;
} QSPIFLASH_Handle;

/*!
 *  @brief Callback function type for DMA transfer operations
 *
 *  This callback is invoked by QSPIFLASH_writeDMA to start a DMA transfer.
 *  The application should configure and enable DMA to transfer data from
 *  the source address to the QSPI TX FIFO.
 *
 *  @param[in]  srcAddr     Source address of data to transfer
 *  @param[in]  sizeBytes   Number of bytes to transfer
 */
typedef void (*QSPIFLASH_DMAStartCallback)(
    const void *srcAddr, uint32_t sizeBytes);

/*!
 *  @brief Configuration structure for DMA-based write operations
 *
 *  This structure contains the DMA callback and completion flag needed
 *  for QSPIFLASH_writeDMA to manage page-by-page DMA transfers.
 */
typedef struct {
    /*! Callback function to start DMA transfer */
    QSPIFLASH_DMAStartCallback startDMA;
    /*! Pointer to volatile flag that indicates DMA transfer completion.
     *  The application's DMA ISR should set this to true when transfer completes.
     *  The library will reset it to false before each DMA transfer. */
    volatile bool *dmaCompleteFlag;
} QSPIFLASH_DMAConfig;

/*!
 *  @brief Initialization parameters for QSPIFLASH driver
 *
 *  This structure contains parameters required to initialize the QSPIFLASH
 *  driver instance.
 */
typedef struct {
    /*! Pointer to QSPI peripheral registers */
    QSPI_Regs *qspiRegs;
    /*! Transfer type to use. One of @ref QSPIFLASH_TransferType */
    QSPIFLASH_TransferType transferType;
} QSPIFLASH_InitParams;

/*
 * Core API Functions
 */

/*!
 *  @brief      Initialize the QSPIFLASH driver
 *
 *  Initializes the QSPIFLASH driver handle with the provided parameters.
 *  This function must be called before any other QSPIFLASH API functions.
 *
 *  @note       This function configures the QSPI peripheral in peripheral port mode.
 *              XIP (direct) mode is not supported.
 *  @note       GPIO/IOMUX configuration must be done separately before calling
 *              this function using SysConfig or manual configuration.
 *
 *  @param[in]  handle  Pointer to the QSPIFLASH handle to be initialized
 *  @param[in]  params  Pointer to initialization parameters
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if initialization was successful
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle, params, or
 *              params->qspiRegs is NULL
 */
QSPIFLASH_Status QSPIFLASH_init(
    QSPIFLASH_Handle *handle, const QSPIFLASH_InitParams *params);

/*!
 *  @brief      Deinitialize the QSPIFLASH driver
 *
 *  Deinitializes the QSPIFLASH driver and releases associated resources.
 *  The handle should not be used after calling this function unless
 *  reinitialized with @ref QSPIFLASH_init.
 *
 *  @param[in]  handle  Pointer to the QSPIFLASH handle
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if deinitialization was successful
 *  @return     QSPIFLASH_STATUS_ERROR if handle is NULL or not initialized
 */
QSPIFLASH_Status QSPIFLASH_deinit(QSPIFLASH_Handle *handle);

/*!
 *  @brief      Send a command to the flash device
 *
 *  Sends a single command byte to the flash device without any address
 *  or data phase. Used for simple commands like write enable, reset, etc.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration. Only
 *                              instruction and transferMode fields are used.
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if command was sent successfully
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not
 *              SPI_HALF_DUPLEX or QPI_HALF_DUPLEX
 */
QSPIFLASH_Status QSPIFLASH_sendCommand(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Execute a configuration frame
 *
 *  Executes a configuration frame which sends command and optional data
 *  to the flash device. Used for write operations, erase commands, and
 *  register writes. This is a TX-only operation with no data returned.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  txData          Pointer to data buffer to transmit (can be NULL)
 *  @param[in]  txDataLen       Length of data to transmit in bytes
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if frame was executed successfully
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not
 *              SPI_HALF_DUPLEX or QPI_HALF_DUPLEX
 */
QSPIFLASH_Status QSPIFLASH_executeConfigurationFrame(QSPIFLASH_Handle *handle,
    const uint16_t *txData, uint32_t txDataLen,
    const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Execute a status frame and receive data
 *
 *  Executes a status frame which sends a command and receives data from
 *  the flash device. Used for reading status registers, JEDEC ID, etc.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[out] rxData          Pointer to buffer to store received data
 *  @param[in]  rxDataLen       Number of bytes to receive
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if frame was executed successfully
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not
 *              SPI_HALF_DUPLEX or QPI_HALF_DUPLEX
 */
QSPIFLASH_Status QSPIFLASH_executeStatusFrame(QSPIFLASH_Handle *handle,
    uint8_t *rxData, uint32_t rxDataLen,
    const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Execute a data frame for read operations
 *
 *  Executes a data frame which sends command, address, and dummy cycles,
 *  then receives data from the flash device. Used for all read operations.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  txData          Pointer to address bytes to transmit
 *  @param[in]  txDataLen       Length of address data in bytes
 *  @param[out] rxData          Pointer to buffer to store received data
 *  @param[in]  rxDataLen       Number of bytes to receive
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if frame was executed successfully
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not a valid
 *              data frame mode (SPI, DUAL_OUT, DUAL_IO, QUAD_OUT, QUAD_IO, QPI)
 */
QSPIFLASH_Status QSPIFLASH_executeDataFrame(QSPIFLASH_Handle *handle,
    const uint8_t *txData, uint32_t txDataLen, uint16_t *rxData,
    uint32_t rxDataLen, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Read JEDEC ID from flash device
 *
 *  Reads the JEDEC manufacturer and device ID from the flash device.
 *  Typically returns 3 bytes: manufacturer ID, memory type, and capacity.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[out] id              Pointer to buffer to store ID bytes
 *  @param[in]  idLen           Length of ID to read (typically 3 bytes)
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if ID was read successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle, id, or commandConfig
 *              is NULL, or idLen is less than 3
 */
QSPIFLASH_Status QSPIFLASH_readID(QSPIFLASH_Handle *handle, uint8_t *id,
    uint32_t idLen, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Read security register from flash device
 *
 *  Reads the security register value from the flash device.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[out] status          Pointer to store the security register value
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if register was read successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle, status, or
 *              commandConfig is NULL, or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_readSecurityRegister(QSPIFLASH_Handle *handle,
    uint8_t *status, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Read status register from flash device
 *
 *  Reads the status register value from the flash device. The status
 *  register contains flags like WIP (Write In Progress), WEL (Write Enable
 *  Latch), and QE (Quad Enable).
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[out] status          Pointer to store the status register value
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if register was read successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle, status, or
 *              commandConfig is NULL, or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_readStatusRegister(QSPIFLASH_Handle *handle,
    uint8_t *status, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Write status register to flash device
 *
 *  Writes a value to the status register of the flash device. This function
 *  handles write enable, register write, and wait for completion internally.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  statusValue     Value to write to the status register
 *  @param[in]  commandConfig   Pointer to command configuration (transferMode used)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if register was written successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_writeStatusRegister(QSPIFLASH_Handle *handle,
    uint8_t statusValue, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Set the QE (Quad Enable) bit in status register
 *
 *  Sets the Quad Enable bit in the flash status register to enable quad
 *  mode I/O operations. This function reads the current status, sets the
 *  QE bit if not already set, and writes back.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration (transferMode used)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if QE bit was set successfully or
 *              was already set
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_setQEBit(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Reset the QE (Quad Enable) bit in status register
 *
 *  Clears the Quad Enable bit in the flash status register to disable quad
 *  mode I/O operations. This function reads the current status, clears the
 *  QE bit if set, and writes back.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration (transferMode used)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if QE bit was cleared successfully or
 *              was already cleared
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_resetQEBit(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Wait for flash device to be ready
 *
 *  Polls the flash status register until the WIP (Write In Progress) bit
 *  is cleared, indicating the flash is ready for the next operation.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if flash is ready
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_waitForReady(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Enable write operations on flash device
 *
 *  Sends the Write Enable command to the flash device. This must be called
 *  before any program or erase operation.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if write enable was successful
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_writeEnable(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Perform software reset on flash device
 *
 *  Sends the Reset Enable (0x66) followed by Reset Memory (0x99) commands
 *  to perform a software reset of the flash device.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration (instruction
 *                              should be RESET_MEMORY_CMD)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if reset was successful
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_reset(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Enable QPI (Quad Peripheral Interface) mode
 *
 *  Sends the Enable QPI command to switch the flash device to QPI mode
 *  where all four data lines are used for command, address, and data phases.
 *
 *  @note       The QE (Quad Enable) bit must be set in the status register
 *              before calling this function. Use @ref QSPIFLASH_setQEBit first.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration (instruction
 *                              should be ENABLE_QPI_CMD)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if QPI mode was enabled successfully
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_enableQuadMode(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Disable QPI (Quad Peripheral Interface) mode
 *
 *  Sends the Disable QPI command to switch the flash device back to
 *  standard SPI mode.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration (instruction
 *                              should be DISABLE_QPI_CMD)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if QPI mode was disabled successfully
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_disableQuadMode(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Read data from flash device
 *
 *  Reads data from the flash device at the specified address using the
 *  configured transfer mode. Supports various read modes including standard
 *  SPI, dual, and quad modes.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[out] data            Pointer to buffer to store read data
 *  @param[in]  size            Number of bytes to read
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction, address, dummyCycles, addressSize,
 *                              and transferMode
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if read was successful
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle, data, or commandConfig
 *              is NULL, size is 0, or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_read(QSPIFLASH_Handle *handle, void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Write data to flash device
 *
 *  Writes data to the flash device at the specified address. This function
 *  handles page boundary crossing automatically, splitting writes across
 *  multiple page program operations as needed.
 *
 *  @note       Only SPI_HALF_DUPLEX and QPI_HALF_DUPLEX transfer modes are
 *              supported for write operations.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  data            Pointer to data buffer to write
 *  @param[in]  size            Number of bytes to write
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction, address, addressSize, and transferMode
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if write was successful
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle, data, or commandConfig
 *              is NULL, size is 0, or handle is not initialized
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not
 *              SPI_HALF_DUPLEX or QPI_HALF_DUPLEX
 */
QSPIFLASH_Status QSPIFLASH_write(QSPIFLASH_Handle *handle, const void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig);

/*
 * Erase Operations
 */

/*!
 *  @brief      Erase a sector of flash memory
 *
 *  Erases a single sector (typically 4KB) of flash memory. The address
 *  is automatically aligned to the sector boundary.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction, address, addressSize, and transferMode
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if sector erase was successful
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_eraseSector(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Erase a block of flash memory
 *
 *  Erases a block (32KB or 64KB depending on command) of flash memory.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction (BLOCK_ERASE_32KB_CMD or
 *                              BLOCK_ERASE_64KB_CMD), address, addressSize,
 *                              and transferMode
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if block erase was successful
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_eraseBlock(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Erase the entire flash chip
 *
 *  Erases all data from the entire flash chip. This operation can take
 *  a significant amount of time to complete (up to several minutes for
 *  large flash devices).
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction and transferMode
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if chip erase was successful
 *  @return     QSPIFLASH_STATUS_ERROR if handle or commandConfig is NULL,
 *              or handle is not initialized
 */
QSPIFLASH_Status QSPIFLASH_eraseChip(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Setup QSPI for read transfer (pre-DMA)
 *
 *  Sets up QSPI for a read operation and sends the address.
 *  After this function returns, DMA can transfer data from QSPI RX FIFO,
 *  then call @ref QSPIFLASH_postReadTransfer to complete the operation.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  size            Number of bytes to read (for RX count setup)
 *  @param[in]  commandConfig   Pointer to command configuration
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if setup was successful
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if parameters are invalid
 */
QSPIFLASH_Status QSPIFLASH_preReadTransfer(QSPIFLASH_Handle *handle,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Complete read transfer (post-DMA)
 *
 *  Completes the read transfer by deasserting chip select and cleanup.
 *  Call this function after DMA has finished transferring data from RX FIFO.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if completed successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle is NULL
 */
QSPIFLASH_Status QSPIFLASH_postReadTransfer(QSPIFLASH_Handle *handle);

/*!
 *  @brief      Setup QSPI for write transfer (pre-DMA)
 *
 *  Sets up QSPI for a write operation and sends the command + address.
 *  After this function returns, DMA can transfer data to QSPI TX FIFO,
 *  then call @ref QSPIFLASH_postWriteTransfer to complete the operation.
 *
 *  @note       Only SPI_HALF_DUPLEX and QPI_HALF_DUPLEX transfer modes are
 *              supported for write operations.
 *  @note       This function automatically enables write operations before
 *              sending the command.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  size            Number of bytes to write (for TX count setup)
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction, address, addressSize, and transferMode
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if setup was successful
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle or commandConfig
 *              is NULL, size is 0, or handle is not initialized
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not
 *              SPI_HALF_DUPLEX or QPI_HALF_DUPLEX
 */
QSPIFLASH_Status QSPIFLASH_preWriteTransfer(QSPIFLASH_Handle *handle,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Complete write transfer (post-DMA)
 *
 *  Completes the write transfer by deasserting chip select, waiting
 *  for QSPI idle, and waiting for flash programming to complete (WIP bit clear).
 *  Call this function after DMA has finished transferring data to TX FIFO.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  commandConfig   Pointer to command configuration (transferMode used
 *                              for status register polling)
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if transfer completed successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if handle or commandConfig is NULL
 *  @return     QSPIFLASH_STATUS_TIMEOUT if flash programming did not complete
 */
QSPIFLASH_Status QSPIFLASH_postWriteTransfer(
    QSPIFLASH_Handle *handle, const QSPIFLASH_CommandConfig *commandConfig);

/*!
 *  @brief      Write data to flash using DMA with automatic page handling
 *
 *  Writes data to the flash device using DMA transfers. This function
 *  automatically handles page boundary crossing by splitting the write
 *  into multiple page program operations (max 256 bytes each).
 *
 *  For each page:
 *  1. Sets up QSPI and sends write enable + command + address
 *  2. Calls the DMA start callback to initiate transfer
 *  3. Waits for DMA completion flag to be set
 *  4. Waits for flash programming to complete (WIP bit clear)
 *
 *  @note       Only SPI_HALF_DUPLEX and QPI_HALF_DUPLEX transfer modes are
 *              supported for write operations.
 *  @note       The DMA completion flag must be set by the application's DMA ISR.
 *  @note       Data buffer must remain valid until the function returns.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[in]  data            Pointer to data buffer to write
 *  @param[in]  size            Total number of bytes to write (can exceed 256)
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction, address, addressSize, and transferMode
 *  @param[in]  dmaConfig       Pointer to DMA configuration with callback and
 *                              completion flag
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if all pages were written successfully
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if any parameter is NULL or invalid
 *  @return     QSPIFLASH_STATUS_INVALID_MODE if transferMode is not half-duplex
 */
QSPIFLASH_Status QSPIFLASH_writeDMA(QSPIFLASH_Handle *handle, const void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig,
    const QSPIFLASH_DMAConfig *dmaConfig);

/*!
 *  @brief      Read data from flash using DMA
 *
 *  Reads data from the flash device using DMA transfer. This function
 *  provides a consistent API with QSPIFLASH_writeDMA for DMA operations.
 *
 *  Flow:
 *  1. Sets up QSPI and sends command + address
 *  2. Calls the DMA start callback to initiate transfer
 *  3. Waits for DMA completion flag to be set
 *  4. Completes the transfer (chip select handling)
 *
 *  @note       The DMA completion flag must be set by the application's DMA ISR.
 *  @note       Data buffer must remain valid until the function returns.
 *
 *  @param[in]  handle          Pointer to the QSPIFLASH handle
 *  @param[out] data            Pointer to buffer to store read data
 *  @param[in]  size            Number of bytes to read
 *  @param[in]  commandConfig   Pointer to command configuration containing
 *                              instruction, address, addressSize, dummyCycles,
 *                              and transferMode
 *  @param[in]  dmaConfig       Pointer to DMA configuration with callback and
 *                              completion flag
 *
 *  @return     QSPIFLASH_STATUS_SUCCESS if read was successful
 *  @return     QSPIFLASH_STATUS_INVALID_PARAMS if any parameter is NULL or invalid
 */
QSPIFLASH_Status QSPIFLASH_readDMA(QSPIFLASH_Handle *handle, void *data,
    uint32_t size, const QSPIFLASH_CommandConfig *commandConfig,
    const QSPIFLASH_DMAConfig *dmaConfig);

#ifdef __cplusplus
}
#endif

#endif /* QSPIFLASH_H_ */
/** @}*/
