/*
 *  Copyright (c) 2004-2012, Dennis Kuschel.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: arch_a.s,v 1.1.1.1 2004/02/16 20:11:33 smocz Exp $
 */


/*----------------------------------------------------------------------------
 |
 |  NOTES:
 |
 |    o  This port and pico]OS itself do not support critical interrupts.
 |
 |    o  A running interrupt service routine can not be interrupted again
 |
 |    o  You must set up a decrementer timer interrupt.
 |       This interrupt must execute the handler  p_asm_timerISR.
 |       You may call the function  p_asm_setupTimerIrq  to set up the timer.
 |
 |    o  You may set up the processors interrupt table to do a jump to
 |       p_asm_extISR  to handle external interrupts.
 |
 *--------------------------------------------------------------------------*/


    .file  "arch_a.s"
    .text



/*----------------------------------------------------------------------------
 |  EXPORTS - Functions
 */
    .global  p_asm_getMSR
    .global  p_asm_getR2
    .global  p_asm_getR13
    .global  p_asm_intsOff
    .global  p_asm_intsOn
    .global  p_asm_timerISR
    .global  p_asm_extISR
    .global  p_asm_setupTimerIrq
    .global  p_pos_startFirstContext
    .global  p_pos_softContextSwitch
    .global  p_pos_intContextSwitch
    .global  p_pos_findbit


/*----------------------------------------------------------------------------
 |  IMPORTS - Functions
 */
    .extern  c_pos_intEnter
    .extern  c_pos_intExit
    .extern  c_pos_timerInterrupt
    .extern  c_extIntHandler


/*----------------------------------------------------------------------------
 |  IMPORTS - Variables
 */
    .extern  posCurrentTask_g
    .extern  posNextTask_g
    .extern  posInInterrupt_g
    .extern  posRunning_g




/*----------------------------------------------------------------------------
 |
 |  DEFINITIONS
 |
 *--------------------------------------------------------------------------*/

/* set to 1 to test in timer ISR if pico]OS is running */
#define TIMERISR_TEST_OSRUNNING     0

/* set to 1 to test in external ISR if pico]OS is running */
#define EXTISR_TEST_OSRUNNING       1

/* Set to 1 to enable dedicated stack for ISRs.  Note:
   The corresponding define in the file arch_c.c must also be set. */
#define DEDICATED_ISR_STACK         0


#define DEC         0x016   /* decrementer timer */
#define DECAR       0x036   /* decrementer auto reload write only */
#define TSR         0x150   /* timer status register */
#define TCR         0x154   /* timer control register */
#define TSR_DIS     0x0800  /* TSR bit DIS */
#define TCR_DIE     0x0400  /* TCR bit DIE */
#define TCR_ARE     0x0040  /* TCR bit ARE */


/* Definition of the stackframe
 * for interrupts and context switches
 */
#define SF_SFPTR    ( 0 *4)
#define SF_RESERVED ( 1 *4)
#define SF_UNUSED   ( 2 *4)
#define SF_R0       ( 3 *4)
#define SF_SRR0     ( 4 *4)
#define SF_SRR1     ( 5 *4)
#define SF_CTR      ( 6 *4)
#define SF_XER      ( 7 *4)
#define SF_CR       ( 8 *4)
#define SF_LR       ( 9 *4)
#define SF_R2       (10 *4)
#define SF_R3       (11 *4)
#define SF_R4       (12 *4)
#define SF_R5       (13 *4)
#define SF_R6       (14 *4)
#define SF_R7       (15 *4)
#define SF_R8       (16 *4)
#define SF_R9       (17 *4)
#define SF_R10      (18 *4)
#define SF_R11      (19 *4)
#define SF_R12      (20 *4)
#define SF_R13      (21 *4)
#define SF_R14      (22 *4)
#define SF_R15      (23 *4)
#define SF_R16      (24 *4)
#define SF_R17      (25 *4)
#define SF_R18      (26 *4)
#define SF_R19      (27 *4)
#define SF_R20      (28 *4)
#define SF_R21      (29 *4)
#define SF_R22      (30 *4)
#define SF_R23      (31 *4)
#define SF_R24      (32 *4)
#define SF_R25      (33 *4)
#define SF_R26      (34 *4)
#define SF_R27      (35 *4)
#define SF_R28      (36 *4)
#define SF_R29      (37 *4)
#define SF_R30      (38 *4)
#define SF_R31      (39 *4)
#define SF_SIZE     (40 *4)  /* = 5 x 32 byte */




