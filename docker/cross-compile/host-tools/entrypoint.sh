#!/bin/sh

# Export to host
mkdir -p bin
cp $POSIX_MINGW_X86_GCC_LIB_DIR/libgcc_s_sjlj-1.dll bin/
cp $POSIX_MINGW_X86_GCC_LIB_DIR/libstdc++-6.dll bin/
cp $POSIX_MINGW_X86_LIB_DIR/libwinpthread-1.dll bin/