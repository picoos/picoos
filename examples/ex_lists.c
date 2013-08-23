/*
 *  Copyright (c) 2004, Dennis Kuschel.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/**
 * @file    lists.c
 * @brief   list test program
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: ex_lists.c,v 1.2 2005/01/17 21:22:31 dkuschel Exp $
 */


/**
 *
 *  This file demonstrates the usage of the lists supported by pico]OS.
 *
 *  When you run this program, you will get this output:
 *  (tested with the 6502 port on a Commodore 64)
 *
 *  |  pico]OS 0.7.0  (c) 2004, Dennis Kuschel
 *  |  Demonstration on how to use lists
 *  |  
 *  |  List 1:
 *  |  1.element, data = 15
 *  |  2.element, data = 0
 *  |  3.element, data = 1
 *  |  4.element, data = 2
 *  |  5.element, data = 13
 *  |  6.element, data = 14
 *  |  Length of list 1 = 6
 *  |  
 *  |  List 2:
 *  |  1.element, data = 20
 *  |  2.element, data = 21
 *  |  3.element, data = 22
 *  |  4.element, data = 23
 *  |  Length of list 2 = 4
 *  |  
 *  |  Remove first element from list1:
 *  |  element.data = 15
 *  |  
 *  |  New list 1: <key>
 *  |  1.element, data = 20
 *  |  2.element, data = 21
 *  |  3.element, data = 22
 *  |  4.element, data = 23
 *  |  5.element, data = 0
 *  |  6.element, data = 1
 *  |  7.element, data = 2
 *  |  8.element, data = 13
 *  |  9.element, data = 14
 *  |  
 *  |  Add element to list 2, list 2 now:
 *  |  1.element, data = 15
 *  |  
 *  |  Move all elements with data < 10 from list 1 to list 2: <key>
 *  |  List 1 now:
 *  |  1.element, data = 20
 *  |  2.element, data = 21
 *  |  3.element, data = 22
 *  |  4.element, data = 23
 *  |  5.element, data = 13
 *  |  6.element, data = 14
 *  |  
 *  |  List 2 now:
 *  |  1.element, data = 15
 *  |  2.element, data = 0
 *  |  3.element, data = 1
 *  |  4.element, data = 2
 *  |  
 *  |  Starting a task that waits for and consumes buffers on list 1: <key>
 *  |  
 *  |  00212 list1_task: got buffer, data = 20
 *  |  00215 list1_task: got buffer, data = 21
 *  |  00218 list1_task: got buffer, data = 22
 *  |  00221 list1_task: got buffer, data = 23
 *  |  00223 list1_task: got buffer, data = 13
 *  |  00226 list1_task: got buffer, data = 14
 *  |  
 *  |  Add two elements to list 1: <key>
 *  |  00257 list1_task: got buffer, data = 33
 *  |  00260 list1_task: got buffer, data = 34
 *  |  
 *  |  Starting a task that waits for buffers on list 2,
 *  |  and forwards them to list 1: <key>
 *  |  00380 list2_task: got buffer, data = 15
 *  |  00383 list2_task: got buffer, data = 0
 *  |  00384 list1_task: got buffer, data = 15
 *  |  00386 list2_task: got buffer, data = 1
 *  |  00390 list1_task: got buffer, data = 0
 *  |  00395 list2_task: got buffer, data = 2
 *  |  00400 list1_task: got buffer, data = 1
 *  |  00459 list1_task: got buffer, data = 2
 *  |  00522 list2_task: timeout
 *  |  00583 list2_task: timeout
 *  |  00645 list2_task: timeout
 *  |  Putting a new buffer to list 2...
 *  |  00652 list2_task: got buffer, data = 44
 *  |  00655 list1_task: got buffer, data = 44
 *  |  00715 list2_task: timeout
 *  |  00777 list2_task: timeout
 *  |  list2_task terminated.
 *  |  -- end of demonstration --
 *
 */


#include <picoos.h>

/* STARTUP CODE */
#define HEAPSIZE 0x4000
static char membuf_g[HEAPSIZE];
void *__heap_start  = (void*) &membuf_g[0];
void *__heap_end    = (void*) &membuf_g[HEAPSIZE-1];
void task1(void *arg);
int main(void) { nosInit(task1, NULL, 2, 0, 0); return 0; }



/*---------------------------------------------------------------------------
 *  TYPEDEFS
 */