/*----------------------------------------------------------------------------
 |
 |  MACROS
 |
 *--------------------------------------------------------------------------*/


#define ENDFUNC(fname)  .type fname,@function ; .size fname,.-fname


.macro PUSH_GPRS
    stw     r2,SF_R2(r1)
    stw	    r3,SF_R3(r1)
    stw	    r4,SF_R4(r1)
    stw	    r5,SF_R5(r1)
    stw	    r6,SF_R6(r1)
    stw	    r7,SF_R7(r1)
    stw	    r8,SF_R8(r1)
    stw	    r9,SF_R9(r1)
    stw	    r10,SF_R10(r1)
    stw	    r11,SF_R11(r1)
    stw	    r12,SF_R12(r1)
    stw     r13,SF_R13(r1)
    stw     r14,SF_R14(r1)
    stw     r15,SF_R15(r1)
    stw     r16,SF_R16(r1)
    stw     r17,SF_R17(r1)
    stw     r18,SF_R18(r1)
    stw     r19,SF_R19(r1)
    stw     r20,SF_R20(r1)
    stw     r21,SF_R21(r1)
    stw     r22,SF_R22(r1)
    stw     r23,SF_R23(r1)
    stw     r24,SF_R24(r1)
    stw     r25,SF_R25(r1)
    stw     r26,SF_R26(r1)
    stw     r27,SF_R27(r1)
    stw     r28,SF_R28(r1)
    stw     r29,SF_R29(r1)
    stw     r30,SF_R30(r1)
    stw     r31,SF_R31(r1)
.endm


.macro POP_GPRS
    lwz     r2,SF_R2(r1)
    lwz     r3,SF_R3(r1)
    lwz     r4,SF_R4(r1)
    lwz     r5,SF_R5(r1)
    lwz     r6,SF_R6(r1)
    lwz     r7,SF_R7(r1)
    lwz     r8,SF_R8(r1)
    lwz     r9,SF_R9(r1)
    lwz     r10,SF_R10(r1)
    lwz     r11,SF_R11(r1)
    lwz     r12,SF_R12(r1)
    lwz     r13,SF_R13(r1)
    lwz     r14,SF_R14(r1)
    lwz     r15,SF_R15(r1)
    lwz     r16,SF_R16(r1)
    lwz     r17,SF_R17(r1)
    lwz     r18,SF_R18(r1)
    lwz     r19,SF_R19(r1)
    lwz     r20,SF_R20(r1)
    lwz     r21,SF_R21(r1)
    lwz     r22,SF_R22(r1)
    lwz     r23,SF_R23(r1)
    lwz     r24,SF_R24(r1)
    lwz     r25,SF_R25(r1)
    lwz     r26,SF_R26(r1)
    lwz     r27,SF_R27(r1)
    lwz     r28,SF_R28(r1)
    lwz     r29,SF_R29(r1)
    lwz     r30,SF_R30(r1)
    lwz     r31,SF_R31(r1)
.endm


.macro PUSH_SPRS
    mfspr   r0,SRR0
    stw	    r0,SF_SRR0(r1)
    mfspr   r0,SRR1
    stw	    r0,SF_SRR1(r1)
    mfspr   r0,CTR
    stw	    r0,SF_CTR(r1)
    mfspr   r0,XER
    stw	    r0,SF_XER(r1)
    mfcr    r0
    stw	    r0,SF_CR(r1)
    mfspr   r0,LR
    stw	    r0,SF_LR(r1)
.endm


