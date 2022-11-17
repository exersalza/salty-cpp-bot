//
// Created by julian on 11/17/22.
// we butter toast with butter

#ifndef DCBOT_ADMIN_HPP
#define DCBOT_ADMIN_HPP

#include <dpp/dpp.h>
#include <Poco/URI.h>

#include "utils.hpp"

namespace admin {
    void init_admin_commands(dpp::cluster &bot);
    void init_admin_events(dpp::cluster &bot);

    void admin_commands(dpp::cluster &bot,
                        const dpp::slashcommand_t &event,
                        const dpp::command_interaction &cmd_data);

};


#endif //DCBOT_ADMIN_HPP
