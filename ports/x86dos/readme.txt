
In this directory are three files missing:

   port.mak,  port.h  and  poscfg.h

For an example how this files may look like, please see the 6502 port.


missing port.mak:

The DOS port can not be compiled with make files, because the
tested compilers (Borland C++ 3.1 and FreeWatcom) have a non
standarized, old style command syntax that is not compatible
with generic make files. Also the length of a DOS command line
is limited, so that not all make commands fit into one line.

To build the DOS port, a DOS batch file is provided that is able to
compile the port with the Borland compiler:  ports/x86dos/test/build.bat


missing port.h and poscfg.h:

I am using the DOS port as reference. The DOS port includes the
configuration file directly from the examples directory.

