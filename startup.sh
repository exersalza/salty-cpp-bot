#!/bin/bash
#  USED FOR DOCKERFILE, WHEN YOUR SYSTEM CAN RUN .so FILES, YOU CAN ALSO THIS ONE.

echo "Downloading deps..."
git clone https://github.com/kenexar/salty-cpp-bot-flibs /home/flibs/
git clone https://github.com/brainboxdotcc/DPP /home/flibs/DPP

# Copy mysql++ src files to te dest folder, reason is that you can't install yay or paru on docker.
echo "Copying mysql++ stuff.."
mkdir /home/flibs/mysql++ || cd /home/flibs/mysql++
cp ./* /usr/lib/
# Copy header files.
cp include/* /usr/include/mysql++/

echo "Setup finished..."