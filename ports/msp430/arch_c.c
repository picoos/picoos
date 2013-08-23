/*
 * Copyright (c) 2011, Ari Suutari, ari@suutari.iki.fi.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 2. Redistributions
 * in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution. 3. The name of the author may
 * not be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 * 
 * CVS-ID $Id: arch_c.c,v 1.91 2012/01/19 09:46:49 ari Exp $
 */

#define NANOINTERNAL
#define PORT_INTERNAL
#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

static inline void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg);
void timerIrqHandler(void);

/*
 * Initialize nano layer heap by overriding mspgcc __low_level_init().
 * Also disable watchdog here.
 */

#ifdef POSCFG_ENABLE_NANO
void *__heap_start;
void *__heap_end;
#endif

unsigned char *portIrqStack;

extern unsigned int _end[];
extern unsigned int __stack[];
void __pos_nos_malloc_init(void);

__attribute__((naked, section(".init8"))) void __pos_nos_malloc_init()
{
  /*
   * Start heap after .bss segment, align it upwards.
   * Reserve IRQ stack at top of memory, heap end before it.
   */
  portIrqStack = (void*) (((unsigned int) __stack - PORTCFG_IRQ_STACK_SIZE) & ~(POSCFG_ALIGNMENT - 1));

#ifdef POSCFG_ENABLE_NANO
  __heap_end = (void*) (portIrqStack - 2);
  __heap_start = (void*) (((unsigned int) _end + POSCFG_ALIGNMENT) & ~(POSCFG_ALIGNMENT - 1));
#endif

#if POSCFG_ARGCHECK > 1

  /*
   * Fill unused portion of IRQ stack with PORT_STACK_MAGIC.
   */
  register unsigned char* s = (unsigned char*) __read_stack_pointer() - 10; // Just to be sure not to overwrite anything

  while (s >= portIrqStack) {

    WDTCTL = WDTPW + WDTCNTCL;
    *(s--) = PORT_STACK_MAGIC;
  }

  *s = 0; // Separator between lowest stack location and heap

#ifdef POSCFG_ENABLE_NANO

  s = (unsigned char*) __heap_start;
  while (s <= (unsigned char*) __heap_end) {

    WDTCTL = WDTPW + WDTCNTCL;
    *(s++) = 'H';
  }
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
  *(--stk) = (unsigned int) funcptr;  /* Entry Point          */
  *(--stk) = (unsigned int) GIE;      /* Status reg           */

  for (r = 4; r <= 14; r++)
    *(--stk) = (unsigned int) r;      /* r4-r15               */

  /*
   * R15 is argument to function.
   */
  *(--stk) = (unsigned int) funcarg;

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

#else
#error "Error in configuration for the port (poscfg.h): POSCFG_TASKSTACKTYPE must be 0, 1 or 2"
#endif

/*
 * Initialize CPU pins, clock and console.
 */

void p_pos_initArch(void)
{
/*
 * XXX: Disable watchdog.
 */

  WDTCTL = WDTPW + WDTHOLD;

#if PORTCFG_BSP_INIT == 1

  portBspInit();

#endif

  portInitBoard();

#if defined(__MSP430_HAS_BC2__) || \
    defined(__MSP430_HAS_UCS__) || \
    defined(__MSP430_HAS_UCS_RF__) || \
    defined(__MSP430_HAS_FLLPLUS__)

  portInitClock();

#else
#warning no suitable clock module
#endif

#if defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_T0A5__) || defined(__MSP430_HAS_TA2__)

  TA0CTL = 0;                         // Stop timer.
  TA0CTL = TASSEL_1;                  // Use ACLK.
  TA0CTL |= TACLR;                    // Clear everything.

#if defined(PORTCFG_XT1_HZ) && PORTCFG_XT1_HZ > 0

  TA0CCR0 = PORTCFG_XT1_HZ / HZ;      // Using crystal XT1

#else

#if defined(__msp430x22x4) || defined(__msp430x22x2)

  TA0CCR0 = 12000 / HZ;      // VLO on msp430x2xx is 12 Khz

#elif defined(__cc430x513x)

  TA0CCR0 = 10000 / HZ;      // VLO on msp430x5xx is 10 Khz

#else

#warning VLO frequency unknown, assuming 12 Khz

  TA0CCR0 = 12000 / HZ;      // VLO on msp430x2xx is 12 Khz

#endif

#endif

  TA0CCTL0 = CCIE;                    // Interrupts ON.
  TA0CTL |= TACLR;                    // Startup clear.
  TA0CTL |= MC_1;                     // Up mode.

#else
#warning No TA0 timer, one required for ticks.
#endif

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

    posCurrentTask_g->stackptr->sr &= ~LPM4_bits;
    asm volatile("mov %0, r1" : : "m"(posCurrentTask_g->stackptr) : "r1");
  }

  asm volatile("pop   r15 \n"
      "         pop   r14 \n"
      "         pop   r13 \n"
      "         pop   r12 \n"
      "         pop   r11 \n"
      "         pop   r10 \n"
      "         pop   r9  \n"
      "         pop   r8  \n"
      "         pop   r7  \n"
      "         pop   r6  \n"
      "         pop   r5  \n"
      "         pop   r4  \n"
      "         reti");
}

void portIdleTaskHook()
{
#if defined(__MSP430_HAS_UART1__) && (PORTCFG_CON_PERIPH == 2)
  if (!(U1TCTL & TXEPT) || !(U1TCTL & SSEL0)) { // Cannot stop SMCLK if USART is working

    __bis_status_register(LPM0_bits);
    return;
  }
#endif

  __bis_status_register(LPM3_bits);
}

#if defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_TA2__)
void PORT_NAKED __attribute__((interrupt(TIMERA0_VECTOR))) timerIrqHandler()
#endif
#ifdef __MSP430_HAS_T0A5__
void PORT_NAKED __attribute__((interrupt(TIMER0_A0_VECTOR))) timerIrqHandler()
#endif
{
  portSaveContext();
  c_pos_intEnter();
  c_pos_timerInterrupt();
  c_pos_intExit();
  portRestoreContext();
}

#ifdef HAVE_PLATFORM_ASSERT
void p_pos_assert(const char* text, const char *file, int line)
{
// Something fatal, stay here forever.
  __dint();
  while(1);
}
#endif
