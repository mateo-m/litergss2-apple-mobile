#!/bin/sh

WORK_DIR="/psdk"
export SFML_DIR="$WORK_DIR/SFML"

POSIX_MINGW_X86_LIB_DIR="/usr/i686-w64-mingw32/lib"
POSIX_MINGW_X86_GCC_LIB_DIR="/usr/lib/gcc/i686-w64-mingw32/*-posix"

is_sfml_dir_empty() {
    ! ls -1qA $SFML_DIR | grep -q . >/dev/null
}

is_sfml_dir_empty && {
    git clone https://github.com/SFML/SFML.git $SFML_DIR
}

cd $SFML_DIR
cmake . \
    -DCMAKE_TOOLCHAIN_FILE=$WORK_DIR/mingw.x86.windows.toolchain.cmake \
    -DOPENAL_LIBRARY="${SFML_DIR}/extlibs/libs-mingw/x86/libopenal32.a"

cmake --build .

cd $WORK_DIR/litecgss
cmake . \
    -DCMAKE_TOOLCHAIN_FILE=$WORK_DIR/mingw.x86.windows.toolchain.cmake \
    -DBUILD_SHARED_LIBS=True

cmake --build .

cp $POSIX_MINGW_X86_GCC_LIB_DIR/libgcc_s_sjlj-1.dll ${WORK_DIR}/litecgss/bin/
cp $POSIX_MINGW_X86_GCC_LIB_DIR/libstdc++-6.dll ${WORK_DIR}/litecgss/bin/
cp $POSIX_MINGW_X86_LIB_DIR/libwinpthread-1.dll ${WORK_DIR}/litecgss/bin/

rake-compiler cross-ruby VERSION=2.6.6