/* define a queue buffer */
typedef struct {
  POSLIST_t   list;  /* this member is the link to a list */
  UINT_t      data;
  UVAR_t      malloc;
  char        buffer[10];
} ELEMENT_t;



/*---------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 */

/* define two lists */
POSLISTHEAD_t   listhead1_g;
POSLISTHEAD_t   listhead2_g;

/* define a queue buffer element for demonstration */
ELEMENT_t       elem0;



/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 */

ELEMENT_t*  newElement(UINT_t data);
void        printListContent1(POSLISTHEAD_t *listhead);
void        printListContent2(POSLISTHEAD_t *listhead);
void        list1_task(void *arg);
void        list2_task(void *arg);



/*---------------------------------------------------------------------------
 *  FUNCTION IMPLEMENTATION
 */


/* This function generates a new queue buffer element
 * by allocating dynamic memory. The element is initialized
 * with user data.
 */
ELEMENT_t* newElement(UINT_t data)
{
  ELEMENT_t *e;
  e = (ELEMENT_t*) nosMemAlloc(sizeof(ELEMENT_t));
  if (e == NULL)
  {
    nosPrint("Error: Out of memory\n");
    for (;;);
  }
  e->data   = data;
  e->malloc = 1;
  return e;
}



/* Print the content of a list by using
 * the POSLIST_FOR_EACH_ENTRY -loop.
 */
void printListContent1(POSLISTHEAD_t *listhead)
{
  POSLIST_t *pl;
  ELEMENT_t *e;
  UVAR_t i;

  i = 0;
  POSLIST_FOR_EACH_ENTRY(listhead, pl)
  {
    i++;
    e = POSLIST_ELEMENT(pl, ELEMENT_t, list);
    nosPrintf2("%i.element, data = %i\n", i, e->data);
  }
}



/* Print the content of a list by using
 * the POSLIST_FOREACH_BEGIN / _END -loop.
 */
void printListContent2(POSLISTHEAD_t *listhead)
{
  ELEMENT_t *e;
  UVAR_t i;

  i = 0;
  POSLIST_FOREACH_BEGIN(listhead, e, ELEMENT_t, list)
  {
    i++;
    nosPrintf2("%i.element, data = %i\n", i, e->data);
  }
  POSLIST_FOREACH_END;
}



/* This task waits on list 1 and consumes
 * all elements (buffers) that are added to the list.
 */
void list1_task(void *arg)
{
  POSLIST_t *pl;
  ELEMENT_t *e;

  (void) arg;

  for (;;)
  {
    /* wait for a new element in list 1 */
    pl = posListGet(&listhead1_g, POSLIST_HEAD, INFINITE);
    if (pl == NULL)
    {
      nosPrintf1("%05u list2_task: error\n", jiffies);
      break;
    }

    /* print element */
    e = POSLIST_ELEMENT(pl, ELEMENT_t, list);
    nosPrintf2("%05u list1_task: got buffer, data = %i\n",
               jiffies, e->data);

    /* free memory of element */
    if (e->malloc != 0)
    {
      nosMemFree(e);
    }
  }
}



/* This task waits with timeout on list 2 and consumes
 * all elements (buffers) that are added to the list.
 * The elements are then added to the global list 1.
 */
void list2_task(void *arg)
{
  POSLIST_t *pl;
  ELEMENT_t *e;
  UVAR_t tocnt = 0;

  (void) arg;

  for (;;)
  {
    /* wait for a new element in list 2, timeout after 2sec */
    pl = posListGet(&listhead2_g, POSLIST_HEAD, MS(2000));
    if (pl == NULL)
    {
      nosPrintf1("%05u list2_task: timeout\n", jiffies);
      if (++tocnt == 6)
      {
        nosPrint("list2_task terminated.\n");
        nosPrint("-- end of demonstration --\n");
        break; /* quit after 6 timeouts */
      }
    }
    else
    {
      /* print element */
      e = POSLIST_ELEMENT(pl, ELEMENT_t, list);
      nosPrintf2("%05u list2_task: got buffer, data = %i\n",
                 jiffies, e->data);

      /* put element into list 1 */
      posListAdd(&listhead1_g, POSLIST_TAIL, &e->list);
    }
  }
}



/* main program
 */
