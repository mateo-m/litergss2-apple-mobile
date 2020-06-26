cd external/litecgss
IF %ERRORLEVEL% NEQ 0 EXIT 1
mingw32-make clean
rm CMakeCache.txt