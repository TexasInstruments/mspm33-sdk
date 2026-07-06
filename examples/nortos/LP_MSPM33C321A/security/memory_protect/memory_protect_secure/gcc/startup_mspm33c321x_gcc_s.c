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
 * GCC Startup File for MSPM33C321A - Secure Side
 *
 * This startup file is used for the TrustZone secure application.
 * It configures the secure vector table, enables fault handlers,
 * sets up the non-secure VTOR, waits for SRAM readiness, then
 * performs standard C runtime initialization before calling main().
 */

#include <stdint.h>
#include <ti/devices/msp/msp.h>

/* Non-secure flash start address - must match linker configuration */
#define NS_FLASH_START 0x00010000

#define SYSCTL_SOCLOCK_Reg (&SYSCTL->SOCLOCK)

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

/* Processor Exceptions - Secure side includes all fault handlers */
extern void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
extern void HardFault_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void MemManage_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void BusFault_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void UsageFault_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void SecureFault_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
extern void PendSV_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
extern void SysTick_Handler(void)
    __attribute__((weak, alias("Default_Handler")));

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
 * Interrupt vector table.
 *
 * Secure side vector table includes MemManage, BusFault, UsageFault,
 * and SecureFault handlers (slots 4-7) which are not available on the
 * non-secure side.
 */
void (*const interruptVectors[])(void) __attribute__((section(".intvecs"))) = {
    (pFunc) &__StackTop,   /* The initial stack pointer */
    Reset_Handler,         /* The reset handler         */
    NMI_Handler,           /* The NMI handler           */
    HardFault_Handler,     /* The hard fault handler    */
    MemManage_Handler,     /* -12 MPU Fault Handler     */
    BusFault_Handler,      /* -11 Bus Fault Handler     */
    UsageFault_Handler,    /* -10 Usage Fault Handler   */
    SecureFault_Handler,   /*  -9 Secure Fault Handler  */
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
 * Reset_Handler - Secure side
 *
 * This is the code that gets called when the processor first starts execution
 * following a reset event. It performs the following secure-specific setup:
 *   1. Sets SCB->VTOR to the secure vector table
 *   2. Enables MemManage, BusFault, and UsageFault handlers
 *   3. Sets SCB_NS->VTOR to the non-secure flash start address
 *   4. Waits for all SRAM banks to become ready
 *   5. Copies .data and .ramfunc from flash to SRAM
 *   6. Zeros .bss
 *   7. Calls SystemInit() then main()
 */
void Reset_Handler(void)
{
    uint32_t *pui32Src, *pui32Dest;
    uint32_t *bs, *be;

    /* Set secure VTOR to this vector table */
    SCB->VTOR = (uint32_t) interruptVectors;

    /* Enable configurable fault handlers (MemManage, BusFault, UsageFault) */
    SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
                   SCB_SHCSR_MEMFAULTENA_Msk);

    /* Set non-secure VTOR to NS flash start - CRITICAL for NS interrupts! */
    SCB_NS->VTOR = (uint32_t) NS_FLASH_START;

    /* Enable FPU access for non-secure state.
     * NSACR (Non-Secure Access Control Register) controls which coprocessors
     * are accessible from non-secure state. Bits 10 and 11 control CP10/CP11 (FPU).
     * Without this, any FPU instruction in NS code causes a NOCP UsageFault.
     */
    SCB->NSACR |= ((1UL << 10) | (1UL << 11)); /* Allow NS access to FPU */

    /* Also enable FPU in secure state (CPACR) */
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* Full FPU access */

    __DSB();
    __ISB();

    /* Wait for SRAM banks to become ready */
    while ((SYSCTL_SOCLOCK_Reg->SYSSTATUS &
               (SYSCTL_SYSSTATUS_SRAMBANK1READY_TRUE |
                   SYSCTL_SYSSTATUS_SRAMBANK2READY_TRUE |
                   SYSCTL_SYSSTATUS_SRAMBANK3READY_TRUE)) !=
           (SYSCTL_SYSSTATUS_SRAMBANK1READY_TRUE |
               SYSCTL_SYSSTATUS_SRAMBANK2READY_TRUE |
               SYSCTL_SYSSTATUS_SRAMBANK3READY_TRUE)) {
    }

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
