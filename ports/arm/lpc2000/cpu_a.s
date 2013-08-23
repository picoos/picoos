/*
 *  Copyright (c) 2006, Ari Suutari, ari@suutari.iki.fi.
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
 * CVS-ID $Id: cpu_a.s,v 1.1 2006/04/29 15:30:30 dkuschel Exp $
 */
 
.include "arch_a_macros.h"

/*
 * Philips LPC 200 registers
 */
   	.set	T0IR, 0xE0004000
	.set	VICVectAddr, 0xFFFFF030

	.text
	.code 32
	.align 0
/* 
 * Declare external & global functions.
 */
	.extern	c_pos_timerInterrupt
	.extern c_pos_intEnter
	.extern c_pos_intExit

	.global	portCpuIrqWrapper

/* 
 * Handler for IRQ exceptions. It saves the current context, loads interrupt handler
 * address from VIC and calls the handler surrounded by required pico]OS calls.
 */

portCpuIrqWrapper:

	sub 	lr, lr, #4
	portSaveContext

    	bl	c_pos_intEnter

	ldr	r1, =VICVectAddr
	ldr	lr, =irqReturn
	mov	r0, #0
	ldr	r1, [r1]
	bx	r1

irqReturn:
    	ldr	r1, =VICVectAddr
    	mov	r0, #0
	str	r0, [r1]

    	bl 	c_pos_intExit

	portRestoreContext
