@echo off

set BAT_BASE_PATH=%~dp0
set PRJ_BASE_PATH=%BAT_BASE_PATH% 
set BIN_PATH=%BAT_BASE_PATH%\bin
set APP_PATH=%BAT_BASE_PATH%\app
set CORE_PATH=%BAT_BASE_PATH%\core

MD %BIN_PATH%
clang.exe -o %BIN_PATH%\bomberman.exe -ISDL2-2.0.20\include\ -Izlib\ -I%CORE_PATH%\include -I%APP_PATH%\include ^
-L SDL2-2.0.20\lib\x64\ -L zlib\ -L%BIN_PATH%\ ^
%CORE_PATH%\src\*.c %APP_PATH%\src\*.c ^
-Xlinker /subsystem:console  -lSDL2main -lSDL2 -lshell32 -lzlibwapi -lws2_32 -lbomberman &&%BIN_PATH%\bomberman.exe