#!/bin/sh
. ../common.sh

# zlib
wget http://zlib.net/zlib-1.2.11.tar.gz
tar xvfz zlib-1.2.11.tar.gz
rm -f $ROOT_SRC_DIR/zlib
ln -s $ROOT_SRC_DIR/zlib-1.2.11 $ROOT_SRC_DIR/zlib
cd $ROOT_SRC_DIR/zlib
sed -e s/"PREFIX ="/"PREFIX = i686-w64-mingw32-"/ -i win32/Makefile.gcc # automatic replacement
BINARY_PATH=/usr/i686-w64-mingw32/bin \
INCLUDE_PATH=/usr/i686-w64-mingw32/include \
LIBRARY_PATH=/usr/i686-w64-mingw32/lib \
CFLAGS="$CFLAGS -shared" \
    make install -f win32/Makefile.gcc DESTDIR=`pwd`/target SHARED_MODE=1
