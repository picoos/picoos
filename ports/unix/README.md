pico]OS port for unix systems
-----------------------------

This is a port of pico]OS to unix using ucontext(3) to implement
threads. It has been tested with FreeBSD operating system.

This is not intended for production use, but can be used
for testing and debugging.

Writing interrupt handlers
--------------------------

Interrupt handlers in unix are signal handers.
Writing them for ucontext threading is a little
bit complicated, it might be best to use
timerExpiredContext() and timerExpired(() functions
in arch_c.c as starting point.

