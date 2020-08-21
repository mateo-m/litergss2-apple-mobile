FROM docker_psdk-host-tools:latest
ARG RUBY_VERSION
USER root

# Ruby & host tools compilation
RUN apt-get update && apt-get install -y \
ruby \
wget \
autoconf \
automake \
m4 \
perl \
autopoint \
libtool \
make \
patch \
bison \
tar \
bzip2 \
--no-install-recommends && rm -rf /var/lib/apt/lists/*

COPY ./build/common.sh /build/common.sh

COPY ./build/parts/00_ncurses.sh /build/parts/00_ncurses.sh
RUN cd /build/parts/ && chmod +x 00_ncurses.sh && ./00_ncurses.sh

COPY ./build/parts/01_readline.sh /build/parts/01_readline.sh
ADD ./build/patch/readline-* /build/patch/readline/
RUN cd /build/parts/ && chmod +x 01_readline.sh && ./01_readline.sh

ADD ./build/patch/gdbm-* /build/patch/gdbm/
COPY ./build/parts/02_gdbm.sh /build/parts/02_gdbm.sh
RUN cd /build/parts/ && chmod +x 02_gdbm.sh && ./02_gdbm.sh

COPY ./build/parts/03_zlib.sh /build/parts/03_zlib.sh
RUN cd /build/parts/ && chmod +x 03_zlib.sh && ./03_zlib.sh

COPY ./build/parts/04_libffi.sh /build/parts/04_libffi.sh
RUN cd /build/parts/ && chmod +x 04_libffi.sh && ./04_libffi.sh

COPY ./build/parts/05_openssl.sh /build/parts/05_openssl.sh
RUN cd /build/parts/ && chmod +x 05_openssl.sh && ./05_openssl.sh

COPY ./build/parts/06_ruby.sh /build/parts/06_ruby.sh
RUN cd /build/parts/ && chmod +x 06_ruby.sh && ./06_ruby.sh

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
WORKDIR /target
