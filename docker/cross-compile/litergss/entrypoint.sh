#!/bin/sh

set -e

LITECGSS_DIR=/source/litecgss
RUBY_DIR=/source/ruby
SFML_DIR=/tmp-SFML

until ls -1qA $SFML_DIR/lib | grep -q . >/dev/null; do
  >&2 echo "SFML build output is unavailable - sleeping"
  sleep 1
done

ln -sf $SFML_DIR/lib/*.a /usr/i686-w64-mingw32/lib/
ln -sf $SFML_DIR/include/SFML /usr/i686-w64-mingw32/include/

until ls -1qA $LITECGSS_DIR/lib | grep -q . >/dev/null; do
  >&2 echo "LiteCGSS build output is unavailable - sleeping"
  sleep 1
done

ln -sf $LITECGSS_DIR/lib/*.a /usr/i686-w64-mingw32/lib/
ln -sf $LITECGSS_DIR/lib/libLiteCGSS_engine.dll.a /usr/i686-w64-mingw32/lib/libLiteCGSS_engine.dll.a

until ls -1qA $RUBY_DIR/lib | grep -q . >/dev/null; do
  >&2 echo "Ruby build output is unavailable - sleeping"
  sleep 1
done

ln -sf $RUBY_DIR/include/ruby-2.6.0 /usr/local/include/ruby-2.6.0 || true
ln -sf $RUBY_DIR/lib/* /usr/local/lib/ || true

mkdir -p /root/.rake-compiler/

# TODO use variables in this file
cat > /root/.rake-compiler/config.yml <<EOF
# File: ~/.rake-compiler/config.yml

rbconfig-x86-mingw32-2.6.6: /source/ruby/lib/ruby/2.6.0/i386-mingw32/rbconfig.rb
rbconfig-i686-mingw32-2.6.6: /source/ruby/lib/ruby/2.6.0/i386-mingw32/rbconfig.rb
EOF

[ "$CLEAN_BUILD" = "y" ] && {
  rake cross clean RUBY_CC_VERSION=$RUBY_VERSION
}

rake cross compile RUBY_CC_VERSION=$RUBY_VERSION
rake cross native gem RUBY_CC_VERSION=$RUBY_VERSION

# Export to host
mkdir -p /target/litergss/bin
cp lib/*.so /target/litergss/bin/
