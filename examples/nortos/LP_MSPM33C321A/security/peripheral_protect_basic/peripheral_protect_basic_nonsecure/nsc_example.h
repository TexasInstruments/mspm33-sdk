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

#ifndef __NSC_EXAMPLE_H__
#define __NSC_EXAMPLE_H__

#include <stdint.h>

/**
 * @brief Non-Secure Callable function to control secure GPIO
 *
 * This function provides a secure service that allows non-secure code to
 * control the secure GPIO (GREEN LED) without direct access to the peripheral.
 *
 * @param state 1 to turn on GREEN LED, 0 to turn off
 */
extern void secure_gpio_control(uint32_t state);

/**
 * @brief Non-Secure Callable function to query peripheral security status
 *
 * This function allows non-secure code to query which peripherals are
 * configured as secure vs non-secure.
 *
 * @return Bit field indicating peripheral security configuration
 *         Bit 0: GPIOA (1=secure, 0=non-secure)
 *         Bit 1: GPIOB (1=secure, 0=non-secure)
 *         Bit 2: Timer (1=secure, 0=non-secure)
 */
extern uint32_t get_peripheral_security_status(void);

#endif /* __NSC_EXAMPLE_H__ */
