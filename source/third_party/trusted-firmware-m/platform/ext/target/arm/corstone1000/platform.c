/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "Driver_Flash.h"
#include "flash_layout.h"

#include "io_gpt.h"

#include "platform.h"

#define PLAT_LOG_MODULE_NAME    "platform"
#include "platform_log.h"

extern ARM_DRIVER_FLASH FLASH_DEV_NAME;


/* Initialize io storage of the platform */
int32_t plat_io_storage_init(void)
{

    /* The GPT library requires a flash driver in order to do I/O */
    int rc = register_flash_io_gpt(&FLASH_DEV_NAME);
    if (rc != 0) {
        ERROR("Failed to register io flash rc: %d", rc);
    }

    return rc;
}