.macro POP_SPRS
    lwz     r0,SF_SRR0(r1)
    mtspr   SRR0,r0
    lwz     r0,SF_SRR1(r1)
    mtspr   SRR1,r0
    lwz	    r0,SF_CTR(r1)
    mtspr   CTR,r0
    lwz	    r0,SF_XER(r1)
    mtspr   XER,r0
    lwz	    r0,SF_CR(r1)
    mtcrf   255,r0
    lwz	    r0,SF_LR(r1)
    mtspr   LR,r0
.endm


.macro OPEN_STACKFRAME
    stwu    r1,-SF_SIZE(r1)
    stw	    r0,SF_R0(r1)
.endm


.macro CTX_SWITCH
    lwz	    r0,SF_R0(r1)
    addi    r1,r1,SF_SIZE
    rfi
.endm


.macro ISR_ENTER
    OPEN_STACKFRAME
    PUSH_SPRS
    PUSH_GPRS
.endm


.macro ISR_EXIT
    POP_SPRS
    POP_GPRS
    CTX_SWITCH
.endm




/*----------------------------------------------------------------------------
 |
 |  FUNCTIONS:
 |
 |    unsigned int p_asm_getMSR(void);
 |    unsigned int p_asm_getR2 (void);
 |    unsigned int p_asm_getR13(void);
 |
 |    Get content of special purpose registers.
 |
 *--------------------------------------------------------------------------*/

    .align  2
p_asm_getMSR:
    mfmsr   r3
    blr
    ENDFUNC(p_asm_getMSR)

    .align  2
p_asm_getR2:
    ori     r3,r2,0
    blr
    ENDFUNC(p_asm_getR2)

    .align  2
p_asm_getR13:
    ori     r3,r13,0
    blr
    ENDFUNC(p_asm_getR13)




/*----------------------------------------------------------------------------
 |
 |  FUNCTIONS:
 |
 |    unsigned int p_asm_intsOff(void);
 |    void         p_asm_intsOn(unsigned int savedflags);
 |
 |    Enable / Disable Interrupts
 |
 *--------------------------------------------------------------------------*/

    /* Align code to cache line boundary.
       Both functions fit into a single cache line. */
    .align 5

p_asm_intsOff:
    mfmsr   r3
    rlwinm  r4,r3,0,17,15   /* disable external interrupts */
    mtmsr   r4
    isync
    blr
    ENDFUNC(p_asm_intsOff)

p_asm_intsOn:
    mtmsr   r3
    blr
    ENDFUNC(p_asm_intsOn)




/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void  p_asm_setupTimerIrq(unsigned int timebase_ticks);
 |
 |    Setup the decrementer timer.
 |
 |    Example:
 |      Call   p_asm_setupTimerIrq( 400000000 / HZ )   at a 400MHz CPU.
 |
 |    Note:
 |      1)  HZ is a constant defined in poscfg.h
 |      2)  The timer interrupt will be started
 |          in the function  p_pos_startFirstContext
 |
 *--------------------------------------------------------------------------*/

    .align  2
p_asm_setupTimerIrq:

    /* disable global interrupts */
    mfmsr   r7
    rlwinm  r4,r7,0,17,15
    mtmsr   r4
    isync

    /* disable decrementer interrupt and auto-reload */
    mfspr   r4,TCR
    addis   r5,0,(TCR_DIE | TCR_ARE)
    andc    r4,r4,r5
	mtspr	TCR,r4

    /* disable decrementer */
    xor     r6,r6,r6
	mtspr	DEC,r6

    /* clear decrementer interrupt status */
    addis   r5,0,TSR_DIS
    mtspr   TSR,r5

    /* setup decrementer counter registers */
	mtspr	DECAR,r3
    mtspr   DEC,r3

    /* enable auto-reload */
    oris    r4,r4,TCR_ARE
    mtspr   TCR,r4

    /* reenable global interrupts */
    mtmsr   r7

    blr

    ENDFUNC(p_asm_setupTimerIrq)



/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    UVAR_t p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset);
 |
 |    Returns the number of the first set bit.
 |
 |    Note:
 |      This code is optimized, it fits into 3 cache lines.
 |
 *--------------------------------------------------------------------------*/

    .align 5     /* align to even 32 byte boundary */
