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

#include "config.h"
#include "sns_helpers.h"
#include "ti_msp_dl_config.h"

/* External reference to secure data buffer defined in memory_protect.c */
extern volatile uint32_t secure_data_buffer[];

#define LED_DELAY_CYCLES 30000000

/*
 * Non-Secure Callable (NSC) function - Read secure data
 * Allows non-secure code to safely read secure memory through this function
 */
NS_FUNC_ENTRY uint32_t read_secure_data(uint32_t index)
{
    /* Validate index to prevent buffer overflow */
    if (index >= 16) {
        return 0xFFFFFFFF; /* Error */
    }

    /* Turn on secure LED to indicate secure operation */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(LED_DELAY_CYCLES);

    /* Read from secure memory - only secure code can do this
    *  Implementations can add more restrictions here
    */
    uint32_t value = secure_data_buffer[index];

    /* Turn off secure LED */
    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);

    return value;
}

/*
 * Non-Secure Callable (NSC) function - Write secure data
 * Allows non-secure code to safely write to secure memory through this function
 */
NS_FUNC_ENTRY void write_secure_data(uint32_t index, uint32_t value)
{
    /* Validate index to prevent buffer overflow */
    if (index >= 16) {
        return;
    }

    /* Turn on secure LED to indicate secure operation */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(LED_DELAY_CYCLES);

    /* Write to secure memory - only secure code can do this
    *  Implementations can add more restrictions here
    */
    secure_data_buffer[index] = value;

    /* Turn off secure LED */
    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
}

/*
 * Non-Secure Callable (NSC) function - Get secure buffer address
 * Returns the address of secure buffer
 */
NS_FUNC_ENTRY uint32_t get_secure_buffer_address(void)
{
    return (uint32_t) secure_data_buffer;
}

/*
 * Non-Secure Callable (NSC) function - Get NSC region address
 * Returns the address of NSC region
 */
NS_FUNC_ENTRY uint32_t get_nsc_region_address(void)
{
    /* Return the address of this function (which is in NSC region) */
    return (uint32_t) get_nsc_region_address;
}
