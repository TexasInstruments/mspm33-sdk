;//*****************************************************************************
;//
;// Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com/
;//
;// Redistribution and use in source and binary forms, with or without
;// modification, are permitted provided that the following conditions
;// are met:
;//
;//  Redistributions of source code must retain the above copyright
;//  notice, this list of conditions and the following disclaimer.
;//
;//  Redistributions in binary form must reproduce the above copyright
;//  notice, this list of conditions and the following disclaimer in the
;//  documentation and/or other materials provided with the
;//  distribution.
;//
;//  Neither the name of Texas Instruments Incorporated nor the names of
;//  its contributors may be used to endorse or promote products derived
;//  from this software without specific prior written permission.
;//
;// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;//
;// MSPM33C321A startup file
;//
;//****************************************************************************
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000100

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     SYSCTL_IRQHandler     ; 1: SYSCTL interrupt handler
                DCD     DEBUGSS_IRQHandler    ; 2: DEBUGSS interrupt handler
                DCD     FLASHCTL_IRQHandler   ; 3: FLASHCTL interrupt handler
                DCD     WWDT0_IRQHandler      ; 4: WWDT0 interrupt handler
                DCD     0                     ; 5: Reserved
                DCD     0                     ; 6: Reserved
                DCD     GPIOA_IRQHandler      ; 7: GPIOA interrupt handler
                DCD     GPIOB_IRQHandler      ; 8: GPIOB interrupt handler
                DCD     GPIOC_IRQHandler      ; 9: GPIOC interrupt handler
                DCD     ADC0_SEQ0_IRQHandler  ; 10: ADC0 interrupt handler
                DCD     ADC0_SEQ1_IRQHandler  ; 11: ADC0 interrupt handler
                DCD     ADC0_SEQ2_IRQHandler  ; 12: ADC0 interrupt handler
                DCD     ADC0_SEQ3_IRQHandler  ; 13: ADC0 interrupt handler
                DCD     ADC0_DCOMP_IRQHandler ; 14: ADC0 interrupt handler
                DCD     ADC1_SEQ0_IRQHandler  ; 15: ADC1 interrupt handler
                DCD     ADC1_SEQ1_IRQHandler  ; 16: ADC1 interrupt handler
                DCD     ADC1_SEQ2_IRQHandler  ; 17: ADC1 interrupt handler
                DCD     ADC1_SEQ3_IRQHandler  ; 18: ADC1 interrupt handler
                DCD     ADC1_DCOMP_IRQHandler ; 19: ADC1 interrupt handler
                DCD     CANFD0_IRQHandler     ; 20: CANFD0 interrupt handler
                DCD     TIMA0_0_IRQHandler    ; 21: TIMA0_0 interrupt handler
                DCD     TIMG4_0_IRQHandler    ; 22: TIMG4_0 interrupt handler
                DCD     TIMG4_1_IRQHandler    ; 23: TIMG4_1 interrupt handler
                DCD     TIMG8_0_IRQHandler    ; 24: TIMG8_0 interrupt handler
                DCD     TIMG12_0_IRQHandler   ; 25: TIMG12_0 interrupt handler
                DCD     UC1_0_IRQHandler      ; 26: UC1_0 interrupt handler
                DCD     UC1_1_IRQHandler      ; 27: UC1_1 interrupt handler
                DCD     UC2_IRQHandler        ; 28: UC2 interrupt handler
                DCD     UC15_0_IRQHandler     ; 29: UC15_0 interrupt handler
                DCD     UC15_1_IRQHandler     ; 30: UC15_1 interrupt handler
                DCD     UC12_IRQHandler       ; 31: UC12 interrupt handler
                DCD     UC13_0_IRQHandler     ; 32: UC13_0 interrupt handler
                DCD     UC14_IRQHandler       ; 33: UC14 interrupt handler
                DCD     CANFD1_IRQHandler     ; 34: CANFD1 interrupt handler
                DCD     TIMA0_1_IRQHandler    ; 35: TIMA0_1 interrupt handler
                DCD     TIMG4_2_IRQHandler    ; 36: TIMG4_2 interrupt handler
                DCD     TIMG4_3_IRQHandler    ; 37: TIMG4_3 interrupt handler
                DCD     TIMG8_1_IRQHandler    ; 38: TIMG8_1 interrupt handler
                DCD     COMP0_IRQHandler      ; 39: COMP0 interrupt handler
                DCD     COMP1_IRQHandler      ; 40: COMP1 interrupt handler
                DCD     TRNG_IRQHandler       ; 41: TRNG interrupt handler
                DCD     AESADV_IRQHandler     ; 42: AESADV interrupt handler
                DCD     LFSS_IRQHandler       ; 43: LFSS interrupt handler
                DCD     DMA0_IRQHandler       ; 44: DMA0 interrupt handler
                DCD     DMA1_IRQHandler       ; 45: DMA1 interrupt handler
                DCD     I2S0_IRQHandler       ; 46: I2S0 interrupt handler
                DCD     I2S1_IRQHandler       ; 47: I2S1 interrupt handler
                DCD     QSPI_IRQHandler       ; 48: QSPI interrupt handler
                DCD     SHAW_IRQHandler       ; 49: SHAW interrupt handler
                DCD     PKA_IRQHandler        ; 50: PKA interrupt handler
                DCD     UC13_1_IRQHandler     ; 51: UC13_1 interrupt handler
                DCD     UC13_2_IRQHandler     ; 52: UC13_2 interrupt handler
                DCD     UC13_3_IRQHandler     ; 53: UC13_3 interrupt handler

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
				; SystemInit can be called here, but not necessary for MSPM0
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT     Default_Handler                 [WEAK]
				EXPORT     SYSCTL_IRQHandler               [WEAK]
                EXPORT     DEBUGSS_IRQHandler              [WEAK]
                EXPORT     FLASHCTL_IRQHandler             [WEAK]
                EXPORT     WWDT0_IRQHandler                [WEAK]
                EXPORT     GPIOA_IRQHandler                [WEAK]
                EXPORT     GPIOB_IRQHandler                [WEAK]
                EXPORT     GPIOC_IRQHandler                [WEAK]
                EXPORT     ADC0_SEQ0_IRQHandler            [WEAK]
                EXPORT     ADC0_SEQ1_IRQHandler            [WEAK]
                EXPORT     ADC0_SEQ2_IRQHandler            [WEAK]
                EXPORT     ADC0_SEQ3_IRQHandler            [WEAK]
                EXPORT     ADC0_DCOMP_IRQHandler           [WEAK]
                EXPORT     ADC1_SEQ0_IRQHandler            [WEAK]
                EXPORT     ADC1_SEQ1_IRQHandler            [WEAK]
                EXPORT     ADC1_SEQ2_IRQHandler            [WEAK]
                EXPORT     ADC1_SEQ3_IRQHandler            [WEAK]
                EXPORT     ADC1_DCOMP_IRQHandler           [WEAK]
                EXPORT     CANFD0_IRQHandler               [WEAK]
                EXPORT     TIMA0_0_IRQHandler              [WEAK]
                EXPORT     TIMG4_0_IRQHandler              [WEAK]
                EXPORT     TIMG4_1_IRQHandler              [WEAK]
                EXPORT     TIMG8_0_IRQHandler              [WEAK]
                EXPORT     TIMG12_0_IRQHandler             [WEAK]
                EXPORT     UC1_0_IRQHandler                [WEAK]
                EXPORT     UC1_1_IRQHandler                [WEAK]
                EXPORT     UC2_IRQHandler                  [WEAK]
                EXPORT     UC15_0_IRQHandler               [WEAK]
                EXPORT     UC15_1_IRQHandler               [WEAK]
                EXPORT     UC12_IRQHandler                 [WEAK]
                EXPORT     UC13_0_IRQHandler               [WEAK]
                EXPORT     UC14_IRQHandler                 [WEAK]
                EXPORT     CANFD1_IRQHandler               [WEAK]
                EXPORT     TIMA0_1_IRQHandler              [WEAK]
                EXPORT     TIMG4_2_IRQHandler              [WEAK]
                EXPORT     TIMG4_3_IRQHandler              [WEAK]
                EXPORT     TIMG8_1_IRQHandler              [WEAK]
                EXPORT     COMP0_IRQHandler                [WEAK]
                EXPORT     COMP1_IRQHandler                [WEAK]
                EXPORT     TRNG_IRQHandler                 [WEAK]
                EXPORT     AESADV_IRQHandler               [WEAK]
                EXPORT     LFSS_IRQHandler                 [WEAK]
                EXPORT     DMA0_IRQHandler                 [WEAK]
                EXPORT     DMA1_IRQHandler                 [WEAK]
                EXPORT     I2S0_IRQHandler                 [WEAK]
                EXPORT     I2S1_IRQHandler                 [WEAK]
                EXPORT     QSPI_IRQHandler                 [WEAK]
                EXPORT     SHAW_IRQHandler                 [WEAK]
                EXPORT     PKA_IRQHandler                  [WEAK]
                EXPORT     UC13_1_IRQHandler               [WEAK]
                EXPORT     UC13_2_IRQHandler               [WEAK]
                EXPORT     UC13_3_IRQHandler               [WEAK]

