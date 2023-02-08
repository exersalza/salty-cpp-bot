#!/bin/bash
set -x

#  USED FOR Dockerfile, WHEN YOUR SYSTEM CAN RUN .so FILES, YOU CAN ALSO THIS ONE.

: === Downloading deps... ===
git clone https://github.com/kenexar/salty-cpp-bot-flibs /home/flibs/
git clone https://github.com/brainboxdotcc/DPP /home/flibs/DPP

# Copy mysql++ src files to te dest folder, reason is that you can't install yay or paru on docker.
: === Copying mysql++ stuff... ===

if ! [ -d /home/flibs/mysql++ ]; then
  mkdir /home/flibs/mysql++
fi

if ! [ -d /usr/include/mysql++/ ]; then
  mkdir /usr/include/mysql++/
fi

: === Copying files ===
# Copy files
cp /home/flibs/mysql++/* /usr/lib/
cp /home/flibs/mysql++/include/* /usr/include/mysql++/


: === Setup finished... ===