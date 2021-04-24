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
#include <msp430.h>
#include <in430.h>
#include <string.h>
#include <stdbool.h>

static inline void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg);

/*
 * Initialize nano layer heap by overriding mspgcc __low_level_init().
 * Also disable watchdog here.
 */

#if POSCFG_ENABLE_NANO != 0
#if NOSCFG_FEATURE_MEMALLOC == 1 && NOSCFG_MEM_MANAGER_TYPE == 1
void *__heap_start;
void *__heap_end;
#endif
#endif

unsigned char *portIrqStack;

extern unsigned int _end[];
extern unsigned int __stack[];
void __pos_nos_malloc_init(void);

#if __GNUC__ == 4
__attribute__((naked, section(".init8"))) __attribute__ ((__optimize__("omit-frame-pointer"))) void __pos_nos_malloc_init()
#else
__attribute__((naked, section(".crt_0690pos_init"))) __attribute__ ((__optimize__("omit-frame-pointer"))) void __pos_nos_malloc_init()
#endif
{
  /*
   * Start heap after .bss segment, align it upwards.
   * Reserve IRQ stack at top of memory, heap end before it.
   */
  portIrqStack = (void*) (((unsigned int) __stack - PORTCFG_IRQ_STACK_SIZE) & ~(POSCFG_ALIGNMENT - 1));

#if POSCFG_ENABLE_NANO != 0
#if NOSCFG_FEATURE_MEMALLOC == 1 && NOSCFG_MEM_MANAGER_TYPE == 1
  __heap_end = (void*) (portIrqStack - 2);
  __heap_start = (void*) (((unsigned int) _end + POSCFG_ALIGNMENT) & ~(POSCFG_ALIGNMENT - 1));
#endif
#endif

#if POSCFG_ARGCHECK > 1

  /*
   * Fill unused portion of IRQ stack with PORT_STACK_MAGIC.
   */
#if __GNUC__ == 4
  register unsigned char* s = (unsigned char*) __read_stack_pointer() - 10; // Just to be sure not to overwrite anything
#else
  register unsigned char* s = (unsigned char*) __get_SP_register() - 10; // Just to be sure not to overwrite anything
#endif

  while (s >= portIrqStack) {

    WDTCTL = WDTPW + WDTCNTCL;
    *(s--) = PORT_STACK_MAGIC;
  }

  *s = 0; // Separator between lowest stack location and heap

#if POSCFG_ENABLE_NANO != 0
#if NOSCFG_FEATURE_MEMALLOC == 1 && NOSCFG_MEM_MANAGER_TYPE == 1

  s = (unsigned char*) __heap_start;
  while (s <= (unsigned char*) __heap_end) {

    WDTCTL = WDTPW + WDTCNTCL;
    *(s++) = 'H';
  }
#endif
#endif

#endif
}

/*
 * Initialize task stack frame. The layout must be same
 * as by context macros in arch_a_macros.h.
 */

static inline void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned int *stk, z;
  int r;

  /*
   * Get aligned stack pointer.
   */

  z = (unsigned int) stackPtr;
  z = z & ~(POSCFG_ALIGNMENT - 1);
  stk = (unsigned int *) z;

  /*
   * Put initial values to stack, including entry point address,
   * some detectable register values, status register (which
   * switches cpu to system mode during context switch) and
   * dummy place for exception stack pointer (see comments
   * assember files for this).
   */

  *(stk) = (unsigned int) posTaskExit; /* bottom               */
  *(--stk) = (unsigned int) funcptr;   /* Entry Point          */
  *(--stk) = (unsigned int) GIE;       /* Status reg           */

  /*
   * R15 is argument to function.
   */
  *(--stk) = (unsigned int) funcarg;

  for (r = 14; r >= 4; r--)
    *(--stk) = (unsigned int) r;       /* r14-r4               */


  task->stackptr = (struct PortMspStack *) stk;
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

  task->stackSize = stacksize;

#if POSCFG_ARGCHECK > 1
  memset(task->stack, PORT_STACK_MAGIC, stacksize);
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

#if PORTCFG_FIXED_STACK_SIZE < 30
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
 * Disable watchdog.
 */

  WDTCTL = WDTPW + WDTHOLD;

