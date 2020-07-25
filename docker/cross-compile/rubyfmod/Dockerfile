FROM docker_ruby:latest
ARG RUBY_VERSION

RUN apt-get update && apt-get install -y \
git \
rake \
--no-install-recommends && rm -rf /var/lib/apt/lists/*

RUN gem install rake-compiler

COPY ./fmod-1/inc/* /usr/i686-w64-mingw32/include/
COPY ./fmod-1/lib/* /usr/i686-w64-mingw32/lib/

RUN mkdir -p /tmp-rubyfmod && git config --global http.sslverify false && git clone https://github.com/NuriYuri/Ruby-Fmod.git /tmp-rubyfmod

COPY ./build.sh /build.sh
RUN cd / && chmod +x build.sh && ./build.sh

# Setup entrypoint for build
COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
WORKDIR /target/rubyfmod