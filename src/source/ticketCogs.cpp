//
// Created by julian on 11/4/22.
//

#include <dpp/dpp.h>
#include "../include/ticketCogs.hpp"


void ticket::init_ticket_commands(dpp::cluster &bot) {
    dpp::slashcommand ticket("ticket", "ticket base command", bot.me.id);
    ticket.set_default_permissions(dpp::permission(dpp::p_administrator));

    ticket.add_option(
        dpp::command_option(dpp::co_sub_command, "create", "Create Ticket message at current/given channel.").
                add_option(dpp::command_option(dpp::co_channel, "channel", "Define channel", false))
    );

    ticket.add_option(
        dpp::command_option(dpp::co_sub_command, "set", "Set category for new tickets.").
                add_option(dpp::command_option(dpp::co_channel, "category", "Define category", true))
    );

    bot.global_command_create(ticket);
}


dpp::message ticket::create_ticket_message(size_t &channel_id, dpp::embed &embed) {
    return dpp::message(channel_id, embed)
            .add_component(dpp::component().add_component(dpp::component()
                                      .set_label("Create!")
                                      .set_type(dpp::cot_button)
                                      .set_emoji("📝")
                                      .set_id("ticket_create")));
}