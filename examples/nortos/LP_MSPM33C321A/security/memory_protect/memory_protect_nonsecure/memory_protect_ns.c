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

#include "nsc_example.h"
#include "ti_msp_dl_config.h"

#define TRANSITION_DELAY_CYCLES 100000000
#define TEST_DELAY_CYCLES 50000000

volatile bool buttonPressed = false;

/* Non-secure data buffer in non-secure SRAM */
volatile uint32_t ns_local_buffer[16] = {0x00000000, 0x11111111, 0x22222222,
    0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888,
    0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE,
    0xFFFFFFFF};

/* Test 1: Non-secure code accessing non-secure memory (should succeed) */
void test_ns_to_ns_access(void)
{
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Read from non-secure buffer */
    volatile uint32_t value = ns_local_buffer[0];

    /* Write to non-secure buffer */
    ns_local_buffer[0] = 0XF00DF00D;

    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

/* Test 2: Non-secure code calling secure functions via NSC (should succeed) */
void test_ns_via_nsc(void)
{
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Call NSC function to read secure data */
    uint32_t secure_value = read_secure_data(0);

    /* Call NSC function to write secure data */
    write_secure_data(1, 0xCAFEF00D);

    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

/* Test 3: Non-secure code attempting to access secure memory (should fault) */
void test_ns_to_secure_violation(void)
{
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Get secure buffer address from NSC function */
    uint32_t secure_addr = get_secure_buffer_address();

    /* Attempt to read/write from secure memory - this triggers a BusFault by
    *  GSC(ref GSC.SPC_ATTRIBVIOLS_CONFIG), which escalates to a
    *  HardFault and is handled by the HardFault handler */
    volatile uint32_t *secure_ptr = (volatile uint32_t *) secure_addr;
    volatile uint32_t value       = *secure_ptr; /* This will fault */

    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

/* Test 4: Non-secure code attempting to execute secure code directly (should fault) */
void test_ns_direct_call_violation(void)
{
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Get secure buffer address (we'll use this as a proxy for secure code address) */
    uint32_t secure_addr = get_secure_buffer_address();

    /* Attempt to read/write from secure memory - this triggers a BusFault by
    *  GSC(ref GSC.SPC_ATTRIBVIOLS_CONFIG), which escalates to a
    *  HardFault and is handled by the HardFault handler */
    typedef void (*func_ptr_t)(void);
    func_ptr_t secure_func =
        (func_ptr_t)(secure_addr | 0x1); /* Thumb bit set */
    secure_func();                       /* This will fault */

    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

/* Test 5: Non-secure code attempting to modify NSC region (should fault) */
void test_ns_nsc_write_violation(void)
{
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Get NSC region address */
    uint32_t nsc_addr = get_nsc_region_address();

    /* Attempt to read/write to secure memory - this triggers a BusFault by
    *  GSC(ref GSC.SPC_ATTRIBVIOLS_CONFIG), which escalates to a
    *  HardFault and is handled by the HardFault handler */
    volatile uint32_t *nsc_ptr = (volatile uint32_t *) nsc_addr;
    *nsc_ptr                   = 0x00FACADE; /* This will fault */

    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

int main(void)
{
    volatile uint32_t ii = 1;

    /* Initialize non-secure world peripherals and GPIO */
    SYSCFG_DL_init();
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);

    /* Wait for button press in non-secure world */
    while (!buttonPressed) {
        __WFI();
    }
    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Clear non-secure LED before running tests */
    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Run non-secure world tests */

    test_ns_to_ns_access();

    test_ns_via_nsc();

    test_ns_to_secure_violation();

    /* The below tests will also trigger a Busfault(escalated to HardFault)
    *  To test them, simply uncomment the function call(s)

    test_ns_direct_call_violation();
    test_ns_nsc_write_violation();
    */

    while (ii) {
    }
}

void GPIOC_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(BUTTON_GRP_PORT)) {
        case DL_GPIO_IIDX_DIO17:
            /* If SW is high, set flag to proceed with tests */
            if (!DL_GPIO_readPins(
                    BUTTON_GRP_PORT, BUTTON_GRP_USER_SWITCH_PIN)) {
                DL_GPIO_clearInterruptStatus(
                    BUTTON_GRP_PORT, BUTTON_GRP_USER_SWITCH_PIN);
                buttonPressed = true;
            }
            /* Otherwise, turn all LEDs ON to indicate failure state */
            else {
                DL_GPIO_setPins(
                    LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
                DL_GPIO_setPins(
                    LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
                DL_GPIO_setPins(
                    LED_GRP_TRANSITION_LED_PORT, LED_GRP_TRANSITION_LED_PIN);
            }
            break;

        default:
            break;
    }
}
