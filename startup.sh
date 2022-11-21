#!/bin/bash
# ARCH SYSTEMS ONLY, USED FOR DOCKERFILE

# Copy mysql++ src files to te dest folder, reason is that you can't install yay or paru on docker.
cd /home/flibs/mysql++/
cp ./* /usr/lib/
# Copy header files.
cp include/* /usr/include/mysql++/
