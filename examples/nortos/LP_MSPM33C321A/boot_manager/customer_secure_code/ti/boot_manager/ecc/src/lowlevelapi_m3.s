@#########################################################################################
@  Filename:       lowlevelapi.s
@  Description:    Low level library for ECC library on Cortex M3
@  Language:       GNU Assembly for ARM
@  Component:      ARM Cortex M3
@  Compiler:       GCC
@  Authors:        Alexandre Berzati
@  Release:        1.0
@  Copyright:      Invia 2012
@  Note:           This library is dedicated to arithmetic operations on large integers.
@                  It emulates a public-key cryptoprocessor and makes intensive use of
@                  the native multiplier and advanced optimizations.
@#########################################################################################
@ Customizations:
@   - Removed global variables and modified APIs accordingly.
@   - Modifications to support TI CLANG compiler tool chain.
@
@ Copyright (c) 2020-2021, Texas Instruments Incorporated
@ All rights reserved.
@#########################################################################################
    .syntax    unified
    #if defined(__ti_version__)
    @ Workaround for a tiarmclang v1.2.1 linker build attribute checking issue
@    .eabi_attribute 25, 1   @ Tag_ABI_align_preserved (indicates assembly code preserves eight-byte alignment of the stack)
@    .eabi_attribute 28, 1   @ Tag_ABI_VFP_args (needed if linked code is built with -mfloat-abi=hard)
    #else
    .cpu       cortex-m3
    #endif
    .file      "lowlevelapi_m3.s"

@-----------------------------------------------------------------------------------------
@  Handy macros
@-----------------------------------------------------------------------------------------

    .macro  GET     r,  var
    MOVW    \r,     #:lower16:\var
    MOVT    \r,     #:upper16:\var
    LDR     \r,     [\r]
    .endm

    .macro  PUT     r,  var, t
    MOVW    \t,     #:lower16:\var
    MOVT    \t,     #:upper16:\var
    STR     \r,     [\t]
    .endm

    .macro  .start_function   name
    .text
    .align  2
    .thumb
    .global \name
    .thumb_func
    .type   \name, %function
    .cfi_startproc
    .endm

    .macro  .end_function   name
    .cfi_endproc
    .endm

@-----------------------------------------------------------------------------------------
@  Low-level functions
@-----------------------------------------------------------------------------------------
@-----------------------------------------------------------------------------------------
@  Modular operators
@-----------------------------------------------------------------------------------------
@(((((((((((((((((((((((((((((((((((((((((((mMULT)))))))))))))))))))))))))))))))))))))))))))
@
@  High-level description
@
@      Montgomery modular multiplication out = in1 * in2 * K mod MOD where K is the
@      Montgomery factor K = 2^-(32*LEN) mod MOD.
@
@  Interface
@
@      Arguments are given through global variables:
@
@      in1     points on multiplicator in operand format
@      in2     points on multiplicand in operand format
@      out     points anywhere in memory except in1 or in2
@
@      A modulus must have been initialized by the mSET routine.
@
@  Developper's notes
@
@      Interleaved Montgomery. The output result is always on |modulus| words, so that mMULT
@      can be used for reiterated squarings. Implements a direct 32-bit strategy using the
@      32x32 -> 64 CPU multiplier.
@
@-----------------------------------------------------------------------------------------
    .start_function mMULT

    u_  .req    R0
    a_  .req    R1
    b_  .req    R2

    m_  .req    R3
    i   .req    R4
    F   .req    R5
    l   .req    R6
    A   .req    R7
    Q   .req    R8
    BB  .req    R9
    M   .req    R9
    U   .req    R9
    H1  .req    R10
    H2  .req    R11
    LO  .req    R12
    AD  .req    R14

