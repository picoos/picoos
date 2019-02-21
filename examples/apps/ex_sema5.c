/*
 *  pico]OS flag example 
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *	@author: ibrahimatay
 */

/* Include source code for pico]OS
 * initialization with nano layer.
 */
#include "ex_init5.c"

/* we need some features to be enabled */
#if POSCFG_FEATURE_SLEEP == 0
#error The feature POSCFG_FEATURE_SLEEP is not enabled!
#endif
#if POSCFG_FEATURE_SEMAPHORES == 0
#error The feature POSCFG_FEATURE_SEMAPHORES is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif

/* methods defication */
void incrementCounter(int tasknbr);
void task1(void *arg);
void task2(void *arg);

/* semaphore defication */
POSSEMA_t   semaphore = NULL;

/*  variable to be atomic at semaphore level */
int  counter = 0;

/* This function is called from two tasks.
 * It increments and prints a global counter variable.
 * The function uses a semaphore to protect the shared variable,
 * so only one task is allowed to modify the variable at the time.
 */
void incrementCounter(int tasknbr) {

	/* temporary variable */
	int c;

	posSemaGet(semaphore);

	/* get the variable value*/
	c = counter;

	/* increase the temporary variable*/
	c++;

	/* wait the 500ms */
	posTaskSleep(MS(500));

	/* swap the variables */
	counter = c;

	/* print the status */ 
	nosPrintf("task%i: counter = %i\n", tasknbr, counter);
	
	/* next the task */
	posSemaSignal(semaphore);
}

/* This function is executed by the first task.
 */
void task1(void *arg)
{
  (void) arg;

	for(;;)
	{
		/* if the counter is divided by value 2, the resource is accessible*/
		if ((counter % 2) == 0) {
			incrementCounter(2);
		}
		
		posTaskSleep(MS(500));
	}
}

void task2(void *arg)
{
  (void) arg;

	for(;;)
	{
		/* if the counter is not divided by value 2, the resource is accessible*/
		if ((counter % 2) == !0) {
			incrementCounter(2);
		}
		
		/* task1 fonskiyonunda 5 kat daha uzun bekler*/
		posTaskSleep(MS(2500));
	}
}

/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  POSTASK_t  t;

	#if 1

	/* creating semaphore object with set one default value */
	semaphore = posSemaCreate(1);

	if (semaphore == NULL)
	{
		nosPrint("Failed to create a semaphore!\n");
		return;
	}

	#endif

	t = nosTaskCreate(task2,   /* ptr to function: first task that is executed */
		NULL,				   /* optional argument, not used here             */
		2,					   /* priority of the first task                   */
		0,					   /* stack size for the first task, 0 = default   */
		"task2");              /* task name      			  				   */

	if (t == NULL)
	{
		nosPrint("Failed to start second task!\n");
	}

    /* task1 handled */
 	task1(arg);
}
