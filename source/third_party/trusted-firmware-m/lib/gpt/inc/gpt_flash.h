/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_GPT_FLASH_H__
#define __TFM_GPT_FLASH_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Error codes returned by GPT flash driver operations.
 */
typedef enum {
    GPT_FLASH_SUCCESS = 0,        /**< Operation completed successfully. */
    GPT_FLASH_GENERIC_ERROR = -1, /**< Unspecified error. */
    GPT_FLASH_NOT_INIT = -2,      /**< Flash driver has not been initialised. */
    GPT_FLASH_UNAVAILABLE = -3,   /**< Flash driver is unavailable. */
    GPT_FLASH_BAD_PARAM = -4,     /**< Parameter supplied to the driver is invalid. */
} gpt_flash_err_t;

/**
 * \brief Function used to initialise the driver.
 *
 * \retval GPT_FLASH_SUCCESS Success.
 * \retval GPT_FLASH_GENERIC_ERROR Driver-specific failure.
 */
typedef gpt_flash_err_t (*gpt_flash_init_t)(void);

/**
 * \brief Function used to uninitialise the driver.
 *
 * \retval GPT_FLASH_SUCCESS Success.
 * \retval GPT_FLASH_GENERIC_ERROR Driver-specific failure.
 */
typedef gpt_flash_err_t (*gpt_flash_uninit_t)(void);

/**
 * \brief Function that reads a logical block address.
 *
 * \param[in]  lba Logical block address to read.
 * \param[out] buf Buffer to populate. Must be at least the size of an LBA.
 *
 * \return Number of bytes read on success or a negative error code on failure.
 * \retval GPT_FLASH_NOT_INIT The flash driver has not been initialised.
 * \retval GPT_FLASH_UNAVAILABLE The flash driver is unavailable.
 * \retval GPT_FLASH_BAD_PARAM \p lba is not a valid address (for example larger than the flash size).
 * \retval GPT_FLASH_GENERIC_ERROR Unspecified error.
 */
__attribute__((nonnull(2)))
typedef ssize_t (*gpt_flash_read_t)(uint64_t lba,
                                    void    *buf);

/**
 * \brief Function that writes to a logical block address.
 *
 * \param[in] lba Logical block address to write to.
 * \param[in] buf Buffer to write from. Must be at least the size of an LBA.
 *
 * \return Number of bytes written on success or a negative error code on failure.
 * \retval GPT_FLASH_NOT_INIT The flash driver has not been initialised.
 * \retval GPT_FLASH_UNAVAILABLE The flash driver is unavailable.
 * \retval GPT_FLASH_BAD_PARAM \p lba is not a valid address (for example larger than the flash size).
 * \retval GPT_FLASH_GENERIC_ERROR Unspecified error.
 */
__attribute__((nonnull(2)))
typedef ssize_t (*gpt_flash_write_t)(uint64_t    lba,
                                     const void *buf);

/**
 * \brief Function that erases consecutive logical blocks.
 *
 * \param[in] lba Starting logical block address.
 * \param[in] num_blocks Number of blocks to erase.
 *
 * \return Number of blocks erased on success or a negative error code on failure.
 * \retval GPT_FLASH_NOT_INIT The flash driver has not been initialised.
 * \retval GPT_FLASH_UNAVAILABLE The flash driver is unavailable.
 * \retval GPT_FLASH_BAD_PARAM \p num_blocks is zero.
 * \retval GPT_FLASH_BAD_PARAM One of the requested blocks is invalid (for example,
 *                            \p lba + \p num_blocks exceeds the flash size).
 * \retval GPT_FLASH_BAD_PARAM \p lba is not a valid address (for example larger than the flash size).
 * \retval GPT_FLASH_GENERIC_ERROR Unspecified error.
 */
typedef ssize_t (*gpt_flash_erase_t)(uint64_t lba, size_t num_blocks);

/**
 * \brief Interface for interacting with the flash driver.
 */
struct gpt_flash_driver_t {
    gpt_flash_init_t init;       /**< Flash initialisation routine. */
    gpt_flash_uninit_t uninit;   /**< Flash deinitialisation routine. */
    gpt_flash_read_t read;       /**< Routine used to read a logical block. */
    gpt_flash_write_t write;     /**< Routine used to write a logical block. */
    gpt_flash_erase_t erase;     /**< Routine used to erase logical blocks. */
};

#ifdef __cplusplus
}
#endif

#endif /* __TFM_GPT_FLASH_H__ */
