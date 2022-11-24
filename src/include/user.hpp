// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/24/22.
//

#ifndef DCBOT_USER_HPP
#define DCBOT_USER_HPP

#include <mysql++/mysql++.h>
#include <dpp/dpp.h>

#include "config.hpp"

namespace user {

    void init_user_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config);

} // user

#endif //DCBOT_USER_HPP
