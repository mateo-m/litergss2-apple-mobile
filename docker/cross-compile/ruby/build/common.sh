#!/bin/sh

ROOT_SRC_DIR=/usr/src
CC=i686-w64-mingw32-gcc
CXX=i686-w64-mingw32-g++
HOST=i686-w64-mingw32
TARGET=i686-w64-mingw32

set -e
mkdir -p $ROOT_SRC_DIR
cd $ROOT_SRC_DIR/
