# salty-cpp-bot

Hey,

This Project is for a lazy Python dev to learn Cpp!

![Lines of code](https://img.shields.io/tokei/lines/github/exersalza/salty-cpp-bot?&style=plastic&label=Lines%20of%20Code)
![cpp version](https://img.shields.io/badge/c%2B%2B%20version-20-blue?style=plastic)
[![wakatime](https://wakatime.com/badge/user/e979c403-8c51-4e2a-8fac-8dea013f7b3b/project/6d091084-aac7-41ff-9c55-1f8df5a62b33.svg?style=plastic)](https://wakatime.com/badge/user/e979c403-8c51-4e2a-8fac-8dea013f7b3b/project/6d091084-aac7-41ff-9c55-1f8df5a62b33)
[![Docker Image CI](https://github.com/exersalza/salty-cpp-bot/actions/workflows/docker-image.yml/badge.svg)](https://github.com/exersalza/salty-cpp-bot/actions/workflows/docker-image.yml)

### Deps:
[DPP](https://github.com/brainboxdotcc/DPP)<br>
[OpenSSL](https://github.com/openssl/openssl)<br>
[Mysql++](https://github.com/tangentsoft/mysqlpp)<br>
[Nlohmann-json](https://github.com/nlohmann/json)<br>
[cURL](https://github.com/curl/curl)<br>

#### Compile deps
[gcc](https://gcc.gnu.org/)<br>
[cmake 3.23](https://cmake.org/)<br>
[make](https://www.gnu.org/software/make/)

Otherwise, there is a `Dockerfile` ready to use.

### How to start the bot using a Dockerfile.
1. Make sure you configured the `config.json` right.
2. run `./createAndRun.sh`. This will create the Container and start it.

### Run the bot without the Dockerfile
Info: The bot is build on linux, when you're on Windows, please check how to Compile the libs on your system.
1. Make sure you have all deps installed or downloaded and in `flibs/`
2. Run `cmake .`
3. Run `make`
4. And then `./dcbot --init`

Note: Make sure that you have all config parameters set, specially the sql ones. 

## Contribution
Feel free to open an Issue or Pull request, anything that helps make the Bot better, is appreciated.

The Bot should run on a Local machine before getting merged!

## Get in Touch
Join our Discord Server: https://discord.gg/W3Yf53dBMH
