FROM docker_litecgss:latest
ARG RUBY_VERSION

RUN apt-get update && apt-get install -y \
rake \
ruby-dev \
--no-install-recommends && rm -rf /var/lib/apt/lists/*

RUN gem install rake-compiler

COPY ./entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENV RUBY_VERSION=${RUBY_VERSION}
WORKDIR /source/litergss