p_pos_findbit:   /* input:  r3 = bitmask, r4 = offset */

    /* rotate r3 right by r4, store result in r6 */
    subfic      r5,r4,32
    rlwnm       r6,r3,r5,0,31

    /* r5 = 1 */
    addi        r5,0,1

    /* if (r6 & 0x0000FFFF == 0) { r5 |= 16; r6 >>= 16 } */
    rlwinm.     r3,r6,0,16,31
    bne         ..nzero0
    rlwinm      r6,r6,16,16,31
    ori         r5,r5,16
..nzero0:

    /* if (r6 & 0x000000FF == 0) { r5 |= 8; r6 >>= 8 } */
    rlwinm.     r3,r6,0,24,31
    bne         ..nzero1
    rlwinm      r6,r6,24,24,31
    ori         r5,r5,8
..nzero1:

    /* if (r6 & 0x0000000F == 0) { r5 |= 4; r6 >>= 4 } */
    rlwinm.     r3,r6,0,28,31
    bne         ..nzero2
    rlwinm      r6,r6,28,28,31
    ori         r5,r5,4
..nzero2:

    /* if (r6 & 0x00000003 == 0) { r5 |= 2; r6 >>= 2 } */
    rlwinm.     r3,r6,0,30,31
    bne         ..nzero3
    rlwinm      r6,r6,30,30,31
    ori         r5,r5,2
..nzero3:

    /* if (r6 & 0x00000001 != 0) { r5 &= ~1; } */
    rlwinm      r3,r6,0,31,31
    xor         r5,r5,r3

    /* return (r5 + r4) & 31; */
    add         r6,r5,r4
    rlwinm      r3,r6,0,27,31

    blr

    ENDFUNC(p_pos_findbit)




/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void p_pos_startFirstContext(void);
 |
 |    Starts the first task.
 |
 *--------------------------------------------------------------------------*/

    .align  2
p_pos_startFirstContext:

    /* start decrementer timer interrupt: */

    /* 1) clear decrementer interrupt status */
    addis   r3,0,TSR_DIS
    mtspr   TSR,r3

    /* 2) enable decrementer interrupt */
    mfspr   r4,TCR
    oris    r4,r4,TCR_DIE
	mtspr	TCR,r4 

    /* get stackptr:  r1 = posCurrentTask_g->stackptr */
    addis   r3,0,posCurrentTask_g@h
    ori     r3,r3,posCurrentTask_g@l
    lwzx    r4,0,r3
    lwzx    r1,0,r4

    /* pop SPRs and GPRs from stack and return from interrupt */
    ISR_EXIT

    ENDFUNC(p_pos_startFirstContext)

    /* align following code to cache line boundary,
       since the last function is used seldom */
    .align  5




/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void p_pos_softContextSwitch(void);
 |
 |    Does a context switch from application level.
 |
 *--------------------------------------------------------------------------*/

    .align  2
p_pos_softContextSwitch:

    /* open new stack frame, and store R0 */
    OPEN_STACKFRAME

    /* push SPRs to the stack, simulate an interrupt */
    mfspr   r0,LR
    stw	    r0,SF_SRR0(r1)
    mfmsr   r0
    stw	    r0,SF_SRR1(r1)
    mfspr   r0,CTR
    stw	    r0,SF_CTR(r1)
    mfspr   r0,XER
    stw	    r0,SF_XER(r1)
    mfcr    r0
    stw	    r0,SF_CR(r1)

    /* push GPRs to the stack */
    PUSH_GPRS

    /* swap task environment and stack */

    /* r3 = &posCurrentTask_g; r4 = posCurrentTask_g */
    addis   r3,0,posCurrentTask_g@h
    ori     r3,r3,posCurrentTask_g@l
    lwzx    r4,0,r3

    /* store stackptr:  posCurrentTask_g->stackptr = r1 */
    stwx    r1,0,r4

    /* r5 = &posNextTask_g; r6 = posNextTask_g */
    addis   r5,0,posNextTask_g@h
    ori     r5,r5,posNextTask_g@l
    lwzx    r6,0,r5

    /* posCurrentTask_g = r6 */
    stwx    r6,0,r3

    /* get stackptr:  r1 = posNextTask_g->stackptr */
    lwzx    r1,0,r6

    /* pop SPRs and GPRs from stack and return from interrupt */
    ISR_EXIT

    ENDFUNC(p_pos_softContextSwitch)




