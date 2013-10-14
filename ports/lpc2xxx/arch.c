/*
 * Copyright (c) 2011-2013, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define NANOINTERNAL
#include <picoos.h>
#include <string.h>
#include "lpc_reg.h"

#define SYS_MODE  0x1f       // System Mode
#define THUMB_MODE  0x20     // Thumb Mode
#define SWI_SOFT_CONTEXT_SWITCH    1

#ifdef __thumb__

#define CPU_MODE  (SYS_MODE | THUMB_MODE)

#else

#define CPU_MODE  SYS_MODE

#endif

static inline void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg);
static void sysCall(unsigned int* args);

#if POSCFG_ENABLE_NANO != 0
#if NOSCFG_FEATURE_MEMALLOC == 1 && NOSCFG_MEM_MANAGER_TYPE == 1
void *__heap_start;
void *__heap_end;
#endif
#endif

unsigned char *portIrqStack;

void Reset_Handler(void);
void SWI_Handler(void);

extern unsigned int _end[];
extern unsigned int __stack[];
extern unsigned int __data_start[];
extern unsigned int __data_load_start[];
extern unsigned int _edata[];
extern unsigned int _etext[];
extern unsigned int __bss_start[];
extern unsigned int __bss_end[];

/*
 * Control gets here after reset.
 * Initialize C environment and heap.
 */

extern int main(void);
void Reset_Handler(void)
{
  unsigned int *src, *dst;

  /*
   *  Copy data section from flash to RAM
   */
  src = __data_load_start;
  dst = __data_start;
  while (dst < _edata)
    *dst++ = *src++;

  /*
   *  Clear the bss section
   */
  dst = __bss_start;
  while (dst < __bss_end)
    *dst++ = 0;

  /*
   * Start heap after .bss segment, align it upwards.
   * Reserve IRQ stack at top of memory, heap end before it.
   */
  portIrqStack = (void*) (((unsigned int) __stack - PORTCFG_IRQ_STACK_SIZE) & ~(POSCFG_ALIGNMENT - 1));

#if POSCFG_ENABLE_NANO != 0
#if NOSCFG_FEATURE_MEMALLOC == 1 && NOSCFG_MEM_MANAGER_TYPE == 1
  __heap_end = (void*) (portIrqStack - 4);
  __heap_start = (void*) (((unsigned int) _end + POSCFG_ALIGNMENT) & ~(POSCFG_ALIGNMENT - 1));
#endif
#endif

#if POSCFG_ARGCHECK > 1

  /*
   * Fill unused portion of IRQ stack with PORT_STACK_MAGIC.
   */
  register uint32_t sp asm("sp");
  register uint32_t si      = sp - 10; // Just to be sure not to overwrite anything
  register unsigned char* s = (unsigned char*) si;

  while (s >= portIrqStack)
  *(s--) = PORT_STACK_MAGIC;

  *s = 0;// Separator between lowest stack location and heap

#if POSCFG_ENABLE_NANO != 0
#if NOSCFG_FEATURE_MEMALLOC == 1 && NOSCFG_MEM_MANAGER_TYPE == 1

  s = (unsigned char*) __heap_start;
  while (s <= (unsigned char*) __heap_end)
  *(s++) = 'H';

#endif
#endif
#endif

  main();
  while (1)
    ;
}

/*
 * Initialize task stack frame. The layout must be same
 * as by context macros in arch_a_macros.h.
 */

static inline void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned int   *stk, z;
  int r;

/*
 * Get aligned stack pointer, reserve 32 bytes.
 */

  z = (unsigned int)stackPtr;
  z = (z - POSCFG_ALIGNMENT) & ~(POSCFG_ALIGNMENT - 1);
  stk = (unsigned int *)z;

/*
 * Put initial values to stack, including entry point address,
 * some detectable register values, status register (which
 * switches cpu to system mode during context switch) and
 * dummy place for exception stack pointer (see comments
 * assember files for this).
 */

  *(stk) = (unsigned int)0x00000000;    /* bottom               */
  *(--stk) = (unsigned int)funcptr;     /* Entry Point          */
  *(--stk) = (unsigned int)posTaskExit; /* lr                   */

  for (r = 12; r >= 1; r--)
    *(--stk) = r;

  *(--stk) = (unsigned int)funcarg;     /* r0 : argument        */
  *(--stk) = (unsigned int)CPU_MODE;    /* CPSR                 */

  task->stackptr = (void *)stk;
}

/*
 * Initialize task context.
 */

#if (POSCFG_TASKSTACKTYPE == 1)

VAR_t p_pos_initTask(POSTASK_t task, UINT_t stacksize, POSTASKFUNC_t funcptr, void *funcarg)
{

  unsigned int z;

  task->stack = NOS_MEM_ALLOC(stacksize);
  if (task->stack == NULL)
  return -1;

#if POSCFG_ARGCHECK > 1
  nosMemSet(task->stack, PORT_STACK_MAGIC, stacksize);
#endif

  z = (unsigned int) task->stack + stacksize - 2;
  constructStackFrame(task, (void*) z, funcptr, funcarg);
  return 0;
}

void p_pos_freeStack(POSTASK_t task)
{
  NOS_MEM_FREE(task->stack);
}

#elif (POSCFG_TASKSTACKTYPE == 2)

#if PORTCFG_FIXED_STACK_SIZE < 256
#error fixed stack size too small
#endif

