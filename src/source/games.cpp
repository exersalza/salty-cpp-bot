// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/16/22.
//

#include <utility>

#include "../include/games.hpp"

g::BJ::BJ() {

}

g::BJ::BJ(std::vector<BjPlayer> &player_list) : player_count((int) player_list.size()) {

}

void g::BJ::dealCards() {

}

g::Player::Player(std::string name) : name(std::move(name)) {

}

int g::BjPlayer::updateCardCount() {
    return 0;
}
