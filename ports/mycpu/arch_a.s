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
; CVS-ID $Id: arch_a.s,v 1.5 2008/05/18 09:03:48 dkuschel Exp $
;


; ---------------------------------------------------------------
; imports / exports and setup
; ---------------------------------------------------------------

    .debuginfo  on

    .export     _p_pos_findbit, _setupStack, _setupZeropage
    .export     _p_pos_startFirstContext
    .export     _p_pos_intContextSwitch
    .export     _p_pos_softContextSwitch
    .export     _p_pos_lock, _p_pos_unlock
    .export     _saveContext, _pPrintErr, _getCurrentOS
    .export     doKernelLock, doKernelUnlock
    .export     doRtlibLock, doRtlibUnlock
    .export     _dropZSPage, _useZSPage, _freeAllZSPages
    .export     _getDataStackPtr, _p_pos_initArch
    .export     _testForCTRLC, _handleCtrlC, _p_putchar
    .import     _posTaskExit
    .import     _posSemaCreate, _p_pos_sleep
    .import     _posSemaSignal, _p_pos_semaGet
    .import     kernelLock, kernelUnlock
    .import     rtlibLock, rtlibUnlock
    .import     sleephook, tsrHook, doneLibHook
    .import     __dint, __eint, __eintl
    .import     k_multiplex, checkCtrlC, _exit
    .import     k_spinlock, k_spinunlock, k_setIdleFunc
    .import     k_getContext, k_setContext, k_restoreContext
    .import     k_allocFreeZSpage, k_allocFreeRamPage
    .import     _installTimer, _timerInterruptHandler
    .import     k_prchar, incsp6, popa, popax
    .import     _posCurrentTask_g, _posNextTask_g
    .import     __errno, _inInterruptFlag_g
    .import     kernelLocked, pusha
    .import     ctrlcHandler, crt0brkFlag
    .importzp   sp, ptr1

    zpspace = 26   ; copied from asminc/zeropage.inc
    KERN_TESTCTRLC = $0240  ;check for control+C



; ---------------------------------------------------------------
; types
; ---------------------------------------------------------------

.struct TASKDATA
    stackptr    .byte
    stackpage   .byte
    serrno      .word
    zeropage    .byte
    datastack   .word
    savedzp     .byte 26
.endstruct



; ---------------------------------------------------------------
; data
; ---------------------------------------------------------------

.data

rampage:  .res 1
bitval:   .byte   $01,$02,$04,$08,$10,$20,$40,$80

.bss

temp:     .res 8
pages:    .res 16  ;bit-table of used zero/stack-pages
memstrp:  .res 2   ;ptr to memory info structure of TSR
hkernel:  .res 2   ;handle to task that executes the kernel
exitflag: .res 1   ;set when kernel task shall terminate the program
donelibf: .res 2   ;pointer to donelib function
inIdleHk: .res 1   ;flag: nonzero while the MyCPU is in the idle hook
ctrlC:    .res 1   ;nonzero when ctrl+C was pressed

rtsem:    .res 2   ;runtime library locking semaphore
rtactr:   .res 1   ;how often a task has got the rtlib lock
rtwctr:   .res 1   ;how many tasks are waiting for the rtlib lock
rttask:   .res 2   ;handle of the task that has the rtlib lock

ksem:     .res 2   ;kernel locking semaphore
kactr:    .res 1   ;how often a task has got the kernel lock
kwctr:    .res 1   ;how many tasks are waiting for the kernel lock
ktask:    .res 2   ;handle of the task that has the kernel lock

.code


; ---------------------------------------------------------------
; char getCurrentOS( void );
; ---------------------------------------------------------------

.proc _getCurrentOS

    ;get the ID of the currently running Operating System
    lda     #4
    jsr     k_multiplex
    txa
    rts
    
.endproc



; ---------------------------------------------------------------
; void p_pos_initArch( void );
; ---------------------------------------------------------------

.proc _p_pos_initArch

    ;Test if an other OS is running. If so, exit now.
    jsr     _getCurrentOS
    jnz     _exit
    rts

.endproc



; ---------------------------------------------------------------
; unsigned char setupZeropage(
;                  unsigned char page, void *datastack );
; ---------------------------------------------------------------

.proc _setupZeropage

    txy
    tax
    spt     temp
    jsr     popa

    ;map zero page
    ldx     $3A00  ;REG_ZEROPAGE
    phx
    sta     $3A00

    lpt     temp
    spt     sp

    ;restore original zero page
    rbk     $3A00

    rts

