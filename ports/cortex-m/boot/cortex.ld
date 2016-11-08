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

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH("arm")

SECTIONS {

  /* Exception vector table at beginning of flash */
  .vectors :
  {
     . = ALIGN(4);
     KEEP(*(.vectors))
     . = ALIGN(4);
  } >flash

  .text :
  {
     . = ALIGN(4);
     *(.text)
     *(.text.*)
     *(.rodata)
     *(.rodata*)
     *(.glue_7)
     *(.glue_7t)
     
     . = ALIGN(4);
     /* preinit data */
     PROVIDE_HIDDEN (__preinit_array_start = .);
     KEEP(*(.preinit_array))
     PROVIDE_HIDDEN (__preinit_array_end = .);

     . = ALIGN(4);
     /* init data */
     PROVIDE_HIDDEN (__init_array_start = .);
     KEEP(*(SORT(.init_array.*)))
     KEEP(*(.init_array))
     PROVIDE_HIDDEN (__init_array_end = .);

     /* skip fini data, we never terminate */
          
     . = ALIGN(4);
     _etext = .;
  }  > flash

  .ARM.extab :
  {
          *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > flash

  __exidx_start = .;
  .ARM.exidx :
  {
          *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > flash
  __exidx_end = .;

  .data   :
  {
     . = ALIGN(4);
     PROVIDE (__data_start = .) ;
    *(.data)
    *(SORT_BY_ALIGNMENT(.data.*))
     . = ALIGN(4);
    *(.gnu.linkonce.d*)
     . = ALIGN(4);
     _edata = . ;
  }  > ram AT > flash

  PROVIDE (__data_load_start = LOADADDR(.data) );
  PROVIDE (__data_size = SIZEOF(.data) );

  .bss   :
  {
     . = ALIGN(4);
     PROVIDE (__bss_start = .) ;
    *(.bss)
    *(SORT_BY_ALIGNMENT(.bss.*))
    *(COMMON)
     . = ALIGN(4);
     PROVIDE (__bss_end = .) ;
     _end = . ;
  }  > ram

  PROVIDE (__bss_size = SIZEOF(.bss) );

  .noinit   :
  {
     . = ALIGN(4);
     PROVIDE (__noinit_start = .) ;
    *(.noinit)
    *(.noinit.*)
    *(COMMON)
     . = ALIGN(4);
     PROVIDE (__noinit_end = .) ;
     _end = . ;
  }  > ram

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }

   PROVIDE (__stack = ORIGIN(ram) + LENGTH(ram));
}
