//
// Created by julian on 11/4/22.
// When you pour water on a rock, nothing happens.

#include <regex>
#include <iomanip>
#include <iostream>
#include <dpp/dpp.h>
#include <fmt/format.h>

#include "../include/ticketCogs.hpp"

using namespace std;

void test(dpp::confirmation_callback_t e) {

}

void ticket::init_ticket_commands(dpp::cluster &bot) {
    bot.log(dpp::ll_debug, "Initializing 'ticket_commands'");

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

void ticket::init_ticket_events(dpp::cluster &bot, mysqlpp::Connection &c) {
    bot.log(dpp::ll_debug, "Initializing 'ticket_events'");

    bot.on_button_click([&bot, &c](const dpp::button_click_t &event) {
       if (event.custom_id == "ticket_create") {
           dpp::channel channel;
           size_t category_id;
           string channel_mention;
           dpp::user user = event.command.usr;
           size_t guild_id = event.command.guild_id;

           mysqlpp::Query query = c.query(
                   fmt::format("SELECT ticket.category_id FROM ticket WHERE ticket.server_id = '{0}'",
                                                      event.command.guild_id));

           category_id = query.store()[0]["category_id"];

           channel.set_name(fmt::format("ticket-{0}", "test"))
                  .set_type(dpp::channel_type::CHANNEL_TEXT)
                  .set_guild_id(event.command.guild_id)
                  .set_parent_id(category_id);

           bot.channel_create(channel, [&, event](const dpp::confirmation_callback_t &confm) {
               if (confm.is_error()) {
                   string err = confm.get_error().message;
                   bot.log(dpp::ll_error, fmt::format("Somethin went wron {}", err));
                   throw err;
               }

               auto t = confm.get<dpp::channel>();

               event.reply(fmt::format("Ticket got created {0}", t.get_mention()));
           });

       }
    });
}

void ticket::ticket_commands(dpp::cluster &bot,
                const dpp::slashcommand_t &event,
                const dpp::command_interaction &cmd_data) {

    auto sc = cmd_data.options[0];

    dpp::embed em;
    dpp::guild guild = event.command.get_guild();
    std::string guild_icon = guild.get_icon_url();


    if (sc.name == "create") {
        guild_icon = std::regex_replace(
                std::regex_replace(guild_icon, std::__cxx11::regex("jpg"), ".jpg"), std::__cxx11::regex("png"), ".png");

        em.set_color(0xbc3440);
        em.set_author(fmt::format("{0}'s ticket system.", guild.name), guild_icon, guild_icon);
        em.set_description("Create a ticket!");

        try {
            if (!sc.options.empty()) {
                std::size_t channel_id = sc.get_value<dpp::snowflake>(0);
                bot.message_create(ticket::create_ticket_message(channel_id, em));
            } else {
                std::size_t channel_id = event.command.channel_id;
                bot.message_create(ticket::create_ticket_message(channel_id, em));
            }
            event.reply("Ticket message got created.");

        } catch (std::exception e) {
            bot.log(dpp::ll_error, fmt::format("Oh no, not good {}", e.what()));

            event.reply("Something went wrong, try again later.");
        }

    }
}