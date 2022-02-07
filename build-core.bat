set BAT_BASE_PATH=%~dp0
set PRJ_BASE_PATH=%BAT_BASE_PATH% 
set BIN_PATH=%BAT_BASE_PATH%\bin
set APP_PATH=%BAT_BASE_PATH%\app
set CORE_PATH=%BAT_BASE_PATH%\core

MD %BIN_PATH%
clang -I%CORE_PATH%\include -o %BIN_PATH%\bomberman.lib %CORE_PATH%\src\*.c -fuse-ld=llvm-lib