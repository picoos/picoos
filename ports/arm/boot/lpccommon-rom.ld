/*
 * Copyright 2003/12/30 Aeolus Development
 * Modified for pico]OS by Ari Suutari.
 *   Modifications copyright 2006 Ari Suutari, ari@suutari.iki.fi
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
 */

/* 
 * Stack sizes. Shoud normally be ok, since tasks
 * have their own stacks.
 */

__stackSizeUND	= 0x4;
__stackSizeABT	= 0x4;
__stackSizeFIQ	= 0x4;
__stackSizeIRQ	= 0x400;
__stackSizeSVC	= 0x400;
__stackSizeSYS	= 0x400;

/*
 * Calculate initial stack pointers.
 */

__stackInitialUND = 0x40000000 + __ramSize - 4;
__stackInitialABT = __stackInitialUND - __stackSizeUND;
__stackInitialFIQ = __stackInitialABT - __stackSizeABT;
__stackInitialIRQ = __stackInitialFIQ - __stackSizeFIQ;
__stackInitialSVC = __stackInitialIRQ - __stackSizeIRQ;
__stackInitialSYS = __stackInitialSVC - __stackSizeSVC;

/*
 * Add "start" as external symbol to force linker to load
 * startup code from pico]OS library.
 */

EXTERN(start);

SECTIONS {
  . = 0;				/*  Start at address 0.		*/

/*
 * Place startup code first.
 */

  startup : { *(.startup)} >flash
/*
 * The program (.text) sections and constant data.
 */

  prog : {
  	*(.text)
	*(.rodata)
	*(.rodata*)
	*(.glue_7)
	*(.glue_7t)
 	} >flash
  __end_of_text__ = .;			/*  Used by startup to find 	*/
					/* initialized vars.		*/

/*
 * Initialized data, located in ram but a copy is placed
 * in flash so it can be used to init the ram on startup.
 */

  .data : { 
	__data_beg__ = .;			/* Used by startup.	*/
	__data_beg_src__ = ABSOLUTE(__end_of_text__);/* Used by startup.*/
 	*(.data)
	__data_end__ = .;			/* Used by startup.	*/
 	} >ram AT>flash

/*
 * Unitialized data, located in ram, no copy in flash needed
 * since startup will zero associated area in RAM.
 */

  .bss : { 
	__bss_beg__ = .;	/*  Used by startup to find start of	*/
				/* unitialized variables.		*/
 	*(.bss)
 	} >ram

/*
 * Align here to ensure that the .bss section occupies space up to
 * _end.  Align after .bss to ensure correct alignment even if the
 * .bss section disappears because there are no input sections.
 */

   . = ALIGN(32 / 8);
  }

. = ALIGN(32 / 8);

/*
 * Used by startup to find end of unitialized variables.
 */

_end = .;
_bss_end__ = . ; __bss_end__ = . ; __end__ = . ;	
PROVIDE (end = .);

