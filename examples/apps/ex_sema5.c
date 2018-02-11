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

/* fonskiyon tan�mlar� */
void incrementCounter(int tasknbr);
void task1(void *arg);
void task2(void *arg);

/* semaphore tan�m� */
POSSEMA_t   semaphore = NULL;

/* semaphore seviyesinde erisilecek atomik degisken tan�m� */
int  counter = 0;

/* This function is called from two tasks.
 * It increments and prints a global counter variable.
 * The function uses a semaphore to protect the shared variable,
 * so only one task is allowed to modify the variable at the time.
 */
void incrementCounter(int tasknbr) {

	/* gecisi degisken tan�m�n yap�lmas� */
	int c;

	posSemaGet(semaphore);

	/* atomic degisken bilgisinin al�nmas� */
	c = counter;

	/* indeks degerinin art�r�lmas� */
	c++;

	/* Isleme beklemesinin saglanmasi */
	posTaskSleep(MS(500));

	/* gecisi indes degerlin, atamic degere aktar�lmas� */
	counter = c;

	/* gorev ve islem indeks degerinin ekranan basilmas� */
	nosPrintf2("task%i: counter = %i\n", tasknbr, counter);
	
	/* bir sonraki gorev durumuna gecisi yap�t�r� */
	posSemaSignal(semaphore);
}

/* This function is executed by the first task.
 */
void task1(void *arg)
{
  (void) arg;

	for(;;)
	{
		/* E�er sayac degeri 2'ye tam bolunuyor 
		 * ise (yani cift ise) kaynak erisimi saglaniyor
		 */
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
		/* E�er sayac degeri 2'ye tam bolunmuyor
		*  ise (yani tek ise) kaynak erisimi saglaniyor
		*/
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

   /* semaphore nesnesinin 1 degeri ile baslatilmas� */
  semaphore = posSemaCreate(1);

  if (semaphore == NULL)
  {
    nosPrint("Failed to create a semaphore!\n");
    return;
  }

#endif

  /* ikinci gorev nesnesi */
  t = nosTaskCreate(task2,    /* gorev methodunun pinter degeri   */
	  NULL,					  /* varsay�lan method arguman degeri */
	  2,					  /* islem oncelik durumu          	  */
	  0,
	  "task2");               /* gorev isimi        			  */

  if (t == NULL)
  {
    nosPrint("Failed to start second task!\n");
  }

  /*  task1 gorvein cagirilmas� */
  task1(arg);
}
