/*
 *  pico]OS bottom half example
 *
 *  How to use a bottom half interrupt handler.
 *  (see also ex_sint1.c)
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
#if NOSCFG_FEATURE_BOTTOMHALF == 0
#error The feature NOSCFG_FEATURE_BOTTOMHALF is not enabled!
#endif
#if POSCFG_FEATURE_SOFTINTS == 0
#error The feature POSCFG_FEATURE_SOFTINTS is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


/* function prototypes */
void bottomhalf(void *arg, UVAR_t bh);
void softisr(UVAR_t arg);

/* global variables */
VAR_t   intCounter = 0;



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
   * routine are posSemaSignal, posFlagSet, posSoftInt,
   * posMessageAlloc/posMessageSend and nosBottomHalfStart.
   *
   * An interrupt service routine must be as short as possible!
   *
   */


  /* start the bottom half number 2 */
  nosBottomHalfStart(2);
}



/* This function is an example for a bottom half.
 * Since the function runs at task level, it is allowed
 * to consume more processing time, since interrupts
 * are not blocked and can occure while this function
 * is executed.
 */
void bottomhalf(void *arg, UVAR_t bh)
{
  /* Avoid compiler warining. arg is the value
   * that was given to the call to nosBottomHalfRegister.
   */
  (void) arg;

  /* increment interrupt counter */
  intCounter++;

  /* print some text (this should usually not be done
     from within a bottom half!) */
  nosPrintf2("bottom half %i running, counter = %i\n", bh, intCounter);
}



/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  VAR_t      status;

  /* avoid compiler warning */
  (void) arg;

  /* register the bottom half number 2 */
  status = nosBottomHalfRegister(2, bottomhalf, NULL);

  if (status != E_OK)
  {
    nosPrint("Failed to install bottom half function!\n");
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
   * Usually, the interrupt would be raised
   * asynchronously.
   */
  for(;;)
  {
    /* raise the software interrupt no. 4 */
    posSoftInt(4, 0);

    /* wait 1 second */
    posTaskSleep(MS(1000));
  }
}
