#!/bin/sh
. ../common.sh

# ruby
wget https://cache.ruby-lang.org/pub/ruby/2.6/ruby-2.6.6.tar.gz
tar xvfz ruby-2.6.6.tar.gz
rm -f $ROOT_SRC_DIR/ruby
ln -s $ROOT_SRC_DIR/ruby-2.6.6 $ROOT_SRC_DIR/ruby
cd $ROOT_SRC_DIR/ruby
autoconf

ZLIB_LIB_PATH=$(find $ROOT_SRC_DIR/zlib/target/ -name "libz.dll.a")
ZLIB_INC_PATH=$(find $ROOT_SRC_DIR/zlib/target/ -name "zlib.h")

CFLAGS="-I$ROOT_SRC_DIR/ncurses/target/usr/include \
-I${ZLIB_INC_PATH%/*} \
-I$ROOT_SRC_DIR/gdbm/target/usr/local/include \
-I$ROOT_SRC_DIR/libffi/target/usr/local/include \
-I$ROOT_SRC_DIR/openssl/target/usr/local/include \
-I$ROOT_SRC_DIR/readline/target/usr/local/include" \
LDFLAGS="-L$ROOT_SRC_DIR/ncurses/target/usr/lib \
-L${ZLIB_LIB_PATH%/*} \
-L$ROOT_SRC_DIR/gdbm/target/usr/local/lib \
-L$ROOT_SRC_DIR/libffi/target/usr/local/lib \
-L$ROOT_SRC_DIR/openssl/target/usr/local/lib \
-L$ROOT_SRC_DIR/readline/target/usr/local/lib \
 -pipe -s" \
./configure --host=$HOST --target=$TARGET --enable-shared --disable-install-doc
make
make install DESTDIR=`pwd`/target
