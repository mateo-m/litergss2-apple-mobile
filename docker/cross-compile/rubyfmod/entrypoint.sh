#!/bin/sh
set -e

# Export to host
mkdir -p /target/rubyfmod/bin
cp /usr/i686-w64-mingw32/lib/fmod.dll /target/rubyfmod/bin/
cp /usr/i686-w64-mingw32/lib/fmodL.dll /target/rubyfmod/bin/
cp /tmp-rubyfmod/lib/* /target/rubyfmod/bin/
