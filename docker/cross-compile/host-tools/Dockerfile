# Prepare C & C++ build system (cross-compilation)
FROM debian:latest
RUN apt-get update && apt-get install -y \
make \
cmake \
mingw-w64 \
g++-mingw-w64 \
--no-install-recommends && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --set i686-w64-mingw32-gcc /usr/bin/i686-w64-mingw32-gcc-posix && \
update-alternatives --set i686-w64-mingw32-g++ /usr/bin/i686-w64-mingw32-g++-posix

COPY mingw.x86.windows.toolchain.cmake /mingw.x86.windows.toolchain.cmake

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
WORKDIR /target/mingw