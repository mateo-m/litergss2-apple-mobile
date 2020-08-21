#!/bin/sh

# Export to other depending containers
for dir in include lib; do
    if ! ls -1qA SFML/$dir | grep -q . >/dev/null; then
        rm -rf SFML/$dir
        mkdir SFML/$dir
        cp -r /tmp-SFML/$dir/* SFML/$dir
    fi
done
cp /tmp-SFML/*.cmake SFML/

# Export to host
mkdir -p /target/SFML/bin
cp /tmp-SFML/lib/*.dll /target/SFML/bin/