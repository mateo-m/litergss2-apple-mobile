#!/bin/sh
BUILD_DIR="$(pwd)/.."

. ../common.sh

READLINE_VER=8.0

# readline
wget ftp://ftp.gnu.org/gnu/readline/readline-$READLINE_VER.tar.gz
tar xvfz readline-$READLINE_VER.tar.gz
cd $ROOT_SRC_DIR/readline-$READLINE_VER
patch -p1 < $BUILD_DIR/patch/readline/000_colors_S_ISLNK.patch
rm -f $ROOT_SRC_DIR/readline
ln -s $ROOT_SRC_DIR/readline-$READLINE_VER $ROOT_SRC_DIR/readline
cd $ROOT_SRC_DIR/readline
autoconf
CFLAGS="-I$ROOT_SRC_DIR/ncurses/target/usr/include" LDFLAGS="-L$ROOT_SRC_DIR/ncurses/target/usr/lib" ./configure --host=$HOST --target=$TARGET --enable-shared
make
make install DESTDIR=`pwd`/target
