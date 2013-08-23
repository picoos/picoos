/*
 *  Copyright (c) 2004, Dennis Kuschel.
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
 * @file    test.c
 * @brief   pico]OS test program
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: test.c,v 1.3 2004/03/13 19:11:25 dkuschel Exp $
 */

#ifndef _NORTLIB
#include <stdio.h>
#endif
#include <picoos.h>


/* FUNCTION PROTOTYPES */
static POSTASK_t    startTask(POSTASKFUNC_t fp, void *arg, UVAR_t prio);
static void         task1(void* arg);
static void         task_printer(void *arg);
static void         task_poll(void *arg);
static void         task_flag(void *arg);
static void         task_timer(void* arg);
static void         task_mutex(void *arg);
static void         print(const char *string);


/* GLOBAL VARIABLES */
static POSTASK_t     printertask_g;
static POSFLAG_t     flags_g;
static POSSEMA_t     timersem_g;
static POSMUTEX_t    mutex_g;
static POSSEMA_t     sema_g;


/* STRING PRINT MACRO */
#ifndef _NORTLIB
#ifndef PRINTSTR
#define PRINTSTR(str)  printf("%s", str)
#endif
#endif


/*=========================================================================*/
#if (POSCFG_TASKSTACKTYPE == 0)
#define STACKMEMSIZE  0x6000
static unsigned short stack_g[STACKMEMSIZE/sizeof(unsigned short)];
static int stkmemp_g = 0;
static void *allocstack(int size);
/* Allocate stack memory. This function is architecture specific.
   This code is for a stack that grows from top down to bottom
   at a 16 bit machine, e.g. a 80x86 processor in real mode. */
static void *allocstack(int size)
{
  unsigned short *stp;
  stkmemp_g += size & ~1;
  stp = &stack_g[stkmemp_g/sizeof(unsigned short) -1];
  return (void*) stp;
}
#endif
/*-------------------------------------------------------------------------*/
static POSTASK_t startTask(POSTASKFUNC_t fp, void *arg, UVAR_t prio)
{
#if (POSCFG_TASKSTACKTYPE == 0)
  return posTaskCreate(fp, arg, prio, allocstack(0x0600));
#elif (POSCFG_TASKSTACKTYPE == 1)
  return posTaskCreate(fp, arg, prio, 0x0600);
#elif (POSCFG_TASKSTACKTYPE == 2)
  return posTaskCreate(fp, arg, prio);
#endif
}
/*=========================================================================*/


static void task_printer(void *arg)
{
  char *buf;

  (void) arg;

  for (;;)
  {
    /* get message */
    buf = posMessageGet();

    /* print content to screen */
    PRINTSTR(buf);

    /* free message buffer */
    posMessageFree(buf);
  }
}

/*-------------------------------------------------------------------------*/

static void print(const char *string)
{
  char *d, *s = (char*) string;
  char *buf;

  /* allocate message buffer */
  buf = posMessageAlloc();

  /* copy string into message buffer */
  for (d = buf; *s != 0; d++, s++)
  {
    *d = *s;
  }
  *d = 0;

  /* send message to printer task */
  posMessageSend(buf, printertask_g);
}

/*-------------------------------------------------------------------------*/

static void softint_handler(UVAR_t param)
{
  posFlagSet(flags_g, param);
}

/*-------------------------------------------------------------------------*/

static void task_poll(void *arg)
{
  char  *text = (char*) arg;
  JIF_t  now, exptime;

  now = jiffies;

  for (;;)
  {
    /* do busy waiting for 1/2 second */
    exptime = now + (HZ / 2);
    do
    {
      now = jiffies;
    }
    while (!POS_TIMEAFTER(now, exptime));

    /* print text every 1/2 second */
    print(text);
  }
}

/*-------------------------------------------------------------------------*/

static void task_softint(void *arg)
{
  UVAR_t intno = (UVAR_t) arg;
  JIF_t  now, exptime;
  UVAR_t c;

  c = 0;
  now = jiffies;

  for (;;)
  {
    /* do busy waiting for 1 2/3 seconds */
    exptime = now + HZ + (2*HZ / 3);
    do
    {
      now = jiffies;
    }
    while (!POS_TIMEAFTER(now, exptime));

    /* rise software interrupt every 1.67 seconds */
    posSoftInt(intno, 6);

    c++;
    if (c == 2)
    {
      posTaskYield();
    }
    if (c == 3)
    {
      posTaskSleep(1);
      c = 0;
    }
  }
}

/*-------------------------------------------------------------------------*/

static void task_flag(void *arg)
{
  char  buf[5];
  VAR_t f;

  (void) arg;

  buf[0] = '>';
  buf[1] = 'F';
  buf[3] = ' ';
  buf[4] = '\0';

  for (;;)
  {
    /* wait for flag event and get flag number */
    f = posFlagGet(flags_g, POSFLAG_MODE_GETSINGLE);

    /* print flag number */
    buf[2] = '0' + (char) f;
    print(buf);
  }
}

/*-------------------------------------------------------------------------*/

static void task_timer(void* arg)
{
  (void) arg;

  for (;;)
  {
    /* wait for semaphore to be signalled */
    posSemaGet(timersem_g);

    /* Set flag. The flag number will be printed out by the flag task. */
    posFlagSet(flags_g, 3);
  }
}

