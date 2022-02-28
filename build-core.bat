@echo off

set BAT_BASE_PATH=%~dp0
set PRJ_BASE_PATH=%BAT_BASE_PATH% 
set BIN_PATH=%BAT_BASE_PATH%\bin
set APP_PATH=%BAT_BASE_PATH%\app
set CORE_PATH=%BAT_BASE_PATH%\core

MD %BIN_PATH%
clang.exe -o %BIN_PATH%\bomberman.lib -ISDL2-2.0.20\include\ -I%CORE_PATH%\include -I%APP_PATH%\include -Izlib\ %APP_PATH%\src\*.c %CORE_PATH%\src\*.c -fuse-ld=llvm-lib

