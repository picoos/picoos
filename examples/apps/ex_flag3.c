/*
 *  pico]OS semaphore example 2
 *
 *  How to use a semaphore to protect a shared resource.
 *  (see also ex_mutx1.c)
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
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


/* method tanımları */
void incrementCounter(int tasknbr);
void task1(void *arg);
void task2(void *arg);
void task3(void *arg);

/* semaphore tanımı */
POSSEMA_t   semaphore = NULL;

/* semaphore seviyesinde erisilecek atomik degisken tanımı */
int  counter = 0;

/* bayrak tanımı */
POSFLAG_t   flagset;

/* sistemde kullanılacak en fazla gorev tanımı*/
int taskCount = 3;


/* This function is called from two tasks.
 * It increments and prints a global counter variable.
 * The function uses a semaphore to protect the shared variable,
 * so only one task is allowed to modify the variable at the time.
 */
void incrementCounter(int tasknbr) {
	
	/* gecisi degisken tanımın yapılması */
	int c;

	
	posSemaGet(semaphore);
	
	/* atomic degisken bilgisinin alınması */
	c = counter;

	/* indeks degerinin artırılması */
	c++;

	/* Isleme beklemesinin saglanmasi */
	posTaskSleep(MS(500)); 
  
	/* gecisi indes degerlin, atamic degere aktarılması */
	counter = c;

	/* gorev ve islem indeks degerinin ekranan basilması */  
	nosPrintf2("task%i: counter = %i\n", tasknbr, counter);
	
	/* bayrak durumuna degistirilme kontrolu */
	if(tasknbr == taskCount){
		tasknbr = 1;
	}
  
  	int tempOfIndexValue = tasknbr;
	
	tempOfIndexValue = tempOfIndexValue+1;
  
	/* Bayrak durumun güncellenmesini saglar */
	posFlagSet(flagset, tempOfIndexValue);
	
	/* bir sonraki gorev durumuna gecisi yapıtırı */
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
		
		nosPrintf1("\nfirsttask: going to signal flag %i\n", f);
		
		if(f==1){
			incrementCounter(1);
			posFlagSet(flagset, 2);
			nosPrintf1("flag value = %i\n",f);
			task2(arg);
		} 
		
		
		/* do something here and waste some time */
		posTaskSleep(MS(500));
	}
	
	posSemaSignal(semaphore);
	
}

void task2(void *arg)
{
	VAR_t f; // bayrak bilgisi tanımı
	
	(void) arg;

	
	for(;;) {
	
		// Gecerli bayrak durumunun alınması
		f = posFlagGet(flagset, POSFLAG_MODE_GETSINGLE);
		
		if(f==2){
			incrementCounter(2);
		}
		
		// Bayrak durumunun guncellenmesi
		posFlagSet(flagset, 3);
		nosPrintf1("flag value = %i\n",f);
		
		//Isleme beklemesinin saglanmasi
		posTaskSleep(MS(500));
	}
	
	// Bir sonra ki islem duruma gecisi
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
		nosPrintf1("flag value = %i\n",f);
		
		posTaskSleep(MS(500));
	}
	
	posSemaSignal(semaphore);
}

void firsttask(void *arg) {
	POSTASK_t  t;
	UVAR_t i;

	/* Ekranan indeks degeri yazılması amacı ile
	 * kullanılacak olan degiskenin sadece bir gorev kullanımında 
	 * sahiplenebilmesi icin ilgili tanımın yapılması
	 */
	posAtomicSet(&counter, 0);

	/* bayrak isleminin baslatilmasi */
	flagset = posFlagCreate();
  
	if (flagset == NULL)
	{
		nosPrint("Failed to create a set of flags!\n");
		return;
	}
  
  
	/* semaphore nesnesinin 1 degeri ile baslatilması */
	semaphore = posSemaCreate(1);

	if (semaphore == NULL){
		
		nosPrint("Failed to create a semaphore!\n");
		return;
	}

	t = nosTaskCreate(task2,    /* gorev methodunun pinter degeri 	*/
		NULL,					/* varsayılan method arguman degeri */
		1,						/* islem oncelik durumu          	*/
		0,
		"task2");				/* gorev isimi        				*/

	if (t == NULL) {
		nosPrint("Failed to start second task!\n");
	}

	/* ikinci gorev nesnesi */
	t = nosTaskCreate(task3,    /* gorev methodunun pinter degeri 	*/
                    NULL,       /* varsayılan method arguman degeri */
                    1,          /* islem oncelik durumu          	*/
                    0,          
                    "task3");   /* gorev isimi        				*/

	if (t == NULL) {
		nosPrint("Failed to start third task!\n");
	}

	/* ilk bayrak durumun belirlenmesi*/
	posFlagSet(flagset, 1);
	
	/*  task1 gorvein cagirilması */
	task1(arg);
}
