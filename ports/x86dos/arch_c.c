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
 * CVS-ID $Id: arch_c.c,v 1.3 2004/03/21 18:30:57 dkuschel Exp $
 */


/*  Note on compiler:
 *
 *  This port was successfully tested with Boarland C++ 3.1
 */

#include <dos.h>
#include <conio.h>
#include <picoos.h>


/*-----------------------------------------------------------------------*/

#define TMRINT        0x08
#define KEYBINT       0x09
#define DOSTMRINT     0x81
#define DOSKEYBINT    0x82
#define OSINT         0x80

/*-----------------------------------------------------------------------*/

/* prototypes of external functions */
extern void _pos_softCtxSw(void);
extern void _timerIrq(void);
#ifdef POSNANO
extern void _keyboardIrq(void);
#endif

/* local prototypes */
void p_installInterrupts(void);
void p_pos_initArch(void);



/*-------------------------------------------------------------------------
 * simple MALLOC and FREE
 *-----------------------------------------------------------------------*/
#if (POSCFG_TASKSTACKTYPE == 1)

static void  mem_init(void);
static void* my_malloc(UINT_t size);
static void  my_free(void *p);

#define MEMSIZE_SMALL    0x0200
#define MEMSIZE_BIG      0x0600
#define MEMBLOCKS_SMALL  16
#define MEMBLOCKS_BIG    16

#define ATABSIZE_SMALL   ((MEMBLOCKS_SMALL + MVAR_BITS - 1) / MVAR_BITS)
#define ATABSIZE_BIG     ((MEMBLOCKS_BIG   + MVAR_BITS - 1) / MVAR_BITS)
static unsigned char memSmall_g[MEMBLOCKS_SMALL][MEMSIZE_SMALL];
static unsigned char memBig_g[MEMBLOCKS_BIG][MEMSIZE_BIG];
static UVAR_t atabSmall_g[ATABSIZE_SMALL];
static UVAR_t atabBig_g[ATABSIZE_BIG];


static void mem_init(void)
{
  UVAR_t i;
  for (i=0; i<ATABSIZE_SMALL; i++)
  {
    atabSmall_g[i] = 0;
#if (ATABSIZE_SMALL != ATABSIZE_BIG)
  }
  for (i=0; i<ATABSIZE_BIG; i++)
  {
#endif
    atabBig_g[i] = 0;
  }
}


static void* my_malloc(UINT_t size)
{
  UVAR_t x, y, b;
  POS_LOCKFLAGS;
  
  if ((size == 0) || (size > MEMSIZE_BIG))
    return NULL;

  POS_SCHED_LOCK;
  if (size <= MEMSIZE_SMALL)
  {
    for (y = 0; y < ATABSIZE_SMALL; y++)
    {
      b = ~atabSmall_g[y];
      if (b != 0)
      {
        x = POS_FINDBIT(b);
        atabSmall_g[y] |= 1 << x;
        POS_SCHED_UNLOCK;
        return (void*) memSmall_g[x + (y * MVAR_BITS)];
      }
    }
  }
  for (y = 0; y < ATABSIZE_BIG; y++)
  {
    b = ~atabBig_g[y];
    if (b != 0)
    {
      x = POS_FINDBIT(b);
      atabBig_g[y] |= 1 << x;
      POS_SCHED_UNLOCK;
      return (void*) memBig_g[x + (y * MVAR_BITS)];
    }
  }
  POS_SCHED_UNLOCK;
  return NULL;  
}


static void my_free(void *p)
{
  UVAR_t x, y;
  POS_LOCKFLAGS;
  
  POS_SCHED_LOCK;
  if ((p >= (void*)memSmall_g[0]) &&
      (p <= (void*)memSmall_g[MEMBLOCKS_SMALL]))
  {
    for (x = 0; x < MEMBLOCKS_SMALL; x++)
    {
      if (p == (void*)memSmall_g[x])
      {
        y = x / MVAR_BITS;
        x &= MVAR_BITS - 1;
        atabSmall_g[y] &= ~(1 << x);
        POS_SCHED_UNLOCK;
        return;
      }
    }
  }
  if (p >= (void*)memBig_g[0])
  {
    for (x = 0; x < MEMBLOCKS_BIG; x++)
    {
      if (p == (void*)memBig_g[x])
      {
        y = x / MVAR_BITS;
        x &= MVAR_BITS - 1;
        atabBig_g[y] &= ~(1 << x);
        POS_SCHED_UNLOCK;
        return;
      }
    }
  }
  POS_SCHED_UNLOCK;
}

