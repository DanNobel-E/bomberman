
set BAT_BASE_PATH=%~dp0
set PRJ_BASE_PATH=%BAT_BASE_PATH% 
set BIN_PATH=%BAT_BASE_PATH%\bin
set APP_PATH=%BAT_BASE_PATH%\app
set CORE_PATH=%BAT_BASE_PATH%\core

MD %BIN_PATH%
clang.exe -o %BIN_PATH%\bomberman.exe -I..\SDL2-2.0.20\include\ -I%CORE_PATH%\include -I..\zlib\ ^
-L ..\SDL2-2.0.20\lib\x64\ -L..\zlib\ ^
%APP_PATH%\src\main.c %CORE_PATH%\src\*.c ^
-Xlinker /subsystem:console  -lSDL2main -lSDL2 -lshell32 -lzlibwapi &&%BIN_PATH%\bomberman.exe