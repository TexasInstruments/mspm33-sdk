/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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
 *  @file       Uart_Hal_Cfg.h
 *  @brief      UART HAL Configuration Header
 *  @defgroup   UART_HAL_CFG UART HAL Configuration
 *
 *  @anchor ti_hal_mspm33_uart_hal_cfg_Overview
 *  # Overview
 *
 *  This file contains compile-time configuration for the UART HAL module.
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup UART_HAL_CFG
 * @{
 */

#ifndef UART_HAL_CFG_H
#define UART_HAL_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/hal/compiler/m33_ti_arm_clang/Std_Types.h>

/* ========================================================================== */
/*                 Configuration Definitions and Macros                       */
/* ========================================================================== */

/**
 * \brief Enable/disable parameter error checking
 *
 * Set to STD_ON to enable runtime parameter validation.
 * Set to STD_OFF to disable checks for better performance.
 */
#define UART_HAL_CFG_ERROR_CHECK STD_ON

/**
 * \brief Maximum number of UART instances supported
 *
 * This defines the maximum number of UART peripherals that can be
 * used simultaneously. Adjust based on device capabilities.
 */
#define UART_HAL_CFG_MAX_INSTANCES (4U)

/**
 * \brief Default UART clock frequency in Hz
 *
 * This is the default peripheral clock frequency for UART.
 * Can be overridden at runtime via InitParams.
 */
#define UART_HAL_CFG_DEFAULT_CLOCK_HZ (32000000U)

/**
 * \brief Enable/disable DMA support
 *
 * Set to STD_ON to include DMA transfer support.
 * Set to STD_OFF to exclude DMA code (smaller footprint).
 */
#define UART_HAL_CFG_DMA_SUPPORT STD_ON

/**
 * \brief Enable/disable polling mode support
 *
 * Set to STD_ON to include polling mode support.
 * Set to STD_OFF to exclude polling code.
 */
#define UART_HAL_CFG_POLLING_SUPPORT STD_ON

/**
 * \brief Enable/disable interrupt mode support
 *
 * Set to STD_ON to include interrupt mode support.
 * Set to STD_OFF to exclude interrupt code.
 */
#define UART_HAL_CFG_INTERRUPT_SUPPORT STD_ON

#ifdef __cplusplus
}
#endif

#endif /* UART_HAL_CFG_H */
/** @}*/
