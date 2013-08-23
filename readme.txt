
pico]OS readme.txt                                                 2005-01-15



                             INTRODUCTION


pico]OS is a  highly configurable  and very fast  real time operating system.
It  targets a wide  range of  architectures, from the  small 8 bit processors
with very low memory till huge architectures like 32 bit processors with lots
of memory.
Please see  the documentation  shipped  with this  package (html  help in the
directory doc/) for further information.



                            GETTING STARTED

There are two methods to work with pico]OS. The first is to build the pico]OS
library and link  it to your project that you may  develope with some kind of
integrated  compiler/debugger IDE.  The second method is  to use the makefile
system that is provided with pico]OS to build all you need - the pico]OS RTOS
library, application libararies and your main program.

Independend of  the way you go, you will need a working GNU make on your host
machine.   For  MS  Windows  platforms,   you   can  download   GNU  make  at
http://mingw.sourceforge.net/download.shtml



                        BUILD THE EXAMPLE PROGRAMS

When you  have a working GNU  make installed, you are  now ready to build the
example  programs  that are  shipped  with  pico]OS.  Assuming your host is a
MS Windows  machine and you have  MS Visual Studio 6 or the MinGW GCC package
installed,  you  can compile  the  RTOS for  MS Windows.  Simply  change into
the examples  directory  and enter  "make PORT=x86w32".  If you have  trouble
building  the  examples,  please read  the  readme file  in the  appropriated
port  directory  (eg. ports/x86w32/readme.txt).  You will find the  generated
executables in the directory out/x86w32/deb/.



                      HOW TO BUILD THE PICO]OS LIBRARY

To build  the pico]OS  realtime operating  system,  you need to  execute  the
makefile in the root directory. The makefiles were tested with GNU Make 3.80.

NOTE:  If your compiler is not yet supported by pico]OS, you will need to
       adapt the port makefile (port.mak) in the port subdirectory.

  The makefile takes two parameters:
    PORT  = name of the port to build (= the subdirectory name)
    BUILD = version to build, possible values are DEBUG and RELEASE

  Example:
    make PORT=avr BUILD=DEBUG

    Builds the Atmel AVR port and includes debug informations.
    The generated library can be found in the directory lib/avr/deb/

  Make targets:
    The makefile knows the targets 'all', 'clean' and 'docu', and
    'all' is the default.  'all' compiles the operating system.
    'clean' removes all generated binaries. 'docu' generates the
    html help with use of the doxygen tool.

  Hint:
    When the makefile is started, it searches for the configuration file
    'config.mak' in the pico]OS root directory.  You can put in there your
    command line. Example:

     #--- start of file ---
     PORT  = avr
     BUILD = DEBUG
     #---- end of file ----


Please report bugs to dennis_k@freenet.de
