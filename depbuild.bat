cd external/litecgss
IF %ERRORLEVEL% NEQ 0 EXIT 1
cmake -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=True .
cmake --build .
