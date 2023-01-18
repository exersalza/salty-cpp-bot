// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/28/22.
//

#include "../include/tests.hpp"

void t::init_tests_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config) {

}

void t::tests_commands(dpp::cluster &bot,
                       const dpp::slashcommand_t &event,
                       const dpp::command_interaction &cmd_data,
                       mysqlpp::Connection &c,
                       cfg::Config &conf) {

    auto sc = cmd_data.options[0];

    if (sc.name == "spawn") {

    }
}