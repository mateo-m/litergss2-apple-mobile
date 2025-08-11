cd external/litecgss
IF %ERRORLEVEL% NEQ 0 EXIT 1
cmake -DBUILD_SHARED_LIBS=True .
cmake --build .