SYSCTL_IRQHandler
DEBUGSS_IRQHandler
FLASHCTL_IRQHandler
WWDT0_IRQHandler
GPIOA_IRQHandler
GPIOB_IRQHandler
GPIOC_IRQHandler
ADC0_SEQ0_IRQHandler
ADC0_SEQ1_IRQHandler
ADC0_SEQ2_IRQHandler
ADC0_SEQ3_IRQHandler
ADC0_DCOMP_IRQHandler
ADC1_SEQ0_IRQHandler
ADC1_SEQ1_IRQHandler
ADC1_SEQ2_IRQHandler
ADC1_SEQ3_IRQHandler
ADC1_DCOMP_IRQHandler
CANFD0_IRQHandler
TIMA0_0_IRQHandler
TIMG4_0_IRQHandler
TIMG4_1_IRQHandler
TIMG8_0_IRQHandler
TIMG12_0_IRQHandler
UC1_0_IRQHandler
UC1_1_IRQHandler
UC2_IRQHandler
UC15_0_IRQHandler
UC15_1_IRQHandler
UC12_IRQHandler
UC13_0_IRQHandler
UC14_IRQHandler
CANFD1_IRQHandler
TIMA0_1_IRQHandler
TIMG4_2_IRQHandler
TIMG4_3_IRQHandler
TIMG8_1_IRQHandler
COMP0_IRQHandler
COMP1_IRQHandler
TRNG_IRQHandler
AESADV_IRQHandler
LFSS_IRQHandler
DMA0_IRQHandler
DMA1_IRQHandler
I2S0_IRQHandler
I2S1_IRQHandler
QSPI_IRQHandler
SHAW_IRQHandler
PKA_IRQHandler
UC13_1_IRQHandler
UC13_2_IRQHandler
UC13_3_IRQHandler
                B       .
                ENDP

                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
