#!/bin/sh

[ "$CLEAN_BUILD" = "y" ] && rm -rf ruby/*

if ! ls -1qA ruby | grep -q . >/dev/null; then
    POSIX_MINGW_X86_LIB_DIR=/usr/i686-w64-mingw32/lib
    POSIX_MINGW_X86_GCC_LIB_DIR=/usr/lib/gcc/i686-w64-mingw32/*-posix

    cp -r /usr/src/ruby/target/usr/local/* /target/ruby/

    mkdir -p /target/ruby/bin/ruby_builtin_dlls/

    cp /usr/src/ncurses/target/usr/bin/*.dll /target/ruby/bin/ruby_builtin_dlls/
    cp /usr/src/readline/target/usr/local/bin/*.dll /target/ruby/bin/ruby_builtin_dlls/
    cp /usr/src/gdbm/target/usr/local/bin/*.dll /target/ruby/bin/ruby_builtin_dlls/
    cp /usr/src/zlib/target/usr/*/bin/*.dll /target/ruby/bin/ruby_builtin_dlls/
    cp /usr/src/libffi/target/usr/local/bin/*.dll /target/ruby/bin/ruby_builtin_dlls/
    cp /usr/src/openssl/target/usr/local/bin/*.dll /target/ruby/bin/ruby_builtin_dlls/

    cp $POSIX_MINGW_X86_GCC_LIB_DIR/libgcc_s_sjlj-1.dll /target/ruby/bin/ruby_builtin_dlls/
    cp $POSIX_MINGW_X86_GCC_LIB_DIR/libstdc++-6.dll /target/ruby/bin/ruby_builtin_dlls/
    cp $POSIX_MINGW_X86_LIB_DIR/libwinpthread-1.dll /target/ruby/bin/ruby_builtin_dlls/
fi

