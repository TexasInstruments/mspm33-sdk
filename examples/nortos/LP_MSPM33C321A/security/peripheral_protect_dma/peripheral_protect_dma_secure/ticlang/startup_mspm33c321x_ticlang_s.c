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

#include <stdint.h>
#include <ti/devices/msp/msp.h>

#define SYSCTL_SOCLOCK_Reg (&SYSCTL->SOCLOCK)
#define NS_FLASH_START 0x00010000

/* Linker variable that marks the top of the stack. */
extern unsigned long __STACK_END;

/* External declaration for the reset handler that is to be called when the */
/* processor is started                                                     */
extern __NO_RETURN void __PROGRAM_START(void);

/* Forward declaration of the default fault handlers. */
void Default_Handler(void) __attribute__((weak));

extern void Reset_Handler(void) __attribute__((weak));

extern void MemManage_Handler(void) __attribute__((weak));
extern void BusFault_Handler(void) __attribute__((weak));
extern void UsageFault_Handler(void) __attribute__((weak));
extern void SecureFault_Handler(void) __attribute__((weak));
extern void Reset_Handler(void) __attribute__((weak));
/* Processor Exceptions */
extern void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
extern void HardFault_Handler(void)
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

/* Interrupt vector table.  Note that the proper constructs must be placed on this to */
/* ensure that it ends up at physical address 0x0000.0000 or at the start of          */
/* the program if located at a start address other than 0.                            */
#if defined(__ARM_ARCH) && (__ARM_ARCH != 0)
void (*const interruptVectors[])(void) __attribute((used))
__attribute__((section(".intvecs"))) =
#elif defined(__TI_ARM__)
#pragma RETAIN(interruptVectors)
#pragma DATA_SECTION(interruptVectors, ".intvecs")
void (*const interruptVectors[])(void) =
#else
#error "Compiler not supported"
#endif
    {
        (void (*)(void))((uint32_t) &__STACK_END),
        /* The initial stack pointer */
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
        UC0_IRQHandler,        /* UC0 interrupt handler */
        UC1_IRQHandler,        /* UC1 interrupt handler */
        UC2_IRQHandler,        /* UC2 interrupt handler */
        UC3_IRQHandler,        /* UC3 interrupt handler */
        UC4_IRQHandler,        /* UC4 interrupt handler */
        UC5_IRQHandler,        /* UC5 interrupt handler */
        UC6_IRQHandler,        /* UC6 interrupt handler */
        UC10_IRQHandler,       /* UC10 interrupt handler */
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
        UC7_IRQHandler,        /* UC7 interrupt handler */
        UC8_IRQHandler,        /* UC8 interrupt handler */
        UC9_IRQHandler,        /* UC9 interrupt handler */

    };

/* Forward declaration of the default fault handlers. */
/* This is the code that gets called when the processor first starts execution */
/* following a reset event.  Only the absolutely necessary set is performed,   */
/* after which the application supplied entry() routine is called.  Any fancy  */
/* actions (such as making decisions based on the reset cause register, and    */
/* resetting the bits in that register) are left solely in the hands of the    */
/* application.                                                                */
void Reset_Handler(void)
{
    SCB->VTOR = (volatile unsigned int) interruptVectors;
    SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
                   SCB_SHCSR_MEMFAULTENA_Msk);
    SCB_NS->VTOR = (volatile unsigned int) NS_FLASH_START;

    /**
      *  Wait for
      */
    while ((SYSCTL_SOCLOCK_Reg->SYSSTATUS &
               (SYSCTL_SYSSTATUS_SRAMBANK1READY_TRUE |
                   SYSCTL_SYSSTATUS_SRAMBANK2READY_TRUE |
                   SYSCTL_SYSSTATUS_SRAMBANK3READY_TRUE)) !=
           (SYSCTL_SYSSTATUS_SRAMBANK1READY_TRUE |
               SYSCTL_SYSSTATUS_SRAMBANK2READY_TRUE |
               SYSCTL_SYSSTATUS_SRAMBANK3READY_TRUE)) {
    }

    /* Jump to the ticlang C Initialization Routine. */
    __asm(
        "    .global _c_int00\n"
        "    b       _c_int00");
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
