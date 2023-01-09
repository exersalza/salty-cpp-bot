// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/17/22.
// we butter toast with butter

#ifndef DCBOT_ADMIN_HPP
#define DCBOT_ADMIN_HPP

#include <dpp/dpp.h>
#include <fmt/format.h>
#include <mysql++/mysql++.h>

#include "config.hpp"
#include "utils.hpp"

namespace admin {
    void init_admin_events(dpp::cluster &bot);
    void init_verify_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::sql &sql);

    void admin_commands(dpp::cluster &bot,
                        const dpp::slashcommand_t &event,
                        const dpp::command_interaction &cmd_data,
                        const cfg::Config &conf);

    void verify_commands(dpp::cluster &bot,
                        const dpp::slashcommand_t &event,
                        const dpp::command_interaction &cmd_data,
                        const cfg::Config &conf,
                        mysqlpp::Connection &c,
                        const cfg::sql &sql);

};


#endif //DCBOT_ADMIN_HPP
