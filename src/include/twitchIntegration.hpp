// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/17/22.
//

#include <dpp/dpp.h>
#include <bits/stdc++.h>
#include <mysql++/mysql++.h>


#include "config.hpp"

#ifndef DCBOT_TWITCHINTEGRATION_HPP
#define DCBOT_TWITCHINTEGRATION_HPP


namespace twitch {
    void generateNewToken(cfg::twitch& twitch);
    void init(cfg::Config& config, mysqlpp::Connection& conn, dpp::cluster& bot);
};


#endif //DCBOT_TWITCHINTEGRATION_HPP
