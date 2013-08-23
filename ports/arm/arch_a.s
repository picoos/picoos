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
 * CVS-ID $Id: arch_a.s,v 1.1 2006/04/30 10:41:24 dkuschel Exp $
 */
 
.include "arch_a_macros.h"

	.text
	.code 32
	.align 0
/* 
 * Declare external & global functions.
 */
	.extern	c_pos_timerInterrupt
	.extern c_pos_intEnter
	.extern c_pos_intExit
	.extern posCurrentTask_g
	.extern posNextTask_g
	.extern posInInterrupt_g

	.global p_pos_startFirstContext
	.global p_pos_intContextSwitch
	.global p_pos_softContextSwitch
	.global portEnterCritical
	.global portExitCritical
	.global	portSwiHandler

/*
 * Called by pico]OS to start first task. Task
 * must be prepared by p_pos_initTask before calling this.
 */

p_pos_startFirstContext:
restore:

	portRestoreContext			@ Simply restore context from stack.

/*
 * Called by pico]OS at end of interrupt handler to switch task.
 * Before switching from current to next task it uses
 * current task stack to restore exception mode stack pointer
 * (which was saved by saveContext macro).
 * After switching task pointers the new task's context is simply restored
 * to get it running.
 */

p_pos_intContextSwitch:

	ldr	r0, =posCurrentTask_g

	ldr	r1, [r0]
    	ldr	r1, [r1]		@ r1 == user mode SP
	ldmfd	r1, {sp}		@ restore IRQ mode stack ptr

	ldr	r1, =posNextTask_g
	ldr	r1, [r1]

    	str	r1, [r0]

	b	restore
/*
 * Called by pico]OS to switch tasks when not serving interrupt.
 * Since we run tasks in system/user mode, "swi" instruction is
 * used to generate an exception to get into suitable mode
 * for context switching. 
 *
 * The actual switching is then performed by armSwiHandler.
 */

p_pos_softContextSwitch:

	swi	#0
	bx	lr

/*
 * Handler for "swi" interrupt. Switch task pointers and
 * restore context of new task.
 */

portSwiHandler:

	portSaveContext
	
	ldr	r0, =posCurrentTask_g
	ldr	r1, =posNextTask_g
	ldr	r1, [r1]

    	str	r1, [r0]

	b	restore
/*
 * Block all interrupts. 
 */

portEnterCritical:
        mrs     r0, CPSR
        orr     r1, r0, #INTR_MASK
        msr     CPSR_c, r1
        bx	lr

/* 
 * Restore interrupts.
 */

portExitCritical:
        msr     CPSR_c, r0
        bx	lr

