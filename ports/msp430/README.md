pico]OS port for Texas Instrument MSP430 CPUs
---------------------------------------------

This is a port of pico]OS to Texas Instruments MSP430 chips.
It has been tested with MSPGCC (from sourceforge) as compiler on following boards:

- Texas Instruments ez430-RF2500 (msp430f2274 + 2.4 Ghz radio)
- Texas Instruments msp430 launchpad (msp430g2533)
- Olimex MSP430-CCR (cc430f5137, includes sub 1Ghz-radio)
- Olimex MSP430-4619LCD (msp430fg4619)

Port includes:
- support nano layer conin/cinout for USCI and USART modules
- clock setup for basic clock (2274), ucs and fll+. ACLK can be
  clocked either from chrystal or VLO (if available). MCLK can
  be clocked from DCO, which can be stablized with FLL (if available)
  
  
Writing interrupt handlers
--------------------------

Interrupt handlers must be declared as naked functions.
Easiest way to do this is to use PORT_NAKED in function
declaration (it ensures also that frame pointer is not
allocated). Interrupt vector is set by normal
mspgcc syntax using \_\_attribute\_\_((interrupt(X))).

Registers must be saved by calling portSaveContext at
function entry and portRestoreContext before exit.

For example, UART handler could look like this:

    void PORT_NAKED 
    __attribute__((interrupt(USART1RX_VECTOR))) uartRxIrqHandler()
    {
      portSaveContext();
      c_pos_intEnter();

      // interrupt handler code here.

      c_pos_intExitQuick();
      portRestoreContext();
    }