/*-------------------------------------------------------------------------*/

static void task_mutex(void *arg)
{
  static INT_t cntr = 0;
  char  *text = (char*) arg;

  for (;;)
  {  
    /* try to get mutex and lock */
    posMutexLock(mutex_g);

    cntr++;

    /* sleep one second */
    posTaskSleep(HZ);

    /* print text */
    print(text);

    cntr--;
    if (cntr != 0)
      print("MUTEX-ERROR");

    /* unlock mutex again */
    posMutexUnlock(mutex_g);
  }
}

/*-------------------------------------------------------------------------*/

static void task_semas(void *arg)
{
  static INT_t cntr = 0;
  char  buf[5];

  buf[1] = 'S';
  buf[2] = (char) ((int)arg);
  buf[3] = ' ';
  buf[4] = '\0';

  (void) arg;

  for (;;)
  {  
    /* try to get semaphore */
    posSemaGet(sema_g);

    cntr++;

    /* sleep one second */
    posTaskSleep(HZ);

    /* print information, how many tasks are in this function */
    buf[0] = '0' + (char) cntr;
    print(buf);
    if ((cntr < 1) || (cntr > 2))
    {
      print("SEMAPHORE-ERROR");
    }

    cntr--;

    /* release semaphore again */
    posSemaSignal(sema_g);
  }
}


/*=========================================================================*/


static void task1(void *arg)
{
  POSTASK_t  self;
  POSTIMER_t timer;
  VAR_t      curprio;

  (void) arg;

  /* get handle and priority of current task */
  self    = posTaskGetCurrent();
  curprio = posTaskGetPriority(self);

  /* try to increase current priority */
  posTaskSetPriority(self, curprio + 1);
  curprio = posTaskGetPriority(self);

  /* start the printer task */
  printertask_g = startTask(task_printer, NULL, curprio + 1);

  /* print first message */
  print("HELLO WORLD!\n");

  /* start three tasks that do busy waiting */
  startTask(task_poll, " P1 ", curprio - 1);
  posTaskSleep(HZ/8);
  startTask(task_poll, " P2 ", curprio - 1);
  posTaskSleep(HZ/8);
  startTask(task_poll, " P3 ", curprio - 1);

  /* register software interrupt handler */
  posSoftIntSetHandler(4, softint_handler);

  /* start a task that rises software interrupts */
  startTask(task_softint, (void*) 4, curprio - 1);

  /* allocate a flag object */
  flags_g = posFlagCreate();
  if (flags_g == NULL)
    print("\nCan not allocate a flag object\n");

  /* start flag task */
  startTask(task_flag, NULL, curprio + 2);

  /* allocate a semaphore object */
  timersem_g = posSemaCreate(0);
  if (timersem_g == NULL)
    print("\nCan not allocate a semaphore\n");

  /* start timer task */
  startTask(task_timer, NULL, curprio + 2);

  /* allocate a timer object and set the timer up */
  timer = posTimerCreate();
  if (timer == NULL)
    print("\nCan not allocate a timer\n");
  posTimerSet(timer, timersem_g, 2*HZ, 2*HZ);

  /* Start the timer. The timer triggers every 2 seconds. */
  posTimerStart(timer);

  /* allocate a mutex object for mutex test */
  mutex_g = posMutexCreate();
  if (mutex_g == NULL)
    print("\nCan not allocate a mutex\n");

  /* start three mutex tasks */
  startTask(task_mutex, ":M1 ", curprio+1);
  startTask(task_mutex, ":M2 ", curprio+1);
  startTask(task_mutex, ":M3 ", curprio+1);

  /* allocate semaphore object for semaphore test,
     allow 2 tasks to get the semaphore */
  sema_g = posSemaCreate(2);
  if (sema_g == NULL)
    print("\nCan not allocate a semaphore\n");

  /* start three semaphore tasks */
  posTaskSleep(HZ/6);
  startTask(task_semas, (void*) (int) '1', curprio+2);
  posTaskSleep(HZ/6);
  startTask(task_semas, (void*) (int) '2', curprio+2);
  posTaskSleep(HZ/6);
  startTask(task_semas, (void*) (int) '3', curprio+2);

  /* Our main loop. We will set the flag number 2 every 3 seconds. */
  for (;;)
  {
    /* suspend this task for 3 seconds */
    posTaskSleep(3*HZ);

    /* set flag number 2 */
    posFlagSet(flags_g, 2);
  }
}


/*=========================================================================*/


int main(void)
{
  PRINTSTR("\n" POS_STARTUPSTRING "\n\n");

  /* start multitasking and execute first task (task1) */
#if (POSCFG_TASKSTACKTYPE == 0)
  posInit(task1, NULL, 1, allocstack(0x0600), allocstack(0x0200));
#elif (POSCFG_TASKSTACKTYPE == 1)
  posInit(task1, NULL, 1, 0x0600, 0x0200);
#elif (POSCFG_TASKSTACKTYPE == 2)
  posInit(task1, NULL, 1);
#endif

  /* we will never get here */
  return 0;
}