void task1(void *arg)
{
  POSLIST_t *listel;
  ELEMENT_t *e;

  (void) arg;

  nosPrint("\n\n" POS_STARTUPSTRING "\n");
  nosPrint("Demonstration on how to use lists\n\n");


  /* initialize list heads */
  posListInit(&listhead1_g);
  posListInit(&listhead2_g);


  /* fill list 1 with some elements */

  elem0.data   = 0;
  elem0.malloc = 0;
  posListAdd(&listhead1_g, POSLIST_TAIL, &elem0.list);

  posListAdd(&listhead1_g, POSLIST_TAIL, &newElement(1)->list);
  posListAdd(&listhead1_g, POSLIST_TAIL, &newElement(2)->list);
  posListAdd(&listhead1_g, POSLIST_TAIL, &newElement(13)->list);
  posListAdd(&listhead1_g, POSLIST_TAIL, &newElement(14)->list);

  posListAdd(&listhead1_g, POSLIST_HEAD, &newElement(15)->list);


  /* fill list 2 with some elements */

  posListAdd(&listhead2_g, POSLIST_TAIL, &newElement(20)->list);
  posListAdd(&listhead2_g, POSLIST_TAIL, &newElement(21)->list);
  posListAdd(&listhead2_g, POSLIST_TAIL, &newElement(22)->list);
  posListAdd(&listhead2_g, POSLIST_TAIL, &newElement(23)->list);

  
  nosPrint("List 1:\n");
  printListContent1(&listhead1_g);
  nosPrintf1("Length of list 1 = %i\n", posListLen(&listhead1_g));
  nosPrint("\nList 2:\n");
  printListContent2(&listhead2_g);
  nosPrintf1("Length of list 2 = %i\n", posListLen(&listhead2_g));


  /* get first element from list 1 */

  nosPrint("\nRemove first element from list1:\n");
  listel = posListGet(&listhead1_g, POSLIST_HEAD, 0);
  nosPrintf1("element.data = %i\n",
             POSLIST_ELEMENT(listel, ELEMENT_t, list)->data);
  nosPrint("\nNew list 1: <key>\n");
  nosKeyGet();
  printListContent1(&listhead1_g);


  /* move content of list 2 to top of list 1 */

  nosPrint("\nMove list2 to top of list1:\n");
  posListJoin(&listhead1_g, POSLIST_HEAD, &listhead2_g);
  printListContent2(&listhead1_g);


  /* add element to list 2 */

  nosPrint("\nAdd element to list 2, list 2 now:\n");
  posListAdd(&listhead2_g, POSLIST_TAIL, listel);
  printListContent2(&listhead2_g);


  /* move all elements with data < 10 from list 1 to list 2 */

  nosPrint("\nMove all elements with data < 10 from list 1 to list 2: <key>\n");
  nosKeyGet();

  POSLIST_FOREACH_BEGIN(&listhead1_g, e, ELEMENT_t, list)
  {
    if (e->data < 10)
    {
      posListRemove(&e->list);
      posListAdd(&listhead2_g, POSLIST_TAIL, &e->list);
    }
  }
  POSLIST_FOREACH_END;

  nosPrint("List 1 now:\n");
  printListContent2(&listhead1_g);
  nosPrint("\nList 2 now:\n");
  printListContent2(&listhead2_g);


  /* start a task that waits on list 1 */
  nosPrint("\nStarting a task that waits for and consumes buffers on list 1: <key>\n");
  nosKeyGet();
  nosTaskCreate(list1_task, NULL, 1, 0, "list1_task");


  /* add one additional element to list 1 */
  posTaskSleep(MS(1000));
  nosPrint("\nAdd two elements to list 1: <key>\n");
  nosKeyGet();
  posListAdd(&listhead1_g, POSLIST_TAIL, &newElement(33)->list);
  posListAdd(&listhead1_g, POSLIST_TAIL, &newElement(34)->list);


  /* start a task that waits on list 2 with a timeout of 2 seconds */
  posTaskSleep(MS(1000));
  nosPrint("\nStarting a task that waits for buffers on list 2\n");
  nosPrint(" and forwards them to list 1: <key>\n");
  nosKeyGet();
  nosTaskCreate(list2_task, NULL, 1, 0, "list2_task");


  /* wait 9 seconds */
  posTaskSleep(MS(9000));


  /* put a new element to list 2 */
  nosPrint("Putting a new buffer to list 2...\n");
  posListAdd(&listhead2_g, POSLIST_TAIL, &newElement(44)->list);

}


