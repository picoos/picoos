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
 * @file   n_core.c
 * @brief  nano layer, core file
 * @author Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: n_core.c,v 1.9 2006/10/16 19:41:27 dkuschel Exp $
 */

#define _N_CORE_C
#include "../src/nano/privnano.h"

/* check features */
#if NOSCFG_FEATURE_CPUUSAGE != 0
#if POSCFG_FEATURE_SLEEP == 0
#error POSCFG_FEATURE_SLEEP not enabled
#endif
#if POSCFG_FEATURE_IDLETASKHOOK == 0
#error POSCFG_FEATURE_IDLETASKHOOK not enabled
#endif
#if POSCFG_FEATURE_JIFFIES == 0
#error POSCFG_FEATURE_JIFFIES not enabled
#endif
#endif
#if POSCFG_TASKSTACKTYPE == 0
#if NOSCFG_FEATURE_MEMALLOC == 0
#error NOSCFG_FEATURE_MEMALLOC not enabled
#endif
#endif



/*---------------------------------------------------------------------------
 *  PROTOTYPES, TYPEDEFS AND VARIABLES
 *-------------------------------------------------------------------------*/

/* imports */
#if (NOSCFG_FEATURE_MEMALLOC != 0) && (NOSCFG_MEM_MANAGER_TYPE == 1)
extern void POSCALL nos_initMem(void);
#endif
#if (NOSCFG_FEATURE_CONIN != 0) || (NOSCFG_FEATURE_CONOUT != 0) || \
    (NOSCFG_FEATURE_PRINTF != 0) || (NOSCFG_FEATURE_SPRINTF != 0)
extern void POSCALL nos_initConIO(void);
#endif
#if NOSCFG_FEATURE_BOTTOMHALF != 0
extern void POSCALL nos_initBottomHalfs(void);
#endif
#if NOSCFG_FEATURE_REGISTRY != 0
extern void POSCALL nos_initRegistry(void);
#endif

/* private */
static void nano_init(void *arg);

#if NOSCFG_FEATURE_CPUUSAGE != 0

static void nano_idlehook(void);
static void POSCALL nano_initCpuUsage(void);

static unsigned long  idle_counter_g = 0;
static unsigned long  idle_loops_g;
static unsigned long  idle_loops_100p_g;
static UVAR_t         idle_predivide_g;
static JIF_t          idle_jiffies_g = 0;

#define IDLE_MULBITS     11  /* = 2048;  2048 / 20 = 102.4 ~ 100% */
#define IDLE_PREDIVIDER  (1<<IDLE_MULBITS)

#define IDLE_MIN_TICKS  200
#define IDLE_FAST_DIV   4

#if HZ >= (IDLE_MIN_TICKS * IDLE_FAST_DIV)
#define IDLE_PERIOD     (HZ / IDLE_FAST_DIV)
#define IDLE_INIT_MULT  IDLE_FAST_DIV
#else 
#define IDLE_PERIOD     HZ
#define IDLE_INIT_MULT  1
#endif

#endif /* NOSCFG_FEATURE_CPUUSAGE */

struct {
  POSTASKFUNC_t  func;
  void           *arg;
} taskparams_g;



/*---------------------------------------------------------------------------
 *  CPU USAGE MEASUREMENT
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_CPUUSAGE != 0

static void nano_idlehook(void)
{
  JIF_t jif;
  POS_LOCKFLAGS;

  jif = jiffies;
  POS_SCHED_LOCK;
  if (POS_TIMEAFTER(jif, idle_jiffies_g))
  {
    idle_loops_g   = idle_counter_g;
    idle_jiffies_g = jif + HZ;
    idle_counter_g = 0;
  }
  else
  {
    idle_counter_g++;
  }
  POS_SCHED_UNLOCK;
}

/*-------------------------------------------------------------------------*/

static void POSCALL nano_initCpuUsage(void)
{
  (void) posInstallIdleTaskHook(nano_idlehook);
  posTaskSleep(1);
  idle_jiffies_g = jiffies + IDLE_PERIOD;
  idle_counter_g = 0;
  posTaskSleep(IDLE_PERIOD + 2);
#if IDLE_INIT_MULT > 1
  idle_loops_g *= IDLE_INIT_MULT;
#endif
  idle_loops_100p_g = idle_loops_g;
  if (idle_loops_100p_g == 0)
    idle_loops_100p_g = 1;
  idle_predivide_g = 0;
  if (idle_loops_100p_g > 1L*(IDLE_PREDIVIDER << (31-(2*IDLE_MULBITS))))
  {
    idle_predivide_g = 1;
    idle_loops_100p_g /= IDLE_PREDIVIDER;
  }
}

