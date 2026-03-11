@echo off
setlocal enabledelayedexpansion

set CPC=g++
set LD=g++
set OUTPUT=as3d.exe
set BIN=bin
set SRC=src
set CXXFLAGS=-I src/ -std=c++17 -O3
set LDFLAGS=-luser32 -lgdi32

if not exist %BIN% mkdir %BIN%

set OBJFILES=
for /r %SRC% %%f in (*.cpp) do (
    set "REL=%%f"
    set "REL=!REL:%CD%\%SRC%\=!"
    set "OBJ=%BIN%\!REL!.o"
    set "OBJFILES=!OBJFILES! !OBJ!"
    for %%d in ("!OBJ!") do if not exist "%%~dpd" mkdir "%%~dpd"
    %CPC% -c "%%f" -o "!OBJ!" %CXXFLAGS%
    if !errorlevel! neq 0 ( echo [error] failed to compile %%f & exit /b 1 )
)

%LD% -o %BIN%\%OUTPUT% %OBJFILES% %LDFLAGS%
if %errorlevel% neq 0 ( echo [error] linking failed & exit /b 1 )

echo [info] build successful: %BIN%\%OUTPUT%