VAR_t p_pos_initTask(POSTASK_t task,
    POSTASKFUNC_t funcptr,
    void *funcarg)
{
  unsigned int z;

#if POSCFG_ARGCHECK > 1
  memset(task->stack, PORT_STACK_MAGIC, PORTCFG_FIXED_STACK_SIZE);
#endif
  z = (unsigned int)task->stack + PORTCFG_FIXED_STACK_SIZE - 2;
  constructStackFrame(task, (void*)z, funcptr, funcarg);
  return 0;
}

void p_pos_freeStack(POSTASK_t task)
{
  (void)task;
}

#else
#error "Error in configuration for the port (poscfg.h): POSCFG_TASKSTACKTYPE must be 0, 1 or 2"
#endif

/*
 * Initialize CPU pins, clock and console.
 */

void p_pos_initArch(void)
{
/*
 * First, put CPU pins to known state.
 */

  PCB_PINSEL0 = PCB_PINSEL0_ALL_GPIO;
  PCB_PINSEL1 = PCB_PINSEL1_ALL_GPIO;

  GPIO0_IOSET = 0x00000000;
  GPIO0_IOCLR = 0x00000000;
  GPIO0_IODIR = 0x00000000;

/*
 * Configure pins for UART 0 (used as console)
 */

  PCB_PINSEL0 |= PCB_PINSEL0_P00_TXD0 | PCB_PINSEL0_P01_RXD0; /* Enable Rs232 RX & TX */

/*
 * Configure PLL so that crystal frequency is multiplied by 4.
 */

  SCB_PLLCFG = SCB_PLLCFG_MUL4;

  SCB_PLLCON  = SCB_PLLCON_PLLE;
  SCB_PLLFEED = SCB_PLLFEED_FEED1;
  SCB_PLLFEED = SCB_PLLFEED_FEED2;

  while (!(SCB_PLLSTAT & SCB_PLLSTAT_PLOCK)); /* Wait for PLL to lock */

  SCB_PLLCON = SCB_PLLCON_MASK;
  SCB_PLLFEED = SCB_PLLFEED_FEED1;
  SCB_PLLFEED = SCB_PLLFEED_FEED2;

/*
 * LPC chips don't have cache, but they have this "MAM".
 * Enable it.
 */

  MAM_TIM = MAM_TIM_3;
  MAM_CR  = MAM_CR_FULL;

/*
 * Make periphral bus clock run same speed as CPU clock
 */

  SCB_VPBDIV = SCB_VPBDIV_100;

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

  portInitUart();

#endif

  portInitTimer();
}

/*
 * Called by pico]OS to switch tasks when not serving interrupt.
 * Since we run tasks in system/user mode, "swi" instruction is
 * used to generate an exception to get into suitable mode
 * for context switching. 
 *
 * The actual switching is then performed by armSwiHandler.
 */

void p_pos_softContextSwitch(void)
{
  asm volatile("swi #1");
}

/*
 * Called by pico]OS at end of interrupt handler to switch task.
 * Before switching from current to next task it uses
 * current task stack to restore exception mode stack pointer
 * (which was saved by saveContext macro).
 * After switching task pointers the new task's context is simply restored
 * to get it running.
 */

void PORT_NAKED p_pos_intContextSwitch(void)
{
  posCurrentTask_g = posNextTask_g;
  asm volatile ("bl portRestoreContextImpl");
}

/*
 * Called by pico]OS to start first task. Task
 * must be prepared by p_pos_initTask before calling this.
 */

void PORT_NAKED p_pos_startFirstContext()
{
  asm volatile ("bl portRestoreContextImpl");
}


/*
 * Nothing to do, put CPU to sleep.
 */
void portIdleTaskHook()
{
  // __bis_status_register(LPM3_bits);
}

/*
 * Handle SVC (System call). Used for starting first task and soft context switch.
 */
static void sysCall(unsigned int* args)
{
  int svcNumber;

#ifdef __thumb__

  unsigned short* swiInstr = (unsigned short*) (posCurrentTask_g->stackptr->pc - 2);

  svcNumber = (*swiInstr) & 0xff;

#else

  unsigned int* swiInstr = (unsigned int*) (posCurrentTask_g->stackptr->pc - 4);

  svcNumber = (*swiInstr) & 0xffffff;

#endif

  switch (svcNumber)
  {
  case SWI_SOFT_CONTEXT_SWITCH: // p_pos_softContextSwitch
    posCurrentTask_g = posNextTask_g;
    break;

  }
}

/*
 * SWI handler wrapper, dig out arguments.
 */

void SWI_Handler()
{
  sysCall((unsigned int*)posCurrentTask_g->stackptr);
}

#ifdef HAVE_PLATFORM_ASSERT

static char* xtoa(int val)
{
  static char buf[32] = { 0 };
  int i = 30;

  for (; val && i; --i, val /= 10)

    buf[i] = "0123456789abcdef"[val % 10];

  return &buf[i + 1];
}

void p_pos_assert(const char* text, const char *file, int line)
{
// Something fatal, stay here forever.

  portEnterCritical();

  while (*text) {

    while (!(UART0_LSR & 0x20));
    UART0_THR = *text;
    ++text;
  }

  while (*file) {

    while (!(UART0_LSR & 0x20));
    UART0_THR = *file;
    ++file;
  }

  char* l = xtoa(line);
  while (*l) {

    while (!(UART0_LSR & 0x20));
    UART0_THR = *l;
    ++l;
  }

  while(1);
}
#endif