/*-------------------------------------------------------------------------*/

UVAR_t POSCALL nosCpuUsage(void)
{
  unsigned long tmp;
  UINT_t  p;
  JIF_t   jif;
  POS_LOCKFLAGS;

  jif = jiffies;
  POS_SCHED_LOCK;
  tmp = idle_loops_g;
  if (POS_TIMEAFTER(jif, idle_jiffies_g + HZ))
  {
    idle_jiffies_g = jif + HZ;
    idle_counter_g = 0;
    idle_loops_g   = 0;
  }
  POS_SCHED_UNLOCK;
  if (idle_predivide_g != 0)
    tmp /= IDLE_PREDIVIDER;
  tmp = idle_loops_100p_g - tmp;
  if ((long)tmp < 0) 
    return 0;
  p = ((UINT_t) ((tmp << IDLE_MULBITS) / idle_loops_100p_g)) / 20;
  if (p > 100)  p = 100;
  return (UVAR_t) p;
}

#endif /* NOSCFG_FEATURE_CPUUSAGE */



/*---------------------------------------------------------------------------
 *  NANO LAYER SEMAPHORE FUNCTIONS
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_SEMAPHORES != 0
#if NOSCFG_FEATURE_REGISTRY != 0

NOSSEMA_t POSCALL nosSemaCreate(INT_t initcount, UVAR_t options, 
                                const char *name)
{
  POSSEMA_t sem;
  REGELEM_t re;

  re = nos_regNewSysKey(REGTYPE_SEMAPHORE,
                        name == NULL ? (const char*)"s*" : name);
  if (re == NULL)
    return NULL;

  (void) options;
  sem = posSemaCreate(initcount);

  if (sem == NULL)
  {
    nos_regDelSysKey(REGTYPE_SEMAPHORE, NULL, re);
  }
  else
  {
    nos_regEnableSysKey(re, sem);
    POS_SETEVENTNAME(sem, re->name);
  }
  return (NOSSEMA_t) sem;
}

#if POSCFG_FEATURE_SEMADESTROY != 0

void POSCALL nosSemaDestroy(NOSSEMA_t sema)
{
  if (sema != NULL)
  {
    nos_regDelSysKey(REGTYPE_SEMAPHORE, sema, NULL);
    posSemaDestroy((POSSEMA_t) sema);
  }
}

#endif /* POSCFG_FEATURE_SEMADESTROY */

#endif /* NOSCFG_FEATURE_REGISTRY */
#endif /* NOSCFG_FEATURE_SEMAPHORES */



/*---------------------------------------------------------------------------
 *  NANO LAYER MUTEX FUNCTIONS
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_MUTEXES != 0
#if NOSCFG_FEATURE_REGISTRY != 0

NOSMUTEX_t POSCALL nosMutexCreate(UVAR_t options, const char *name)
{
  POSMUTEX_t mtx;
  REGELEM_t re;

  re = nos_regNewSysKey(REGTYPE_MUTEX,
                        name == NULL ? (const char*)"m*" : name);
  if (re == NULL)
    return NULL;

  (void) options;
  mtx = posMutexCreate();

  if (mtx == NULL)
  {
    nos_regDelSysKey(REGTYPE_MUTEX, NULL, re);
  }
  else
  {
    nos_regEnableSysKey(re, mtx);
    POS_SETEVENTNAME(mtx, re->name);
  }
  return (NOSMUTEX_t) mtx;
}

#if POSCFG_FEATURE_MUTEXDESTROY != 0

void POSCALL nosMutexDestroy(NOSMUTEX_t mutex)
{
  if (mutex != NULL)
  {
    nos_regDelSysKey(REGTYPE_MUTEX, mutex, NULL);
    posMutexDestroy((POSMUTEX_t) mutex);
  }
}

#endif /* POSCFG_FEATURE_MUTEXDESTROY */

