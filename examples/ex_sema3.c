/*
 *  pico]OS semaphore example 3
 *
 *  How to use a semaphore to limit access to a shared resource.
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
#if POSCFG_FEATURE_ATOMICVAR == 0
#error The feature POSCFG_FEATURE_ATOMICVAR is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void sharedResource(int tasknbr);
void task1(void *arg);
void task2(void *arg);
void task3(void *arg);


/* global variable: handle to semaphore */
POSSEMA_t   semaphore;

/* global variable: this is a task counter */
POSATOMIC_t  counter;



/* This function is called from up to three tasks.
 * The function contains a section of code that is protected
 * by a semphore, so that only a maximum number of tasks is allowed
 * to execute the code simultanously. In this example program
 * the semaphore is initialized to 2, the counter variable will
 * count between 1 and 2, and the output is printed by all three
 * tasks. Example printout:
 *   task2: counter = 2
 *   task3: counter = 1
 *   task1: counter = 2
 *   task3: counter = 2
 */
void sharedResource(int tasknbr)
{
  INT_t c;

  /* wait for shared resource */
  posSemaGet(semaphore);

  /* --------------------------------------------------- */

  /* increment global counter */
  posAtomicAdd(&counter, 1);

  /* waste some time (this allows other tasks
     to enter this section of code, too) */
  posTaskSleep(MS(800));

  /* Get and print current counter value.
   * The counter value shows the number of tasks
   * that are currently executing this section of code.
   */
  c = posAtomicGet(&counter);
  nosPrintf2("task%i: counter = %i\n", tasknbr, c);

  /* decrement global counter */
  posAtomicSub(&counter, 1);

  /* --------------------------------------------------- */

  /* quit access to shared resource */
  posSemaSignal(semaphore);
}



/* This function is executed by the first task.
 */
void task1(void *arg)
{
  int i = 0;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* call function */
    sharedResource(1);

    /* do something here and waste some time */
    posTaskSleep(MS(100 + i*20));
    if (++i == 5)  i = 0;
  }
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void task2(void *arg)
{
  int i = 5;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* call function */
    sharedResource(2);

    /* do something here and waste some time */
    posTaskSleep(MS(100 + i*20));
    if (++i == 11)  i = 0;
  }
}



/* This function is executed by the third task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void task3(void *arg)
{
  int i = 2;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* call function */
    sharedResource(3);

    /* do something here and waste some time */
    posTaskSleep(MS(100 + i*20));
    if (++i == 7)  i = 0;
  }
}



/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;

  posAtomicSet(&counter, 0);

  /* Create a semaphore, initialize to 2.
   * You may vary the initialization count between
   * 1 and 3 and observe the output of this program.
   *
   * The initialization semaphore count limits the number
   * of tasks that are allowed to access a shared resource
   * at the same time.
   */
  semaphore = posSemaCreate(2);

  if (semaphore == NULL)
  {
    nosPrint("Failed to create a semaphore!\n");
    return;
  }

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

  /* start a second task */
  t = nosTaskCreate(task3,      /* pointer to new task-function            */
                    NULL,       /* optional argument for the task-function */
                    3,          /* priority level of the new task          */
                    0,          /* stack size (0 = default size)           */
                    "task3");   /* optional name of the third task         */

  if (t == NULL)
  {
    nosPrint("Failed to start third task!\n");
  }

  /* continue execution in function task1 */
  task1(arg);
}
