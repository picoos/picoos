;*
;*  Copyright (c) 2004-2012, Dennis Kuschel.
;*  All rights reserved. 
;*
;*  Redistribution and use in source and binary forms, with or without
;*  modification, are permitted provided that the following conditions
;*  are met:
;*
;*   1. Redistributions of source code must retain the above copyright
;*      notice, this list of conditions and the following disclaimer.
;*   2. Redistributions in binary form must reproduce the above copyright
;*      notice, this list of conditions and the following disclaimer in the
;*      documentation and/or other materials provided with the distribution.
;*   3. The name of the author may not be used to endorse or promote
;*      products derived from this software without specific prior written
;*      permission. 
;*
;*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
;*  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;*  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
;*  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
;*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
;*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
;*  OF THE POSSIBILITY OF SUCH DAMAGE.
;*


;
; This file is originally from the pico]OS realtime operating system
; (http://picoos.sourceforge.net).
;
; CVS-ID $Id: arch_a.s,v 1.4 2005/01/03 16:32:29 dkuschel Exp $
;


; ---------------------------------------------------------------
; configuration
; ---------------------------------------------------------------

.IFDEF POSNANO

;Set this define to nonzero (=1) to enable nano layer keyboard input
.DEFINE  NOS_KEYINPUT  1

;Set this define to nonzero (=1) to enable nano layer display output
.DEFINE  NOS_DISPOUT   1

.ELSE
.DEFINE  NOS_KEYINPUT  0
.DEFINE  NOS_DISPOUT   0
.ENDIF


; ---------------------------------------------------------------
; imports / exports and setup
; ---------------------------------------------------------------

    .autoimport on
    .case       on
    .debuginfo  on
    .importzp   sp, ptr1
    .macpack    longbranch

    .import     _posCurrentTask_g
    .import     _posNextTask_g
    .import     _posInInterrupt_g

    .import     _c_pos_intEnter
    .import     _c_pos_intExit
    .import     _c_pos_timerInterrupt
    .import     incsp1
    .import     incsp2

    .export     _p_get_sp
    .export     _p_clear_cpustack
    .export     _p_pos_startFirstContext
    .export     _p_pos_softContextSwitch
    .export     _p_pos_intContextSwitch
    .export     _p_pos_findbit

.IF NOS_KEYINPUT
    .import     _c_nos_keyinput
    .import     pusha
.ENDIF

.IF NOS_DISPOUT
    .export     _p_putchar
.ENDIF


; local defines

zpsize          = $1A
cstacksize      = $40
snbr_sflag      = $FF
isrstacksize    = 256

KEY_COUNT      	:= $C6
KEY_BUF         := $0277
SCREEN_OUT      := $E716
CURSOR_COLUMN   := $D3
C64IRQVECT      := $0314
_orgC64irqvect  := $004B    ;temp.mem for basic pointers (2 bytes)


; local variables

.segment  "BSS"

;_orgC64irqvect
;    .res    2
_saved_sp:
    .res    2
_tmrdiv:
    .res    1
_doorgirq:
    .res    1
_isrstack:
    .res    256
.IF NOS_DISPOUT
_savedchars:
    .res    2
.ENDIF

; start of source code
.segment  "CODE"



.IF NOS_DISPOUT

; ---------------------------------------------------------------
; void _p_putchar(char c)
; ---------------------------------------------------------------

.proc  _p_putchar
    ldy     #$00
    lda     (sp),y

    cmp     #13
    bne     putc0
    ldx     _savedchars
    sta     _savedchars
    beq     putc2
    lda     #0

putc3:
    sta     _savedchars+1
putc7:
    lda     CURSOR_COLUMN
    sta     _savedchars
    beq     putc4
    lda     #$9D  ;cursor left
    jsr     SCREEN_OUT
    jmp     putc7
putc4:
    ldx     _savedchars+1
    beq     putc2
    lda     #0
    sta     _savedchars+1
    txa

putc0:
    cmp     #10
    bne     putc1
    lda     _savedchars
    bne     putc6
    lda     #$11  ;cursor down
    bne     putc5

putc6:
    lda     #0
    sta     _savedchars
    lda     #$0D  ;CR+LF

putc1:
    ldx     _savedchars
    bne     putc3
putc5:
    jsr     SCREEN_OUT
putc2:
    lda     #1
    jmp     incsp1

.endproc

.ENDIF



; ---------------------------------------------------------------
; unsigned char* p_get_sp (void)
; ---------------------------------------------------------------

.proc  _p_get_sp

    lda     sp
    ldx     sp+1
    rts

.endproc



; ---------------------------------------------------------------
; void p_clear_cpustack (void)
; ---------------------------------------------------------------

.proc  _p_clear_cpustack

    sei
    tsx
    ldy     #$00
ccs00:
    lda     $0101,x
    sta     $0100,y
    iny
    inx
    bne     ccs00
    ldx     #$00
    ldy     #$e0
ccs01:
    lda     $0100,x
    sta     $0100,y
    inx
    iny
    bne     ccs01
    ldx     #$df
    txs
    cli
    rts

.endproc



; ---------------------------------------------------------------
; void p_pos_startFirstContext(void)
; ---------------------------------------------------------------

.proc  _p_pos_startFirstContext

    ; initialize C64 timer intrrupt
    jsr     _p_start_timer

    ; load ptr
    lda     _posCurrentTask_g
    ldx     _posCurrentTask_g+1
    sta     ptr1
    stx     ptr1+1

    ; csp = task->cstackptr
    ldy     #$00
    lda     (ptr1),y
    tax
    txs

    ; dsp = task->dstackptr
    iny
    lda     (ptr1),y
    sta     sp
    iny
    lda     (ptr1),y
    sta     sp+1

    pla
    tay
    pla
    tax
    pla
    rti

.endproc



; ---------------------------------------------------------------
; void p_pos_softContextSwitch(void)
; ---------------------------------------------------------------
;
; Do a context switch from software level.
;
; Steps that are performed:
;   1. save flags and CPU registers to processor stack
;   2. save the context of the current task:
;        - save processor stack pointer
;        - save data stack pointer
;        - save current zeropage environment
;        - save processor stack frame into data stack
;          if the currently running task has no own
;          dedicated processor stack
;   3. switch context variable
;   4. restore context of the next task to execute:
;        - restore processor stack pointer
;        - restore data stack pointer
;        - restore processor stack frame from data stack
;          if the next task has no own processor stack
;        - restore zeropage environment
;   5. restore CPU registers and flags from processor stack
;   6. execute next task
;
; ---------------------------------------------------------------

.proc  _p_pos_softContextSwitch

    php
    pha
    txa
    pha
    tya
    pha

    ; adjust back jump address to be RTI conform
    tsx
    inc     $0105,x
    bne     scs10
    inc     $0106,x
scs10:

    ; -- save context of current task --

    ; save sp
    lda     sp
    ldy     sp+1
    sta     _saved_sp
    sty     _saved_sp+1

    ; get ptr:  sp = task->cstackptr
    lda     _posCurrentTask_g
    ldy     _posCurrentTask_g+1
    sta     sp
    sty     sp+1
    ldy     #$00

    ; store current csp:  *sp = csp
;    tsx  - x has still the value of sp
    txa
    sta     (sp),y

    ; get ptr:  sp = task->dstackptr
    iny

    ; store current dsp:  *sp = dsp
    lda     _saved_sp
    sta     (sp),y
    iny
    lda     _saved_sp+1
    sta     (sp),y

    ; x = task->cstacknbr
    ldy     #$05
    lda     (sp),y
    tax

    ; get ptr:  sp = task->savedzp
    lda     sp
    clc
    adc     #$07
    sta     sp
    bcc     scs00
    inc     sp+1
scs00:

    ; store zero page content
    ldy     #zpsize-3
scs01:
    lda     sp+2,y
    sta     (sp),y
    dey
    bpl     scs01

    ; copy cstack frame to dstack when slow task is detected

    ; test task->cstacknbr
    cpx     #snbr_sflag
    bne     scs02

    ; get ptr to save area on dstack
    ; ptr1 = task->dstackroot - csp
    lda     _posCurrentTask_g
    ldy     _posCurrentTask_g+1
    sta     sp
    sty     sp+1
    ldy     #$04
    lda     (sp),y
    sta     ptr1+1
    dey
    lda     (sp),y
    tsx
    stx     ptr1
    sec
    sbc     ptr1
    sta     ptr1
    bcs     scs03
    dec     ptr1+1
scs03:

    ; calculate count of bytes to copy
;    tsx  - x has still the value of sp
    txa
    tay
    and     #cstacksize - 1
    eor     #cstacksize - 1
    beq     scs02
    tax

    ; copy bytes
scs04:
    lda     $0101,y
    sta     (ptr1),y
    iny
    dex
    bne     scs04

scs02:

    ; proceed with the interrupt code,
    ; since it is the same we would do here.
    jmp     _p_pos_intContextSwitch

.endproc



; ---------------------------------------------------------------
; void p_pos_intContextSwitch(void)
; ---------------------------------------------------------------
;
; Do a context switch from software level.
;
; Steps that are performed:
;   1. switch context variable
;   2. restore context of the next task to execute:
;        - restore processor stack pointer
;        - restore data stack pointer
;        - restore processor stack frame from data stack
;          if the next task has no own processor stack
;        - restore zeropage environment
;   3. restore CPU registers and flags from processor stack
;   4. return from interrupt, execute next task
;
; ---------------------------------------------------------------

.proc  _p_pos_intContextSwitch

    ; -- swap context variable --

    ; posCurrentTask_g = posNextTask_g
    lda     _posNextTask_g
    ldx     _posNextTask_g+1
    sta     _posCurrentTask_g
    stx     _posCurrentTask_g+1

    ; -- restore context of next task --

    ; load  sp = posCurrentTask_g
    sta     sp
    stx     sp+1

    ; load new csp
    ldy     #$00
    lda     (sp),y
    tax
    txs

    ; load new dsp to temp
    iny
    lda     (sp),y
    sta     _saved_sp
    iny
    lda     (sp),y
    sta     _saved_sp+1

    ; copy cstack frame from dstack when slow task is detected

    ; test task->cstacknbr
    ldy     #$05
    lda     (sp),y
    cmp     #snbr_sflag
    bne     ics05

    ; get ptr to save area on dstack
    ; ptr1 = task->dstackroot - csp
    dey
    lda     (sp),y
    sta     ptr1+1
    dey
    lda     (sp),y
;    tsx  - x has still the value of sp
    stx     ptr1
    sec
    sbc     ptr1
    sta     ptr1
    bcs     ics06
    dec     ptr1+1
ics06:

    ; calculate count of bytes to copy
    txa
    tay
    and     #cstacksize - 1
    eor     #cstacksize - 1
    beq     ics05
    tax

    ; copy bytes
ics07:
    lda     (ptr1),y
    sta     $0101,y
    iny
    dex
    bne     ics07

ics05:
    jmp     return_from_irq

.endproc



; ---------------------------------------------------------------
;
; Return from interrupt (finally do a context switch)
;
; This function does:
;   -  restore the zeropage environment
;   -  return from interrupt (=context switch)  OR
;   -  call original C64 timer interrupt and do the ctx switch
;
; Note: 
;   -  _saved_sp must point to original sp
;   -  sp must point to current task environment
;
; ---------------------------------------------------------------

.proc  return_from_irq

    ; get ptr:  sp = task->savedzp
    lda     sp
    clc
    adc     #$07
    sta     sp
    bcc     rfi01
    inc     sp+1
rfi01:

    ; store zero page content
    ldy     #zpsize-3
rfi02:
    lda     (sp),y
    sta     sp+2,y
    dey
    bpl     rfi02

    ; set dsp
    lda     _saved_sp
    ldx     _saved_sp+1
    sta     sp
    stx     sp+1

    ; back to next context
    lda     _doorgirq
    bne     rfi03
    pla
    tay
    pla
    tax
    pla
    rti

    ;C64: call original irq handler if requested
rfi03:
    lda     #$00
    sta     _doorgirq
    jmp     (_orgC64irqvect)

.endproc



; ---------------------------------------------------------------
;
; Timer interrupt service routine
;
; Steps that are performed:
;   1. save flags and CPU registers to processor stack
;   2. if we are not yet in an interrupt (_posInInterrupt_g == 0),
;      we save the context of the current task:
;        - save processor stack pointer
;        - save data stack pointer
;        - save current zeropage environment
;        - save processor stack frame into data stack
;          if the currently running task has no own
;          dedicated processor stack
;   3. call _c_pos_intEnter
;   4. call _c_pos_timerInterrupt
;   5. call original interrupt handler (C64 only)
;   6. call _c_pos_intExit
;   7. restore last context and return from interrupt
;
; ---------------------------------------------------------------

.proc  p_timerisr

;-> This code is left out on a C64, since it is
;   already done by the isr in the kernal
;    pha
;    txa
;    pha
;    tya
;    pha

    ; -- test if we are not in an interrupt --
    ; Note:
    ; On C64 posInInterrupt_g is always 0.
    ; With the CC65 compiler we have a stack problem:
    ; When this ISR is interrupted, the stack will be destroyed.
    ; So we optimize this code for C64 w/ CC65: simply remove it.

;    lda     _posInInterrupt_g
;    bne     tmi02

    ; -- save context of current task --

    ; save sp
    lda     sp
    ldy     sp+1
    sta     _saved_sp
    sty     _saved_sp+1

    ; get ptr:  sp = task->cstackptr
    lda     _posCurrentTask_g
    ldy     _posCurrentTask_g+1
    sta     sp
    sty     sp+1
    ldy     #$00

    ; store current csp:  *sp = csp
    tsx
    txa
    sta     (sp),y

    ; get ptr:  sp = task->dstackptr
    iny

    ; store current dsp:  *sp = dsp
    lda     _saved_sp
    sta     (sp),y
    iny
    lda     _saved_sp+1
    sta     (sp),y

    ; x = task->cstacknbr
    ldy     #$05
    lda     (sp),y
    tax

    ; get ptr:  sp = task->savedzp
    lda     sp
    clc
    adc     #$07
    sta     sp
    bcc     tmi00
    inc     sp+1
tmi00:

    ; store zero page content
    ldy     #zpsize-3
tmi01:
    lda     sp+2,y
    sta     (sp),y
    dey
    bpl     tmi01

    ; copy cstack frame to dstack when slow task is detected

    ; test task->cstacknbr
    cpx     #snbr_sflag
    bne     tmi02

    ; get ptr to save area on dstack
    ; ptr1 = task->dstackroot - csp
    lda     _posCurrentTask_g
    ldy     _posCurrentTask_g+1
    sta     sp
    sty     sp+1
    ldy     #$04
    lda     (sp),y
    sta     ptr1+1
    dey
    lda     (sp),y
    tsx
    stx     ptr1
    sec
    sbc     ptr1
    sta     ptr1
    bcs     tmi03
    dec     ptr1+1
tmi03:

    ; calculate count of bytes to copy
    txa
    tay
    and     #cstacksize - 1
    eor     #cstacksize - 1
    beq     tmi02
    tax

    ; copy bytes
tmi04:
    lda     $0101,y
    sta     (ptr1),y
    iny
    dex
    bne     tmi04

tmi02:

    ; Set up a data stack for the isr.
    ; This is required for the CC65 compiler.
    lda     #<(_isrstack + (isrstacksize - 1))
    ldx     #>(_isrstack + (isrstacksize - 1))
    sta     sp
    stx     sp+1

    ; -- pico]OS interrupt code --

    ; call pico]OS:  c_pos_intEnter()
    jsr     _c_pos_intEnter

    ; get character from keyboard buffer
