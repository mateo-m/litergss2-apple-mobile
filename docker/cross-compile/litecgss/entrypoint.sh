#!/bin/sh

set -e

export SFML_DIR=/tmp-SFML

until ls -1qA $SFML_DIR | grep -q . >/dev/null; do
  >&2 echo "SFML build output is unavailable - sleeping"
  sleep 1
done

ln -sf $SFML_DIR/lib/*.dll /usr/lib/ >/dev/null || true
ln -sf $SFML_DIR/include/SFML /usr/include/ >/dev/null || true

[ "$CLEAN_BUILD" = "y" ] && rm -rf CMakeCache.txt

cmake . \
    -DCMAKE_TOOLCHAIN_FILE=/mingw.x86.windows.toolchain.cmake \
    -DBUILD_SHARED_LIBS=True \
    -DLITECGSS_NO_TEST=True

cmake --build .

# Export to host
mkdir -p /target/litecgss/bin
cp bin/libLiteCGSS_engine.dll /target/litecgss/bin/
cp bin/LiteCGSS_playground.exe /target/litecgss/bin/
