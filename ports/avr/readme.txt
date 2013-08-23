AVR-Port of pico]OS
-------------------

The AVR port has been developed with WinAVR 20040404 (GCC 3.3.2 and avr-libc 1.0.3, 
see http://winavr.sourceforge.net). Until now the code has only been tested with real
hardware on the ATMEGA232. 
If you want use pico]OS on other AVR types, you only have to add a timer 
configuration on the file "timerdef.h" for the targe cpu. 


TODO:

- test the pico]os-features:
  Create programs for testing the features of the os (semaphores, mutex, 
  events, messages, ect.).

- malloc() and free():
  It shold be found a way to use malloc() and free() from avr-libc whithout 
  memory damage cause by a thread. First workaround can be a lock around the call of
  these functions.
  
- documentation and examples:
  Create pattern-like example programms to demonstrate the different features.
  It should provide an easy start for beginners by using pico]os and RTOS in general.
  
