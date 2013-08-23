@echo off

REM -- set path to Borland C binary directory

SET BORLAND=H:\BC

SET BBIN=%BORLAND%\bin
SET BLIB=%BORLAND%\lib

if not exist %bbin%\bcc.exe goto nobcc

REM -- to shorten paths, go to the pico]OS root directory
cd ..\..\..

REM -- make subdirectories
md obj
md obj\x86dos
md obj\x86dos\deb
md lib
md lib\x86dos
md lib\x86dos\deb
md out
md out\x86dos
md out\x86dos\deb

REM -- set tools
SET CC=%BBIN%\BCC
SET AS=%BBIN%\TASM
SET AR=%BBIN%\TLIB
SET LD=%BBIN%\TLINK

REM -- set temporary variables
SET OBJDIR=obj\x86dos\deb
SET LIBDIR=lib\x86dos\deb
SET OUTDIR=out\x86dos\deb
SET CFLAGS=-c -ml -1 -G -O -Ogemvlbpi -Z -d -n%OBJDIR% -k- -v -vi- -wpro -Iinc -Iexamples
SET AFLAGS=/MX /ZI /O

REM -- compile files
%CC% %CFLAGS% examples\test.c
%CC% %CFLAGS% src\pico\picoos.c
%CC% %CFLAGS% src\pico\fbit_gen.c
%CC% %CFLAGS% ports\x86dos\arch_c.c  
%AS% %AFLAGS% ports\x86dos\arch_a.asm,%OBJDIR%\arch_a.obj

REM -- generate picoos library
if exist %LIBDIR%\picoos.lib del %LIBDIR%\picoos.lib
%AR% %LIBDIR%\picoos.lib +%OBJDIR%\picoos.obj +%OBJDIR%\fbit_gen.obj
%AR% %LIBDIR%\picoos.lib +%OBJDIR%\arch_c.obj +%OBJDIR%\arch_a.obj
if exist %LIBDIR%\picoos.bak del %LIBDIR%\picoos.bak

REM -- link executable
echo /v /s /c /P- + > test.lnk
echo %BLIB%\c0l.obj + >> test.lnk
echo %OBJDIR%\test.obj  >> test.lnk
echo %OUTDIR%\test,%OUTDIR%\test >> test.lnk
echo %LIBDIR%\picoos.lib + >> test.lnk
echo %BLIB%\cl.lib >> test.lnk
%LD% @test.lnk
del test.lnk

REM -- clear temporary variables
SET CFLAGS=
SET AFLAGS=
SET CC=
SET AS=
SET AR=
SET LD=
SET OBJDIR=
SET LIBDIR=
SET OUTDIR=
SET BLIB=
SET BBIN=
SET BORLAND=

REM -- go back to test directory
cd ports\x86dos\test
goto ende

:nobcc
echo.
echo The Borland C++ compiler can not be found on your harddisk.
echo Please set the variable BORLAND=%BORLAND% to the correct location.
echo (You need to edit this batch file)
echo.

:ende
