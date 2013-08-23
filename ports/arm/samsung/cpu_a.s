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
 * CVS-ID $Id: cpu_a.s,v 1.1 2006/04/29 15:32:44 dkuschel Exp $
 */
 
.include "arch_a_macros.h"

	.text
	.code 32
	.align 0
/*
 * S3C2510 registers
 */

	.equ	ASIC_BASE,   	0xf0000000
	.equ	INTOFFSET_IRQ,  ASIC_BASE+0x14001C
	.equ	TIC,		ASIC_BASE+0x040004

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

    	ldr     r1, =INTOFFSET_IRQ          @ Get location of interrupt contrl
    	ldr	r0, [r1]                    @ Get current IRQ status
    	and	r0, r0, #0xff

	bl	portCpuIrqHandler
    	bl 	c_pos_intExit

	portRestoreContext
