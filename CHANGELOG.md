# Change Log

## [1.2.0]
- add posTimerCallbackSet to create a callback timer instead of normal one.
  Callback is done from timer interrupt context with scheduler lock,
  which limits it's usefulness (causes easily jitter in interrupt response).
- fix registry allocation error when NOS_REGKEY_PREALLOC was set to 0.
- Fix tickless problems on CC430 chips caused by TAB23 chip errata.
- Add support for CMake as an alternative build system.

## [1.1.1]
- bug fixes to tickless idle
- make 6502 port work with modern cc65 compiler
- reorganize examples
- add tickless implementation for msp430 port
- add support for Nordic Semiconductor NRF51 chips
- add support for scheduler locking with NVIC instead of PRIMASK on Cortex-M0
- fix race condition with sleep and high priority Cortex-M interrupts, which
  resulted in infinite sleep.

## 1.1.0
- add power management api and support for tickless idle
- add tickless idle implementation for cortex-m stm32

## 1.0.4a
- add stdarg support to nosPrint -functions
- add C++ support to makefiles
- add some MSP430X support
- bug fixes and enhancements to cortex-m and msp430 ports
- add PIC32 port
  
## 1.0.4
- bugs fixed in these ports ARM Cortex-M, MSP430, Unix
- fixed some minor compilation issues, removed some compiler warnings
- fixed a problem with NOSCFG_FEATURE_SPRINTF configuration

## 1.0.3
- picoos Function c_pos_intExitQuick() added (required by Cortex-M port)
- new ports added ARM Cortex-M, MSP430, Unix
- Makefiles removed some garbage characters

## 1.0.2
- AVR port Memory mode 1 added (POSCFG_TASKSTACKTYPE = 1) for dynamic memory access.
- AVR port Support for the avr6 architecture with 3 byte PC added (for example ATmega2561,..)
- AVR port Timer mapping in timerdef.h added for several AVR devices.
- AVR port Save RAMPZ and EIND for MCUs with >=128 KB ROM.
  This is needed if ELPM/SPM and EICALL/EIJMP are used.
- MyCPU port improved, now faster context switching and better memory management
- Bug in function posTaskSetPriority() fixed

## 1.0.1
- define "POSCALL" added to every function prototype
- port specific functions p_pos_lock() / p_pos_unlock() added
- port for MyCPU (http//www.mycpu.eu) added

## 1.0.0
- ARM port re-structured, new processors added
- function nosMemRealloc() added
- fixed some compiler warnings in nano layer
- small configuration problem fixed in picoos.h
- small configuration problem fixed in x86w32 port
- ports can now be split into a common and a CPU specific part
- "HZ" definition moved from port.h to poscfg.h
- The x86w32-port does no more consume 100% CPU time when pico]OS is idle.

## 0.9.4
- fixed a NULL pointer write in function posListRemove()
- fixed a bug in nano layer initialization
- picoos core assertion macros added for better debugging
- x86w32 port new functions for simulated hardware interrupts added

## 0.9.3
- bug fix in nosTaskCreate() fixed a possible configuration problem
- makefiles changed, output directory structure is now selfexplaining
- makefiles extended to support a modularized software structure
- the win32 port does now support POSCFG_TASKSTACKTYPE 0/1/2
- void* types exchanged by structure pointers (makes the compiler more sensitive)
- configuration flag POSCFG_FEATURE_LISTLEN added
- more debugging features added

## 0.9.2
- debugging feature added  POSCFG_FEATURE_DEBUGHELP
- implemented a more flexible scheme to disable interrupts
- some small bug fixes in MS Windows 32Bit port
- some small bug fixes in the example code
- documentation updated, now easier for beginners

## 0.9.1
- bug fix jiffies, posCurrentTask_g, posNextTask_g,
  posInInterrupt_g, posRunning_g are now marked as "volatile"
- bug fix file compile.mak, DOS include path is now correctly converted
- bug fix in posListAdd() was the new list head not set (new->head=listhead)
- fix errno variable fixed for some external runtime library header files.
- directory structure changed Now there is a new subdirectory in the port
  directories. All config files have been moved into the subdir "defaults".
- MS Windows 32Bit port added. Now you are able to develope, test and
  debug your pico]OS programs with the MS Visual Studio IDE (for example).