.IF NOS_KEYINPUT
    lda     KEY_COUNT
    beq     tmi05
    dec     KEY_COUNT
    lda     KEY_BUF
    ldx     KEY_BUF+1
    stx     KEY_BUF
    jsr     pusha
    jsr     _c_nos_keyinput
tmi05:
.ENDIF

    ; call pico]OS:  c_pos_timerInterrupt()
    jsr     _c_pos_timerInterrupt

    ; only C64: remember to call original timer irq handler
    inc     _doorgirq

    ; call pico]OS:  c_pos_intExit()
    jsr     _c_pos_intExit

    ; -- leave interrupt --

    ; we must restore the zeropage environment here

    ; get ptr
    lda     _posCurrentTask_g
    ldx     _posCurrentTask_g+1
    sta     sp
    stx     sp+1

    ; load dsp to temp
    ldy     #$01
    lda     (sp),y
    sta     _saved_sp
    iny
    lda     (sp),y
    sta     _saved_sp+1

    ; return
    jmp     return_from_irq

.endproc



; ---------------------------------------------------------------
;
; Initialize timer interrupt.
;
; This function is special to the C64.
; The timer is executed 60 times per second.
; The ISR will divide it down to 30 timer interrupts per second.
;
; ---------------------------------------------------------------

.proc  _p_start_timer

    ; disable interrupts
    php
    sei

    ; save original interrupt vector
    lda     C64IRQVECT
    ldx     C64IRQVECT+1
    sta     _orgC64irqvect
    stx     _orgC64irqvect+1

    ; set new vector
    lda     #<mytimerisr
    ldx     #>mytimerisr
    sta     C64IRQVECT
    stx     C64IRQVECT+1

    ; clear flag
    lda     #$00
    sta     _doorgirq

