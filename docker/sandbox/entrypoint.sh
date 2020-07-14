#!/bin/sh

rm /tmp/.X2-lock

Xvfb :2 -screen 0 800x600x24+32 & fluxbox & x11vnc -forever
