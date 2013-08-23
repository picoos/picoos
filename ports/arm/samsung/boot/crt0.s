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

	.extern portSwiHandler
	.extern portCpuIrqWrapper

	.global start

	.text 
	.code 32 

	.align 	0

/*
 * S3C2510 registers
 */

	.equ	ASIC_BASE,   	0xf0000000
	.equ	CLKCON,      	ASIC_BASE+0x0008
	.equ	PCLKDIS,     	ASIC_BASE+0x000c    

	.equ	TIC,		ASIC_BASE+0x040004

	.equ	INTMOD,      	ASIC_BASE+0x140000
	.equ	EXTMOD,      	ASIC_BASE+0x140004
	.equ	INTMASK,     	ASIC_BASE+0x140008
	.equ	EXTMASK,     	ASIC_BASE+0x14000C
	.equ	INTOFFSET_IRQ,  ASIC_BASE+0x14001C

/*
 * This is the point where control is passed when
 * CPU starts up.
 */

start: 
_start:
_mainCRTStartup:

    	ldr	r1, =CLKCON 
    	ldr	r0, =0x0    
    	str	r0, [r1]

    	ldr	r1, =PCLKDIS
    	ldr	r0, =0x0
    	str	r0, [r1]

/*
 * interrupt mode
 */
    	ldr	r0, =INTMOD         
    	mov     r3, #0x0                
    	str     r3, [r0]        

    	ldr	r0, =EXTMOD         
    	mov     r3, #0x0                
    	str     r3, [r0]        

/*
 * disable external interrupt source
 */
    	ldr     r0, =0x8000003f         
    	ldr	r1, =EXTMASK            
    	str     r0, [r1]                    

/*
 *  disable internal interrupt source
 */
    	ldr     r0, =0xffffffff         
    	ldr	r1, =INTMASK            
    	str     r0, [r1]    

	bl	initStackDataBss
	
/*
 * Set up arguments to main and call.
 */

	mov		r0, #0		/*  no arguments  */ 
	mov		r1, #0		/* no argv either */

	bl		main 

/*
 * Endless loop for unhandled exceptions.
 */

endless_loop:
	b		endless_loop

/*
 * Exception / interrupt table.
 */

.section .startup,"ax" 
	.code 32 
	.align 0

vectorTable:

	b	start
	b	endless_loop /* undefined instr */
	b	portSwiHandler
	b	endless_loop /* pabt */
	b	endless_loop /* dabt */
	nop
	b	portCpuIrqHandlerWrapper /* irq */
	b	endless_loop /* fiq */
