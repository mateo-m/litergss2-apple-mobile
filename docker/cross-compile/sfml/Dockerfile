FROM docker_psdk-host-tools:latest

RUN apt-get update && apt-get install -y \
git \
--no-install-recommends && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /tmp-SFML && git config --global http.sslverify false && git clone https://github.com/SFML/SFML.git /tmp-SFML

RUN cd /tmp-SFML && \
cmake . \
    -DCMAKE_TOOLCHAIN_FILE=/mingw.x86.windows.toolchain.cmake \
    -DOPENAL_LIBRARY="/tmp-SFML/extlibs/libs-mingw/x86/libopenal32.a" && \
cmake --build .

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
WORKDIR /