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
 * CVS-ID $Id: arch_c.c,v 1.67 2012/01/27 06:30:28 ari Exp $
 */

#define NANOINTERNAL
#define PORT_INTERNAL
#include <picoos.h>
#include <string.h>

#define SVC_START_FIRST_CONTEXT    0
#define SVC_SOFT_CONTEXT_SWITCH    1

static inline void constructStackFrame(POSTASK_t task, void* stackPtr, POSTASKFUNC_t funcptr, void *funcarg);
void timerIrqHandler(void);

#ifdef POSCFG_ENABLE_NANO
void *__heap_start;
void *__heap_end;
#endif

unsigned char *portIrqStack;

void __attribute__((weak)) Reset_Handler(void);
void __attribute__((weak)) SVC_Handler(void);
void __attribute__((weak)) PendSV_Handler(void);
void __attribute__((weak)) SysTick_Handler(void);
void __attribute__((weak)) HardFault_Handler(void);
void __attribute__((weak)) UsageFault_Handler(void);

void sysCall(unsigned int*);
void sysCallWrapper(unsigned int callerSp, unsigned int callerLr);
void hardFault(void);

extern unsigned int _end[];

extern unsigned int __stack[];
extern unsigned int __data_start[];
extern unsigned int __data_load_start[];
extern unsigned int _edata[];
extern unsigned int _etext[];
extern unsigned int __bss_start[];
extern unsigned int __bss_end[];

unsigned int * myvectors[] __attribute__ ((section(".vectors"))) =
{ (unsigned int *) __stack, // stack pointer
    (unsigned int *) Reset_Handler, // code entry point
    (unsigned int *) Reset_Handler, // NMI handler (not really)
    (unsigned int *) HardFault_Handler, // hard fault handler (let's hope not)
    (unsigned int *) Reset_Handler, // MemManage failt
    (unsigned int *) Reset_Handler, // Bus fault
    (unsigned int *) UsageFault_Handler, // Usage fault
    (unsigned int *) 0, // Reserved
    (unsigned int *) 0, // Reserved
    (unsigned int *) 0, // Reserved
    (unsigned int *) 0, // Reserved
    (unsigned int *) SVC_Handler, // SVC
    (unsigned int *) Reset_Handler, // Debug monitor
    (unsigned int *) 0, // Reserved
    (unsigned int *) PendSV_Handler, // Context switch
    (unsigned int *) SysTick_Handler };

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

#ifdef POSCFG_ENABLE_NANO
  __heap_end = (void*) (portIrqStack - 4);
  __heap_start = (void*) (((unsigned int) _end + POSCFG_ALIGNMENT) & ~(POSCFG_ALIGNMENT - 1));
#endif

#if POSCFG_ARGCHECK > 1

  /*
   * Fill unused portion of IRQ stack with PORT_STACK_MAGIC.
   */
  register uint32_t si      = __get_MSP() - 10; // Just to be sure not to overwrite anything
  register unsigned char* s = (unsigned char*) si;

  while (s >= portIrqStack)
  *(s--) = PORT_STACK_MAGIC;

  *s = 0;// Separator between lowest stack location and heap

#ifdef POSCFG_ENABLE_NANO

  s = (unsigned char*) __heap_start;
  while (s <= (unsigned char*) __heap_end)
  *(s++) = 'H';

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

  *(stk) = (unsigned int) 0x00000000; /* bottom     */
  *(--stk) = 0x01000000;              /* thumb      */
  *(--stk) = (unsigned int) funcptr; /* Entry point */
  *(--stk) = posTaskExit; /* LR */
  *(--stk) = 12;

  for (r = 3; r >= 1; r--)
    *(--stk) = r;

  *(--stk) = (unsigned int) funcarg;

  for (r = 11; r >= 4; r--)
    *(--stk) = r;

  *(--stk) = (unsigned int) 0; // basepri
  task->stackptr = (struct PortArmStack *) stk;
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