#endif /* NOSCFG_FEATURE_REGISTRY */
#endif /* NOSCFG_FEATURE_MUTEXES */



/*---------------------------------------------------------------------------
 * NANO LAYER MESSAGE BOX FUNCTIONS
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_MSGBOXES != 0

void* POSCALL nosMessageAlloc(UINT_t msgSize)
{
  void *buf;
#if POSCFG_MSG_MEMORY != 0
  if (msgSize > POSCFG_MSG_BUFSIZE)
    return NULL;
  buf = posMessageAlloc();
#else
#if NOSCFG_FEATURE_MEMALLOC == 0
#error NOSCFG_FEATURE_MEMALLOC not enabled
#endif
  buf = nosMemAlloc(msgSize);
#endif
  return buf;
}

void POSCALL nosMessageFree(void *buf)
{
#if POSCFG_MSG_MEMORY != 0
  posMessageFree(buf);
#else
  nosMemFree(buf);
#endif
}

VAR_t POSCALL nosMessageSend(void *buf, NOSTASK_t taskhandle)
{
  VAR_t rc;
  rc = posMessageSend(buf, (POSTASK_t) taskhandle);
#if POSCFG_MSG_MEMORY == 0
  if (rc != E_OK)
    nosMemFree(buf);
#endif
  return rc;
}

#endif  /* NOSCFG_FEATURE_MSGBOXES */


/*---------------------------------------------------------------------------
 * NANO LAYER FLAG FUNCTIONS
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_FLAGS != 0
#if NOSCFG_FEATURE_REGISTRY != 0

NOSFLAG_t POSCALL nosFlagCreate(const char* name)
{
  POSFLAG_t flg;
  REGELEM_t re;

  re = nos_regNewSysKey(REGTYPE_FLAG,
                        name == NULL ? (const char*)"f*" : name);
  if (re == NULL)
    return NULL;

  flg = posFlagCreate();
  if (flg == NULL)
  {
    nos_regDelSysKey(REGTYPE_FLAG, NULL, re);
  }
  else
  {
    nos_regEnableSysKey(re, flg);
    POS_SETEVENTNAME(flg, re->name);
  }
  return (NOSFLAG_t) flg;
}

#if POSCFG_FEATURE_FLAGDESTROY != 0

void POSCALL nosFlagDestroy(NOSFLAG_t flg)
{
  if (flg != NULL)
  {
    nos_regDelSysKey(REGTYPE_FLAG, flg, NULL);
    posFlagDestroy((POSFLAG_t) flg);
  }
}

#endif /* POSCFG_FEATURE_FLAGDESTROY */

#endif /* NOSCFG_FEATURE_REGISTRY */
#endif /* NOSCFG_FEATURE_FLAGS */



/*---------------------------------------------------------------------------
 * NANO LAYER TIMER FUNCTIONS
 *-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_TIMER != 0
#if NOSCFG_FEATURE_REGISTRY != 0

NOSTIMER_t POSCALL nosTimerCreate(const char *name)
{
  POSTIMER_t tmr;
  REGELEM_t re;

  re = nos_regNewSysKey(REGTYPE_TIMER, 
                        name == NULL ? (const char*)"r*" : name);
  if (re == NULL)
    return NULL;

  tmr = posTimerCreate();
  if (tmr == NULL)
  {
    nos_regDelSysKey(REGTYPE_TIMER, NULL, re);
  }
  else
  {
    nos_regEnableSysKey(re, tmr);
  }
  return (NOSTIMER_t) tmr;
}

void POSCALL nosTimerDestroy(NOSTIMER_t tmr)
{
  if (tmr != NULL)
  {
    nos_regDelSysKey(REGTYPE_TIMER, tmr, NULL);
    posTimerDestroy((POSTIMER_t) tmr);
  }
}

#endif /* NOSCFG_FEATURE_REGISTRY */
#endif /* NOSCFG_FEATURE_TIMER */



/*---------------------------------------------------------------------------
 *  NANO LAYER TASK FUNCTIONS
 *-------------------------------------------------------------------------*/

#ifdef NOS_NEEDTASKEXITHOOK

