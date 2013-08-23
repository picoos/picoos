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


;
; This file is originally from the pico]OS realtime operating system
; (http://picoos.sourceforge.net).
;
; CVS-ID $Id: arch_a.asm,v 1.2 2004/03/21 18:30:57 dkuschel Exp $
;


;-----------------------------------------------------------------------------
;  NOTE:  THIS FILE IS FOR THE BORLAND ASSEMBLER  (BC 3.1)
;-----------------------------------------------------------------------------


;-----------------------------------------------------------------------------
;  EXPORTS

    ;functions
    PUBLIC _p_pos_startFirstContext
    PUBLIC __pos_softCtxSw
    PUBLIC _p_pos_intContextSwitch
    PUBLIC __timerIrq
IFDEF POSNANO
    PUBLIC __keyboardIrq
    PUBLIC _p_putchar
ENDIF


;-----------------------------------------------------------------------------
;  IMPORTS

    ;functions
    EXTRN _c_pos_intEnter:FAR
    EXTRN _c_pos_intExit:FAR
    EXTRN _c_pos_timerInterrupt:FAR
    EXTRN _p_installInterrupts:FAR
IFDEF POSNANO
    EXTRN _posSoftInt:FAR
ENDIF

    ;variables
    EXTRN _posCurrentTask_g:DWORD
    EXTRN _posNextTask_g:DWORD
    EXTRN _posInInterrupt_g:BYTE


;-----------------------------------------------------------------------------
;  OUTPUT FORMAT DEFINITION

.286
.MODEL LARGE
.CODE



;-----------------------------------------------------------------------------
;  START FIRST TASK (start the multitasking)
;
;    void p_pos_startFirstContext(void);
;
; Before calling this function, 
; the following stack frame must have been set up:
;
; posCurrentTask_g->stackptr->    (from low to high memory)
;   ES | DS | DI | SI | BP | SP | BX | DX | CX | AX |
;   OFS (first task function address) | SEG (first task function address) |
;   processor flags |
;   OFS (posTaskExit function address) | SEG (posTaskExit function address) |
;   OFS (argument pointer  void *arg) | SEG (argument pointer  void *arg)
;
; This function initializes the timer interrupt and
; does then a "return from interrupt" that restores all registers.
;-----------------------------------------------------------------------------
_p_pos_startFirstContext:
    CALL FAR PTR _p_installInterrupts       ;initialize timer interrupt
    JMP  SHORT   __pos_startFirstContext


;-----------------------------------------------------------------------------
;  CONTEXT SWITCH FROM TASK LEVEL AND INTERRUPT LEVEL
;
;    void p_pos_softCtxSw(void);
;    void p_pos_intCtxSw(void);
;    void p_pos_startFirstContext(void);
;
; Note:
;   The function p_pos_softCtxSw is implemented in the C-file
;   and is responsible for calling _pos_softCtxSw.
;   (This is done by a DOS sofware interrupt.)
;
; This code is an example of how to combine the three functions
; p_pos_startFirstContext, pos_softCtxSw and pos_intCtxSw.
; Please see the pico]OS manual for details on porting the RTOS.
;-----------------------------------------------------------------------------
__pos_softCtxSw:                            ;start of func. p_pos_softCtxSw
    PUSHA                                   ;save context of current task
    PUSH DS
    PUSH ES
    MOV  AX, SEG _posCurrentTask_g          ;set d.seg to posCurrentTask_g
    MOV  DS, AX
    LES  BX, DWORD PTR DS:_posCurrentTask_g
    MOV  ES: [BX],   SP                     ;posCurrentTask_g->stackptr = stk
    MOV  ES: [BX+2], SS

_p_pos_intContextSwitch:                    ;start of p_pos_intContextSwitch
    MOV  AX, SEG _posNextTask_g             ;set d.seg to posNextTask_g
    MOV  DS, AX
    MOV  CX, WORD PTR DS:_posNextTask_g     ;load posNextTask_g to CX/DX
    MOV  DX, WORD PTR DS:_posNextTask_g+2
    MOV  AX, SEG _posCurrentTask_g          ;set d.seg to posCurrentTask_g
    MOV  DS, AX
    MOV  WORD PTR DS:_posCurrentTask_g  ,CX ;store CX/DX to posCurrentTask_g
    MOV  WORD PTR DS:_posCurrentTask_g+2,DX

