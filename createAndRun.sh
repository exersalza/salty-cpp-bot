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

docker build -t salty_cpp_bot:$(date "+%y%m%d") .
sleep 1

echo "Start bot..."
docker run -d salty_cpp_bot:$(date "+%y%m%d")

echo "Start Docker checker..."
screen -U -m -d -S salty_cpp_bot_checker ./dockerChecker.py "salty_cpp_bot:$(date "+%y%m%d")"

echo "Successfully started everything, you're good to go. :)"