#endif



/*---------------------------------------------------------------------------
 * INITIALIZE THIS PORT
 *-------------------------------------------------------------------------*/

void p_pos_initArch(void)
{
  unsigned short *pvect;
  unsigned short seg, ofs;

#if (POSCFG_TASKSTACKTYPE == 1)
  mem_init();
#endif

  /* disable interrupts */
  POS_SCHED_LOCK;

  /* set pico/OS interrupt vector for software context switch */
  pvect = (unsigned short*) ((void*) MK_FP(0, OSINT * 4));
  *pvect++ = (unsigned short) FP_OFF(_pos_softCtxSw); 
  *pvect   = (unsigned short) FP_SEG(_pos_softCtxSw);

  /* enable interrupts */
  POS_SCHED_UNLOCK;
}


void p_installInterrupts(void)
{
  unsigned short *pvect;
  unsigned short seg, ofs;

  /* disable interrupts */
  POS_SCHED_LOCK;

  /* copy original DOS timer interrupt vector */
  pvect = (unsigned short*) ((void*) MK_FP(0, TMRINT * 4));
  ofs = *pvect++;
  seg = *pvect;
  pvect = (unsigned short*) ((void*) MK_FP(0, DOSTMRINT * 4));
  *pvect++ = ofs;
  *pvect   = seg;

  /* set pico]OS timer interrupt vector (18 ticks per second) */
  pvect = (unsigned short*) ((void*) MK_FP(0, TMRINT * 4));
  *pvect++ = (unsigned short) FP_OFF(_timerIrq); 
  *pvect   = (unsigned short) FP_SEG(_timerIrq);

#ifdef POSNANO
  /* copy original DOS keyboard interrupt vector */
  pvect = (unsigned short*) ((void*) MK_FP(0, KEYBINT * 4));
  ofs = *pvect++;
  seg = *pvect;
  pvect = (unsigned short*) ((void*) MK_FP(0, DOSKEYBINT * 4));
  *pvect++ = ofs;
  *pvect   = seg;

  /* set pico]OS keyboard interrupt vector */
  pvect = (unsigned short*) ((void*) MK_FP(0, KEYBINT * 4));
  *pvect++ = (unsigned short) FP_OFF(_keyboardIrq); 
  *pvect   = (unsigned short) FP_SEG(_keyboardIrq);
#endif

  /* enable interrupts */
  POS_SCHED_UNLOCK;
}


/* bridge to software context switch handler (arch_a.asm) */
void p_pos_softContextSwitch(void)
{
#ifdef WATCOM
  _asm {"int 0x80"}; /* this will call _pos_softCtxSw() */
#else
  asm INT OSINT;  /* this will call _pos_softCtxSw() */
#endif
}



/*---------------------------------------------------------------------------
 * INIT TASK CONTEXT  (and allocate stack memory if supposed)
 *-------------------------------------------------------------------------*/

#if (POSCFG_TASKSTACKTYPE == 0)

void p_pos_initTask(POSTASK_t task, void *user,
                   POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned short var_ds, *st;

  st = (unsigned short*)(((char*)user)-1);  /* get stack pointer */

  /* get current DS */
#ifdef WATCOM
  _asm {"mov var_ds, ds" };
#else
  asm { mov var_ds, ds };
#endif

  /* setup the stack */
  *st-- = FP_SEG(funcarg);      /* function call with one argument */
  *st-- = FP_OFF(funcarg);         
#if (POSCFG_FEATURE_EXIT != 0)
  *st-- = FP_SEG(posTaskExit);  /* ptr to task exit function */
  *st-- = FP_OFF(posTaskExit);  /* (call posTaskExit when task func returns)*/
#else
  *st-- = FP_SEG(funcptr);      /* ptr to task entry function */
  *st-- = FP_OFF(funcptr);      /* (restart task when task func returns)*/
#endif
  *st-- = 0x0202;               /* SW = interrupts enabled */
  *st-- = FP_SEG(funcptr);      /* ptr to task entry function */
  *st-- = FP_OFF(funcptr);
  *st-- = 0; /* AX */
  *st-- = 0; /* CX */
  *st-- = 0; /* DX */
  *st-- = 0; /* BX */
  *st-- = 0; /* SP */
  *st-- = 0; /* BP */
  *st-- = 0; /* SI */
  *st-- = 0; /* DI */
  *st-- = var_ds;  /* _DS; */ /* current DS */
  *st   = 0; /* ES */
  task->stackptr = (void*) st;
}


