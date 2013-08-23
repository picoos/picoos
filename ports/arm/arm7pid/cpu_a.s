/*
 *  Copyright (c) 2004, Jun Li, lj_sourceforge@users.sourceforge.net.
 *  Copyright (c) 2004, Dennis Kuschel.
 *  All rights reserved. 
 * 
 *  Converted to new arm port by Ari Suutari, ari@suutari.iki.fi.
 *  UNTESTED CONVERSION - I HAVE NO HARDWARE TO TEST THIS.
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
 * CVS-ID $Id: cpu_a.s,v 1.2 2006/04/29 15:25:54 dkuschel Exp $
 */
 
.include "arch_a_macros.h"

	.text
	.code 32
	.align 0
/*
 * arm7pid registers
 */

	.equ	IrqStatus,      0xA000000
	.equ	IrqRawStatus,   0xA000004
	.equ	IrqEnable,      0xA000008
	.equ	IrqEnableClear, 0xA00000C
	.equ	IrqSoft,        0xA000010
	.equ	FiqStatus,      0xA000100
	.equ	FiqRawStatus,   0xA000104
	.equ	FiqEnable,      0xA000108
	.equ	FiqEnableClear, 0xA00010C

	.equ	Timer0Load,     0xA800000
	.equ	Timer0Value,    0xA800004
	.equ	Timer0Ctrl,     0xA800008
	.equ	Timer0Clr,      0xA80000C
	.equ	Timer0IrqBMap,  0x10

	.equ	Timer1Load,     0xA800020
	.equ	Timer1Value,    0xA800024
	.equ	Timer1Ctrl,     0xA800028
	.equ	Timer1Clr,      0xA80002C
	.equ	Timer1IrqBMap,  0x20

/* 
 * Declare external & global functions.
 */
	.extern c_pos_intEnter
	.extern c_pos_intExit
	.extern portCpuIrqHandler

	.global	portCpuIrqHandlerWrapper

/* 
 * Handler for IRQ exceptions. It saves the current context, checks
 * for timer interrupt and executs pico]OS calls for it.
 */

portCpuIrqHandlerWrapper:

	sub 	lr, lr, #4
	portSaveContext

    	bl	c_pos_intEnter

    	ldr     r1, =IrqStatus              @ Get location of interrupt contrl
    	ldr	r0, [r1]                    @ Get current IRQ status
	bl	portCpuIrqHandler	

    	bl 	c_pos_intExit

	portRestoreContext
