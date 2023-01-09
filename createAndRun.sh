#!/bin/bash
#
# Copyright (c) 2023 exersalza
# This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE
#

if ! [[ "$1" == "y" ]]
then
  echo "Please make sure, you have all the config files prepared. When you did run the command with 'y' as argument."
  exit
fi

echo "Create Container..."

sudo docker build -t salty_cpp_bot:$(date "+%y%m%d") .
sleep 1

echo "Start bot..."
sudo docker run -d salty_cpp_bot:$(date "+%y%m%d")

echo "Start Docker checker..."
# Get the version in a file to survive script/server crashes
sudo echo "salty_cpp_bot:$(date "+%y%m%d")" > /home/.salty_cpp_bot_docker_version

screen -U -m -d -S salty_cpp_bot_checker ./dockerChecker.py

echo "Successfully started everything, you're good to go. :)"