mMULT:

    @ Starting Bloc C

    PUSH    {R0-R7,R8-R12,R14}

    LDR     l,      [SP,    #56]            @ l = LEN

    ADD     m_,     #8
    MOV     i,      l                       @ i = LEN

    MOV     F,      #0                      @ F = overflow = 0

    @ First external loop specialized for u = 0

    LDR     A,      [a_],   #4              @ A = a[i]
    LDR     BB,     [b_],   #4              @ B = b[0]
    LDR     Q,      [m_,    #-8]            @ Q = alpha = -1/m mod 2^32

    UMULL   LO,     H2,     A,      BB      @ H2|LO = a[i] * b[0]
    MUL     Q,      LO                      @ Q = alpha * LO mod 2^32

    LDR     M,      [m_],   #4              @ M = m[0]
    UMULL   M,      H1,     Q,      M       @ H1|M = Q * m[j]
    ADDS    M,      LO
    ADCS    LO,     H1,     H2
    MOV     H2,     #0
    ADC     H1,     H2,     H2              @ H1|LO = H1 + H2 = 33-bit carry

    MOV     AD,     #-32
    MUL     AD,     l
    ADD     AD,     PC                      @ branch address for internal loop
    ADD     AD,     #549                    @ making sure that lsb(AD) is 1
    BX      AD

.rept 16

    LDR     BB,     [b_],   #4              @ B = b[j]                                  @@ 2 cycles
    UMLAL   LO,     H1,     A,     BB       @ H1|LO = a[i] * b[j] + 33-bit carry        @@ 7 cycles
    LDR     M,      [m_],   #4              @ M = m[j]                                  @@ 2 cycles
    UMLAL   LO,     H2,     Q,     M        @ H2|LO = Q * m[j] + LO + U                 @@ 7 cycles
    STR     LO,     [u_],   #4              @ u[j] = LO                                 @@ 1 cycle
    ADDS    LO,     H1,     H2                                                          @@ 1 cycle
    MOV     H2,     #0                                                                  @@ 1 cycle
    ADC     H1,     H2,     H2              @ H1|LO = H1 + H2 = 33-bit carry            @@ 1 cycle

.endr

    ADDS    LO,     F
    ADC     F,      H1,     #0              @ H1|LO = F + 33-bit carry
    STR     LO,     [u_],   #4              @ u[j] = LO

    SUB     b_,     b_,     l,      LSL #2  @ b = b - 4*LEN points on b[0]
    SUB     u_,     u_,     l,      LSL #2  @ u = u - 4*LEN points on u[0]
    SUB     m_,     m_,     l,      LSL #2  @ m = m - 4*LEN points on m[0]

    @ Generic external loop

    MOV     AD,     #-44
    MUL     AD,     l
    ADD     AD,     PC                      @ branch address for internal loop
    ADD     AD,     #753                    @ making sure that lsb(AD) is 1

    B.N     mMULT_external

    @ Ending Bloc C

    @ Starting Bloc B

mMULT_internal:

.rept 16

    LDR     BB,     [b_],   #4              @ B = b[j]                                  @@ 2 cycles
    UMLAL   LO,     H1,     A,     BB       @ H1|LO = a[i] * b[j] + 33-bit carry        @@ 7 cycles
    LDR     U,      [u_,    #4]             @ U = u[j+1]                                @@ 2 cycles
    ADDS    LO,     U                                                                   @@ 1 cycle
    ADC     H2,     H2,     H2              @ H2|LO = LO + U                            @@ 1 cycle
    LDR     M,      [m_],   #4              @ M = m[j]                                  @@ 2 cycles
    UMLAL   LO,     H2,     Q,     M        @ H2|LO = Q * m[j] + LO + U                 @@ 7 cycles
    STR     LO,     [u_],   #4              @ u[j] = LO                                 @@ 1 cycle
    ADDS    LO,     H1,     H2                                                          @@ 1 cycle
    MOV     H2,     #0                                                                  @@ 1 cycle
    ADC     H1,     H2,     H2              @ H1|LO = H1 + H2 = 33-bit carry            @@ 1 cycle

.endr

    ADDS    LO,     F
    ADC     F,      H1,     #0              @ H1|LO = F + 33-bit carry
    STR     LO,     [u_],   #4              @ u[j] = LO

    SUB     b_,     b_,     l,      LSL #2  @ b = b - 4*LEN points on b[0]
    SUB     u_,     u_,     l,      LSL #2  @ u = u - 4*LEN points on u[0]
    SUB     m_,     m_,     l,      LSL #2  @ m = m - 4*LEN points on m[0]

    @ Ending Bloc B

    @ Starting Bloc A

mMULT_external:

    SUBS    i,     #1
    BEQ     mMULT_sub_once

    LDR     A,      [a_],   #4              @ A = a[i]
    LDR     BB,     [b_],   #4              @ B = b[0]
    LDR     LO,     [u_]                    @ LO = u[0]
    LDR     Q,      [m_,    #-8]            @ Q = alpha = -1/m mod 2^32

    MOV     H2,     #0
    UMLAL   LO,     H2,     A,      BB      @ H2|LO = a[i] * b[0] + u[0] since H2 = 0
    MUL     Q,      LO                      @ Q = alpha * LO mod 2^32

    LDR     M,      [m_],   #4              @ M = m[0]
    UMULL   M,      H1,     Q,      M       @ H1|M = Q * m[j]
    ADDS    M,      LO
    ADCS    LO,     H1,     H2
    MOV     H2,     #0
    ADC     H1,     H2,     H2              @ H1|LO = H1 + H2 = 33-bit carry

    BX      AD

    @ Ending Bloc A

    @ Step 3: optional subtraction of modulus

mMULT_sub_once:

    CMP     F,      #0
    BEQ     mMULT_sub_once_done

    MOV     H1,     #1                      @ H1 = subtraction carry = 1

mMULT_sub_once_loop:

    LDR     LO,     [u_]
    LDR     M,      [m_],   #4

    ADDS    LO,     H1
    SBC     H2,     H2,     H2
    MVN     M,      M
    ADDS    LO,     M
    ADC     H1,     H2,     #1

    STR     LO,     [u_],   #4

    SUBS    l,      #1
    BNE     mMULT_sub_once_loop

mMULT_sub_once_done:

    POP     {R0-R7,R8-R12,R14}
    BX      LR

    .end_function mMULT

    .unreq  a_
    .unreq  b_
    .unreq  u_
    .unreq  m_
    .unreq  i
    .unreq  F
    .unreq  l
    .unreq  A
    .unreq  Q
    .unreq  BB
    .unreq  M
    .unreq  U
    .unreq  H1
    .unreq  H2
    .unreq  LO
    .unreq  AD

@(((((((((((((((((((((((((((((((((((((((((((mMULT)))))))))))))))))))))))))))))))))))))))))))

@(((((((((((((((((((((((((((((((((((((((((((mADD)))))))))))))))))))))))))))))))))))))))))))
@
@  High-level description
@
@      Computes out = in1 + in2 mod MOD
@
@  Interface
@
@      Arguments are given through global variables:
@
@      in1     points on first operand in operand format
@      in2     points on second operand in operand format
@      out     points on result to be written in operand format
@
@      A modulus must have been initialized by the mSET routine.
@
@  Developper's notes
@
@      First pass u = in1 + in2 - d * MOD where d is the (over-)estimated quotient,
@      followed by a second-pass, optional addition of MOD.
@
@-----------------------------------------------------------------------------------------
    .start_function mADD

    u_  .req    R0
    a_  .req    R1
    b_  .req    R2

    m_  .req    R8
    j   .req    R9
    M   .req    R5
    A   .req    R6
    BB  .req    R3
    F   .req    R3
    D   .req    R4
    H   .req    R3
    LO  .req    R10
    U   .req    R3

mADD:

    PUSH    {R0-R6,R8-R10}

    MOV     m_,     R3
    LDR     j,      [SP,    #40]            @ j = LEN

    @ Step 1: over-estimate quotient D = (31msb(a)+31msb(b))/(31msb(m)+1)

    ADD     m_,     #8
    SUB     j,      #1
    LDR     M,      [m_,    j,  LSL #2]     @ M = m[LEN-1]
    LDR     A,      [a_,    j,  LSL #2]     @ A = a[LEN-1]
    LDR     BB,     [b_,    j,  LSL #2]     @ B = b[LEN-1]

    LSR     M,      #1
    ADD     M,      #1                      @ M = 31msb(m) + 1

    LSR     A,      #1
    LSR     BB,     #1
    ADD     A,      BB                      @ A = 31msb(a) + 31msb(b)

    UDIV    D,      A,      M               @ D = estimated quotient

    ADD     j,      #1                      @ j = LEN
    CBNZ    D,      mADD_generic            @ detecting D = 0

    @ Step 2: treat separately the case D = 0

    MOV     LO,     #0                      @ set carry = 0

    @ Since we are not going through the mADD_generic -> mADD_loop branch,
    @ we need to adjust m_ since mADD_loop increments it by 4 on each iteration.
    ADD     m_,     m_,     j,      LSL #2  @ m = m + 4*LEN points on m[LEN-1]

mADD_zero_case_loop:

    LDR     A,      [a_],   #4              @ A = a[j]
    LDR     BB,     [b_],   #4              @ B = b[j]
    ADDS    A,      LO
    SBC     LO,     LO,     LO
    ADDS    A,      BB
    ADC     LO,     LO,     #1              @ (LO|A) = A + B + LO

    STR     A,      [u_],   #4

    SUBS    j,      #1
    BNE     mADD_zero_case_loop

    MOV     F,      LO
    B       mADD_correction

    @ Step 3: generic pass u = a + b - D * m for D > 0

mADD_generic:

    MOV     LO,     D                       @ LO = carry = D

mADD_loop:

    LDR     A,      [a_],   #4
    LDR     BB,     [b_],   #4
    LDR     M,      [m_],   #4
    ADDS    A,      BB
    MVN     M,      M
    SBC     H,      H,      H
    ADD     H,      #1                      @ H|A = A + B

    UMLAL   A,      H,      D,      M       @ H|A = D * (~M) + A + B
    ADDS    A,      LO
    ADC     LO,     H,      #0              @ LO|A = D * (~M) + A + B + LO

    STR     A,      [u_],   #4

    SUBS    j,      #1
    BNE     mADD_loop

    SUBS    F,      LO,     D

    @ Step 4: optional correction pass (F|u) = (F|u) - m

mADD_correction:

    CBZ     F,      mADD_done

    LDR     j,      [SP,    #40]            @ l = LEN
    SUB     u_,     u_,     j,      LSL #2  @ u = u - 4*LEN points on u[0]
    SUB     m_,     m_,     j,      LSL #2  @ m = m - 4*LEN points on m[0]

    MOV     LO,     #1                      @ set carry = 1

mADD_correction_loop:

    LDR     U,      [u_]                    @ U = u[j]
    LDR     M,      [m_],   #4              @ M = m[j]
    ADDS    U,      LO
    MVN     M,      M
    SBC     LO,     LO,     LO
    ADDS    U,      M
    ADC     LO,     LO,     #1              @ LO = sum of two carries

    STR     U,      [u_],   #4

    SUBS    j,      #1
    BNE     mADD_correction_loop

    ADD     F,      LO
    SUBS    F,      #1
    B       mADD_correction

    @ Step 5: done

mADD_done:

    POP     {R0-R6,R8-R10}
    BX      LR

    .end_function mADD

    .unreq  a_
    .unreq  b_
    .unreq  u_
    .unreq  m_
    .unreq  j
    .unreq  M
    .unreq  A
    .unreq  BB
    .unreq  F
    .unreq  D
    .unreq  H
    .unreq  LO
    .unreq  U

@(((((((((((((((((((((((((((((((((((((((((((mADD)))))))))))))))))))))))))))))))))))))))))))

@(((((((((((((((((((((((((((((((((((((((((((mSUB)))))))))))))))))))))))))))))))))))))))))))
@
@  High-level description
@
@      Computes out = in1 - in2 mod MOD
@
@  Interface
@
@      out     points to result to be written in operand format
@      in1     points to first operand in operand format
@      in2     points to second operand in operand format
@      mod     points to modulus value
@      len     data length
@
@  Developer's notes
@
@
@-----------------------------------------------------------------------------------------
    .start_function mSUB

    u_  .req    R0
    a_  .req    R1
    b_  .req    R2

    m_  .req    R11
    j   .req    R4
    M   .req    R5
    A   .req    R6
    BB  .req    R8
    F   .req    R3
    D   .req    R9
    H   .req    R8
    LO  .req    R10
    U   .req    R8

mSUB:

    PUSH    {R0-R6,R8-R11}

    MOV     m_,     R3                      @ modulus value
    LDR     j,      [SP,    #44]            @ j = LEN

    @ Step 1: over-estimate quotient ((b - a + 1)/m) + 1

    ADD     m_,     #8
    SUB     j,      #1
    LDR     M,      [m_,    j,  LSL #2]     @ M = m[LEN-1]
    LDR     A,      [a_,    j,  LSL #2]     @ A = a[LEN-1]
    LDR     BB,     [b_,    j,  LSL #2]     @ B = b[LEN-1]
    ADD     j,      #1                      @ j = LEN

    MOV     LO,     #1                      @ set carry = 1

    CMP     A,      BB
    BHI     mSUB_zero_case_loop

    SUB     BB,     A
    MVNS    A,      BB
    IT      NE
    ADDNE   BB,     #1
    UDIV    D,      BB,     M
    ADD     D,      #1                      @ estimated quotient D

    @ Step 2: generic pass u = a - b + D * m for D > 0

mSUB_loop:

    LDR     A,      [a_],   #4
    LDR     BB,     [b_],   #4
    LDR     M,      [m_],   #4

    MVN     BB,     BB
    ADDS    A,      BB
    SBC     H,      H,      H
    ADD     H,      #1                      @ H|A = A + (~B)

    UMLAL   A,      H,      D,      M       @ H|A = D * M + A + (~B)
    ADDS    A,      LO
    ADC     LO,     H,      #0              @ LO|A = D * M + A + (~B) + LO

    STR     A,      [u_],   #4

    SUBS    j,      #1
    BNE     mSUB_loop

    SUB     F,      LO,     #1              @ F = overflow = LO - 1

    @ Step 3: optional correction pass u = u - m

mSUB_correction:

    CBZ     F,      mSUB_done

    LDR     j,      [SP,    #44]            @ j = LEN
    SUB     u_,     u_,     j,      LSL #2  @ u = u - 4*LEN points on u[0]
    SUB     m_,     m_,     j,      LSL #2  @ m = m - 4*LEN points on m[0]

    MOV     LO,     #1                      @ LO = carry = 1

mSUB_correction_loop:

    LDR     U,      [u_]                    @ U = u[j]
    LDR     M,      [m_],   #4              @ M = m[j]
    ADDS    U,      LO
    SBC     LO,     LO,     LO
    MVN     M,      M
    ADDS    U,      M
    ADC     LO,     LO,     #1

    STR     U,      [u_],   #4

    SUBS    j,      #1
    BNE     mSUB_correction_loop

    SUB     LO,     #1
    ADD     F,      LO

    B       mSUB_correction

    @ Step 4: treat separately the case D = 0

mSUB_zero_case_loop:

    LDR     A,      [a_],   #4              @ A = a[j]
    LDR     BB,     [b_],   #4              @ B = b[j]
    ADDS    A,      LO
    SBC     LO,     LO,     LO
    MVN     BB,     BB
    ADDS    A,      BB
    ADC     LO,     LO,     #1              @ LO = sum of two carries

    STR     A,      [u_],   #4

    SUBS    j,      #1
    BNE     mSUB_zero_case_loop

mSUB_done:

    POP     {R0-R6,R8-R11}
    BX      LR

    .end_function mSUB

    .unreq  a_
    .unreq  b_
    .unreq  u_
    .unreq  m_
    .unreq  j
    .unreq  M
    .unreq  A
    .unreq  BB
    .unreq  F
    .unreq  D
    .unreq  H
    .unreq  LO
    .unreq  U

@(((((((((((((((((((((((((((((((((((((((((((mSUB)))))))))))))))))))))))))))))))))))))))))))

@(((((((((((((((((((((((((((((((((((((((((((mOUT)))))))))))))))))))))))))))))))))))))))))))
@
@  High-level description
@
@      Computes the scrict modular remainder out = in1 mod MOD < MOD.
@
@  Interface
@
@      Arguments are given through global variables:
@
@      in1     points on integer in operand format
@      out     points on result to be written in operand format
@
@      A modulus must have been initialized by the mSET routine.
@
@  Developper's notes
@
@      Exact quotient d = in1 / MOD then subtract d.MOD from in1 on LEN words.
@
@-----------------------------------------------------------------------------------------
    .start_function mOUT

    u_  .req    R0
    a_  .req    R1
    b_  .req    R2

    m_  .req    R3
    j   .req    R4
    M   .req    R5
    A   .req    R6
    F   .req    R8
    D   .req    R9
    H   .req    R8
    LO  .req    R10
    U   .req    R8

mOUT:

    PUSH    {R0-R7,R8-R10}

    MOV     R7,     R3                      @ R7 = LEN
    MOV     j,      R7                      @ j = LEN
    MOV     m_,     R2                      @ m_ = MOD

    @ Step 1: over-estimate quotient a/m

    ADD     m_,     #8
    SUB     j,      #1
    LDR     A,      [a_,    j,  LSL #2]     @ A = a[LEN-1]
    LDR     M,      [m_,    j,  LSL #2]     @ M = m[LEN-1]

    ADDS    A,      #1
    IT      EQ
    SUBEQ   A,      #1                      @ A = A + 1 if A != FFFFFFFF

    UDIV    D,      A,      M               @ D = estimated quotient A/M

    MOVS    D,      D                       @ if D = 0, force D = 1
    IT      EQ
    ADDEQ   D,      #1

    ADD     j,      #1                      @ j = LEN

    @ Step 2: generic pass u = a - D * m for D > 0

    MOV     LO,     D                       @ LO = carry = D
    MOV     H,      #0

mOUT_loop:

    LDR     A,      [a_],   #4
    LDR     M,      [m_],   #4
    MVN     M,      M

    UMLAL   LO,     H,      D,      M       @ (H|LO) = (H|LO) + D * (~M) + A
    ADDS    LO,     A
    STR     LO,     [u_],   #4

    ADC     LO,     H,      #0
    MOV     H,      #0

    SUBS    j,      #1
    BNE     mOUT_loop

    @ Step 3: optional correction pass u = u + m

    SUBS    LO,     D
    BEQ     mOUT_done

    MOV     j,      R7                      @ j = LEN
    SUB     u_,     u_,     j,      LSL #2  @ u = u - 4*LEN points on u[0]
    SUB     m_,     m_,     j,      LSL #2  @ m = m - 4*LEN points on m[0]

    ADDS    U,      #0                      @ set carry = 0

mOUT_correction_loop:

    LDR     U,      [u_]                    @ U = u[j]
    LDR     M,      [m_],   #4              @ M = m[j]
    ADCS    U,      U,      M               @ (carry | U) = U + M + carry
    STR     U,      [u_],   #4

    SUB     j,      #1
    CBZ     j,      mOUT_done
    NOP
    NOP
    B       mOUT_correction_loop

mOUT_done:

    POP     {R0-R7,R8-R10}
    BX      LR

    .end_function mOUT

    .unreq  a_
    .unreq  b_
    .unreq  u_
    .unreq  m_
    .unreq  j
    .unreq  M
    .unreq  A
    .unreq  F
    .unreq  D
    .unreq  H
    .unreq  LO
    .unreq  U

@(((((((((((((((((((((((((((((((((((((((((((mOUT)))))))))))))))))))))))))))))))))))))))))))

@-----------------------------------------------------------------------------------------
@  End of file
@-----------------------------------------------------------------------------------------

    .end
