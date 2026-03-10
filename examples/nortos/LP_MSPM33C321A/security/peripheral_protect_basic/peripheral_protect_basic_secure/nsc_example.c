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

#include "sns_helpers.h"
#include "ti_msp_dl_config.h"

/**
 * @brief Non-Secure Callable (NSC) function - secure GPIO control service
 *
 * This NSC function provides a secure service that can be called from non-secure code.
 * It demonstrates how non-secure code can safely request secure GPIO operations
 * through a controlled interface without direct access to the secure peripheral.
 *
 * NS_FUNC_ENTRY marks this as an NSC function, creating a secure gateway
 * that allows non-secure code to request secure GPIO operations safely.
 *
 * @param state 1 to turn on GREEN LED, 0 to turn off
 */
NS_FUNC_ENTRY void secure_gpio_control(uint32_t state)
{
    if (state) {
        /* Turn on GREEN LED (secure GPIO) */
        DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    } else {
        /* Turn off GREEN LED (secure GPIO) */
        DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    }
}

/**
 * @brief Non-Secure Callable (NSC) function - get peripheral security status
 *
 * This NSC function allows non-secure code to query the security configuration
 * of peripherals. This is useful for diagnostics and verification.
 *
 * @return Bit field indicating which peripherals are configured as secure
 *         Bit 0: GPIOA (1=secure, 0=non-secure)
 *         Bit 1: GPIOB (1=secure, 0=non-secure)
 *         Bit 2: Timer (1=secure, 0=non-secure)
 */
NS_FUNC_ENTRY uint32_t get_peripheral_security_status(void)
{
    uint32_t status = 0;

    /* Check GPIOA security status (bit 0) */
    status |= (1 << 0); /* GPIOA is secure */

    /* Check GPIOB security status (bit 1) */
    status |= (0 << 1); /* GPIOB is non-secure */

    /* Check Timer security status (bit 2) */
    status |= (1 << 2); /* TIMG4_0 is secure */

    return status;
}