#else
#error "Error in configuration for the port (poscfg.h): POSCFG_TASKSTACKTYPE must be 0, 1 or 2"
#endif

/*
 * Initialize CPU pins, clock and console.
 */

void p_pos_initArch(void)
{
  SystemInit();
  SCB->CCR = SCB->CCR | 0x200; // Stack align ?

#if __CORTEX_M >= 3
  __set_BASEPRI(portCmsisPrio2HW(PORT_SVCALL_PRI + 1)); // Allow SVCall, but no Timer/PendSV
#else
  __disable_irq();
#endif

  //  portInitBoard();

  SysTick_Config(SystemCoreClock / HZ);

  NVIC_SetPriority(SVCall_IRQn, PORT_SVCALL_PRI);
  NVIC_SetPriority(SysTick_IRQn, PORT_SYSTICK_PRI);
  NVIC_SetPriority(PendSV_IRQn, PORT_PENDSV_PRI);

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1


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

void p_pos_softContextSwitch(void)
{
  asm volatile("svc 1");
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
 * Called when context switch should be marked pending,
 * but cannot be done immediately.
 */

void p_pos_intContextSwitchPending(void)
{
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/*
 * Called by pico]OS to start first task. Task
 * must be prepared by p_pos_initTask before calling this.
 */

void PORT_NAKED p_pos_startFirstContext()
{
  asm volatile("svc 0");
}

/*
 * Restore task context.
 */

void PORT_NAKED portRestoreContextImpl(void)
{
#if POSCFG_ARGCHECK > 1
  P_ASSERT("IStk", (portIrqStack[0] == PORT_STACK_MAGIC));
#endif

  /*
   * RestoreContext can happen only at lowest level of interrupt.
   * Thus it is safe to restore main stack pointer to initial value.
   * If not restored, stack would grow and grow as control
   * doesn't via same path as we got here.
   */
#if __CORTEX_M >= 3

  asm volatile("mov sp, %[initMsp]     \n"  // Return MSP to initial value
      "         ldr r0, %[newPsp]      \n"  // Get PSP for next task
      "         ldmia r0!, {r3-r11}    \n"  // Restore registers not handled by HW
      "         msr psp, r0            \n"  // Set PSP
      "         msr basepri, r3        \n"  // Adjust BASEPRI back to task's setting
      "         mvn lr, #2             \n"  // LR = 0xFFFFFFFD: Return to thread mode with PSP
      "         bx lr"
      : : [newPsp]"m"(posCurrentTask_g->stackptr), [initMsp]"r"(__stack));

#else

  asm volatile("mov sp, %[initMsp]     \n"  // Return MSP to initial value
      "         ldr r0, %[newPsp]      \n"  // Get PSP for next task
      "         ldmia r0!, {r3-r7}     \n"  // Restore registers
      "         msr primask, r3        \n"  // Restore PRIMASK to task's setting
      "         ldmia r0!, {r1-r2}     \n"  // Restore more ...
      "         mov r8, r1             \n"  // .. Cortex-m0 can only use low
      "         mov r9, r2             \n"  //    registers in ldmia/stmia
      "         ldmia r0!, {r1-r2}     \n"
      "         mov r10, r1            \n"
      "         mov r11, r2            \n"
      "         msr psp, r0            \n"  // Set PSP
      "         mov r0, #3             \n"
      "         neg r0, r0             \n"  // LR = 0xFFFFFFFD: Return to thread mode with PSP
      "         mov lr, r0             \n"
      "         bx lr"
      : : [newPsp]"m"(posCurrentTask_g->stackptr), [initMsp]"r"(__stack));

#endif
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
void sysCall(unsigned int* args)
{
  unsigned int callerLR = args[6];
  unsigned char* svcNumber = (unsigned char*)(callerLR - 2);
  unsigned int ctrl;

  switch (*svcNumber)
  {
  case SVC_SOFT_CONTEXT_SWITCH: // p_pos_softContextSwitch
    posCurrentTask_g = posNextTask_g;
    portRestoreContext();
    break;

  case SVC_START_FIRST_CONTEXT: // Start first context

    ctrl = __get_CONTROL();

    ctrl &= ~(1 << 0); // prileged thread mode
    ctrl |= (1 << 1); // use separate thread stack

    __set_CONTROL(ctrl);
    portRestoreContext(); // This will lower BASEPRI to 0 during restore
    break;
  }
}

/*
 * SVC handler wrapper, dig out arguments from correct stack.
 */

void PORT_NAKED SVC_Handler()
{
  asm volatile("mov   r1, #4         \n" // Check which stack was used by caller
      "         mov   r0, lr         \n"
      "         tst   r1, r0         \n"
      "         beq   .Lsv_msp");        // MSP was used

  portSaveContext();

  asm volatile("mrs   r0, psp        \n" // Get PSP into R0
      "         b     .Lsv_spdone    \n"
      ".Lsv_msp:                     \n"
      "         mrs   r0, msp        \n" // Get MSP into R0
      ".Lsv_spdone:                  \n"
      "         b sysCall");
}

void PORT_NAKED hardFault()
{
  __disable_irq();
  while(1);
}

void PORT_NAKED HardFault_Handler()
{
  // Cortex-0 does not have BASEPRI. Pico[OS has to use primask
  // to block interrupts, which causes SVC to fault into hard fault
  // handler. Check here if fault was caused by SVC and redirect
  // control to svc routines.

#if __CORTEX_M < 3

#ifdef CHECK_HFSR_AND_CFSR

  // If this was not forced fault, it cannot be caused by SVC instruction.
  // If CFSR is non-zero this is escalated from another fault.
  if ((SCB->HFSR & SCB_HFSR_FORCED_Msk) == 0 || SCB->CFSR != 0)
    asm volatile("b hardFault");

  // Clear forced bit from HFSR, it is sticky.

  SCB->HFSR |= SCB_HFSR_FORCED_Msk;

#endif

  asm volatile("movs  r1, #4         \n" // Check which stack was used by caller
      "         mov   r0, lr         \n"
      "         tst   r1, r0         \n"
      "         beq   .Lhf_msp");        // MSP in use

  portSaveContext();

  asm volatile("mrs   r0, psp        \n" // Get PSP into R0
      "         b     .Lhf_spdone    \n"
      ".Lhf_msp:                     \n"
      "         mrs   r0, msp        \n" // Get MSP into R0
      ".Lhf_spdone:                  \n"
      "         ldr   r1, [r0, #24]   \n"
      "         sub   r1, r1, #2     \n" // Calculate faulted instruction address
      "         ldrb  r2, [r1, #1]   \n" // Check for SVC instruction
      "         cmp   r2, #0xdf      \n" // SVC = 0xdf
      "         bne   .Lhf_nosvc     \n"
      "         b     sysCall        \n" // Jump to svc handler
      ".Lhf_nosvc:");
#endif

  asm volatile("b hardFault");
}

void UsageFault_Handler()
{
  __disable_irq();
  while(1);
}

/*
 * Handle delayed task context switch.
 */
void PORT_NAKED PendSV_Handler()
{
  portSaveContext();
  c_pos_intEnter();
  c_pos_intExit();
  portRestoreContext();
}

/*
 * Timer interrupt from SysTick.
 */
void SysTick_Handler()
{
  c_pos_intEnter();
  c_pos_timerInterrupt();
  c_pos_intExitQuick();
}

/*
 * Send character to debugger.
 */

#if NOSCFG_FEATURE_CONOUT == 1 && PORTCFG_CONOUT_ITM == 1

UVAR_t p_putchar(char ch)
{
  ITM_SendChar(ch);
  return 1;
}

#endif

#ifdef HAVE_PLATFORM_ASSERT
void p_pos_assert(const char* text, const char *file, int line)
{
// Something fatal, stay here forever.

  __disable_irq();
  while(1);
}
#endif