static void nos_taskExitHook(POSTASK_t task, texhookevent_t event);
static void nos_taskExitHook(POSTASK_t task, texhookevent_t event)
{
#if NOSCFG_FEATURE_REGISTRY != 0
  if (event == texh_exitcalled)
    nos_regDelSysKey(REGTYPE_TASK, task, NULL);
#endif
#if POSCFG_TASKSTACKTYPE == 0
  if (event == texh_freestackmem)
    NOS_MEM_FREE(task->nosstkroot);
#endif
}

#endif /* NOS_NEEDTASKEXITHOOK */

/*-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_TASKCREATE != 0

#ifdef NOS_NEEDTASKEXITHOOK
#if POSCFG_FEATURE_INHIBITSCHED == 0
#error POSCFG_FEATURE_INHIBITSCHED not enabled
#endif
#endif

NOSTASK_t POSCALL nosTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                                VAR_t priority, UINT_t stacksize,
                                const char* name)
{
  POSTASK_t task;
#if NOSCFG_FEATURE_REGISTRY != 0
  REGELEM_t re;
#endif
#if POSCFG_TASKSTACKTYPE == 0
  void *stk;
#endif

#if NOSCFG_FEATURE_REGISTRY != 0
  re = nos_regNewSysKey(REGTYPE_TASK,
                        name == NULL ? (const char*)"t*" : name);
  if (re == NULL)
    return NULL;
#else
  (void) name;
#endif

  /*===============================================*/
#if POSCFG_TASKSTACKTYPE == 0

  if (stacksize == 0)
    stacksize = NOSCFG_DEFAULT_STACKSIZE;

  task = NULL;
  stk = nosMemAlloc(NOSCFG_STKMEM_RESERVE + stacksize);
  if (stk != NULL)
  {
    posTaskSchedLock();
#if NOSCFG_STACK_GROWS_UP == 0
    task = posTaskCreate(funcptr, funcarg, priority, 
                         (void*) (((MEMPTR_t)stk) + stacksize -
                                  (NOSCFG_STKMEM_RESERVE + 1)));
#else
    task = posTaskCreate(funcptr, funcarg, priority,
                         (void*) (((MEMPTR_t)stk) + NOSCFG_STKMEM_RESERVE));
#endif
    if (task != NULL)
    {
      task->nosstkroot = stk;
      task->exithook   = nos_taskExitHook;
#if NOSCFG_FEATURE_REGISTRY != 0
      nos_regEnableSysKey(re, task);
      POS_SETTASKNAME(task, re->name);
#else
      POS_SETTASKNAME(task, name);
#endif
    }
    posTaskSchedUnlock();

#if POSCFG_FEATURE_SLEEP != 0
    posTaskSleep(0);
#elif POSCFG_FEATURE_YIELD != 0
    posTaskYield();
#endif
  }

  /*-----------------------------------------------*/
#elif POSCFG_TASKSTACKTYPE == 1

  if (stacksize == 0)
    stacksize = NOSCFG_DEFAULT_STACKSIZE;

#if NOSCFG_FEATURE_REGISTRY != 0
  posTaskSchedLock();
#endif

  task = posTaskCreate(funcptr, funcarg, priority, stacksize);

#if NOSCFG_FEATURE_REGISTRY != 0
  if (task != NULL)
  {
    task->exithook = nos_taskExitHook;
    nos_regEnableSysKey(re, task);
    POS_SETTASKNAME(task, re->name);
  }
  posTaskSchedUnlock();
#else
  POS_SETTASKNAME(task, name);
#endif

  /*-----------------------------------------------*/
#elif POSCFG_TASKSTACKTYPE == 2

#if NOSCFG_FEATURE_REGISTRY != 0
  posTaskSchedLock();
#endif

  (void) stacksize;
  task = posTaskCreate(funcptr, funcarg, priority);

#if NOSCFG_FEATURE_REGISTRY != 0
  if (task != NULL)
  {
    task->exithook = nos_taskExitHook;
    nos_regEnableSysKey(re, task);
    POS_SETTASKNAME(task, re->name);
  }
  posTaskSchedUnlock();
#else
  POS_SETTASKNAME(task, name);
#endif

#endif /* POSCFG_TASKSTACKTYPE == 2 */

  /*===============================================*/
#if NOSCFG_FEATURE_REGISTRY != 0
  if (task == NULL)
  {
    nos_regDelSysKey(REGTYPE_TASK, NULL, re);
  }