/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void p_pos_intContextSwitch(void);
 |
 |    Does a context switch from interrupt level.
 |
 *--------------------------------------------------------------------------*/

    .align  2
p_pos_intContextSwitch:

    /* r6 = posNextTask_g */
    addis   r5,0,posNextTask_g@h
    ori     r5,r5,posNextTask_g@l
    lwzx    r6,0,r5

    /* posCurrentTask_g = r6 */
    addis   r4,0,posCurrentTask_g@h
    ori     r4,r4,posCurrentTask_g@l
    stwx    r6,0,r4

    /* get stackptr:  r1 = posNextTask_g->stackptr */
    lwzx    r1,0,r6

    /* pop SPRs and GPRs from stack and return from interrupt */
    ISR_EXIT

    ENDFUNC(p_pos_intContextSwitch)




/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void p_asm_timerISR(void);
 |
 |    Timer interrupt service routine.
 |    You must set up a decrementer timer interrupt with a tick rate
 |    of 1000 Hz. This function must be called (via direct jump)
 |    when the decrementer signals an interrupt.
 |
 *--------------------------------------------------------------------------*/

    .align      2
p_asm_timerISR:

    /* open new stack frame, push SPRs and GPRs to the stack */
    ISR_ENTER

    /* clear interrupting device */
    addis   r3,0,TSR_DIS
    mtspr   TSR,r3
      
    /* test if pico]OS is ready to accept timer ticks */
#if (TIMERISR_TEST_OSRUNNING != 0)
    addis   r4,0,posRunning_g@h
    ori     r4,r4,posRunning_g@l
    lwzx    r3,0,r4
    cmpi    CR0,0,r3,0
    beq     ..osnotrdy1
#endif

    /* store stack pointer when posInInterrupt_g == 0 */
#if (POSCFG_ISR_INTERRUPTABLE != 0)
    addis   r5,0,posInInterrupt_g@h
    ori     r5,r5,posInInterrupt_g@l
    lwzx    r3,0,r5
    cmpi    CR0,0,r3,0
    bne     ..iinoz1
#endif

    /* store stackptr:  posCurrentTask_g->stackptr = r1 */
    addis   r4,0,posCurrentTask_g@h
    ori     r4,r4,posCurrentTask_g@l
    lwzx    r5,0,r4
    stwx    r1,0,r5

    /* set stack pointer to dedicated ISR stack memory */
#if (DEDICATED_ISR_STACK != 0)
    .extern  isrstack_g
    addis   r3,0,isrstack_g@h
    ori     r3,r3,isrstack_g@l
    lwzx    r1,0,r3
#endif

..iinoz1:

    /* call pico]OS timer interrupt handler */
    bl      c_pos_intEnter
    bl      c_pos_timerInterrupt
    bl      c_pos_intExit  /* Note: This funcion call may not return. */

    /* get old stack pointer back */
#if (DEDICATED_ISR_STACK != 0)
    addis   r4,0,posCurrentTask_g@h
    ori     r4,r4,posCurrentTask_g@l
    lwzx    r5,0,r4
    lwzx    r1,0,r5
#endif

..osnotrdy1:

    /* pop SPRs and GPRs from stack and return from interrupt */
    ISR_EXIT

    ENDFUNC(p_asm_timerISR)




/*----------------------------------------------------------------------------
 |
 |  FUNCTION:
 |
 |    void p_asm_extISR(void);
 |
 |    External interrupt service routine, must be called by a direct jump.
 |    This function handles level and edge triggered external interrupts.
 |    When an interrupt happens, this function does a call to the C-function
 |    c_extIntHandler.   (Note that a default handler is in arch_c.c)
 |
 |      void c_extIntHandler(unsigned int uic0s, unsigned int uics1s);
 |
 |      Parameters: uic0s and uic1s contain the status bits of the UICs.
 |
 *--------------------------------------------------------------------------*/

    .align      2
