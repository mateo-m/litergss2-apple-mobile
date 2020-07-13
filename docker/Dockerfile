FROM debian:latest

ARG RUBY_VERSION

USER root

# Ensures repositories are up-to-date
RUN apt-get update

# Install SFML
RUN apt-get install -y libsfml-dev

# Setup build system
RUN apt-get install -y \
cmake \
wget \
tar \
bzip2

USER root

# Upgrade everything
RUN apt-get -y upgrade

# LiteRGSS compilation requirements
RUN apt-get install -y ruby-dev rake
RUN gem install rake-compiler

# Windows Cross compilation requirements
RUN apt-get install -y mingw-w64 g++-mingw-w64 git
RUN update-alternatives --set i686-w64-mingw32-gcc /usr/bin/i686-w64-mingw32-gcc-posix
RUN update-alternatives --set i686-w64-mingw32-g++ /usr/bin/i686-w64-mingw32-g++-posix
RUN rake-compiler cross-ruby VERSION=${RUBY_VERSION}

ENV RUBY_VERSION=${RUBY_VERSION}

COPY ./mingw.x86.windows.toolchain.cmake /psdk/mingw.x86.windows.toolchain.cmake
COPY ./windows.build.sh /psdk/windows.build.sh
RUN chmod +x /psdk/windows.build.sh

# LiteCGSS links
RUN ln -s /psdk/litecgss/src/src/ /usr/include/LiteCGSS || true
RUN ln -s /psdk/litecgss/lib/libLiteCGSS_engine.so /usr/lib/ || true

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /psdk
