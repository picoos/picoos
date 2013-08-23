/*
 *  pico]OS mutex example 1
 *
 *  How to use a mutex to protect a shared resource.
 *  (see also ex_sema2.c)
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
#if POSCFG_FEATURE_MUTEXES == 0
#error The feature POSCFG_FEATURE_MUTEXES is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void incrementCounter(int tasknbr);
void task1(void *arg);
void task2(void *arg);


/* global variable: handle to mutex */
POSMUTEX_t   mutex = NULL;

/* global variable: this resource is protected by the mutex */
int  counter = 0;



/* This function is called from two tasks.
 * It increments and prints a global counter variable.
 * The function uses a mutex to protect the shared variable,
 * so only one task is allowed to modify the variable at the time.
 */
void incrementCounter(int tasknbr)
{
  int c;

  /* wait for exclusive access to counter variable */
  posMutexLock(mutex);

  /* load variable */
  c = counter;

  /* change copy of variable */
  c++;

  /* waste some time */
  posTaskSleep(MS(800));

  /* store changed variable */
  counter = c;

  /* print some text */
  nosPrintf2("task%i: counter = %i\n", tasknbr, counter);

  /* quit exclusive access to the counter variable */
  posMutexUnlock(mutex);
}



/* This function is executed by the first task.
 */
void task1(void *arg)
{
  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* call function */
    incrementCounter(1);

    /* do something here and waste some time */
    posTaskSleep(MS(200));
  }
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void task2(void *arg)
{
  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* call function */
    incrementCounter(2);

    /* do something here and waste some time */
    posTaskSleep(MS(300));
  }
}



/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;

  /* Note: You may uncomment the source below by setting the #if to
   *       zero and then observe the output of this program.
   *       This is a nice example of how synchronization works.
   */
#if 1

  /* create a mutex */
  mutex = posMutexCreate();

  if (mutex == NULL)
  {
    nosPrint("Failed to create a mutex!\n");
    return;
  }

#endif

  /* start a second task */
  t = nosTaskCreate(task2,      /* pointer to new task-function            */
                    NULL,       /* optional argument for the task-function */
                    2,          /* priority level of the new task          */
                    0,          /* stack size (0 = default size)           */
                    "task2");   /* optional name of the second task        */

  if (t == NULL)
  {
    nosPrint("Failed to start second task!\n");
  }

  /* continue execution in function task1 */
  task1(arg);
}
