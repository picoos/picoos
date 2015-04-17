pico]OS port for Arm Cortex-M CPUs
----------------------------------

This is a port of pico]OS to arm cortex-m3 and cortex-m0 chips.
It has been tested with GCC 4.8 as compiler on following boards:

- Olimex STM32-H103 header board
- Olimex STM32-P152 prototype board
- Olimex LPC-P1343 prototype board
- Olimex EM-32G210F128-H header board
- NXP LPC1114 on breadboard (DIP28 package!)
- NXP LPC810 on breadboard (DIP8 package!)
- TI MSP432 Launchpad

It has also been tested with Keil simulator with STM32F103 (cortex-m3) and
NXP LPC1114 (cortex-m0) cpus.

Note that port works only with CPUs that have CMSIS library available.
There is a collection of cmsis libraries in my cmsis-ports library.

Your configuration directory (the one having poscfg.h header) must
have a header called pos_cmsis.h which should include main
CMSIS header file for your CPU (for example for stm32f10x it
will be #include "stm32f10x.h")

Cortex-M0 implementation is not highly elegant as SVC instruction
always traps through hard fault due to way interrupts are blocked.

Writing interrupt handlers
--------------------------

Interrupt handlers for Cortex-M are easy to write,
since hardware pushes some of the registers automatically
to interrupt stack. Remaining registers are saved
by function prologue if necessary.

Interrupt handler names must be declared in 
vector table. Common approach is to declare a weak
default handler for all interrupts in vector table
so all that is needed to do by application is to define
a function with correct name.

For example, UART handler could look like this:

    void UARTx_Handler()
    {
      c_pos_intEnter();

      // interrupt handler code here.

      c_pos_intExitQuick();
    }