.IF NOS_KEYINPUT
    ; clear keyboard buffer
    sta     KEY_COUNT
.ENDIF

    ; enable interrupts and return
    plp
    rts


    ; timer interrupt service routine
mytimerisr:

    ; divide timer ticks by 2
    inc     _tmrdiv
    lda     _tmrdiv
    lsr
    bcc     orgisr

    ; call timer isr 30 times per second
    jmp     p_timerisr

orgisr:
    jmp     (_orgC64irqvect)

.endproc



; ---------------------------------------------------------------
; UVAR_t p_pos_findbit (const UVAR_t bitfield, UVAR_t rrOffset)
; ---------------------------------------------------------------

.proc _p_pos_findbit
    ldy     #$00
    lda     (sp),y
    tax
    iny
    lda     (sp),y
    ;accu = bitfield
    ;x = offset

    stx     ptr1   ;store to temp. memory
    inx
    clc

offsetLoop:
    ror
    dex
    bne     offsetLoop

    ldx     ptr1
    bcs     bitFound

nextBitLoop:
    inx
    ror
    bcc     nextBitLoop

bitFound:
    txa
    cmp     #8
    bcs     correctOffset

    ; return a
    jmp     incsp2
         
correctOffset:  
    sbc     #9
    ; return a
    jmp     incsp2

.endproc
