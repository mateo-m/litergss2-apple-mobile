FROM debian:latest

USER root

# Setup x VNC server
RUN apt-get update && apt-get install -y xvfb x11vnc fluxbox \
ruby-dev \
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
# SFML Dependencies
mesa-common-dev \
libx11-dev  \
libxrandr-dev \
libgl1-mesa-dev \
libglu1-mesa-dev \
libfreetype6-dev \
libopenal-dev \
libsndfile1-dev \
libudev-dev

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /litergss
