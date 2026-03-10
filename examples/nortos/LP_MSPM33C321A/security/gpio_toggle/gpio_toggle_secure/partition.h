/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m33/dl_core.h>
#include "config.h"

#ifndef __CONFIG_TZ_TEST_APP_PARTITION__
#define __CONFIG_TZ_TEST_APP_PARTITION__

/* SAU Control Register Configuration */
#define SAU_INIT_CTRL 1

/**
 * @brief Enable SAU
 * @note Value for SAU->CTRL register bit ENABLE
 */
#define SAU_INIT_CTRL_ENABLE 1

/**
 * @brief When SAU is disabled
 * @param 0 All Memory is Secure
 * @param 1 All Memory is Non-Secure
 * @note Value for SAU->CTRL register bit ALLNS
 * @note When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
 */
#define SAU_INIT_CTRL_ALLNS 0

#define SAU_REGIONS_MAX 4 /**< Maximum number of SAU regions */

/* SAU Region 0: Non-Secure Application Flash */
#define SAU_INIT_REGION0 1
#define SAU_INIT_START0 \
    NON_SECURE_APP0_START /**< Start address of SAU region 0 */
#define SAU_INIT_END0                        \
    (SAU_INIT_START0 + NON_SECURE_APP0_LEN - \
        1)              /**< End address of SAU region 0 */
#define SAU_INIT_NSC0 0 /**< Region 0 is Non-Secure */

/* SAU Region 1: Non-Secure SRAM */
#define SAU_INIT_REGION1 1
#define SAU_INIT_START1 \
    NON_SECURE_APP0_SRAM_START /**< Start address of non-secure SRAM */
#define SAU_INIT_END1                             \
    (SAU_INIT_START1 + NON_SECURE_APP0_SRAM_LEN - \
        1)              /**< End address of non-secure SRAM */
#define SAU_INIT_NSC1 0 /**< Region 1 is Non-Secure */

/* SAU Region 2: Non-Secure Peripheral Access
 * Note: ALL peripherals are being marked as non-secure
 * This allows the non-secure application to access all peripheral registers */
#define SAU_INIT_REGION2 1
#define SAU_INIT_START2 0x40000000 /**< Start address of peripheral region */
#define SAU_INIT_END2 \
    (SAU_INIT_START2 + 0x10000000 - 1) /**< End address of peripheral region */
#define SAU_INIT_NSC2 0                /**< Region 2 is Non-Secure */

/* SAU Region 3: Non-Secure Callable Region */
#define SAU_INIT_REGION3 1
#define SAU_INIT_START3 0x1000F800 /**< Start address of NSC region */
#define SAU_INIT_END3 0x1000FFFF   /**< End address of NSC region */
#define SAU_INIT_NSC3 1            /**< Region 3 is Non-Secure Callable */

/**
 * @brief Macro to initialize a specific SAU region
 * @param n Region number (0-3)
 */
#define SAU_INIT_REGION(n)                                                   \
    SAU->RNR  = (n & SAU_RNR_REGION_Msk);                                    \
    SAU->RBAR = (SAU_INIT_START##n & SAU_RBAR_BADDR_Msk);                    \
    SAU->RLAR = (SAU_INIT_END##n & SAU_RLAR_LADDR_Msk) |                     \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos) & SAU_RLAR_NSC_Msk) | \
                1

/**
 * @brief Initialize Security Attribution Unit (SAU) configuration
 * @note This function must be called from secure state before transitioning to non-secure
 * @return void
 */
static inline void TZ_SAU_Setup(void)
{
#if defined(__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)

    /* Initialize SAU Region 0: Non-Secure Application Flash */
#if defined(SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
    SAU_INIT_REGION(0);
#endif

    /* Initialize SAU Region 1: Non-Secure SRAM */
#if defined(SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
    SAU_INIT_REGION(1);
#endif

    /* Initialize SAU Region 2: Non-Secure Peripherals */
#if defined(SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
    SAU_INIT_REGION(2);
#endif

    /* Initialize SAU Region 3: Non-Secure Callable Region */
#if defined(SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
    SAU_INIT_REGION(3);
#endif

    /* Enable SAU with configured settings */
    SAU->CTRL =
        ((SAU_INIT_CTRL_ALLNS << SAU_CTRL_ALLNS_Pos) & SAU_CTRL_ALLNS_Msk) |
        ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk);

#endif /* __SAUREGION_PRESENT */
}

#endif /* __CONFIG_TZ_TEST_APP_PARTITION__ */
