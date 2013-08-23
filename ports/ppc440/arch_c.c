/*
 *  Copyright (c) 2004-2012, Dennis Kuschel.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: arch_c.c,v 1.1.1.1 2004/02/16 20:11:33 smocz Exp $
 */


#include <picoos.h>


/* If no handler is available for external interrupts,
   set this define to 1 to avoid unresolved references. */
#define DUMMY_ISR_HANDLER       1

/* Set to 1 to enable dedicated stack for ISRs.  Note:
   The corresponding define in the file arch_a.s must also be set. */
#define DEDICATED_ISR_STACK     0

/* Interrupt service routine stack size (in bytes) */
#define ISR_STACK_SIZE     0x1000



/*---------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *-------------------------------------------------------------------------*/

#if (DEDICATED_ISR_STACK != 0)
static unsigned int isrStackMem_g[ISR_STACK_SIZE / sizeof(unsigned int)];
unsigned int *isrstack_g;
#endif



/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 *-------------------------------------------------------------------------*/

/* prototypes of external functions from file arch_a.s */
extern unsigned int  p_asm_getMSR(void);
extern unsigned int  p_asm_getR2(void);
extern unsigned int  p_asm_getR13(void);

/* prototypes of local functions */
#if (DUMMY_ISR_HANDLER != 0)
void c_extIntHandler(unsigned int uic0s, unsigned int uic1s);
#endif



/*---------------------------------------------------------------------------
 *  INITIALIZE THIS PORT
 *-------------------------------------------------------------------------*/

void p_pos_initArch(void)
{
#if (DEDICATED_ISR_STACK != 0)
  unsigned int i;

  /* get aligned stack pointer, reserve 32 bytes */
  isrstack_g =  (unsigned int *)
                ((((unsigned int) isrStackMem_g) + ISR_STACK_SIZE - 32) &
                 ~(POSCFG_ALIGNMENT - 1));

  /* clear top of stack frame (usually 32 bytes) */
  for (i = 0; i < (32 / sizeof(unsigned int)); i++)
  {
    isrstack_g[i] = 0;
  }
#endif
}



/*---------------------------------------------------------------------------
 *  DEFAULT EXT INTERRUPT HANDLER (DUMMY FUNCTION)
 *-------------------------------------------------------------------------*/

#if (DUMMY_ISR_HANDLER != 0)
void c_extIntHandler(unsigned int uic0s, unsigned int uic1s)
{
  /* The interrupts must be handled here.
   * The UIC is cleared in the assembly part of the ISR handler.
   */
  (void) uic0s;
  (void) uic1s;
}
#endif



/*---------------------------------------------------------------------------
 *  INIT TASK CONTEXT  (and allocate stack memory if supposed)
 *-------------------------------------------------------------------------*/

#if (POSCFG_TASKSTACKTYPE == 0)

void p_pos_initTask(POSTASK_t task, void *user,
                    POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned int *st, *tst, i, z;
  unsigned int msr, r2, r13;

  /* get aligned stack pointer, reserve 32 bytes */
  z = (unsigned int) user;
  z = (z - 32) & ~(POSCFG_ALIGNMENT - 1);
  st = (unsigned int*) z;

  /* clear top of stack frame (usually 32 bytes) */
  for (i = 0; i < (32 / sizeof(unsigned int)); i++)
  {
    st[i] = 0;
  }

  /* get default values */
  msr = p_asm_getMSR() | 0x00008000;  /* msr with enabled interrupts */
  r2  = p_asm_getR2();
  r13 = p_asm_getR13();

  /* setup initial stack frame */
  tst = st;
  *--st = 0;    /* R31 */
  *--st = 0;    /* R30 */
  *--st = 0;    /* R29 */
  *--st = 0;    /* R28 */
  *--st = 0;    /* R27 */
  *--st = 0;    /* R26 */
  *--st = 0;    /* R25 */
  *--st = 0;    /* R24 */
  *--st = 0;    /* R23 */
  *--st = 0;    /* R22 */
  *--st = 0;    /* R21 */
  *--st = 0;    /* R20 */
  *--st = 0;    /* R19 */
  *--st = 0;    /* R18 */
  *--st = 0;    /* R17 */
  *--st = 0;    /* R16 */
  *--st = 0;    /* R15 */
  *--st = 0;    /* R14 */
  *--st = r13;  /* R13 */
  *--st = 0;    /* R12 */
  *--st = 0;    /* R11 */
  *--st = 0;    /* R10 */
  *--st = 0;    /* R09 */
  *--st = 0;    /* R08 */
  *--st = 0;    /* R07 */
  *--st = 0;    /* R06 */
  *--st = 0;    /* R05 */
  *--st = 0;    /* R04 */
  *--st = (unsigned int) funcarg; /* r03 */
  *--st = r2;   /* R02 */
#if (POSCFG_FEATURE_EXIT != 0)
  *--st = (unsigned int) ((void*) posTaskExit); /* LR */
#else
  *--st = (unsigned int) funcptr; /* LR */
#endif
  *--st = 0;    /* CR */
  *--st = 0;    /* XER */
  *--st = 0;    /* CTR */
  *--st = msr;  /* SRR1 */
  *--st = (unsigned int) funcptr; /* SRR0 */
  *--st = 0;    /* R00 */
  --st;         /* yet unused field */
  --st;         /* space for link register of next called function */
  *--st = (unsigned int) tst;  /* stack frame pointer */

  task->stackptr = (void*) st;
}


