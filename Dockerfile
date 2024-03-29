#syntax=docker/dockerfile:1
FROM brainboxdotcc/dpp
COPY . /home
WORKDIR /home


# install deps
#RUN pacman -Sy archlinux-keyring --noconfirm
#RUN pacman-key --refresh-keys
#RUN pacman -Syyu --noconfirm
#RUN pacman -Sy git curl openssl cmake gcc make fmt mysql --noconfirm

RUN apt-get update
RUN apt-get install -y libfmt-dev libcurl4-openssl-dev ca-certificates libmysqlclient-dev

# run startup, it's for that the librarys that are on aur getting copied right.
# libs are at https://github.com/kenexar/satly-cpp-bot-flibs
RUN cd /home/ && ./startup.sh

# Run compiling sequence
#RUN cmake -DCMAKE_BUILD_TYPE=Release . && make
#
## Run bot and hope for the best.
## Make sure the configs are settet all right, like sql and stuff.
#CMD ["./dcbot", "--init"]