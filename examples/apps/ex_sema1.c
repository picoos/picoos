/*
 *  pico]OS semaphore example 1
 *
 *  How to use a semaphore to signal an event.
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
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void eventtask(void *arg);


/* global variable: handle to semaphore */
POSSEMA_t   semaphore;


/* This function is executed by the first task that is started
 * by pico]OS ( see the nosInit()-call in main(), file ex_init4.c ).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;

  /* Avoid compiler warning. "arg" is the "NULL" in the nosInit()-call */
  (void) arg;

  /* create a semaphore, initialize to 0 */
  semaphore = posSemaCreate(0);

  if (semaphore == NULL)
  {
    nosPrint("Failed to create a semaphore!\n");
    return;
  }

  /* start a second task */
  t = nosTaskCreate(eventtask,  /* pointer to new task-function            */
                    NULL,       /* optional argument for the task-function */
                    2,          /* priority level of the new task          */
                    0,          /* stack size (0 = default size)           */
                    "task2");   /* optional name of the second task        */

  if (t == NULL)
  {
    nosPrint("Failed to start second task!\n");
  }

  /* endless loop: signal the event every two seconds */
  for(;;)
  {
    /* print some text */
    nosPrint("\nfirsttask: going to signal the event\n");

    /* signal the event semaphore (trigger the other task) */
    posSemaSignal(semaphore);

    /* sleep (=do nothing) for two seconds */
    posTaskSleep(MS(2000));
  }
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void eventtask(void *arg)
{
  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* wait for event */
    posSemaGet(semaphore);

    /* print some text */
    nosPrint("eventtask: got the event\n");
  }
}

