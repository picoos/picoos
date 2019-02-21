/*
 *  pico]OS flag example 
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *	@author: ibrahimatay
 */


/* Include source code for pico]OS
 * initialization with nano layer.
 */
#include "ex_flag3_init.c"

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
void task3(void *arg);

/* semaphore defication */
POSSEMA_t   semaphore = NULL;

/*  variable to be atomic at semaphore level */
int  counter = 0;

/* flag variable */
POSFLAG_t   flagset;

/* task counter varible */
int taskCount = 3;


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
	
	/* flag state change control */
	if(tasknbr == taskCount){
		tasknbr = 1;
	}
  
  	int tempOfIndexValue = tasknbr;
	
	tempOfIndexValue = tempOfIndexValue+1;
  
	/* flag updated */
	posFlagSet(flagset, tempOfIndexValue);
	
	/* next the semaphore */
	posSemaSignal(semaphore);
}

/* This function is executed by the first task.
 */
void task1(void *arg)
{
	VAR_t f;
	
	/* avoid compiler warning */
	(void) arg;

	for(;;) {
	
		f = posFlagGet(flagset, POSFLAG_MODE_GETSINGLE);
		
		nosPrintf("\nfirsttask: going to signal flag %i\n", f);
		
		if(f==1){
			incrementCounter(1);
			posFlagSet(flagset, 2);
			nosPrintf("flag value = %i\n",f);
			task2(arg);
		} 
		
		
		/* do something here and waste some time */
		posTaskSleep(MS(500));
	}
	
	posSemaSignal(semaphore);
	
}

void task2(void *arg)
{
	VAR_t f; // flag definition variable
	
	(void) arg;

	for(;;) {
	
		// get the flag status
		f = posFlagGet(flagset, POSFLAG_MODE_GETSINGLE);
		
		if(f==2){
			incrementCounter(2);
		}
		
		/* flag status update */
		posFlagSet(flagset, 3);
		nosPrintf("flag value = %i\n",f);
		
		/* do something here and waste some time */
		posTaskSleep(MS(500));
	}
	
	// next the task
	posSemaSignal(semaphore);
}

void task3(void *arg)
{
	VAR_t f;
	
	(void) arg;
		
	for(;;) {
	
		f = posFlagGet(flagset, POSFLAG_MODE_GETSINGLE);
		
		if(f==3){
			incrementCounter(3);
		}
			
		posFlagSet(flagset, 1);
		nosPrintf("flag value = %i\n",f);
		
		posTaskSleep(MS(500));
	}
	
	posSemaSignal(semaphore);
}

void firsttask(void *arg) {
	POSTASK_t  t;
	UVAR_t i;

	posAtomicSet(&counter, 0);

	/* creating the flag */
	flagset = posFlagCreate();
  
	if (flagset == NULL)
	{
		nosPrint("Failed to create a set of flags!\n");
		return;
	}

	/* creating semaphore object with set one default value */
	semaphore = posSemaCreate(1);

	if (semaphore == NULL){
		
		nosPrint("Failed to create a semaphore!\n");
		return;
	}

	t = nosTaskCreate(task2,    /* ptr to function:  task2 that is executed 	*/
		NULL,					/* optional argument, not used here             */
		1,						/* priority of the first task                   */
		0,						/* stack size for the first task, 0 = default   */
		"task2");				/* task name       								*/

	if (t == NULL) {
		nosPrint("Failed to start second task!\n");
	}

	t = nosTaskCreate(task3,    /* ptr to function: task3 that is executed */
                    NULL,       /* optional argument, not used here             */
                    1,          /* priority of the first task                   */
                    0,          /* stack size for the first task, 0 = default   */
                    "task3");   /* task name       				*/

	if (t == NULL) {
		nosPrint("Failed to start third task!\n");
	}

	/* first flag status change */
	posFlagSet(flagset, 1);
	
  	/* task1 handled */
	task1(arg);
}
