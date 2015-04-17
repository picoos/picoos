This is a port of pico]OS to NXP (formerly Philips) LPC2000 Arm cips.
It has been tested with GCC 4.8 on following boards:

- Olimex LPC-E2129 development board (see www.olimex.com)
- Embedded Artists Quickstart ethernet board (see www.embeddedartists.com)

Embedded Artists board is no longer available.

Toolchain is same as cortex-m port. Compared to my older
arm port, this is a rewrite for LPC2000 chips based on ideas
from cortex-m port.

- OpenOCD for my lpc2129 required:
  lpc2xxx.cfg: reset_config trst_and_srst srst_pulls_trst
  lpc2129.cfg: setup_lpc2xxx lpc2129 0x4f1f0f0f <-- hex value is different
  lpc2129.cfg: setup_lpc2129 14745 <-- crystal is different

Writing interrupt handlers
--------------------------

Interrupt handlers mimic Cortex-M port, with exception that
one should call portSaveContext() at beginning of interrupt handler and 
portRestoreContext before returning (leaving those out
would mean that task overflows wouldn't be checked).

Interrupt handlers must be registered in LPC2000 VIC.

For example, UART handler could look like this:

    void UARTx_Handler()
    {
      portSaveContext();
      c_pos_intEnter();

      // interrupt handler code here.
    
      c_pos_intExit();
      portRestoreContext();
    }
