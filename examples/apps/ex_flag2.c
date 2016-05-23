/*
 *  pico]OS flag example 2
 *
 *  How to use flags as bit events.
 *  This example demonstrates the mode POSFLAG_MODE_GETMASK.
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
#if POSCFG_FEATURE_FLAGS == 0
#error The feature POSCFG_FEATURE_FLAGS is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void eventtask(void *arg);


/* global variable: handle to a set of flags */
POSFLAG_t   flagset;


/* This function is executed by the first task that is started
 * by pico]OS ( see the nosInit()-call in main(), file ex_init4.c ).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;
  UVAR_t i;

  /* Avoid compiler warning. "arg" is the "NULL" in the nosInit()-call */
  (void) arg;

  /* create a set of flags */
  flagset = posFlagCreate();

  if (flagset == NULL)
  {
    nosPrint("Failed to create a set of flags!\n");
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

  /* endless loop: signal the flags every two seconds */
  i = 0;
  for(;;)
  {
    /* print some text */
    nosPrintf1("\nfirsttask: going to signal flag %i\n", i);

    /* signal the flag event (trigger the other task) */
    posFlagSet(flagset, i);

    /* increment flag number */
    if (++i > MVAR_BITS - 2)
      i = 0;

    /* sleep (=do nothing) for 500ms */
    posTaskSleep(MS(500));
  }
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void eventtask(void *arg)
{
  VAR_t f;
  int i;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* wait for flag to be signalled */
    f = posFlagGet(flagset, POSFLAG_MODE_GETMASK);

    if (f < 0)
    {
      nosPrint("Failed to get flag!\n");
    }
    else
    {
      nosPrintf1("eventtask: got flags: 0x%02x -", f);

      i = 0;
      while (f != 0)
      {
        if ((f & 1) != 0)
        {
          nosPrintf1(" %i", i);
        }
        i++;
        f >>= 1;
      }
      nosPrint("\n");
    }

    /* Wait some time. We do this because we want some flags
     * to be accumulated, so posGetFlag will really return a
     * bitfield of flags.
     */
    posTaskSleep(MS(1700));
  }
}