.endproc



; ---------------------------------------------------------------
; unsigned char setupStack(
;                  unsigned char page, unsigned char zeropage,
;                  void (*exitfunc)(void),
;                  POSTASKFUNC_t funcptr, void *funcarg );
; ---------------------------------------------------------------

.proc _setupStack

    sta     temp+6
    stx     temp+7

    ldy     #5
    clx
L1: lda     (sp),y
    sta     temp,x
    inx
    dey
    jnv     L1

    ;map stack page to zero page area
    lda     $3A00  ;REG_ZEROPAGE
    pha
    lda     temp
    sta     $3A00

    ;ptr to exit function
    lda     temp+2
    ldx     temp+3
    jnz     L2
    dec
L2: dex
    sta     $FF
    stx     $FE
    
    ;flags
    stz     $FD

    ;ptr to task function
    lda     temp+4
    ldx     temp+5
    sta     $FC
    stx     $FB

    ;registers (function argument in AX)
    lda     temp+6
    ldx     temp+7
    sta     $FA     ;A
    stx     $F9     ;X

    ;REG_RAMPAGE
    lda     $3800   ;REG_RAMPAGE
    sta     $F7

    ;REG_ROMPAGE
    lda     $3900   ;REG_ROMPAGE
    sta     $F6

    ;REG_ZEROPAGE
    lda     temp+1
    sta     $F5

    ;restore original zero page
    rbk     $3A00
    
    lda     #$F4
    clx
    jmp     incsp6

.endproc



; ---------------------------------------------------------------
; void saveContext(void)
; ---------------------------------------------------------------

.proc _saveContext

    jsr     k_getContext
    txa
    phy

    ;save context (stack pointer and stack page)
    lpt     _posCurrentTask_g
    spa
    pla
    spa

    ;save errno
    lda     __errno
    spa
    lda     __errno+1
    spa

    rts

.endproc



; ---------------------------------------------------------------
; void testForCTRLC(void)
; ---------------------------------------------------------------

.proc _testForCTRLC

    lda     kernelLocked
    jnz     L1
    jsr     k_spinlock
    jsr     (KERN_TESTCTRLC)
    php
    jsr     k_spinunlock
    plp
    jnc     L1
    inc     ctrlC
L1: rts

.endproc



; ---------------------------------------------------------------
; void _handleCtrlC(UVAR_t arg)
; ---------------------------------------------------------------

.proc _handleCtrlC

    lda     ctrlC
    jpz     L1
    lda     _inInterruptFlag_g
    ora     inIdleHk
    jnz     L1  ;called from wrong task, delay execution of ctrl+C
    lda     ctrlcHandler
    ora     ctrlcHandler+1
    jpz     L2
    jmp     (ctrlcHandler)
L2: inc     crt0brkFlag
    jmp     _exit
L1: rts

.endproc



; ---------------------------------------------------------------
; void p_pos_startFirstContext(void)
; ---------------------------------------------------------------

.proc _p_pos_startFirstContext

    ;set pico]OS as the OS that is running
    ldx     #$D5    ;flag for pico]OS
    ldy     $3900   ;REG_ROMPAGE
    lda     #5
    jsr     k_multiplex

    ;install hooks for cc65 runtime lib
    jsr     installHooks
    
    ;disable interrupts in kernel
    jsr     __dint

    ;install the timer interrupt
    lpt     #_timerInterruptHandler
    txa
    tyx
    jsr     _installTimer

    ;simulate an context switch from interrupt level
    jsr     _p_pos_intContextSwitch

    ;enable interrupts in kernel but do not do "sei"
    jsr     __eintl

    ;Start the first task. This will also do the "sei"
    jmp     k_restoreContext

.endproc



; ---------------------------------------------------------------
; void p_pos_intContextSwitch(void)
; ---------------------------------------------------------------

.proc _p_pos_intContextSwitch

    lpt     _posNextTask_g
    spt     _posCurrentTask_g

    ;load context (stack pointer and stack page)
    lpa
    pha
    lpa
    pha

    ;load errno
    lpa
    sta     __errno
    lpa
    sta     __errno+1

    ;set new context
    ply
    plx
    jmp     k_setContext

.endproc



; ---------------------------------------------------------------
; void p_pos_softContextSwitch(void)
; ---------------------------------------------------------------

