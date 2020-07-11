#!/bin/sh

WORK_DIR="/psdk"
export SFML_DIR="$WORK_DIR/SFML"

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
