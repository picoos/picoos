pico]OS port form Texas Instrument MSP430 CPUs
----------------------------------------------

This is a port of pico]OS to Texas Instruments MSP430 chips.
It has been tested with MSPGCC (from sourceforge) as compiler on following boards:

- Texas Instruments ez430-RF2500 (msp430f2274 + 2.4 Ghz radio)
- Olimex MSP430-CCR (cc430f5137, includes sub 1Ghz-radio)
- Olimex MSP430-4619LCD (msp430fg4619)

Port includes:
- support nano layer conin/cinout for USCI and USART modules
- clock setup for basic clock (2274), ucs and fll+. ACLK can be
  clocked either from chrystal or VLO (if available). MCLK can
  be clocked from DCO, which can be stablized with FLL (if available)
  
  