#endif

  return (POSTASK_t) task;
}

#endif /* NOSCFG_FEATURE_TASKCREATE != 0 */



/*---------------------------------------------------------------------------
 *  NANO LAYER INITIALIZATION
 *-------------------------------------------------------------------------*/

static void nano_init(void *arg)
{
#if NOSCFG_FEATURE_REGISTRY != 0
  REGELEM_t re;
#endif
  POS_SETTASKNAME(posCurrentTask_g, "root task");
#if POSCFG_TASKSTACKTYPE == 0
  posCurrentTask_g->nosstkroot = arg;
  posCurrentTask_g->exithook = nos_taskExitHook;
#else
  (void) arg;
#if NOSCFG_FEATURE_REGISTRY != 0
  posCurrentTask_g->exithook = nos_taskExitHook;
#endif
#endif

#if NOSCFG_FEATURE_CPUUSAGE != 0
  nano_initCpuUsage();
#endif
#if NOSCFG_FEATURE_REGISTRY != 0
  nos_initRegistry();
#endif
#if (NOSCFG_FEATURE_CONIN != 0) || (NOSCFG_FEATURE_CONOUT != 0) || \
    (NOSCFG_FEATURE_PRINTF != 0) || (NOSCFG_FEATURE_SPRINTF != 0)
  nos_initConIO();
#endif
#if NOSCFG_FEATURE_BOTTOMHALF != 0
  nos_initBottomHalfs();
#endif

#if NOSCFG_FEATURE_REGISTRY != 0
  re = nos_regNewSysKey(REGTYPE_TASK, "root-task");
  if (re != NULL)
  {
    nos_regEnableSysKey(re, posTaskGetCurrent());
  }
#endif

  (taskparams_g.func)(taskparams_g.arg);
}

/*-------------------------------------------------------------------------*/

void POSCALL nosInit(POSTASKFUNC_t firstfunc, void *funcarg, VAR_t priority,
                     UINT_t taskStackSize, UINT_t idleStackSize)
{
#if POSCFG_TASKSTACKTYPE == 0
  void *stk_task1, *stk_idle;
#endif

  taskparams_g.func = firstfunc;
  taskparams_g.arg  = funcarg;

#if (NOSCFG_FEATURE_MEMALLOC != 0) && (NOSCFG_MEM_MANAGER_TYPE == 1)
  nos_initMem();
#endif

#if POSCFG_TASKSTACKTYPE == 0

  if (taskStackSize == 0)
    taskStackSize = NOSCFG_DEFAULT_STACKSIZE;
  if (idleStackSize == 0)
    idleStackSize = NOSCFG_DEFAULT_STACKSIZE;
  stk_task1 = NOS_MEM_ALLOC(NOSCFG_STKMEM_RESERVE + taskStackSize);
  stk_idle  = NOS_MEM_ALLOC(NOSCFG_STKMEM_RESERVE + idleStackSize);
  if ((stk_task1 != NULL) && (stk_idle != NULL))
  {
#if NOSCFG_STACK_GROWS_UP == 0
    posInit(nano_init, stk_task1, priority,
            (void*) (((MEMPTR_t)stk_task1) + taskStackSize -
                     (NOSCFG_STKMEM_RESERVE + 1)),
            (void*) (((MEMPTR_t)stk_idle) + idleStackSize -
                     (NOSCFG_STKMEM_RESERVE + 1)) );
#else
    posInit(nano_init, stk_task1, priority,
            (void*) (((MEMPTR_t)stk_task1) + NOSCFG_STKMEM_RESERVE),
            (void*) (((MEMPTR_t)stk_idle) + NOSCFG_STKMEM_RESERVE) );
#endif
  }

#elif POSCFG_TASKSTACKTYPE == 1

  if (taskStackSize == 0)
    taskStackSize = NOSCFG_DEFAULT_STACKSIZE;
  if (idleStackSize == 0)
    idleStackSize = NOSCFG_DEFAULT_STACKSIZE;
  posInit(nano_init, NULL, priority, taskStackSize, idleStackSize);

#elif POSCFG_TASKSTACKTYPE == 2

  (void) taskStackSize;
  (void) idleStackSize;
  posInit(nano_init, NULL, priority);

#endif
}
