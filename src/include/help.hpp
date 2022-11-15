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


} // glob

#endif //DCBOT_HELP_HPP
