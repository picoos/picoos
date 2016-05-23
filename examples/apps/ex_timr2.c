/*
 *  pico]OS timer example 2
 *
 *  How to set up a continously running timer.
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
 */


/* Include source code for pico]OS
 * initialization with nano layer.
 */
#include "ex_init4.c"


/* we need some features to be enabled */
#if POSCFG_FEATURE_SLEEP == 0
#error The feature POSCFG_FEATURE_SLEEP is not enabled!
#endif
#if POSCFG_FEATURE_SEMAPHORES == 0
#error The feature POSCFG_FEATURE_SEMAPHORES is not enabled!
#endif
#if POSCFG_FEATURE_TIMER == 0
#error The feature POSCFG_FEATURE_TIMER is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void timertask(void *arg);


/* global variable: handle to semaphore */
POSSEMA_t   semaphore;


/* This function is executed by the first task that is started
 * by pico]OS ( see the nosInit()-call in main(), file ex_init4.c ).
 */
void firsttask(void *arg)
{
  POSTASK_t   tsk;
  POSTIMER_t  tmr;
  VAR_t       status;

  /* Avoid compiler warning. "arg" is the "NULL" in the nosInit()-call */
  (void) arg;

  /* create a semaphore, initialize to 0 */
  semaphore = posSemaCreate(0);

  if (semaphore == NULL)
  {
    nosPrint("Failed to create a semaphore!\n");
    return;
  }

  /* Start a second task. The task will pend on the semaphore. */
  tsk = nosTaskCreate(timertask,/* pointer to new task-function            */
                      NULL,     /* optional argument for the task-function */
                      2,        /* priority level of the new task          */
                      0,        /* stack size (0 = default size)           */
                      "task2"); /* optional name of the second task        */

  if (tsk == NULL)
  {
    nosPrint("Failed to start second task!\n");
    return;
  }

  /* create a timer */
  tmr = posTimerCreate();
  
  if (tmr == NULL)
  {
    nosPrint("Failed to create a timer!\n");
    return;
  }

  /* set up a continously running timer that triggers all 2 seconds */
  status = posTimerSet(tmr,       /* handle to timer object            */
                       semaphore, /* handle to sempahore to trigger    */ 
                       MS(2000),  /* first wait time: set to 2 seconds */
                       MS(2000)); /* period: set to 2 seconds          */

  if (status != E_OK)
  {
    nosPrint("Failed to set the timer!\n");
    return;
  }
  
  /* start the timer */  
  status = posTimerStart(tmr);

  if (status != E_OK)
  {
    nosPrint("Failed to start the timer!\n");
  }
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void timertask(void *arg)
{
  int i = 0;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* wait for timer event */
    posSemaGet(semaphore);

    /* print some text */
    nosPrintf1("%i. timertask: got the semaphore\n", ++i);
  }
}

