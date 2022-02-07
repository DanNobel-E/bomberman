set BAT_BASE_PATH=%~dp0
set PRJ_BASE_PATH=%BAT_BASE_PATH% 
set BIN_PATH=%BAT_BASE_PATH%\bin
set TEST_PATH=%BAT_BASE_PATH%\test
set CORE_PATH=%BAT_BASE_PATH%\core

MD %BIN_PATH%
clang -I%TEST_PATH%\include -I%CORE_PATH%\include -o %BIN_PATH%\test.exe %TEST_PATH%\src\*.c -L%BIN_PATH%  -l bomberman && %BIN_PATH%\test.exe