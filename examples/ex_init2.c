/*
 *  pico]OS initialization example 2
 *
 *  How to initialize pico]OS with POSCFG_TASKSTACKTYPE set to 1.
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
 */

#include <picoos.h>

#if POSCFG_ENABLE_NANO != 0
#error This example must be compiled with nano layer option disabled!
#endif
#if POSCFG_TASKSTACKTYPE != 1
#error This example is for POSCFG_TASKSTACKTYPE == 1 only!
#endif


/* function prototypes */
void firsttask(void *arg);



/* Program main function.
 * This function starts pico]OS by initializing the pico layer.
 */
int main(void)
{
  /* initialize pico]OS (pico layer only) */
  posInit(firsttask, /* ptr to function: first task that is executed */
          NULL,      /* optional argument, not used here             */
          1,         /* priority of the first task                   */
          0x1000,    /* stack size for the first task                */
          0x0800);   /* stack size for the idle task                 */

  /* Note: The call to posInit() will never return */

  return 0;
}