__pos_startFirstContext:                    ;start of p_pos_startFirstContext
    MOV  AX, SEG _posCurrentTask_g          ;set d.seg to posCurrentTask_g
    MOV  DS, AX
    LES  BX, DWORD PTR DS:_posCurrentTask_g
    MOV  SP, ES: [BX]                       ;stk = posCurrentTask_g->stackptr
    MOV  SS, ES: [BX+2]
    POP  ES                                 ;restore context of new task
    POP  DS
    POPA
    IRET


;-----------------------------------------------------------------------------
;  TIMER INTERRUPT
;
; The DOS timer interrupt is called with 18.2 Hz.
; This interrupt is used to generate the pico]OS time slices.
;
; This function does the following:
; a) save task context to stack
; b) if (posInInterrupt_g == 0), save stack ptr to current task environment
; c) call to  c_pos_intEnter()
; d) call to  c_pos_timerInterrupt()
; e) call DOS timer interrupt handler
; f) call to  c_pos_intExit()
;-----------------------------------------------------------------------------
__timerIrq:
    PUSHA                                   ;save current task context
    PUSH DS
    PUSH ES
    MOV  AX, SEG _posInInterrupt_g          ;set data seg to posInInterrupt_g
    MOV  DS, AX
    CMP  BYTE PTR DS:_posInInterrupt_g, 0   ;if (posInInterrupt_g == 0)
    JNE  SHORT _intRun                      ;{
    MOV  AX, SEG _posCurrentTask_g          ;  set d.seg to posCurrentTask_g
    MOV  DS, AX                             ;
    LES  BX, DWORD PTR DS:_posCurrentTask_g ;  posCurrentTask_g->stackptr=stk
    MOV  ES: [BX],   SP                     ;
    MOV  ES: [BX+2], SS                     ;} 
_intRun:
    CALL _c_pos_intEnter                    ;enter pico]OS interrupt level
    CALL _c_pos_timerInterrupt              ;call timer interrupt
    INT  081h                               ;call DOS's tick ISR
    CALL FAR PTR _c_pos_intExit             ;leave pico]OS interrupt level
    POP  ES                                 ;back from interrupt (old task)
    POP  DS
    POPA
    IRET



IFDEF POSNANO
;-----------------------------------------------------------------------------
;  FUNCTIONS NEEDED BY THE NANO LAYER


;-----------------------------------------------------------------------------
;  KEYBOARD INTERRUPT  - nano layer option
;
; This system interrupt (INT 09h) is executed every time a key is pressed.
; The key code is read from the DOS keyboard buffer and 
; is then fed into the nano layer via the software interrupt 0.
;-----------------------------------------------------------------------------
__keyboardIrq:
    PUSHA                                   ;save registers
    PUSH DS
    PUSH ES
    INT  82h                                ;call original INT 09
    MOV  AH, 0Bh                            ;call DOS, test if key pressed
    INT  21h
    CMP  AL, 0FFh
    JNE  _nokey
    MOV  AH, 07h                            ;call DOS, get key code
    INT  21h
    MOV  AH, 0                              ;prepare to execute sw-int 0
    PUSH AX
    PUSH 0
    MOV  AX, SEG _posCurrentTask_g          ;set DS to our data segment
    MOV  DS, AX
    CALL _posSoftInt                        ;call software interrupt
    ADD  SP, 4
_nokey:
    POP  ES                                 ;restore registers
    POP  DS
    POPA
    IRET                                    ;back from interrupt


;-----------------------------------------------------------------------------
;  PRINT CHARACTER  - nano layer option
;
; The nano layer calls this function to print a character to the screen.
;-----------------------------------------------------------------------------
_p_putchar:
    PUSH  BP
    MOV   BP,SP
    MOV   AL,[BP+06]                        ;get parameter: char
    MOV   AH,14
    MOV   BL,15
    INT   10h                               ;print character to screen
    POP   BP
    MOV   AL,1                              ;return 1 (=TRUE)
    RETF

ENDIF


;End Of File
END