#if defined(__MSP430_HAS_BC2__) || \
    defined(__MSP430_HAS_UCS__) || \
    defined(__MSP430_HAS_UCS_RF__) || \
    defined(__MSP430_HAS_FLLPLUS__)

  portInitClock();

#else
#warning no suitable clock module
#endif

  portInitTimer();

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

  portInitConsole();

#endif
}

/*
 * Called by pico]OS to switch tasks when not serving interrupt.
 * Since we run tasks in system/user mode, "swi" instruction is
 * used to generate an exception to get into suitable mode
 * for context switching. 
 *
 * The actual switching is then performed by armSwiHandler.
 */

void PORT_NAKED p_pos_softContextSwitch(void)
{
  asm volatile ("push r2");
  __dint();

  portSaveContext();
  posCurrentTask_g = posNextTask_g;
  portRestoreContext();
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
  portRestoreContext();
}

/*
 * Called by pico]OS to start first task. Task
 * must be prepared by p_pos_initTask before calling this.
 */

void PORT_NAKED p_pos_startFirstContext()
{
  portRestoreContext();
}

void PORT_NAKED portRestoreContextImpl(void)
{
#if POSCFG_ARGCHECK > 1
  P_ASSERT("IStk", (portIrqStack[0] == PORT_STACK_MAGIC));
#endif

  if (posInInterrupt_g == 0) {

    asm volatile("mov %0, r1" : : "m"(posCurrentTask_g->stackptr) : "r1");
  }

#ifdef __MSP430X__
  asm volatile("popm.w #12, r15");
#else
  asm volatile("pop   r4  \n"
      "         pop   r5  \n"
      "         pop   r6  \n"
      "         pop   r7  \n"
      "         pop   r8  \n"
      "         pop   r9  \n"
      "         pop   r10 \n"
      "         pop   r11 \n"
      "         pop   r12 \n"
      "         pop   r13 \n"
      "         pop   r14 \n"
      "         pop   r15");
#endif

  asm volatile("reti");
}

#if POSCFG_FEATURE_POWER != 0

void p_pos_powerWakeup()
{
  posCurrentTask_g->stackptr->sr &= ~LPM4_bits; // Ensure CPU is active for next task
}

#ifndef PORTCFG_POWER_TICKLESS_MIN
#define PORTCFG_POWER_TICKLESS_MIN 10
#endif

void p_pos_powerSleep()
{
#if defined(__MSP430_HAS_UART1__) && (PORTCFG_CON_PERIPH == 2)
  if (!(U1TCTL & TXEPT) || !(U1TCTL & SSEL0)) { // Cannot stop SMCLK if USART is working

#if __GNUC__ == 4
    __bis_status_register(LPM0_bits | GIE);
#else
    __bis_SR_register(LPM0_bits | GIE);
#endif
    __dint();
    return;
  }
#endif

#if POSCFG_FEATURE_TICKLESS

  UVAR_t nextWake = c_pos_nextWakeup();
  bool restoreTick = false;

  if (nextWake >= PORTCFG_POWER_TICKLESS_MIN) {

    p_pos_powerTickSuspend(nextWake);
    restoreTick = true;
  }

#endif

#if __GNUC__ == 4
  __bis_status_register(LPM3_bits | GIE);
#else
  __bis_SR_register(LPM3_bits | GIE);
#endif
  __dint();

#if defined(__MSP430_HAS_UCS_RF__) || defined(__MSP430_HAS_UCS__)

  // UCS7 Errata workaround, FLL must be on for 3 * refclk cycles

#if PORTCFG_XT1_HZ > 0
  __delay_cycles((1000000L * PORTCFG_CPU_CLOCK_MHZ / PORTCFG_XT1_HZ) * 3);
#else
  __delay_cycles((1000000L * PORTCFG_CPU_CLOCK_MHZ / 32768) * 3);
#endif
#endif

#if POSCFG_FEATURE_TICKLESS

  if (restoreTick)
    p_pos_powerTickResume();

#endif

}

#endif

#ifdef HAVE_PLATFORM_ASSERT
void p_pos_assert(const char* text, const char *file, int line)
{
// Something fatal, stay here forever.
  __dint();
  while(1);
}
#endif
