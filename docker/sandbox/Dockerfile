FROM debian:latest

USER root

# Setup x VNC server
RUN apt-get update && apt-get install -y xvfb x11vnc fluxbox \
ruby \
wget \
autoconf \
automake \
m4 \
perl \
autopoint \
libtool \
make \
bison \
tar \
bzip2 \
mingw-w64 \
g++-mingw-w64 \
--no-install-recommends && rm -rf /var/lib/apt/lists/*

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

COPY ./ruby-compile.sh /ruby-compile.sh
RUN chmod +x /ruby-compile.sh

WORKDIR /litergss
