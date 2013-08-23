/*
 * Copyright (c) 2006, Ari Suutari, ari@suutari.iki.fi.
 * Copyright (c) 2004, Jun Li, lj_sourceforge@users.sourceforge.net.
 * Copyright (c) 2004, Dennis Kuschel. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 2. Redistributions
 * in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution. 3. The name of the author may
 * not be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 * 
 * CVS-ID $Id: arch_c.c,v 1.1 2006/04/30 10:41:24 dkuschel Exp $
 */

#define NANOINTERNAL
#include <picoos.h>

#define SYS_MODE  0x1f       // System Mode
#define THUMB_MODE  0x20     // Thumb Mode

#ifdef __thumb__

#define	CPU_MODE	(SYS_MODE | THUMB_MODE)

#else

#define	CPU_MODE	SYS_MODE

#endif

static void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg);

/*
 * Initialize task context.
 */

#if (POSCFG_TASKSTACKTYPE == 0)

void p_pos_initTask(POSTASK_t task, 
                   void *user,
                   POSTASKFUNC_t funcptr, 
                   void *funcarg)
{
  constructStackFrame(task, user, funcptr, funcarg);
}


#elif (POSCFG_TASKSTACKTYPE == 1)


VAR_t p_pos_initTask(POSTASK_t task, 
                    UINT_t stacksize,
                    POSTASKFUNC_t funcptr, 
                    void *funcarg)
{
                      
  unsigned int   z;

  task->stackstart = NOS_MEM_ALLOC(stacksize);
  if (task->stackstart == NULL)
    return -1;

  z = (unsigned int)task->stackstart + stacksize - 4;
  constructStackFrame(task, (void*)z, funcptr, funcarg);
  return 0;
}


void  p_pos_freeStack(POSTASK_t task)
{
  NOS_MEM_FREE(task->stackstart);
}


#elif (POSCFG_TASKSTACKTYPE == 2)


VAR_t p_pos_initTask(POSTASK_t task,
                    POSTASKFUNC_t funcptr, 
                    void *funcarg)
{
  unsigned int   z;

  z = (unsigned int)task->stack + FIXED_STACK_SIZE - 4;
  constructStackFrame(task, (void*)z, funcptr, funcarg);
  return 0;
}

void  p_pos_freeStack(POSTASK_t task)
{
  (void) task;
}


#else
#error "Error in configuration for the port (poscfg.h): POSCFG_TASKSTACKTYPE must be 0, 1 or 2"
#endif

/*
 * Initialize task stack frame. The layout must be same
 * as by context macros in arch_a_macros.h.
 */

static void constructStackFrame(POSTASK_t task, void* stackPtr,
                                POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned int   *stk, z;

/*
 * Get aligned stack pointer, reserve 32 bytes.
 */

  z = (unsigned int)stackPtr;
  z = (z - POSCFG_ALIGNMENT) & ~(POSCFG_ALIGNMENT - 1);
  stk = (unsigned int *)z;

/*
 * Put initial values to stack, including entry point address,
 * some detectable register values, status register (which
 * switches cpu to system mode during context switch) and
 * dummy place for exception stack pointer (see comments
 * assember files for this).
 */

  *(stk) = (unsigned int)0x00000000;	/* bottom		*/
  *(--stk) = (unsigned int)funcptr;	/* Entry Point          */
  *(--stk) = (unsigned int)posTaskExit;	/* lr                   */
  *(--stk) = (unsigned int)z;	        /* stack bottom         */
  *(--stk) = (unsigned int)0xcccccccc;	/* r12                  */
  *(--stk) = (unsigned int)0xbbbbbbbb;	/* r11                  */
  *(--stk) = (unsigned int)0xaaaaaaaa;	/* r10                  */
  *(--stk) = (unsigned int)0x99999999;	/* r9                   */
  *(--stk) = (unsigned int)0x88888888;	/* r8                   */
  *(--stk) = (unsigned int)0x77777777;	/* r7                   */
  *(--stk) = (unsigned int)0x66666666;	/* r6                   */
  *(--stk) = (unsigned int)0x55555555;	/* r5                   */
  *(--stk) = (unsigned int)0x44444444;	/* r4                   */
  *(--stk) = (unsigned int)0x33333333;	/* r3                   */
  *(--stk) = (unsigned int)0x22222222;	/* r2                   */
  *(--stk) = (unsigned int)0x11111111;	/* r1                   */
  *(--stk) = (unsigned int)funcarg;	/* r0 : argument        */
  *(--stk) = (unsigned int)CPU_MODE;	/* CPSR                 */

  /*
   * Space reservation for IRQ stack pointer. It is saved to task stack so
   * p_pos_intContextSwitch can restore it before switching context. If
   * p_pos_intContextSwitch is not called this value is ignored.
   */

  *(--stk) = (unsigned int)0xfafafafa;	/* IRQ stack pointer    */

  task->stackptr = (void *)stk;
}
