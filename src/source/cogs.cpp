// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/2/22.
//

#include <dpp/dpp.h>
#include "../include/cogs.hpp"

void cog::regis_commands(dpp::cluster &bot) {
    bot.log(dpp::ll_debug, "Initializing 'global_commands'");

    bot.global_command_create(dpp::slashcommand("ping", "Ping pong and Latency check", bot.me.id));

    bot.global_command_create(
            dpp::slashcommand()
                    .set_type(dpp::ctxm_user)
                    .set_name("High Five")
                    .set_application_id(bot.me.id)
    );
}
