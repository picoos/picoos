
          ---===  pico]OS MS Windows 32Bit port  ===---


This port enables the use of pico]OS on MS Windows based host machines.
In general, all 32Bit Microsoft Windows versions are supported. But there
is still a minor problem with the Windows 9X group of operating systems:
In contrast to the Windows NT based operating systems, the systems
Windows 95, Windows 98 and Windows ME provide really bad multitasking.
This may lead to an unpredictable behaviour of this port. In most cases
all goes fine (especially when the Windows 95 host is fresh booted :-))
but in rare cases the port will screw up. So I recommend you to use this
port only with Windows NT 4.0, Windows 2000, Windows XP or newer systems.

This port is based on Windows threads, but it supports the full pico]OS
scheduling scheme. This does not mean that the port turns MS Windows into
a realtime operating system! MS Windows is not, and will never be a
realtime operating system. This port is thought to help you developing
and testing your pico]OS application, maybe by using the integrated
MS Visual Studio development environment (IDE).

Some hints on the compilers:
This port supports MS Visual C++ v5.0 / v6.0, and the MinGW GCC suite.

1. MinGW / GCC

  If you are using MinGW GCC, you will need to change a line in the
  makefile port.mak. Search in the file for "GCC_DIR" and set this
  variable to your installation path of MinGW. Alternatively, you can
  set this variable in the "make"-call on the command line.

  Quick start guide for MinGW:

    1. Download the MinGW-GCC and MinGW-Make binaries from:
       http://mingw.sourceforge.net/download.shtml
       (search for MinGW-x.y.z.exe and mingw32-make-x.y.z.exe)

    2. Install this two packages on your MS Windows machine.

    3. Modify the file port.mak in this directory:
       Search in the file for "GCC_DIR" and set this variable
       to your installation path of the MinGW GCC compiler.

    4. Open a command box, and change into the directory
       picoos-x.y.z/examples

    5. Type "make PORT=x86w32 COMPILER=GCC"
       If this does not work properly:
       Make sure that the call to "make" really executes the GNU-make
       you have installed in step 2. You may need to change the
       order in the search-path of your MS Windows installation.

    6. Now you will find a couple of executables in the
       directory picoos-x.y.z/out/x86w32/deb

    7. For your own projects, please see the example in
       picoos-x.y.z/ports/x86w32/test


2. MS Visual Studio V5.0 and V6.0:

  If you are using MS Visual C++, you need to execute the batchfile
  "VCVARS32.BAT" that is stored in the installation path of the Visual
  Studio IDE. This batchfile sets the environment variables that are
  needed to be able to execute the compiler from the command line.

  Quick start guide for the MS Visual Studio IDE:

    1. Download the MinGW-Make binary from:
       http://mingw.sourceforge.net/download.shtml
       (search for mingw32-make-x.y.z.exe).
       Install this package.

    2. Open a command box, and change into the directory
       picoos-x.y.z/examples

    3. Execute the batchfile "VCVARS32.BAT". You may need to search
       your Visual Studio installation path for that batch file. If
       you have found the file, make a copy of it to a place where
       it is easy to access (eg. copy the batchfile to the directory
       C:\Windows or C:\WINNT).

    4. Type "make PORT=x86w32"
       (if this does not work, try "make PORT=x86w32 COMPILER=VC6")
       Make sure that the call to "make" really executes the GNU-make
       you have installed in step 1. You may need to change the
       order in the search-path of your MS Windows installation.

    5. Now you will find a couple of executables in the
       directory picoos-x.y.z/out/x86w32/deb

    6. For your own projects, please see the example in
       picoos-x.y.z/ports/x86w32/test

  The debugging of a pico]OS application with the integrated
  Visual Studio IDE is also really easy:

    1. Start the MS Visual Studio IDE.

    2. Create a new project, click on File->New..., click on tab Project,
       and mark "Win32 console application" (the last entry in the list).
       Choose a project name, and a place to store the project. I prefer
       a place in the picoos directory tree.

    3. If the new (and still empty) project is create, please press ALT+F7,
       choose the tab "linker", and set the output path (the path to the
       executable and the filename) as follows (example):
       C:/picoos-0.9.1/out/x86w32/deb/test.exe

    4. Even you do not need it, you may now add some source files
       to the project (this is usefull to set breakpoints):
         picoos-x.x.x/examples/*.c
         picoos-x.x.x/inc/*.h
         picoos-x.x.x/src/nano/*.c
         picoos-x.x.x/src/pico/*.c
         picoos-x.x.x/src/ports/x86w32/*.c

    5. To start the program in the context of the debugger, press F5.
       The IDE will ask you to recompile the project. Please select "NO".

    6. The program is running in the context of the debugger.
       Now you can debug the program as usual.


If you still have questions, please email me:  dennis_k@freenet.de

<EOF>