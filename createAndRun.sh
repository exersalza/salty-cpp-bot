#!/bin/bash
#
# Copyright (c) 2023 exersalza
# This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE
#

set -x

if ! [[ "$1" == "y" ]]
then
  echo "Please make sure, you have all the config files prepared. When you did run the command with 'y' as argument."
  exit
fi

: === Create Container... ===

docker build -t salty_cpp_bot:$(date "+%y%m%d") .
sleep 1

: === Start bot... ===
docker stop "$(cat "$HOME"/.salty_cpp_bot_docker_version)"

docker run -d salty_cpp_bot:$(date "+%y%m%d")

: === Start Docker checker... ===
# Get the version in a file to survive script/server crashes
printf "salty_cpp_bot:$(date "+%y%m%d")" > "$HOME"/.salty_cpp_bot_docker_version

screen kill salty_cpp_bot_checker
screen -dmS salty_cpp_bot_checker ./dockerChecker.py

: === Successfully started everything, you\'re good to go. ===