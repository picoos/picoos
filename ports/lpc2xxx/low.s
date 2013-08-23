/*
 * Copyright (c) 2011-2013, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Startup code and interrupt handler wrappers. Idea is to
 * provide layer a little like cortex-m hardware does, ie. handle
 * saving of registers on interrupt entry and restoration on interrupt
 * exit here. Actual interrupt handler logic is in C-code.
 */

  .extern Reset_Handler
  .extern SWI_Handler
  .global portRestoreContextImpl
  .global portEnterCritical
  .global portExitCritical

  .set  VICVectAddr, 0xFFFFF030

  .text
  .code 32

  .align   0

/*
 * Arm CPSR bits.
 */
      .set  INTR_MASK, 0xC0    @ disable IRQ & FIQ

      .set  MODE_UDF, 0x1B    @ Undefine Mode(UDF)
      .set  MODE_ABT, 0x17    @ Abort Mode(ABT)
      .set  MODE_SVC, 0x13         @ Supervisor Mode (SVC)
      .set  MODE_IRQ, 0x12         @ Interrupt Mode (IRQ)
      .set  MODE_FIQ, 0x11         @ Fast Interrupt Mode (FIQ)
      .set  MODE_SYS, 0x1F

      .equ  I_BIT, 0x80             @ when I bit is set, IRQ is disabled
      .equ  F_BIT, 0x40             @ when F bit is set, FIQ is disabled
/* 
 * Save task context to stack of current task.
 * The layout is mostly dictated by stmfd/ldmfd instructions.
 * If this is modified, changes are required to restoreContext
 * and p_pos_initTask also. Current layout is
 * PC LR SP R12-R0 SPSR. System mode SP is left into R0, which
 * allows C code to do further processing.
 */

.macro  portSaveContext

  ldr     sp, =__stack          @ Empty IRQ/SVC stack
  stmfd   sp, {sp}^             @ Get user/system mode SP (task stack)
  ldr     sp, [sp, #-4]         @   into IRQ/SVC SP.

  stmfd   sp!, {lr}             @ 1) Push LR as return address
  stmfd   sp, {r0-r12,r14}^     @ 2) Save all system/user mode regs
  sub     sp, sp, #14*4
  mrs     r0, spsr              @ 3) Status reg
  stmfd   sp!, {r0}

  ldr     r0, =posCurrentTask_g @ Get POSTASK pointer
  ldr     r1, [r0]
  str     sp, [r1]              @ Save task SP

  ldr     sp, =__stack          @ Empty IRQ/SVC stack again for
                                @ interrupt handling
.endm

/* 
 * Restore task context from stack of current task.
 * Just helper to jump to common restore routine.
 */

.macro  portRestoreContext

  b       portRestoreContextImpl

.endm

/* 
 * Restore task context from stack of current task.
 */
portRestoreContextImpl:

  ldr     r0, =posCurrentTask_g   @ Get POSTASK pointer
  ldr     r1, [r0]
  ldr     r2, [r1]                @ Get task SP into R2

  add     r3, r2, #16*4           @ Calculate task SP value after task context frame is removed
  stmfd   sp!, {r3}               @ Move SP value to User/Sys SP
  ldmfd   sp, {sp}^
  nop

  mov     sp, r2                  @ Switch IRQ/SVC SP to Task SP also

  ldmfd   sp!, {r0}               @ 3) Restore status reg
  msr     spsr, r0
  ldmfd   sp, {r0-r12,r14}^       @ 2) User mode regs
  nop

  add     sp, sp, #14*4
  ldmfd   sp, {pc}^               @ 1) return address, jump and switch mode

/*
 * Setup SVC stack pointer and jump to Reset_Handler.
 */

Reset_Wrapper:

/*
 * Initialize stacks in unused modes to 0.
 * IRQ stack is initialized later.
 */
  msr     CPSR_c, #MODE_UDF|I_BIT|F_BIT @ Undefined Instruction Mode
  mov     sp, #0

  msr     CPSR_c, #MODE_ABT|I_BIT|F_BIT @ Abort Mode
  mov     sp, #0

  msr     CPSR_c, #MODE_FIQ|I_BIT|F_BIT @ FIQ Mode
  mov     sp, #0

  msr     CPSR_c, #MODE_IRQ|I_BIT|F_BIT @ IRQ Mode
  mov     sp, #0

  msr     CPSR_c, #MODE_SYS|I_BIT|F_BIT @ System Mode
  mov     sp, #0

/*
 * SVC stack is used during startup so it must be initialized.
 */

  msr     cpsr_c, #MODE_SVC|I_BIT|F_BIT;
  ldr     sp, =__stack

/*
 * Ready, now jump to cortex-m style Reset_Handler.
 */
  bl      Reset_Handler

/*
 * Returning from main in this environment is really an error.
 * Go into a dead loop. Used also as implementation for
 * those vectors that don't have any implementation.
 */

endlessLoop:
  b  endlessLoop

/*
 * Wrap SWI instruction (system call).
 */

SWI_Wrapper:
  portSaveContext
  bl      SWI_Handler
  portRestoreContext


/* 
 * Wrapper for IRQ exceptions. It saves the current context, loads interrupt handler
 * address from VIC and calls actual IRQ handler.
 */
IRQ_Wrapper:
  sub     lr, lr, #4

  portSaveContext

  ldr     r1, =VICVectAddr
  ldr     lr, =portRestoreContextImpl
  mov     r0, #0
  ldr     r1, [r1]
  bx      r1

/*
 * Block all interrupts.
 */
portEnterCritical:
        mrs     r0, CPSR
        orr     r1, r0, #INTR_MASK
        msr     CPSR_c, r1
        bx  lr

/*
 * Restore interrupts.
 */
portExitCritical:
        msr     CPSR_c, r0
        bx  lr

/*
 * Exception / interrupt table.
 */
.section .startup,"ax" 
  .code 32
  .align 0

vectorTable:

  b  Reset_Wrapper
  b  endlessLoop   /* undefined instr   */
  b  SWI_Wrapper
  b  endlessLoop   /* pabt              */
  b  endlessLoop   /* dabt              */
  nop
  b  IRQ_Wrapper   /* irq               */
  b  endlessLoop   /* fiq               */
