/*
 *  Copyright (c) 2006, Ari Suutari, ari@suutari.iki.fi.
 *  Copyright (c) 2004, Jun Li, lj_sourceforge@users.sourceforge.net.
 *  Copyright (c) 2004, Dennis Kuschel.
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
 * CVS-ID $Id: arch_a_macros.h,v 1.1 2006/04/30 10:41:24 dkuschel Exp $
 */
 
/*
 * Arm CPSR bits.
 */
	.set	INTR_MASK, 0xC0		@ disable IRQ & FIQ

	.set	MODE_UDF, 0x1B		@ Undefine Mode(UDF)
	.set	MODE_ABT, 0x17		@ Abort Mode(ABT)
	.set	MODE_SVC, 0x13       	@ Supervisor Mode (SVC)
	.set	MODE_IRQ, 0x12       	@ Interrupt Mode (IRQ)
	.set	MODE_FIQ, 0x11       	@ Fast Interrupt Mode (FIQ)
	.set	MODE_SYS, 0x1F

    	.equ	I_BIT, 0x80             @ when I bit is set, IRQ is disabled
    	.equ	F_BIT, 0x40             @ when F bit is set, FIQ is disabled
/* 
 * Save task context to stack of current task.
 * The layout is mostly dictated by stmfd/ldmfd instructions.
 * If this is modified, changes are required to restoreContext
 * and p_pos_initTask also. Current layout is
 * PC LR SP R12-R0 SPSR SP_irq
 */

.macro	portSaveContext

/*
 * We need a work register to get started. Since
 * there are no free ones available, push R0 to stack
 * so it can be used.
 */
	stmfd	sp!, {r0}		@ Save R0 temporarily

	stmfd	sp, {sp}^		@ Get user/system mode SP
	ldr	r0, [sp, #-4]		@   into R0

	stmfd	r0!, {lr}		@ 1) Push LR as return address
/*
 * Now LR is saved to stack. Use it instead of
 * R0 (pop R0 from stack now)
 */
	mov	lr, r0			@ Use LR as system mode 'SP'
	ldmfd	sp!, {r0}		@ Pop original R0
/*
 * Save most of the registers with one
 * instruction ! 
 */
	stmfd	lr, {r0-r14}^		@ 2) Save all system/user mode regs
	sub	lr, lr, #60

	mrs	r0, spsr		@ 3) Status reg
	stmfd	lr!, {r0}
/*
 * Save exception mode SP to task stack. This might
 * look a bit odd, but it is used with interrupt handler
 * perform context switching via p_pos_intContextSwitch
 * to restore exception mode SP. If no context switching is
 * done in interrupt handler, the value is simply discarded by
 * restoreContext.
 */
	stmfd	lr!, {sp}		@ 4) Save IRQ SP (for p_pos_intContextSwitch)
/* 
 * Now, save the current task SP to task structure.
 */	
	ldr	r0, =posCurrentTask_g	@ Prepare to save SP
	ldr	r1, [r0]

    	str	lr, [r1]		@ Save SP

.endm

/* 
 * Restore task context from stack of current task.
 * For stack layout, see restoreContext macro.
 */

.macro	portRestoreContext

/*
 * First, find out the task stack pointer from task
 * structure.
 */
	ldr	r0, =posCurrentTask_g
	ldr	r1, [r0]
/*
 * Use LR as task's stack pointer during restoration.
 */
    	ldr	lr, [r1]		@ Use LR as system mode 'SP'
/*
 * First value is the exception stack pointer, which can now
 * be discarded. If it was needed, it has already been used.
 */
	add	lr, lr, #4		@ 4) Skip saved IRQ SP
/*
 * Restore status register.
 */
	ldmfd	lr!, {r0}		@ 3) Restore status reg
	msr	spsr, r0
/*
 * Restore most system/user mode registers 
 * with one instruction !
 */
	ldmfd	lr, {r0-r14}^		@ 2) User mode regs
	mov	r0, r0			@ NOP
	add	lr, lr, #60
/*
 * Last, restore return address, switch processor
 * mode and jump to it.
 */
	ldmfd	lr, {pc}^		@ 1) return address

.endm
