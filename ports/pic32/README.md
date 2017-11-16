pico]OS port for Microchip PIC32 CPUs
-------------------------------------

This is a port of pico]OS to Microchip PIC32 (mips architecture) chips.

Following features are supported:

- Tick sources:
  - MIPS core timer
  - TIMER1
- UART2 as console

Port has been tested with breadboard setup
using PIC32MX250F128B chip with MPLAB X XC32 Compiler.

Writing interrupt handlers
--------------------------

Interrupt handlers can be written as documented in
XC32 compiler manual. However, if one wants to use
separate interrupt handler stack same way as handlers
in the port code do, function must be declared as naked function.
Easiest way to do this is to use PORT_NAKED in function
declaration. Interrupt vector is set by normal
XC32 syntax using \_\_attribute\_\_((vector(X), nomips16)).

Registers must be saved by calling portSaveContext at
function entry and portRestoreContext before exit.
portSaveContext enables also nested interrupts.

For example, UART handler could look like this:

    void  PORT_NAKED 
    __attribute__((vector(_UART1_VECTOR), nomips16)) void UARTx_Handler()
    {
      portSaveContext();
      c_pos_intEnter();

      // interrupt handler code here.

      c_pos_intExitQuick();
      portRestoreContext();
    }
