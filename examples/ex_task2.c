/*
 *  pico]OS task example 2
 *
 *  How to create a new task by using the pico-layer.
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
 */

#include <stdio.h>


/* Include source code for pico]OS initialization
 * with POSCFG_TASKSTACKTYPE set to 1.
 */
#include "ex_init2.c"


/* we need some features to be enabled */
#if POSCFG_FEATURE_INHIBITSCHED == 0
#error The feature POSCFG_FEATURE_INHIBITSCHED is not enabled!
#endif
#if POSCFG_FEATURE_SLEEP == 0
#error The feature POSCFG_FEATURE_SLEEP is not enabled!
#endif


/* function prototypes */
void secondtask(void *arg);



/* This function is executed by the first task that is started
 * by pico]OS ( see the posInit()-call in main(), file ex_init2.c ).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;

  /* Avoid compiler warning. "arg" is the "NULL" in the posInit()-call */
  (void) arg;

  printf("First task started\n");

  /* start a second task */
  t = posTaskCreate(secondtask, /* pointer to new task-function            */
                    "Task 2",   /* optional argument for the task-function */
                    2,          /* priority level of the new task          */
                    0x1000);    /* stack size in bytes                     */

  if (t == NULL)
  {
    printf("Failed to start second task!\n");
  }

  for(;;)
  {
    /* print string: "Task 1" */
    posTaskSchedLock();
    printf("Task 1\n");
    posTaskSchedUnlock();

    /* sleep (=do nothing) for one second */
    posTaskSleep(MS(1000));
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
    posTaskSchedLock();
    printf("%s\n", name);
    posTaskSchedUnlock();

    /* sleep (=do nothing) for two seconds */
    posTaskSleep(MS(2000));
  }
}

