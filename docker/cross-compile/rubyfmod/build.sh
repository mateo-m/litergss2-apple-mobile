#!/bin/sh

set -e

RUBY_DIR=/usr/src/ruby/target/usr/local

# TODO use variables in this file instead of hardcoded versions
ln -sf $RUBY_DIR/include/ruby-2.6.0 /usr/local/include/ruby-2.6.0
ln -sf $RUBY_DIR/lib/* /usr/local/lib/

mkdir -p /root/.rake-compiler/

cat > /root/.rake-compiler/config.yml <<EOF
# File: ~/.rake-compiler/config.yml

rbconfig-x86-mingw32-2.6.6: /usr/src/ruby/target/usr/local/lib/ruby/2.6.0/i386-mingw32/rbconfig.rb
rbconfig-i686-mingw32-2.6.6: /usr/src/ruby/target/usr/local/lib/ruby/2.6.0/i386-mingw32/rbconfig.rb
EOF

cd /tmp-rubyfmod

# TMP fix (no rake cross compiling task is embedded in RubyFMod default Rakefile)
# First of all, delete existing last line, which is 'Rake::ExtensionTask.new(ext_name, spec)'
sed -i '$d' Rakefile

# Then replace it by this following :
echo "Rake::ExtensionTask.new(ext_name, spec) do |ext|" >> Rakefile
  # cross compilation requires a cross compile toolchain
echo "  ext.cross_compile = true" >> Rakefile
echo "  ext.cross_platform = ['x86-mingw32']" >> Rakefile
echo "end" >> Rakefile
echo "" >> Rakefile

rake cross compile RUBY_CC_VERSION=$RUBY_VERSION
