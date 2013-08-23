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


/**
 * @file    arch_c.c
 * @brief   pico]OS port for C64 (6502)
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: arch_c.c,v 1.5 2006/10/15 08:46:16 dkuschel Exp $
 */


/*-------------------------------------------------------------------------
 *
 *  6502 / 65c02 / 6510  Architecture Port               by Dennis Kuschel
 *
 *  This port was especially developed for the good old Commodore 64
 *  with the CC65 cross compiler, v 2.11.0, http://www.cc65.org/
 *
 *  The CC65 cross compiler is a bit tricky: It uses some space in the
 *  zero page to store runtime variables, also the CPU stack is expanded
 *  by an additional data stack. 
 * 
 *  To switch the task context, this port must swap the zero page
 *  environment, change the data stack pointer and swap the processor
 *  call stack, so a context switch is very expensive.
 *
 *  To speed up the context switches, the processors call stack is
 *  divided into four task call stacks with 64 bytes each. If only four
 *  tasks are used (POSCFG_MAX_TASKS = 4), the stack must not be copied.
 *  If more than four tasks are used, the first three tasks are fast,
 *  and the other tasks are slow, since the stack must be copied.
 *
 *  With a call stack of 64 bytes the user can do function calls
 *  with a depth of 17. 14 bytes of the stack are needed by the OS,
 *  16 bytes are needed by the interrupt service routine, 34 bytes
 *  remain for the user.
 *
 *  The data stack is limited to 1024 bytes (4 tasks), 768 bytes (5 to 8
 *  tasks) or 512 bytes (more than 8 tasks). The idle task uses the
 *  default stack the CC65 main program would use.
 * 
 *  Keep in mind not to waste memory in a multitasking environment!
 *
 *  The OS timer interrupt handler is called 30 times per second on a C64.
 *  To speed up the system a bit, you may decrease timer calls to 15 HZ.
 *
 *
 *  The configuration file must be changed like this:
 *
 *  #define MVAR_t                char
 *  #define MVAR_BITS                8
 *  #define MPTR_t                 int
 *  #define POSCFG_FASTCODE          0
 *  #define POSCFG_SMALLCODE         1
 *  #define POSCFG_TASKSTACKTYPE     2
 *  #define POSCFG_ALIGNMENT         1
 *  #define POSCFG_ISR_INTERRUPTABLE 0
 *  #define HZ                      30
 *
 *  #define ZPBYTES  (26 - 2)  // -2 because sp is stored in dstackptr
 *
 *  #define POS_USERTASKDATA    \
 *     UVAR_t   cstackptr;      \
 *     UVAR_t   *dstackptr;     \
 *     UVAR_t   *dstackroot;    \
 *     UVAR_t   cstacknbr;      \
 *     UVAR_t   dstacknbr;      \
 *     UVAR_t   savedzp[ZPBYTES];
 *
 *-----------------------------------------------------------------------*/


#define NANOINTERNAL
#include <picoos.h>



/*-------------------------------------------------------------------------
 *  DEFINITIONS
 *-----------------------------------------------------------------------*/

#if (SYS_POSTALLOCATE != 0) && (POSCFG_MAX_TASKS < 5)
#define NUMBER_OF_TASKS  5
#else
#define NUMBER_OF_TASKS  POSCFG_MAX_TASKS
#endif


/* you may change this stack size values */
#if (NUMBER_OF_TASKS < 5)
#define FTASK_DSTACK_SIZE       0x0400
#elif (NUMBER_OF_TASKS < 9)
#define FTASK_DSTACK_SIZE       0x0300
#else
#define FTASK_DSTACK_SIZE       0x0200
#endif
#define FTASK_DYNDSTACK_SIZE    0x0300


/* Do not touch the following definitions!!! */

#if (NUMBER_OF_TASKS < 5)
#define FTASKS      4
#define STASKS      0 
#define CSTACKS     FTASKS
#else
#define FTASKS      3
#define STASKS      (NUMBER_OF_TASKS - FTASKS)
#define CSTACKS     (FTASKS + 1)
#endif

#define CSTACK_SIZE             (0x0100 / CSTACKS)
#define STASK_DSTACK_SIZE       (FTASK_DSTACK_SIZE + CSTACK_SIZE)
#define SCSTACK_BOTTOM          (FTASKS * CSTACK_SIZE)
#define SCSTACK_TOP             (SCSTACK_BOTTOM + CSTACK_SIZE - 1)

#define SNBR_SFLAG              0xFF
#define CPUSTACK                ((UVAR_t*)((void*)0x0100))



/*-------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *-----------------------------------------------------------------------*/

static UVAR_t  ftasks_dstack_g[FTASKS-1][FTASK_DSTACK_SIZE];
#if (STASKS != 0)
static UVAR_t  stasks_dstack_g[STASKS][STASK_DSTACK_SIZE];
static UVAR_t  stasks_alloctab_g[STASKS];
#endif
static UVAR_t  cstack_alloctab_g;



/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES OF IMPORTED FUNCTIONS
 *-------------------------------------------------------------------------*/

extern UVAR_t*  p_get_sp(void);
extern void     p_clear_cpustack(void);



/*---------------------------------------------------------------------------
 *  LOCAL FUNCTION PROTOTYPES
 *-------------------------------------------------------------------------*/

static void     alloc_stacks(POSTASK_t task);
static void     free_stacks(POSTASK_t task);



