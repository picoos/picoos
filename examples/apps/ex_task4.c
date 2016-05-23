/*
 *  pico]OS task example 4
 *
 *  How to create a new task by using the nano-layer.
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
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void secondtask(void *arg);



/* This function is executed by the first task that is started
 * by pico]OS ( see the nosInit()-call in main(), file ex_init4.c ).
 */
void firsttask(void *arg)
{
  NOSTASK_t  t;

  /* Avoid compiler warning. "arg" is the "NULL" in the nosInit()-call */
  (void) arg;

  nosPrint("First task started\n");

  /* start a second task */
  t = nosTaskCreate(secondtask, /* pointer to new task-function            */
                    "Task 2",   /* optional argument for the task-function */
                    2,          /* priority level of the new task          */
                    0,          /* stack size (0 = default size)           */
                    "task2");   /* optional name of the second task        */

  if (t == NULL)
  {
    nosPrint("Failed to start second task!\n");
  }

  for(;;)
  {
    /* print string: "Task 1" */
    nosPrint("Task 1\n");

    /* sleep (=do nothing) for one second */
    nosTaskSleep(MS(1000));
  }
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void secondtask(void *arg)
{
  char *name = (char*) arg;

  for(;;)
  {
    /* print string: "Task 2" */
    nosPrintf1("%s\n", name);

    /* sleep (=do nothing) for two seconds */
    nosTaskSleep(MS(2000));
  }
}

