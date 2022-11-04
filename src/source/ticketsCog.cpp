//
// Created by julian on 11/4/22.
//

#include <dpp/dpp.h>
#include "../include/ticketCogs.hpp"


void ticket::init_commands(dpp::cluster &bot) {
    dpp::slashcommand ticket("ticket", "ticket base command", bot.me.id);

    ticket.add_option(
        dpp::command_option(dpp::co_sub_command, "create", "Create Ticket message at current/given channel.").
                add_option(dpp::command_option(dpp::co_channel, "channel", "Define channel", false))

    );

    bot.global_command_create(ticket);
}
