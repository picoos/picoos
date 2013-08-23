/*
 *  pico]OS initialization example 1
 *
 *  How to initialize pico]OS with POSCFG_TASKSTACKTYPE set to 0.
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
 */

#include <picoos.h>

#if POSCFG_ENABLE_NANO != 0
#error This example must be compiled with nano layer option disabled!
#endif
#if POSCFG_TASKSTACKTYPE != 0
#error This example is for POSCFG_TASKSTACKTYPE == 0 only!
#endif


/* function prototypes */
void firsttask(void *arg);


/* setup stack memory for the first two tasks */

#define STACKSIZE_TASK1     0x1000
#define STACKSIZE_IDLETASK  0x0800

static char stack_task1[STACKSIZE_TASK1];
static char stack_idletask[STACKSIZE_IDLETASK];



/* Program main function.
 * This function starts pico]OS by initializing the pico layer.
 */
int main(void)
{
  char *sp1, *sp2;

  /* set up stack pointers (assuming stack grows from top to bottom) */
  sp1 = stack_task1    + STACKSIZE_TASK1    - 1;
  sp2 = stack_idletask + STACKSIZE_IDLETASK - 1;

  /* initialize pico]OS (pico layer only) */
  posInit(firsttask, /* ptr to function: first task that is executed */
          NULL,      /* optional argument, not used here             */
          1,         /* priority of the first task                   */
          sp1,       /* ptr to top of stack for the first task       */
          sp2);      /* ptr to top of stack for the idle task        */

  /* Note: The call to posInit() will never return */

  return 0;
}