## 0.9.0
- nono layer registry added, now support for named tasks/semas/timer etc.
- faster initialization of the CPU usage measurement on fast CPUs
- documentation updated
- minor changes
  - keyword 'extern' added to function prototypes
  - test for correct jiffies configuration added to n_core.c
  - NOS_FEATURE_CPUUSAGE renamed to NOSCFG_FEATURE_CPUUSAGE

## 0.8.1
- many new examples added to the examples directory
- port directory structure changed subdirectory 'boot' introduced
- Bottom Halfs can now be triggered from outside the pico]OS interrupt scope
- ARM port for SAMSUNG S3C2510A added (thanks Jun Li)
- AVR-Port 
  - Bugfix Set R0 in the frame for the ISR to Zero(0)
  - New Feature Added a dedicated stack for the interrupts.
  - New Feature Added a macro for generating a picoos compatible
    interrupt frame (PICOOS_SIGNAL).

## 0.8.0
- nano layer introduced. New features are
  - Bottom Halfs
  - Multitasking able console I/O
  - Multitasking able dynamic memory management
  - Standardized API for easier task creation
  - CPU usage measurement

## 0.7.0
- nonblocking/blocking lists added (lists can be used for packet queues)
- added user local storage in task control block
- some small bug fixes in dynamic memory management
- 6502 port supports now dynamic count of tasks

## 0.6.0
- atomic variables added
- posInstallIdleTaskHook returns now the ptr to the previous hook function
- error code names changed to resolve a conflict with Unix style error codes
- bug fixed in dynamic memory management, memory management improved

## 0.5.1
- added lookup-table for better bit-shift performance on 8bit CPUs
- several functions renamed, postfix alloc->create, free->destroy  
- feature names have changed accordingly to the function names
- added macro MS() that converts milliseconds into timer ticks
- added defined error codes, e.g. -EOK, -EFAIL, -ENOMEM and so on.

## 0.5.0
- this is the first public version
- Atmel AVR port added
- 6502 port findbit function improved
- functions posMessageAlloc/posMessageSend are now interrupt save 
- file poscfg.h split into poscfg.h and port.h

## 0.4.4
- message box functions renamed Msg -> Message
- function posInstallIdleTaskHook added
- function posIsTaskUnused renamed to posTaskUnused
- preprocessor directives corrected (unneeded braces removed)
- pico]OS core prepared for additional nano layer

## 0.4.3
- fixed some type writing errors (lower/upper case) in the sources
- some improvements (no bugs found)
- generic findbit (file fbit_gen.c) improved
- architecure port added the PowerPC IBM PPC440 is now supported

## 0.4.2
- documentation updated
- all prefixes 'os' changed to 'pos'

## 0.4.1
- software interrupts added
- bug fixed in round robin scheduling mechanism
- bug fixed in function os_sched_event
- bug fixed in function posTaskYield

## 0.4
- minor bugfixes
- make file support added

## 0.3.2
- filename changed core.c renamed to picoos.c
- the function p_pos_initArch is called from posInit to initialize the port
- minor bugfixes and improvements
- architecure port added  6502 compatible processors are now supported

## 0.3.1
- flags-functions added (= one bit semaphores / events)
- some code is put into separate subroutines for small code configuration
- bad bugs in the event system fixed
- license changed to the the modified BSD license
- doxygen documentation tags added

## 0.3
- sleep-timer lists can be double chained
- function renamed  posSemaWait to posSemaGet
- function added posSemaWait
- function added posMsgWait
- code can be defined to be small POSCFG_SMALLCODE
- jiffies counter variable added
- timer added
- internal data structures are now memory aligned
- much improvements and bugfixes

## 0.2
- MTYPE_t renamed to UMVAR_t
- signed type of MTYPE_t added  MVAR_t
- function return status is now 0 on success and -1 on error
- function p_pos_initTask() for TASKSTACKTYPE == 2 returns now a status
- semaphore functions now return an error status
- names of defines SYS_TASKTABSIZE_X and SYS_TASKTABSIZE_Y swapped
- code size improvements
- array posTaskTable_g[] is now one-dimensional
- posTaskCreate returns now a handle to the new task
- function added osGetCurrentTask
- function added posTaskUnused
- function added posTaskGetPriority
- function added posTaskSetPriority
- mutexes added
- message boxes added
- argument checking added
- much improvements and bugfixes

## 0.1
- Initial version
