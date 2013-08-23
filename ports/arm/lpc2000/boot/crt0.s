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
	.extern initStackDataBss

	.global start

	.text 
	.code 32 

	.align 	0

/*
 * This is the point where control is passed when
 * CPU starts up.
 */

start: 
_start:
_mainCRTStartup:

	bl	initStackDataBss
/*
 * Set up arguments to main and call.
 */

	mov		r0, #0		/*  no arguments  */ 
	mov		r1, #0		/* no argv either */

	bl		main 

/*
 * Returning from main in this environment is really an error.
 * Go into a dead loop.
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
	b	portCpuIrqWrapper /* irq */
	b	endless_loop /* fiq */
