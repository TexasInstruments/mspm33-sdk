/*****************************************************************************

  Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

   Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

   Neither the name of Texas Instruments Incorporated nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/

/*
 * GCC Startup File for MSPM33C321A - Non-Secure Side
 *
 * This startup file is used for the TrustZone non-secure application.
 * The non-secure side runs after the secure side has configured SAU/GSC
 * and branched to the non-secure reset vector.
 *
 * The non-secure vector table does not include MemManage, BusFault,
 * UsageFault, or SecureFault handlers (slots 4-7 are reserved/zero).
 */

#include <stdint.h>

/* Entry point for the application. */
extern void SystemInit(void);
extern int main(void);

/* Linker-defined symbols */
extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __ramfunct_load__;
extern uint32_t __ramfunct_start__;
extern uint32_t __ramfunct_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

typedef void (*pFunc)(void);

/* Forward declaration of the default fault handlers. */
void Default_Handler(void);
extern void Reset_Handler(void) __attribute__((weak));

/* Processor Exceptions */
extern void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
extern void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
extern void PendSV_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void SysTick_Handler(void)
    __attribute__((weak, alias("Default_Handler")));

/*
 * HardFault_Handler - Non-secure side concrete implementation.
 * This is NOT weak-aliased so that the application (gpio_toggle_ns.c)
 * or this file provides the actual handler.
 */
void HardFault_Handler(void);

/* Device Specific Interrupt Handlers */
extern void SYSCTL_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void DEBUGSS_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void FLASHCTL_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void WWDT0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void GPIOA_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void GPIOB_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void GPIOC_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC0_SEQ0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC0_SEQ1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC0_SEQ2_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC0_SEQ3_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC0_DCOMP_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC1_SEQ0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC1_SEQ1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC1_SEQ2_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC1_SEQ3_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void ADC1_DCOMP_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void CANFD0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMA0_0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG4_0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG4_1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG8_0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG12_0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC2_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC3_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC4_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC5_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC6_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC10_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void CANFD1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMA0_1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG4_2_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG4_3_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TIMG8_1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void COMP0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void COMP1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void TRNG_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void AESADV_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void LFSS_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void DMA0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void DMA1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void I2S0_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void I2S1_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void QSPI_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void SHAW_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void PKA_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC7_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC8_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UC9_IRQHandler(void)
    __attribute__((weak, alias("Default_Handler")));

/*
 * Interrupt vector table - Non-Secure Side
 *
 * Non-secure vector table has slots 4-7 as reserved (0) since
 * MemManage, BusFault, UsageFault, and SecureFault are only
 * available from the secure side.
 */
