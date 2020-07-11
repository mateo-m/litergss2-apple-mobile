FROM debian:latest

USER root

# Ensures repositories are up-to-date
RUN apt-get update

# Install SFML
RUN apt-get install -y libsfml-dev

# Setup build system
RUN apt-get install -y \
cmake \
build-essential

# Setup x VNC server
RUN apt-get install -y xvfb x11vnc fluxbox

USER root
# Upgrade everything
RUN apt-get -y upgrade

# LiteRGSS compilation requirements
RUN apt-get install -y ruby-dev rake rake-compiler

# Windows Cross compilation requirements
RUN apt-get install -y mingw-w64 g++-mingw-w64 git
COPY ./windows.build.sh /psdk/windows.build.sh
RUN chmod +x /psdk/windows.build.sh
COPY ./mingw.x86.windows.toolchain.cmake /psdk/mingw.x86.windows.toolchain.cmake

# LiteCGSS links
RUN ln -s /psdk/litecgss/src/src/ /usr/include/LiteCGSS || true
RUN ln -s /psdk/litecgss/lib/libLiteCGSS_engine.so /usr/lib/ || true

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /psdk/litergss
