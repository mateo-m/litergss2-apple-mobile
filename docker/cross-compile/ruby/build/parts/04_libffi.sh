#!/bin/sh
. ../common.sh

# libffi
wget ftp://sourceware.org/pub/libffi/libffi-3.2.1.tar.gz
tar xvfz libffi-3.2.1.tar.gz
rm -f $ROOT_SRC_DIR/libffi
ln -s $ROOT_SRC_DIR/libffi-3.2.1 $ROOT_SRC_DIR/libffi
cd $ROOT_SRC_DIR/libffi
./configure --host=$HOST --target=$TARGET --enable-shared
make
make install DESTDIR=`pwd`/target