/*-------------------------------------------------------------------------
 *  STACK ALLOCATION ROUTINES
 *-----------------------------------------------------------------------*/

static void alloc_stacks(POSTASK_t task)
{
  UVAR_t n, *p;
  UINT_t s;

#if (STASKS != 0)
  if (cstack_alloctab_g == (UVAR_t)~0)
  {
    /* allocate a STASK cstack */
    task->cstackptr  = SCSTACK_TOP;
    task->cstacknbr  = SNBR_SFLAG;

    /* allocate a STASK dstack */
    for (n = 0; (n < STASKS-1) && (stasks_alloctab_g[n] != 0); n++);
#if (SYS_POSTALLOCATE != 0) && (POSCFG_ENABLE_NANO != 0)
    if (n == STASKS-1)
    {
      n = 0;
      s = FTASK_DYNDSTACK_SIZE;
      p = (UVAR_t*) NOS_MEM_ALLOC(s); /* must use 'NOS'-mem-alloc here */
      while (p == NULL);
      task->dstacknbr = SNBR_SFLAG;
    }
    else
#endif
    {
      stasks_alloctab_g[n] = 1;
      task->dstacknbr = n;
      p = stasks_dstack_g[0];
      s = STASK_DSTACK_SIZE;
    }
  }
  else
#endif
  {
    /* allocate a FTASK cstack */
    n = POS_FINDBIT(~cstack_alloctab_g);
    cstack_alloctab_g |= 1 << n;
    task->cstackptr  = (n * CSTACK_SIZE) + (CSTACK_SIZE - 1);
    task->cstacknbr  = n;

    /* allocate a FTASK dstack */
#if (STASKS != 0)
    task->dstacknbr  = (UVAR_t) (n - 1);
#endif
    s = FTASK_DSTACK_SIZE;
    if (n == 0)
    {
      /* For the first task (=idle task), we use the current stack.
         But we leave a bit room for the just running function. */
      p = p_get_sp() - (FTASK_DSTACK_SIZE + 0x0020);
    }
    else
    {
      p = ftasks_dstack_g[0];
      --n;
    }
  }
  for (; n != 0; --n) p += s; /* multiply */
  task->dstackroot = p;
  task->dstackptr = p + s - 1;
}


static void free_stacks(POSTASK_t task)
{
#if (STASKS != 0)
  if (task->cstacknbr == SNBR_SFLAG)
  {
#if (SYS_POSTALLOCATE != 0) && (POSCFG_ENABLE_NANO != 0)
    if (task->dstacknbr == SNBR_SFLAG)
    {
      NOS_MEM_FREE(task->dstackroot);  /* must use 'NOS'-mem-free here */
    }
    else
#endif
    stasks_alloctab_g[task->dstacknbr] = 0;
  }
  else
#endif
  {
    cstack_alloctab_g &= ~(1 << task->cstacknbr);
  }
}



/*---------------------------------------------------------------------------
 * INITIALIZE THIS PORT
 *-------------------------------------------------------------------------*/

void p_pos_initArch(void)
{
#if (STASKS != 0)
  unsigned char *p = (unsigned char*) stasks_alloctab_g;
  unsigned int  n = STASKS;
  while (n != 0) { *p = 0; ++p; --n; };
#endif
  cstack_alloctab_g = ~((1 << FTASKS)-1);
  p_clear_cpustack();
}



/*---------------------------------------------------------------------------
 * INIT TASK CONTEXT
 *-------------------------------------------------------------------------*/

#if (POSCFG_TASKSTACKTYPE != 2)
#error Only  POSCFG_TASKSTACKTYPE = 2  supported
#endif

VAR_t p_pos_initTask(POSTASK_t task,
                    POSTASKFUNC_t funcptr, void *funcarg)
{
  UVAR_t *sp;

  /* allocate call- and data-stack memory */
  alloc_stacks(task);

  /* initialize data stack */
  sp = task->dstackptr - 2;
  task->dstackptr = sp;
  sp[1] = (UVAR_t)(((MEMPTR_t)funcarg) >> 8);
  sp[0] = (UVAR_t)((MEMPTR_t)funcarg);

  /* initialize call stack */

#if (POSCFG_FEATURE_EXIT != 0)
  task->cstackptr -= 8;
#else
  task->cstackptr -= 6;
#endif

#if (STASKS != 0)
  if (task->cstacknbr == SNBR_SFLAG)
  {
    sp = task->dstackroot;
  }
  else
#endif
  {
    sp = CPUSTACK + 1 + task->cstackptr;
  }

#if (POSCFG_FEATURE_EXIT != 0)
  sp[7] = (UVAR_t)((((MEMPTR_t)posTaskExit)-1) >> 8);
  sp[6] = (UVAR_t)(((MEMPTR_t)posTaskExit)-1);
#endif

  sp[5] = (UVAR_t)(((MEMPTR_t)((void*)funcptr)) >> 8);
  sp[4] = (UVAR_t)((MEMPTR_t)((void*)funcptr));

  sp[3] = 0x00;  /* flags. irqs enabled. */
  sp[2] = 0x00;  /* A */
  sp[1] = 0x00;  /* X */
  sp[0] = 0x02;  /* Y */

  return 0;
}


void  p_pos_freeStack(POSTASK_t task)
{
  free_stacks(task);
}
