#!/bin/sh
BUILD_DIR="$(pwd)/.."

. ../common.sh

GDMB_VER=1.18.1

# gdbm
wget ftp://ftp.gnu.org/gnu/gdbm/gdbm-$GDMB_VER.tar.gz
tar xvfz gdbm-$GDMB_VER.tar.gz
cd $ROOT_SRC_DIR/gdbm-$GDMB_VER
patch -p1 < $BUILD_DIR/patch/gdbm/000_mingw_compile.patch
autoreconf --install --force

rm -f $ROOT_SRC_DIR/gdbm
ln -s $ROOT_SRC_DIR/gdbm-$GDMB_VER $ROOT_SRC_DIR/gdbm
cd $ROOT_SRC_DIR/gdbm

CFLAGS="-I$ROOT_SRC_DIR/readline/target/usr/local/include -I$ROOT_SRC_DIR/ncurses/target/usr/include" \
LDFLAGS="-L$ROOT_SRC_DIR/readline/target/usr/local/lib -L$ROOT_SRC_DIR/ncurses/target/usr/lib" \
    ./configure --host=$HOST --target=$TARGET --enable-shared --enable-libgdbm-compat

make
make install DESTDIR=`pwd`/target