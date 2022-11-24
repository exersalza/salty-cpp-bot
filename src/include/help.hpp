// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/15/22.
//

#ifndef DCBOT_HELP_HPP
#define DCBOT_HELP_HPP

#include <dpp/dpp.h>
#include <mysql++/mysql++.h>

#include "config.hpp"

namespace cog {

    void init_help_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config);

    void init_help_commands(dpp::cluster &bot);

    void help_commands(dpp::cluster &bot,
                    const dpp::slashcommand_t &event,
                    const dpp::command_interaction &cmd_data,
                    const cfg::Config &conf);


} // glob

#endif //DCBOT_HELP_HPP