.proc _p_pos_softContextSwitch

    cli     ;simulate interrupt

    ;save current context (stack pointer and stack page)
    tsx
    txa
    sec
    sbc     #9-2  ;context requires 9 bytes on stack, PC is already on stack (2 bytes)
    lpt     _posCurrentTask_g
    spa
    lda     $3B00   ;REG_STACKPAGE
    spa

    ;save errno
    lda     __errno
    spa
    lda     __errno+1
    spa

    ;get program address from stack
    ply
    plx
    iny
    jnz     L2
    inx
L2:
    ;push flags
    php

    ;push program address to stack
    phx
    phy

    ;push registers
    phr

    ;push pages
    lda     $3800   ;REG_RAMPAGE
    pha
    lda     $3900   ;REG_ROMPAGE
    pha
    lda     $3A00   ;REG_ZEROPAGE
    pha

    jsr     _p_pos_intContextSwitch
    jmp     k_restoreContext

.endproc



; ---------------------------------------------------------------
; UVAR_t p_pos_findbit (const UVAR_t bitfield, UVAR_t rrOffset)
; ---------------------------------------------------------------

.proc _p_pos_findbit

    tax
    lda     (sp)
    ;accu = bitfield
    ;x = offset

    stx     ptr1   ;store to temp. memory
    inx
    clc

offsetLoop:
    ror
    dxjp    offsetLoop

    ldx     ptr1
    jpc     bitFound

nextBitLoop:
    inx
    ror
    jnc     nextBitLoop

bitFound:
    txa
    cmp     #8
    jnc     L1   ;ok, return bit number in Accu

    ;correct the offset
    sbc     #9
    
    ;return
L1: inc     sp
    jnz     L2
    inc     sp+1
L2: rts

.endproc



; ---------------------------------------------------------------
; Zero/Stack-Page recording
; ---------------------------------------------------------------

    ; void dropZSPage( unsigned char page )
_dropZSPage:
    clc
    ska

    ; void useZSPage( unsigned char page )
_useZSPage:
    sec
    jsr     getXYfromA
    lda     bitval,y
    jpc     @L
    eor     #$ff
    and     pages,x
    skc
@L: ora     pages,x
    sta     pages,x
    rts

    ; void freeAllZSPages( void )
_freeAllZSPages:
    cla
@N: pha
    jsr     getXYfromA
    lda     pages,x
    and     bitval,y
    jpz     @L
    pla
    pha
    clc
    jsr     k_allocFreeZSpage
@L: pla
    inc
    jnv     @N
    rts

getXYfromA:
    tax
    and     #7
    tay
    txa
    div     #8
    tax
    rts



; ---------------------------------------------------------------
; Install Hooks (locking / unlocking of resources, exit function)
; ---------------------------------------------------------------

.proc installHooks

    ;allocate semaphores for locking the runtime-lib and the kernel
    cla
    clx
    jsr     _posSemaCreate
    sta     rtsem
    stx     rtsem+1
    txa
    ora     rtsem
L1: jpz     L1
    cla
    clx
    jsr     _posSemaCreate
    sta     ksem
    stx     ksem+1
    txa
    ora     ksem
    jpz     L1

    ;install locking mechanism
    lpt     #doKernelLock
    spt     kernelLock
    lpt     #doKernelUnlock
    spt     kernelUnlock
    lpt     #doRtlibLock
    spt     rtlibLock
    lpt     #doRtlibUnlock
    spt     rtlibUnlock
    lpt     #_p_pos_sleep
    spt     sleephook
    
    ;install idle handler
    lda     $3800  ;REG_RAMPAGE
    sta     rampage
    sec
    lpt     #idlehook
    jsr     k_setIdleFunc
    
    ;install TSR hook
    lpt     #exitAsTsr
    spt     tsrHook
    
    ;install the "library done" hook
    lpt     doneLibHook
    spt     donelibf
    lpt     #stopRTOS
    spt     doneLibHook
    
    ;install Ctrl-C checker
    lpt     #_handleCtrlC
    spt     checkCtrlC

    rts

.endproc


; ---------------------------------------------------------------
; Locking / Unlocking of resources
; ---------------------------------------------------------------

.proc saveZeropage

    lpt     _posCurrentTask_g
    spt     L2+1
    ldy     #TASKDATA::savedzp + zpspace - 1
    ldx     #zpspace
L1: lda     sp-1,x
L2: sta     $ffff,y
    dey
    dxjp    L1
    rts

.endproc


