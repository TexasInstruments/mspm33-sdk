/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

#include <stdint.h>

#include <ti/driverlib/m33/dl_interrupt.h>

#define NUM_RES_M33_EXC ((uint32_t) 16)   // 16 reserved M33 core exceptions
#define NUM_DEV_PERIPH_EXC ((uint32_t) 64) //currently 53 device exception + 11 reserved for future
#define NUM_INTERRUPTS (NUM_DEV_PERIPH_EXC + NUM_RES_M33_EXC)

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 256
static __no_init void (*InterruptRAMVectors[NUM_INTERRUPTS])(void) @ "VTABLE";
#else
static __attribute__((section(".vtable"))) void (
    *InterruptRAMVectors[NUM_INTERRUPTS])(void) __attribute__((aligned(256)));
#endif
void DL_Interrupt_registerInterrupt(
    uint32_t exceptionNumber, void (*intHandler)(void))
{
    void (**baseAddress)(void);
    uint32_t i;

    /* See if the RAM vector table has been initialized */
    if (SCB->VTOR != (uint32_t) InterruptRAMVectors) {
        baseAddress = (void (**)(void)) SCB->VTOR;

        /* Copy the vector table from Flash to RAM */
        for (i = 0; i < NUM_INTERRUPTS; i++) {
            InterruptRAMVectors[i] = baseAddress[i];
        }

        SCB->VTOR = (uint32_t) InterruptRAMVectors;
    }

    InterruptRAMVectors[exceptionNumber + NUM_RES_M33_EXC] = intHandler;
}

void DL_Interrupt_unregisterInterrupt(uint32_t exceptionNumber)
{
    InterruptRAMVectors[exceptionNumber + NUM_RES_M33_EXC] = Default_Handler;
}
