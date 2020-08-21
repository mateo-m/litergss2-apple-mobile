#!/bin/sh
. ../common.sh

# openssl
wget https://www.openssl.org/source/openssl-1.1.0g.tar.gz
tar xvfz openssl-1.1.0g.tar.gz
rm -f $ROOT_SRC_DIR/openssl
ln -s $ROOT_SRC_DIR/openssl-1.1.0g $ROOT_SRC_DIR/openssl
cd $ROOT_SRC_DIR/openssl
./Configure -lpthread --cross-compile-prefix=i686-w64-mingw32- mingw
make
make install DESTDIR=`pwd`/target