void (*const interruptVectors[])(void)
    __attribute__((used, section(".intvecs"))) = {
        (pFunc) &__StackTop,   /* The initial stack pointer */
        Reset_Handler,         /* The reset handler         */
        NMI_Handler,           /* The NMI handler           */
        HardFault_Handler,     /* The hard fault handler    */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        SVC_Handler,           /* SVCall handler            */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        PendSV_Handler,        /* The PendSV handler        */
        SysTick_Handler,       /* SysTick handler           */
        SYSCTL_IRQHandler,     /* SYSCTL interrupt handler  */
        DEBUGSS_IRQHandler,    /* DEBUGSS interrupt handler */
        FLASHCTL_IRQHandler,   /* FLASHCTL interrupt handler */
        WWDT0_IRQHandler,      /* WWDT0 interrupt handler   */
        0,                     /* Reserved                  */
        0,                     /* Reserved                  */
        GPIOA_IRQHandler,      /* GPIOA interrupt handler   */
        GPIOB_IRQHandler,      /* GPIOB interrupt handler   */
        GPIOC_IRQHandler,      /* GPIOC interrupt handler   */
        ADC0_SEQ0_IRQHandler,  /* ADC0 interrupt handler    */
        ADC0_SEQ1_IRQHandler,  /* ADC0 interrupt handler    */
        ADC0_SEQ2_IRQHandler,  /* ADC0 interrupt handler    */
        ADC0_SEQ3_IRQHandler,  /* ADC0 interrupt handler    */
        ADC0_DCOMP_IRQHandler, /* ADC0 interrupt handler    */
        ADC1_SEQ0_IRQHandler,  /* ADC1 interrupt handler    */
        ADC1_SEQ1_IRQHandler,  /* ADC1 interrupt handler    */
        ADC1_SEQ2_IRQHandler,  /* ADC1 interrupt handler    */
        ADC1_SEQ3_IRQHandler,  /* ADC1 interrupt handler    */
        ADC1_DCOMP_IRQHandler, /* ADC1 interrupt handler    */
        CANFD0_IRQHandler,     /* CANFD0 interrupt handler  */
        TIMA0_0_IRQHandler,    /* TIMA0_0 interrupt handler */
        TIMG4_0_IRQHandler,    /* TIMG4_0 interrupt handler */
        TIMG4_1_IRQHandler,    /* TIMG4_1 interrupt handler */
        TIMG8_0_IRQHandler,    /* TIMG8_0 interrupt handler */
        TIMG12_0_IRQHandler,   /* TIMG12_0 interrupt handler */
        UC0_IRQHandler,        /* UC0 interrupt handler     */
        UC1_IRQHandler,        /* UC1 interrupt handler     */
        UC2_IRQHandler,        /* UC2 interrupt handler     */
        UC3_IRQHandler,        /* UC3 interrupt handler     */
        UC4_IRQHandler,        /* UC4 interrupt handler     */
        UC5_IRQHandler,        /* UC5 interrupt handler     */
        UC6_IRQHandler,        /* UC6 interrupt handler     */
        UC10_IRQHandler,       /* UC10 interrupt handler    */
        CANFD1_IRQHandler,     /* CANFD1 interrupt handler  */
        TIMA0_1_IRQHandler,    /* TIMA0_1 interrupt handler */
        TIMG4_2_IRQHandler,    /* TIMG4_2 interrupt handler */
        TIMG4_3_IRQHandler,    /* TIMG4_3 interrupt handler */
        TIMG8_1_IRQHandler,    /* TIMG8_1 interrupt handler */
        COMP0_IRQHandler,      /* COMP0 interrupt handler   */
        COMP1_IRQHandler,      /* COMP1 interrupt handler   */
        TRNG_IRQHandler,       /* TRNG interrupt handler    */
        AESADV_IRQHandler,     /* AESADV interrupt handler  */
        LFSS_IRQHandler,       /* LFSS interrupt handler    */
        DMA0_IRQHandler,       /* DMA0 interrupt handler    */
        DMA1_IRQHandler,       /* DMA1 interrupt handler    */
        I2S0_IRQHandler,       /* I2S0 interrupt handler    */
        I2S1_IRQHandler,       /* I2S1 interrupt handler    */
        QSPI_IRQHandler,       /* QSPI interrupt handler    */
        SHAW_IRQHandler,       /* SHAW interrupt handler    */
        PKA_IRQHandler,        /* PKA interrupt handler     */
        UC7_IRQHandler,        /* UC7 interrupt handler     */
        UC8_IRQHandler,        /* UC8 interrupt handler     */
        UC9_IRQHandler,        /* UC9 interrupt handler     */
};

/*
 * Reset_Handler - Non-Secure side
 *
 * This is the code that gets called when the processor starts executing
 * the non-secure application (branched from secure side). It performs
 * standard C runtime initialization:
 *   1. Copies .data and .ramfunc from flash to SRAM
 *   2. Zeros .bss
 *   3. Calls SystemInit() then main()
 */
void Reset_Handler(void)
{
    uint32_t *pui32Src, *pui32Dest;
    uint32_t *bs, *be;

    /* Copy the data segment initializers from flash to SRAM. */
    pui32Src = &__data_load__;
    for (pui32Dest = &__data_start__; pui32Dest < &__data_end__;) {
        *pui32Dest++ = *pui32Src++;
    }

    /* Copy the ramfunc segment initializers from flash to SRAM. */
    pui32Src = &__ramfunct_load__;
    for (pui32Dest = &__ramfunct_start__; pui32Dest < &__ramfunct_end__;) {
        *pui32Dest++ = *pui32Src++;
    }

    /* Initialize .bss to zero */
    bs = &__bss_start__;
    be = &__bss_end__;
    while (bs < be) {
        *bs = 0;
        bs++;
    }

    /* Call SystemInit for early system initialization (FPU config, etc.) */
    SystemInit();

    /* Call the application's entry point. */
    main();

    /* If we ever return, signal error */
    HardFault_Handler();
}

/* This is the code that gets called when the processor receives an unexpected  */
/* interrupt.  This simply enters an infinite loop, preserving the system state */
/* for examination by a debugger.                                               */
void Default_Handler(void)
{
    /* Enter an infinite loop. */
    while (1) {
    }
}

/*
 * HardFault_Handler - concrete implementation for non-secure side.
 * Enters an infinite loop for debugger examination.
 */
void HardFault_Handler(void)
{
    while (1) {
    }
}
