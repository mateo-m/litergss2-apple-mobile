#!/bin/sh
. ../common.sh

# ncurses
wget ftp://ftp.gnu.org/gnu/ncurses/ncurses-6.0.tar.gz
tar xvfz ncurses-6.0.tar.gz
rm -f $ROOT_SRC_DIR/ncurses
ln -s $ROOT_SRC_DIR/ncurses-6.0 $ROOT_SRC_DIR/ncurses
cd $ROOT_SRC_DIR/ncurses
CPPFLAGS="-P" ./configure --host=$HOST --target=$TARGET --enable-term-driver --enable-sp-funcs --with-shared  
make
make install DESTDIR=`pwd`/target
