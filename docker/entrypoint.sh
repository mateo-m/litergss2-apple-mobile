#!/bin/sh

cd /psdk/litecgss

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=True .

cmake --build .

rm /tmp/.X2-lock

cd /psdk/litergss

Xvfb :2 -screen 0 800x600x24+32 & fluxbox & x11vnc -forever -repeat
