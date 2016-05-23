/*
 *  pico]OS software interrupt example 1
 *
 *  How to setup and use a software interrupt.
 *  (see also ex_bhalf.c)
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
#if POSCFG_FEATURE_SOFTINTS == 0
#error The feature POSCFG_FEATURE_SOFTINTS is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void task2(void *arg);
void softisr(UVAR_t arg);

/* global variables */
POSSEMA_t   event;
VAR_t       isrCounter = 0;



/* This is an example of an interrupt service routine.
 */
void softisr(UVAR_t arg)
{
  /* Avoid compiler warining. arg is the value
   * that was given to the call to posSoftInt.
   */
  (void) arg;

  /* Do something useful here.
   * Note that this function is running at interrupt level,
   * that means the ISR is not allowed to block, and it is not
   * allowed to call operating system functions that may block.
   * The only useful functions that may be called by this
   * routine are posSemaSignal, posFlagSet, posSoftInt and
   * posMessageAlloc/posMessageSend.
   *
   * An interrupt service routine must be as short as possible!
   * (Please see the bottom halfs as a variant of this example)
   *
   */

  /* increment interrupt counter */
  isrCounter++;

  /* signal an event */
  posSemaSignal(event);
}



/* This function is executed by the second task that is set up
 * in the function firsttask() by a call to nosTaskCreate()
 */
void task2(void *arg)
{
  POSSEMA_t  sem;

  /* The handle to the semaphore is passed as
   * argument to this task. Please see the call
   * to the function nosTaskCreate() for details.
   */
  sem = (POSSEMA_t) arg;

  for(;;)
  {
    /* Pend on semaphore.
     * The semaphore is signalled in the ISR.
     */
    posSemaGet(sem);

    /* We got a signal from the ISR.
     * Print now the counter.
     */
    nosPrintf1("interrupt signalled, counter = %i\n", isrCounter);
  }
}



/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;
  VAR_t      status;

  (void) arg;

  /* create a semaphore, initialize to 0 */
  event = posSemaCreate(0);

  if (event == NULL)
  {
    nosPrint("Failed to create a semaphore!\n");
    return;
  }

  /* start a second task */
  t = nosTaskCreate(task2,      /* pointer to new task-function            */
                    event,      /* optional argument for the task-function */
                    2,          /* priority level of the new task          */
                    0,          /* stack size (0 = default size)           */
                    "task2");   /* optional name of the second task        */

  if (t == NULL)
  {
    nosPrint("Failed to start second task!\n");
    return;
  }

  /* install software interrupt handler (no. 4) */
  status = posSoftIntSetHandler(4, softisr);

  if (status != E_OK)
  {
    nosPrint("Failed to install software interrupt handler!\n");
    return;
  }


  /* Signal the interrupt every second.
   * Usually, the software interrupt would be raised
   * asynchronously through eg. a hardware isr handler.
   */
  for(;;)
  {
    /* raise the software interrupt no. 4 */
    posSoftInt(4, 0);

    /* wait 1 second */
    posTaskSleep(MS(1000));
  }
}
