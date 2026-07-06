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
 *  @file       NOR_FLASH_COMMANDS.h
 *  @brief      NOR Flash Command Definitions
 *  @defgroup   NOR_FLASH_COMMANDS NOR Flash Commands and Configuration
 *
 *  @anchor ti_dl_NOR_FLASH_COMMANDS_Overview
 *  # Overview
 *
 *  This file contains command definitions, timing parameters, and register
 *  bit masks for external NOR flash devices. Currently supports:
 *      - Macronix MX25L12833F (128Mbit)
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup NOR_FLASH_COMMANDS
 * @{
 */

#ifndef NOR_FLASH_COMMANDS_H_
#define NOR_FLASH_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  MX25L12833F Flash Memory Configuration
 ******************************************************************************/

/** @defgroup NOR_FLASH_SIZE Flash Size Configuration
 *  @{
 */
/*! Total flash size: 128 MBits = 16 MBytes */
#define FLASH_SIZE                           0x1000000U

/*! Sector size: 4 KBytes (smallest erasable unit) */
#define SECTOR_SIZE                          0x1000U

/*! Page size: 256 Bytes (maximum bytes per program operation) */
#define PAGE_SIZE                            0x100U
/** @} */

/** @defgroup NOR_FLASH_DUMMY_CYCLES Dummy Cycle Configuration
 *  @brief Number of dummy clock cycles required for various read operations
 *  @{
 */
/*! Dummy cycles for Fast Read command (0x0B) */
#define DUMMY_CYCLES_FAST_READ               8U

/*! Dummy cycles for Dual I/O Read command (0xBB) */
#define DUMMY_CYCLES_DUAL_IO_READ            4U

/*! Dummy cycles for Dual Output Read command (0x3B) */
#define DUMMY_CYCLES_DUAL_OUTPUT_READ        8U

/*! Dummy cycles for Quad I/O Read command (0xEB) */
#define DUMMY_CYCLES_QUAD_IO_READ            6U

/*! Dummy cycles for Quad Output Read command (0x6B) */
#define DUMMY_CYCLES_QUAD_OUTPUT_READ        8U
/** @} */

/** @defgroup NOR_FLASH_TIMEOUTS Timeout Configuration
 *  @brief Default timeout values for flash operations in milliseconds
 *  @{
 */
/*! Timeout for page program operation */
#define TIMEOUT_PAGE_PROGRAM_MS              1000U

/*! Timeout for sector erase operation (4KB) */
#define TIMEOUT_SECTOR_ERASE_MS              3000U

/*! Timeout for block erase operation (32KB/64KB) */
#define TIMEOUT_BLOCK_ERASE_MS               5000U

/*! Timeout for chip erase operation (entire flash) */
#define TIMEOUT_CHIP_ERASE_MS                60000U

/*! Default timeout for general operations */
#define TIMEOUT_DEFAULT_MS                   1000U
/** @} */

/*******************************************************************************
 *  MX25L12833F Command Definitions
 ******************************************************************************/

/** @defgroup NOR_FLASH_ID_CMDS Identification Commands
 *  @{
 */
/*! Read JEDEC ID - Returns manufacturer ID, memory type, capacity (3 bytes) */
#define READ_ID_CMD                          0x9FU

/*! Read Electronic ID - Returns device ID */
#define READ_ELECTRONIC_ID_CMD               0xABU

/*! QPI Read ID - Read ID in QPI mode */
#define QPI_READ_ID_CMD                      0xAFU

/*! Read SFDP Table - Serial Flash Discoverable Parameters */
#define READ_SFDP_TABLE_CMD                  0x5AU
/** @} */

/** @defgroup NOR_FLASH_RESET_CMDS Reset Commands
 *  @{
 */
/*! Reset Enable - Must be sent before Reset Memory command */
#define RESET_ENABLE_CMD                     0x66U

/*! Reset Memory - Performs software reset of the flash device */
#define RESET_MEMORY_CMD                     0x99U
/** @} */

/** @defgroup NOR_FLASH_WRITE_EN_CMDS Write Enable/Disable Commands
 *  @{
 */
/*! Write Enable - Sets WEL bit, required before program/erase operations */
#define WRITE_ENABLE_CMD                     0x06U

/*! Write Disable - Clears WEL bit */
#define WRITE_DISABLE_CMD                    0x04U
/** @} */

/** @defgroup NOR_FLASH_WRITE_CMDS Write/Program Commands
 *  @{
 */
/*! Page Program - Programs up to 256 bytes in SPI/QPI mode */
#define PAGE_PROGRAM_CMD                     0x02U

/*! Quad Page Program - Programs up to 256 bytes using quad data lines */
#define QUAD_PAGE_PROGRAM_CMD                0x38U
/** @} */

/** @defgroup NOR_FLASH_ERASE_CMDS Erase Commands
 *  @{
 */
/*! Sector Erase - Erases 4KB sector */
#define SECTOR_ERASE_CMD                     0x20U

/*! Block Erase 32KB - Erases 32KB block */
#define BLOCK_ERASE_32KB_CMD                 0x52U

/*! Block Erase 64KB - Erases 64KB block */
#define BLOCK_ERASE_64KB_CMD                 0xD8U

/*! Chip Erase - Erases entire flash memory */
#define CHIP_ERASE_CMD                       0xC7U
/** @} */

/** @defgroup NOR_FLASH_READ_CMDS Read Commands
 *  @{
 */
/*! Normal Read - Standard SPI read, no dummy cycles, up to 50MHz */
#define NORMAL_READ_CMD                      0x03U

/*! Fast Read - SPI read with dummy cycles, higher frequency support */
#define FAST_READ_CMD                        0x0BU

/*! Dual Output Read - Command/address on single line, data on 2 lines (1-1-2) */
#define DUAL_OUTPUT_READ_CMD                 0x3BU

/*! Dual I/O Read - Command on single line, address/data on 2 lines (1-2-2) */
#define DUAL_IO_READ_CMD                     0xBBU

/*! Quad Output Read - Command/address on single line, data on 4 lines (1-1-4) */
#define QUAD_OUTPUT_READ_CMD                 0x6BU

/*! Quad I/O Read - Command on single line, address/data on 4 lines (1-4-4) */
#define QUAD_IO_READ_CMD                     0xEBU
/** @} */

/** @defgroup NOR_FLASH_REG_CMDS Register Commands
 *  @{
 */
/*! Read Status Register - Returns status register value */
#define READ_STATUS_REG_CMD                  0x05U

/*! Read Configuration Register - Returns configuration register value */
#define READ_CFG_REG_CMD                     0x15U

/*! Write Status/Configuration Register - Writes status and config registers */
#define WRITE_STATUS_CFG_REG_CMD             0x01U

/*! Read Security Register - Returns security register value */
#define READ_SECURITY_REG_CMD                0x2BU

/*! Write Security Register - Writes security register */
#define WRITE_SECURITY_CFG_REG_CMD           0x2FU
/** @} */

/** @defgroup NOR_FLASH_QPI_CMDS QPI Mode Commands
 *  @{
 */
/*! Enable QPI Mode - Switches flash to QPI (4-4-4) mode */
#define ENABLE_QPI_CMD                       0x35U

/*! Disable QPI Mode - Switches flash back to SPI mode */
#define DISABLE_QPI_CMD                      0xF5U
/** @} */

/** @defgroup NOR_FLASH_STATUS_BITS Status Register Bit Definitions
 *  @{
 */
/*! Write In Progress bit - Set when program/erase operation is in progress */
#define STATUS_WIP                           0x01U

/*! Write Enable Latch bit - Set when write operations are enabled */
#define STATUS_WEL                           0x02U

/*! Quad Enable bit mask - Enables quad I/O operations when set */
#define STATUS_QE_BIT_MASK                   0x40U

/*! @deprecated Use STATUS_QE_BIT_MASK instead */
#define statusQEBitMask                      STATUS_QE_BIT_MASK
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NOR_FLASH_COMMANDS_H_ */
/** @}*/