.proc restoreZeropage

    lpt     _posCurrentTask_g
    spt     L1+1
    ldy     #TASKDATA::savedzp + zpspace - 1
    ldx     #zpspace
L1: lda     $ffff,y
L2: sta     sp-1,x
    dey
    dxjp    L1
    rts

.endproc


.proc _p_pos_lock
    jsr     doRtlibLock
    jmp     doKernelLock
.endproc


.proc _p_pos_unlock
    jsr     doKernelUnlock
    jmp     doRtlibUnlock
.endproc


.proc doKernelLock   ;this is a very fast lock (like a mutex)

    php
    phr
    lda     _inInterruptFlag_g
    jnz     L4
    jsr     __dint
    lpt     _posCurrentTask_g
    lda     kactr
    jnz     L1
    spt     ktask
L3: inc     kactr
    jsr     __eint
L4: plr
    plp
    rts

L1: cpx     ktask
    jnz     L2
    cpy     ktask+1
    jpz     L3

L2: inc     kwctr
    jsr     saveZeropage
    jsr     __eint
    sei     ;we want the interrupts to be enabled here!
    lda     ksem
    ldx     ksem+1
    jsr     _p_pos_semaGet
    cli
    jsr     restoreZeropage
    sei
    lpt     _posCurrentTask_g
    spt     ktask
    plr
    plp
    rts

.endproc


.proc doKernelUnlock

    php
    phr
    lda     _inInterruptFlag_g
    jnz     L1
    jsr     __dint
    dec     kactr
    jnz     L2
    lda     kwctr
    jpz     L2
    dec     kwctr
    inc     kactr
    stz     ktask
    stz     ktask+1
    jsr     saveZeropage
    jsr     __eint
    lda     ksem
    ldx     ksem+1
    jsr     _posSemaSignal
    jsr     __dint
    jsr     restoreZeropage
    jsr     __eint
    plr
    plp
    rts

L2: jsr     __eint
L1: plr
    plp
    rts

.endproc


.proc doRtlibLock   ;this is a very fast lock (like a mutex)

    phr
    jsr     __dint
    lpt     _posCurrentTask_g
    lda     rtactr
    jnz     L1
    spt     rttask
L3: inc     rtactr
    jsr     __eint
    plr
    rts

L1: cpx     rttask
    jnz     L2
    cpy     rttask+1
    jpz     L3

L2: inc     rtwctr
    jsr     __eint
    lda     rtsem
    ldx     rtsem+1
    jsr     _p_pos_semaGet
    lpt     _posCurrentTask_g
    spt     rttask
    plr
    rts

.endproc


.proc doRtlibUnlock

    phr
    jsr     __dint
    dec     rtactr
    jnz     L1
    lda     rtwctr
    jpz     L1
    dec     rtwctr
    inc     rtactr
    stz     rttask
    stz     rttask+1
    jsr     __eint
    lda     rtsem
    ldx     rtsem+1
    jsr     _posSemaSignal
    plr
    rts
L1: jsr     __eint
    plr
    rts

.endproc


.proc idlehook

    ;set correct RAMPAGE
    lda     $3800  ;REG_RAMPAGE
    pha
    lda     rampage
    sta     $3800  ;REG_RAMPAGE

    ;test if somebody waits for kernel access
    lda     kwctr
    ora     exitflag
    jpz     L2

    ;do nothing when the idle hook was called from within an interrupt
    lda     _inInterruptFlag_g
    jnz     L2

    ;get posCurrentTask_g->zeropage into accu
    lpt     _posCurrentTask_g
    txa
    clc
    adc     #TASKDATA::zeropage
    tax
    jnc     L1
    iny
L1: lpa

    ;set task zeropage
    ldx     $3A00  ;REG_ZEROPAGE
    phx
    sta     $3A00  ;REG_ZEROPAGE

    ;now we have set up rompage, rampage, zeropage, datastack

    ;unlock the kernel for a short moment!
    inc     inIdleHk
    ldx     kactr
    jpz     L5
    phx
L3: jsr     doKernelUnlock
    dxjp    L3
    
    ;we want the interrupts enabled here!
    sei

    ;test if the program shall be exited (only if we ran as TSR)
    lda     exitflag
    jpz     L6
    lpt     _posCurrentTask_g
    cpx     hkernel
    jnz     L6
    cpy     hkernel+1
    jnz     L6
    plx
    jsr     exitFromTSR
    jmp     L5
