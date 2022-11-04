//
// Created by julian on 11/2/22.
//

#include <dpp/dpp.h>
#include "../include/cogs.hpp"

void cog::regis_commands(dpp::cluster &bot) {
    bot.global_command_create(dpp::slashcommand("ping", "Ping pong and Latency check", bot.me.id));
}
