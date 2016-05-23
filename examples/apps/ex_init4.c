/*
 *  pico]OS initialization example 4
 *
 *  How to initialize the pico- and the nano- layer.
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
 */

#include <picoos.h>

#if POSCFG_ENABLE_NANO == 0
#error This example needs the nano layer!
#endif


/* setup memory for the heap that is managed by the nano layer */
#define HEAPSIZE 0x2000
static char membuf_g[HEAPSIZE];
void *__heap_start  = (void*) &membuf_g[0];
void *__heap_end    = (void*) &membuf_g[HEAPSIZE-1];


/* function prototype: first task function */
void firsttask(void *arg);


/* Program main function.
 * This function starts pico]OS by initializing the nano layer.
 */
int main(void)
{
  nosInit(firsttask, /* ptr to function: first task that is executed */
          NULL,      /* optional argument, not used here             */
          1,         /* priority of the first task                   */
          0,         /* stack size for the first task, 0 = default   */
          0);        /* stack size for the idle task, 0 = default    */

  /* Note: The call to nosInit() will never return */

  return 0;
}

