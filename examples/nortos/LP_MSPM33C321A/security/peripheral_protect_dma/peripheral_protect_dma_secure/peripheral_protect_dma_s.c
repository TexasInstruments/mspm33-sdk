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
#include "partition.h"
#include "sns_helpers.h"
#include "ti_msp_dl_config.h"

const uint32_t gFillPattern = FILL_PATTERN;
uint32_t gDstData[DMA_FILL_SIZE_WORDS];
volatile bool gChannel0InterruptTaken = false;
volatile bool gVerifyResult           = false;

/* Forward declarations */
void configure_sau(void);
void soc_configure(void);
void perform_secure_dma_fill(void);

/**
 * @brief Simple delay function using CPU cycles
 * @param cycles Number of cycles to delay
 */
void delay_cycles(uint32_t cycles)
{
    while (cycles--) {
        __NOP();
    }
}

/**
 * @brief Configure Security Attribution Unit (SAU) for memory region security
 */
void configure_sau(void)
{
    /* Disable SAU during configuration */
    SAU->CTRL = 0;

    /* Setup SAU regions for secure/non-secure memory partitioning */
    TZ_SAU_Setup();

    /* Enable SAU */
    SAU->CTRL = SAU_CTRL_ENABLE_Msk;
}

/**
 * @brief Perform secure DMA0 fill operation
 *
 * This function demonstrates secure DMA0 operation:
 * - DMA0 is secure by default on MSPM33C321A
 * - Fills secure SRAM buffer with a pattern
 * - Uses software trigger for simplicity
 * - Verifies data integrity after transfer
 */
void perform_secure_dma_fill(void)
{
    /* Configure DMA source, destination and size */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, gFillPattern);
    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gDstData[0]);
    DL_DMA_setTransferSize(
        DMA0, DMA0_CH0_CHAN_ID, sizeof(gDstData) / sizeof(uint32_t));
    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    /* Start fill data */
    gChannel0InterruptTaken = false;
    DL_DMA_startTransfer(DMA0, DMA0_CH0_CHAN_ID);

    /* Wait for transfer */
    while (gChannel0InterruptTaken == false) {
        __WFE();
    }

    gVerifyResult = true;
    for (int i = 0; i < DMA_FILL_SIZE_WORDS; i++) {
        gVerifyResult &= (gDstData[i] == gFillPattern);
    }

#ifdef __SLATE_INTERNAL__
    gTestComplete = true;
#endif
    if (gVerifyResult == true) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
}

/**
 * @brief Configure SoC security partitioning for secure/non-secure memory regions
 */
void soc_configure(void)
{
    /* Set non-secure vector table and stack pointer */
    DL_GSC_setVTOR_NS(GSC, NON_SECURE_APP0_START);
    __TZ_set_MSP_NS(NON_SECURE_APP0_SRAM_START + NON_SECURE_APP0_SRAM_LEN);

    /* Allow access to SYSCTL registers from Non-Secure */
    DL_GSC_allowSYSCTLNonSecureAccess(GSC);

    /* Allow non-secure access to GPIO for LED control */
    DL_GSC_allowGPIONonSecureAccess(GSC, DL_GSC_GPIO_SECURE_ACCESS_GPIOA);

    /* Configure DMA1 for non-secure access with privilege */
    DL_GSC_allowDMA1NonSecureAccess(GSC);
    DL_GSC_allowDMA1PrivilegeAccess(GSC);

    /* Configure DMA1 interrupt for non-secure world
     * In TrustZone, secure NVIC controls interrupt routing to non-secure world */
    NVIC_SetTargetState(DMA1_INT_IRQn); /* Set DMA1 interrupt as non-secure */
    NVIC_EnableIRQ(DMA1_INT_IRQn); /* Enable DMA1 interrupt in secure NVIC */

    /* Configure Flash Security - all flash accessible from both secure/non-secure
     * This is for demonstration purposes. In production, you would restrict this. */
    DL_GSC_configureMainFlashSecurity(
        GSC, DL_GSC_MAIN_FLASH_BANK_A, 0xFFFFFFFF, 0x00000000);

    /* Configure SAU regions */
    configure_sau();

    /* Memory barriers to ensure configuration is complete */
    __ISB();
    __DSB();
}

int main(void)
{
    /* Initialize peripherals via SysConfig (includes SRAM security config) */
    SYSCFG_DL_init();

    /* Enable DMA0 interrupt in NVIC */
    NVIC_EnableIRQ(DMA0_INT_IRQn);
    /* Enable DMA1 interrupt in NVIC */
    // NVIC_EnableIRQ(DMA1_INT_IRQn);
    DL_SYSCTL_disableSleepOnExit();

    /* Perform secure DMA0 fill operation */
    perform_secure_dma_fill();

    /* Delay before transitioning to non-secure world */
    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Configure system for non-secure world */
    soc_configure();

    /* Get non-secure reset handler from non-secure vector table */
    const uint32_t* p_ns_vector_table = (uint32_t*) NON_SECURE_APP0_START;
    const uint32_t* p_ns_reset_address =
        (const uint32_t*) ((uint32_t) NON_SECURE_APP0_START +
                           sizeof(uint32_t));
    nonsecure_func_t p_ns_reset = (nonsecure_func_t)(*p_ns_reset_address);

    /* Branch to non-secure application */
    p_ns_reset();

    /* Should never reach here */
    while (1) {
        __WFI();
    }
}

/**
 * @brief DMA0 Interrupt Handler
 *
 * Handles DMA0 transfer completion interrupt for secure DMA operations
 */
void DMA0_IRQHandler(void)
{
    switch (DL_DMA_getPendingInterrupt(DMA0)) {
        case DL_DMA_EVENT_IIDX_DMACH0:
            gChannel0InterruptTaken = true;
            break;
        default:
            break;
    }
}

/**
 * @brief NMI Handler - catches non-maskable interrupts and security violations
 *
 * This handler is invoked when a security violation occurs, such as:
 * - Non-secure code attempting to access secure memory
 * - Invalid DMA access across security boundaries
 * - Other security-related faults
 *
 * Visual feedback: ALL LEDs turn ON to indicate fault condition
 */
void NMI_Handler(void)
{
    /* Turn on all LEDs to indicate fault condition */
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);

    /* Halt system */
    while (1) {
        __WFI();
    }
}