#elif (POSCFG_TASKSTACKTYPE == 1)

#error POSCFG_TASKSTACKTYPE = 1 is not supported by this port

#elif (POSCFG_TASKSTACKTYPE == 2)


VAR_t p_pos_initTask(POSTASK_t task,
                     POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned int *st, *tst, i, z;
  unsigned int msr, r2, r13;

  /* get aligned stack pointer, reserve 32 bytes */
  z = ((unsigned int) task->stack) + FIXED_STACK_SIZE;
  z = (z - 32) & ~(POSCFG_ALIGNMENT - 1);
  st = (unsigned int*) z;

  /* clear top of stack frame (usually 32 bytes) */
  for (i = 0; i < (32 / sizeof(unsigned int)); i++)
  {
    st[i] = 0;
  }

  /* get default values */
  msr = p_asm_getMSR() | 0x00008000;  /* msr with enabled interrupts */
  r2  = p_asm_getR2();
  r13 = p_asm_getR13();

  /* setup initial stack frame */
  tst = st;
  *--st = 0;    /* R31 */
  *--st = 0;    /* R30 */
  *--st = 0;    /* R29 */
  *--st = 0;    /* R28 */
  *--st = 0;    /* R27 */
  *--st = 0;    /* R26 */
  *--st = 0;    /* R25 */
  *--st = 0;    /* R24 */
  *--st = 0;    /* R23 */
  *--st = 0;    /* R22 */
  *--st = 0;    /* R21 */
  *--st = 0;    /* R20 */
  *--st = 0;    /* R19 */
  *--st = 0;    /* R18 */
  *--st = 0;    /* R17 */
  *--st = 0;    /* R16 */
  *--st = 0;    /* R15 */
  *--st = 0;    /* R14 */
  *--st = r13;  /* R13 */
  *--st = 0;    /* R12 */
  *--st = 0;    /* R11 */
  *--st = 0;    /* R10 */
  *--st = 0;    /* R09 */
  *--st = 0;    /* R08 */
  *--st = 0;    /* R07 */
  *--st = 0;    /* R06 */
  *--st = 0;    /* R05 */
  *--st = 0;    /* R04 */
  *--st = (unsigned int) funcarg; /* r03 */
  *--st = r2;   /* R02 */
#if (POSCFG_FEATURE_EXIT != 0)
  *--st = (unsigned int) ((void*) posTaskExit); /* LR */
#else
  *--st = (unsigned int) funcptr; /* LR */
#endif
  *--st = 0;    /* CR */
  *--st = 0;    /* XER */
  *--st = 0;    /* CTR */
  *--st = msr;  /* SRR1 */
  *--st = (unsigned int) funcptr; /* SRR0 */
  *--st = 0;    /* R00 */
  --st;         /* yet unused field */
  --st;         /* space for link register of next called function */
  *--st = (unsigned int) tst;  /* stack frame pointer */

  task->stackptr = (void*) st;
  return 0;
}

void  p_pos_freeStack(POSTASK_t task)
{
  /* this function is called but not needed */
  (void) task;
}


#endif
