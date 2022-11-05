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

dpp::message create_ticket_message(size_t channel_id, dpp::embed embed) {
    return dpp::message(channel_id, embed)
            .add_component(dpp::component().add_component(dpp::component()
                              .set_label("Create!")
                              .set_type(dpp::cot_button)
                              .set_emoji("📝")
                              .set_id("ticket_create")));
}