#elif (POSCFG_TASKSTACKTYPE == 1)


VAR_t p_pos_initTask(POSTASK_t task, UINT_t stacksize,
                     POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned short var_ds, *st;

  /* get current DS */
#ifdef WATCOM
  _asm {"mov var_ds, ds" };
#else
  asm { mov var_ds, ds };
#endif
  
  /* allocate stack memory */
  st = (unsigned short*) my_malloc(stacksize);
  if (st == NULL)
    return -1;

  task->stackroot = st;
  st += (stacksize / sizeof(unsigned short)) - 1;

  /* setup the stack */
  *st-- = FP_SEG(funcarg);      /* function call with one argument */
  *st-- = FP_OFF(funcarg);         
#if (POSCFG_FEATURE_EXIT != 0)
  *st-- = FP_SEG(posTaskExit);  /* ptr to task exit function */
  *st-- = FP_OFF(posTaskExit);  /* (call posTaskExit when task func returns)*/
#else
  *st-- = FP_SEG(funcptr);      /* ptr to task entry function */
  *st-- = FP_OFF(funcptr);      /* (restart task when task func returns)*/
#endif
  *st-- = 0x0202;               /* SW = interrupts enabled */
  *st-- = FP_SEG(funcptr);      /* ptr to task entry function */
  *st-- = FP_OFF(funcptr);
  *st-- = 0; /* AX */
  *st-- = 0; /* CX */
  *st-- = 0; /* DX */
  *st-- = 0; /* BX */
  *st-- = 0; /* SP */
  *st-- = 0; /* BP */
  *st-- = 0; /* SI */
  *st-- = 0; /* DI */
  *st-- = var_ds;  /* _DS; */ /* current DS */
  *st   = 0; /* ES */

  task->stackptr = (void*) st;
  return 0;
}

void  p_pos_freeStack(POSTASK_t task)
{
  my_free(task->stackroot);
}


#elif (POSCFG_TASKSTACKTYPE == 2)


VAR_t p_pos_initTask(POSTASK_t task,
                     POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned short var_ds, *st;

  /* get current DS */
#ifdef WATCOM
  _asm {"mov var_ds, ds" };
#else
  asm { mov var_ds, ds };
#endif
  
  /* Get stack pointer.
   * This is very simple because the stack memory
   * (with fixed size) is in the task structure.
   */
  st = &task->stack[FIXED_STACK_SIZE-1];

  /* setup the stack */
  *st-- = FP_SEG(funcarg);      /* function call with one argument */
  *st-- = FP_OFF(funcarg);         
#if (POSCFG_FEATURE_EXIT != 0)
  *st-- = FP_SEG(posTaskExit);  /* ptr to task exit function */
  *st-- = FP_OFF(posTaskExit);  /* (call posTaskExit when task func returns)*/
#else
  *st-- = FP_SEG(funcptr);      /* ptr to task entry function */
  *st-- = FP_OFF(funcptr);      /* (restart task when task func returns)*/
#endif
  *st-- = 0x0202;               /* SW = interrupts enabled */
  *st-- = FP_SEG(funcptr);      /* ptr to task entry function */
  *st-- = FP_OFF(funcptr);
  *st-- = 0; /* AX */
  *st-- = 0; /* CX */
  *st-- = 0; /* DX */
  *st-- = 0; /* BX */
  *st-- = 0; /* SP */
  *st-- = 0; /* BP */
  *st-- = 0; /* SI */
  *st-- = 0; /* DI */
  *st-- = var_ds;  /* _DS; */ /* current DS */
  *st   = 0; /* ES */
  task->stackptr = (void*) st;
  
  return 0;
}

void  p_pos_freeStack(POSTASK_t task)
{
  /* this function is called but not needed */
  (void) task;
}

#endif

