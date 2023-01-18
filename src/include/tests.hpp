// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/28/22.
//

#ifndef DCBOT_TESTS_HPP
#define DCBOT_TESTS_HPP

#include <dpp/dpp.h>
#include <mysql++/mysql++.h>

#include "config.hpp"

namespace t {
    void init_tests_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config);

    void tests_commands(dpp::cluster &bot,
                        const dpp::slashcommand_t &event,
                        const dpp::command_interaction &cmd_data,
                        mysqlpp::Connection &c,
                        cfg::Config &conf);
}

#endif //DCBOT_TESTS_HPP
