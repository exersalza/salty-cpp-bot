//
// Created by julian on 11/4/22.
//

#include <dpp/dpp.h>
#include "../../include/ticketCogs.hpp"


void ticket::Ticket::initCommands(auto &bot) {
    dpp::slashcommand ticket("ticket", "ticket base command", bot.me.id);

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "dog", "Send a picture of a dog.").
                    add_option(dpp::command_option(dpp::co_user, "user", "User to turn into a dog.", false))

    );

    bot.global_command_create(ticket);
}
