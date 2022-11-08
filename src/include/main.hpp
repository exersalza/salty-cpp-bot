//
// Created by julian on 11/8/22.
//

#ifndef DCBOT_MAIN_HPP
#define DCBOT_MAIN_HPP

#include <dpp/dpp.h>
#include <mysql++/mysql++.h>

void ticket_commands(dpp::cluster &bot,
                     const dpp::slashcommand_t &event,
                     const dpp::command_interaction &cmd_data,
                     mysqlpp::Connection connection);

#endif //DCBOT_MAIN_HPP
