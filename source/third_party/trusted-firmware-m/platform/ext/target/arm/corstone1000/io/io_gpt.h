/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __IO_GPT_H__
#define __IO_GPT_H__

#include "io_gpt.h"

#include "Driver_Flash.h"
#include "gpt_flash.h"

/* The pseudo flash driver used by the GPT library. Each LBA maps
 * FLASH_SECTOR_SIZE:TFM_GPT_BLOCK_SIZE to a sector of flash (so 1024:512 means
 * two LBAs per sector of flash)
 */
extern struct gpt_flash_driver_t io_gpt_flash_driver;

/* Sets up the gpt_flash_driver with the given driver and initialises the latter */
int register_flash_io_gpt(ARM_DRIVER_FLASH *driver);

#endif /* __IO_GPT_H__ */
