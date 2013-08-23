/*
 * Copyright 2003/12/28 Aeolus Development				
 * Modified for pico]OS by Ari Suutari,
 *   modifications Copyright 2006 Ari Suutari, ari@suutari.iki.fi
 *									
 * Freely modifiable and redistributable.  Modify to suit your own needs
 * Please remove Aeolus Development copyright for any significant	
 * modifications or add explanatory notes to explain the mods and	
 * list authour(s).							
 *									
 * THIS SOFTWARE IS PROVIDED BY THE AEOULUS DEVELOPMENT "AS IS" AND ANY	
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE	
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR	
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AEOLUS DEVELOPMENT BE	
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 	
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 	
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.			
 *									
 *  Startup for LPC210X.  Use with associated linker script and newlib.	
 * Developed from source and hints from multiple startup modules 	
 * developed by others for various ARM systems.				
 */

.include "arch_a_macros.h"

	.extern	main 		/*  Usual C startup.			*/
	.extern armSwiHandler
	.extern lpcIrqHandler
/*
 * These are defined in linker script.
 */
	.extern	__bss_beg__	
	.extern	__bss_end__
	.extern __stack_end__
	.extern __data_beg__
	.extern __data_end__
	.extern __data_beg_src__

	.global initStackDataBss

	.comm	__heap_start, 4, 4
	.comm	__heap_end, 4, 4

	.text 
	.code 32 

	.align 	0

/*
 * Initialize runtime environment.
 * Should be called by cpu-specific startup.
 */

initStackDataBss:

/*
 * Initialize stacks, keep interrupts disabled.
 * Stack sizes are defined in linker script, which 
 * must be compatible with this.
 */
	msr	CPSR_c, #MODE_UDF|I_BIT|F_BIT /* Undefined Instruction Mode
	ldr	sp, =__stackInitialUND
	msr	CPSR_c, #MODE_ABT|I_BIT|F_BIT /* Abort Mode */
	ldr	sp, =__stackInitialABT
	msr	CPSR_c, #MODE_FIQ|I_BIT|F_BIT /* FIQ Mode */
	ldr	sp, =__stackInitialFIQ
	msr	CPSR_c, #MODE_IRQ|I_BIT|F_BIT /* IRQ Mode */
	ldr	sp, =__stackInitialIRQ
	msr	CPSR_c, #MODE_SVC|I_BIT|F_BIT /* Supervisor Mode */
	ldr	sp, =__stackInitialSVC
	msr	CPSR_c, #MODE_SYS|I_BIT|F_BIT /* System Mode */
	ldr	sp, =__stackInitialSYS

	msr	cpsr_c, #MODE_SVC|I_BIT|F_BIT;
/*
 * Stack limit reg. I think this is not used with gcc now.
 */
	sub	sl, sp, #512	/* Assumes 512 bytes below sp 		*/
/*
 * Prepare to initialize bss with zeros.
 */
	mov 	a2, #0		/* Fill value 				*/
	mov	fp, a2		/* Null frame pointer 			*/ 
	mov	r7, a2		/* Null frame pointer for Thumb 	*/
	
	ldr 	r1, .LC1	/*  __bss_beg__ set in link script to 	*/
				/* point at beginning of uninitialized	*/
				/* ram.					*/ 
	ldr 	r3, .LC2	/*  __bss_beg__ set in link script to 	*/
				/* point at end of uninitialized ram.	*/ 
	subs 	r3, r3, r1	/*  Subtract two to find length of 	*/
				/* uninitialized ram.			*/ 
	beq	.end_clear_loop	/*  If no uninitialzed ram skip init.	*/

	mov 	r2, #0		/*  Value used to init ram.		*/

.clear_loop:
	strb	r2, [r1], #1	/*  Clear byte at r1, advance to next	*/
	subs	r3, r3, #1	/*  One less to do			*/
	bgt	.clear_loop	/*  If not done go the next.		*/

.end_clear_loop:

/*
 * Copy initialize data to ram.
 */
	ldr	r1, .LC3	/*  __data_beg__ set in link script to 	*/
				/* point at beginning of initialized ram*/
	ldr 	r2, .LC4	/*  __data_beg_src__ set in link script	*/
				/* to point to beginning of flash copy 	*/
				/* of the initial values of initialized	*/
				/* variables.				*/ 
	ldr 	r3, .LC5	/*  __data_end__ set in link script to 	*/
				/* point at end of initialized ram	*/
	subs	r3, r3, r1	/*  Calculate length of area in ram	*/
				/* holding initialzed variables.	*/
	beq	.end_set_loop	/*  If no initialized vars skip init.	*/

.set_loop: 
	ldrb	r4, [r2], #1	/* Read byte from flash (increment ptr),*/
	strb	r4, [r1], #1  	/* store it in ram (increment ptr) and, */
	subs	r3, r3, #1  	/* reduce bytes to copy by 1.		*/
	bgt 	.set_loop	/* Continue until all copied.		*/

.end_set_loop: 

/*
 * Setup up heap.
 */

	ldr 	r0, =__stackInitialSYS
	ldr	r1, =__stackSizeSYS
	sub	r0, r0, r1

	ldr	r1, =__heap_end
	str	r0, [r1]

	ldr	r1, =__bss_end__
	ldr	r0, =__heap_start
	str	r1, [r0]
/*
 * Done, return to cpu-specific startup.
 */
	bx	lr

/*
 * For Thumb, constants must be after the code since only 
 * positive offsets are supported for PC relative addresses.
 */
	
	.align 0
.LC1: 
	.word	__bss_beg__ 
.LC2:   
	.word	__bss_end__
.LC3:
	.word	__data_beg__
.LC4:
	.word	__data_beg_src__
.LC5:
	.word	__data_end__


