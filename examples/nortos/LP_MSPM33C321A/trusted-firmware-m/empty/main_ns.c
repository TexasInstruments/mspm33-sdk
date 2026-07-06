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

/**
 * @file main_ns.c
 * @brief Minimal TF-M Non-Secure Application
 *
 * This example demonstrates the simplest TF-M Non-Secure application:
 * - Calls psa_framework_version() to verify TF-M is running and accessible
 * - Enters an infinite loop
 *
 * When flashed alongside tfm_s.bin, this proves:
 * 1. TF-M secure firmware boots successfully
 * 2. Non-Secure world can communicate with Secure via PSA API
 * 3. UART output confirms both S and NS are operational
 *
 * To extend this example:
 * - Add PSA Crypto API calls (psa_crypto_init, psa_cipher_*, etc.)
 * - Add PSA Storage API calls (psa_its_set, psa_ps_set, etc.)
 * - Add PSA Attestation API calls (psa_initial_attest_get_token, etc.)
 */

#include <stdio.h>
#include "psa/client.h"
#include "uart_stdout.h"

int main(void)
{
    uint32_t version;

    /* Initialize the NS-side UART driver so printf() reaches the serial port.
     * The UART hardware is already configured by the Secure image — this call
     * only initializes the CMSIS driver state (sets is_initialized = true). */
    stdio_init();

    /* Query the PSA Framework version to verify TF-M is operational */
    version = psa_framework_version();

    printf("\n--- TF-M Non-Secure Application ---\n");
    printf("PSA Framework version: 0x%08lx (v%ld.%ld)\n", version,
        (version >> 8) & 0xFF, /* major */
        version & 0xFF);       /* minor */

    /* Expected: 0x0101 (v1.1) per PSA Firmware Framework API 1.1 */
    if (version == 0x0101) {
        printf("TF-M is running successfully!\n");
    } else {
        printf("Warning: Unexpected PSA Framework version\n");
    }

    printf("\nEntering infinite loop (application running)...\n");

    /* Application main loop */
    while (1) {
        /* Add your application code here */
        __asm("wfi"); /* Wait for interrupt to save power */
    }

    return 0;
}