p_asm_extISR:

    /* open new stack frame, push SPRs and GPRs to the stack */
    ISR_ENTER

    /* test if pico]OS is ready to accept interrupts */
#if (EXTISR_TEST_OSRUNNING != 0)
    addis   r4,0,posRunning_g@h
    ori     r4,r4,posRunning_g@l
    lwzx    r3,0,r4
    cmpi    CR0,0,r3,0
    beq     ..osnotrdy2
#endif

    /* store stack pointer when posInInterrupt_g == 0 */
#if (POSCFG_ISR_INTERRUPTABLE != 0)
    addis   r5,0,posInInterrupt_g@h
    ori     r5,r5,posInInterrupt_g@l
    lwzx    r3,0,r5
    cmpi    CR0,0,r3,0
    bne     ..iinoz2
#endif

    /* store stackptr:  posCurrentTask_g->stackptr = r1 */
    addis   r4,0,posCurrentTask_g@h
    ori     r4,r4,posCurrentTask_g@l
    lwzx    r5,0,r4
    stwx    r1,0,r5

    /* set stack pointer to dedicated ISR stack memory */
#if (DEDICATED_ISR_STACK != 0)
    addis   r3,0,isrstack_g@h
    ori     r3,r3,isrstack_g@l
    lwzx    r1,0,r3
#endif

..iinoz2:

    /* set old MSR with external interrupts disabled */
    mfspr   r4,SRR1
    rlwinm  r4,r4,0,17,15
    mtmsr   r4

    /* enter pico]OS interrupt level */
    bl      c_pos_intEnter

    /* load interrupt trigger type masks to regs r25 (UIC0) and r26 (UIC1) */
	mfdcr   r25,0xC5  /* UIC0_TR */
	mfdcr   r26,0xD5  /* UIC1_TR */

    /* get signaled interrupts from UIC0 and UIC1: r3 (UIC0) and r4 (UIC1) */
	mfdcr   r3,0xC6   /* UIC0_MSR */
    mfdcr   r4,0xD6   /* UIC1_MSR */

    /* clear edge-triggered interrupts from UIC1, then from UIC0 */
    and     r27,r26,r4
    mtdcr   0xD0,r27  /* UIC1_SR */
    and     r28,r25,r3
    mtdcr   0xC0,r28  /* UIC0_SR */

    /* get masks of level-triggered interrupts: r27 (UIC0) and r28 (UIC1) */
    andc    r27,r3,r25
    andc    r28,r4,r26
    /* Note: registers r27 and r28 are volatile, that means,
       C functions that need this registers by itself will save
       the old register content to the stack. */

    /* Call external interrupt handler. The parameters
       uic0s (=r3) and uic1s (=r4) are passed to the function. */
    bl      c_extIntHandler

    /* clear level-triggered interrupts from UIC1 and UIC0 */
    mtdcr   0xD0,r28  /* UIC1_SR */
    mtdcr   0xC0,r27  /* UIC0_SR */

    /* exit pico]OS interrupt level */
    bl      c_pos_intExit  /* Note: This funcion call may not return. */

    /* get old stack pointer back */
#if (DEDICATED_ISR_STACK != 0)
    addis   r4,0,posCurrentTask_g@h
    ori     r4,r4,posCurrentTask_g@l
    lwzx    r5,0,r4
    lwzx    r1,0,r5
#endif

..intex2:

    /* pop SPRs and GPRs from stack and return from interrupt */
    ISR_EXIT

..osnotrdy2:

    /* clear pending  interrupts from UIC1 and UIC0 */
    mfdcr   r4,0xD6   /* r4 = UIC1_MSR */
    mtdcr   0xD0,r4   /* UIC1_SR = r4  */
	mfdcr   r3,0xC6   /* r3 = UIC0_MSR */
    mtdcr   0xC0,r3   /* UIC0_SR = r3  */

    b       ..intex2

    ENDFUNC(p_asm_extISR)

