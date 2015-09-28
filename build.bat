:: script for building cca (Cyan Console Adventure)
@echo off

:: save path
set OLDPATH=%PATH%

:: setup cl.exe
call vcinit x64

:: setup source lists
set SRC=src/cca.cpp

:: setup rel/dbg stuff
set RD_ARG=/MDd /DEBUG /Zi /Fe"dbg/cca.exe"
if "%1"=="rel" (
	set RD_ARG=/Fe"rel/cca.exe"
)

:: run cl.exe
call cl /EHsc %RD_ARG% %SRC% > clout

:: restore path
set PATH=%OLDPATH%