L6:
    ;lock the kernel again
    plx
L4: jsr     doKernelLock
    dxjp    L4
    stz     inIdleHk

    ;we want the interrupts enabled here!
    sei

    ;restore pages and return
L5: rbk     $3A00  ;REG_ZEROPAGE
L2: rbk     $3800  ;REG_RAMPAGE
    rts

.endproc



; ---------------------------------------------------------------
; This function is called if the program is exit'ed as TSR.
; ---------------------------------------------------------------

.proc exitAsTsr

    ;save pointer to memory info structure
    spt     memstrp

    ;save the handle of the task that will execute the kernel
    lpt     _posCurrentTask_g
    spt     hkernel

    ;exit with kernel locked
    jmp     (kernelLock)

.endproc



; ---------------------------------------------------------------
; Exit Pico]OS
; ---------------------------------------------------------------

.proc goSingleThreaded

    ;check if this function is called by a "kernel task"
    lda     kactr
    jpz     L1
    lpt     _posCurrentTask_g
    cpx     ktask
    jnz     L1
    cpy     ktask+1
    jpz     L2  ;yes, skip locking because it is already locked

L1: ;ensure that no task is in the runtime library
    jsr     doRtlibLock

    ;ensure that no task is in the kernel
    jsr     doKernelLock

L2: ;disable interrupts
    jsr     k_spinlock

    ;uninstall idle handler
    clc
    lpt     #idlehook
    jsr     k_setIdleFunc

    ;uninstall timer interrupt handler
    cla
    clx
    jsr     _installTimer

    ;uninstall RTOS hooks
    lpt     #_rts
    spt     kernelLock
    spt     kernelUnlock
    spt     rtlibLock
    spt     rtlibUnlock
    spt     sleephook

    ;enable interrupts again
    jmp     k_spinunlock

_rts:
    rts

.endproc


.proc stopRTOS

    jpc     L1  ;Are we running as TSR and need special handling?

    ;check if exit() was called from interrupt or from idle hook
    lda     _inInterruptFlag_g
    ora     inIdleHk
    jnz     L2

    ;switch to single threaded mode
    jsr     goSingleThreaded

    ;call library destructors
    jsr     (donelibf)

    ;clean all common resources
    jsr     _freeAllZSPages

    ;reset pico]OS flag in kernel
    clx
    cly
    lda     #5
    jsr     k_multiplex

    ;return to exit()-handler
    rts

L1: ;This is tricky: We are running as TSR and must silently quit.
    ;The problem is now that only the task that is mainly executing
    ;the kernel code can quit the program. So we pass the work to
    ;that task by setting a flag. After that, we quit this task.

    inc     exitflag
    jmp     _posTaskExit

L2: ;We can not exit because we are at interrupt level or in the idle hook.
    pla     ;remove return address from stack (2 bytes)
    pla
    pla     ;get return value for exit() from stack
    rts     ;this will jump back to the user's program behind the exit()-call

.endproc


.proc exitFromTSR

    ;Note: When we are here, we are the kernel task.

    ;stop the RTOS
    clc
    stz     inIdleHk
    jsr     stopRTOS

    ;clean all common resources
    jsr     _freeAllZSPages

    ;clean all memory that is associated with the TSR
    lpt     memstrp
    lpa
    pha     ;save ROM-Page to stack
    lpa
    lpa     ;get and free RAM-Page
    clc
    jsr     k_allocFreeRamPage
    lpa     ;get and free zero-page
    jpz     L1
    clc
    jsr     k_allocFreeZSpage
L1: lpa     ;get and free stack-page
    jpz     L2
    clc
    jsr     k_allocFreeZSpage
L2: pla     ;get and free ROM-Page
    clc
    jsr     k_allocFreeRamPage  ;Note: This function can also be used
                                ;      to free a ROM-Page.
    ;all done
    rts

.endproc



; ---------------------------------------------------------------
; Minimal Nano-Layer Support
; ---------------------------------------------------------------

.proc _p_putchar

    jsr     k_prchar
    jmp     _handleCtrlC

.endproc



; ---------------------------------------------------------------
; Tool functions
; ---------------------------------------------------------------

_pPrintErr:
    txy
    tax
pPrintErr:
@L2:
    lpa
    jpz     @L1
    phr
    jsr     k_prchar
    plr
    jmp     @L2
@L1:
    rts


_getDataStackPtr:
    lda     sp
    ldx     sp+1
    rts

