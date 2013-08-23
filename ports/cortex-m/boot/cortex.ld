/*
 * Copyright 2006 Ari Suutari, ari@suutari.iki.fi
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

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH("arm")

SECTIONS {

  /* Read-only sections, merged into text segment.  */
  .hash          : { *(.hash)             }
  .dynsym        : { *(.dynsym)           }
  .dynstr        : { *(.dynstr)           }
  .gnu.version   : { *(.gnu.version)      }
  .gnu.version_d   : { *(.gnu.version_d)  }
  .gnu.version_r   : { *(.gnu.version_r)  }
  .rel.init      : { *(.rel.init) }
  .rela.init     : { *(.rela.init) }
  .rel.text      :
    {
      *(.rel.text)
      *(.rel.text.*)
      *(.rel.gnu.linkonce.t*)
    }
  .rela.text     :
    {
      *(.rela.text)
      *(.rela.text.*)
      *(.rela.gnu.linkonce.t*)
    }
  .rel.fini      : { *(.rel.fini) }
  .rela.fini     : { *(.rela.fini) }
  .rel.rodata    :
    {
      *(.rel.rodata)
      *(.rel.rodata.*)
      *(.rel.gnu.linkonce.r*)
    }
  .rela.rodata   :
    {
      *(.rela.rodata)
      *(.rela.rodata.*)
      *(.rela.gnu.linkonce.r*)
    }
  .rel.data      :
    {
      *(.rel.data)
      *(.rel.data.*)
      *(.rel.gnu.linkonce.d*)
    }
  .rela.data     :
    {
      *(.rela.data)
      *(.rela.data.*)
      *(.rela.gnu.linkonce.d*)
    }
  .rel.got       : { *(.rel.got)          }
  .rela.got      : { *(.rela.got)         }
  .rel.bss       : { *(.rel.bss)          }
  .rela.bss      : { *(.rela.bss)         }
  .rel.plt       : { *(.rel.plt)          }
  .rela.plt      : { *(.rela.plt)         }

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
     _etext = .;
  }  > flash

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

  /* Stabs for profiling information*/
  .profiler 0 : { *(.profiler) }

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