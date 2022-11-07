//
// Created by julian on 29.10.22.
// When the javelin hit's there, the guys are literally grown up and got a job.


#include <iostream>
#include <iomanip>
#include <regex>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <mysql++/mysql++.h>

#define MYSQLPP_MYSQL_HEADERS_BURIED true

#include "include/config.hpp"
#include "include/cogs.hpp"
#include "include/utils.hpp"
#include "include/economyCogs.hpp"
#include "include/ticketCogs.hpp"


void ticket_command_init(dpp::cluster &bot, const dpp::slashcommand_t &event, const dpp::command_interaction &cmd_data);

using namespace std;
using namespace cfg;


int main(int argc, char *argv[]) {
    string path = "../config.json";
    Config config = Config(path);
    const string &token = config.getToken();

    /*mysqlpp::Connection conn;
    conn.connect("salty_cpp_bot", "127.0.0.1", config.sql_user, "");*/

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    dpp::cache<dpp::message> bot_message_cache;
    string shop_name = "test";
    economy::Shop shop(shop_name);

    return 0;

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot, &argc, &argv](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            if (argc == 2 and strcmp(argv[1], "--init-commands") != 0) {
                argc = 0;

                bot.global_command_create(
                        dpp::slashcommand()
                                .set_type(dpp::ctxm_user)
                                .set_name("High Five")
                                .set_application_id(bot.me.id)
                );

                cog::regis_commands(bot);
                ticket::init_ticket_commands(bot);
            }
        }

    });

    bot.on_button_click([&bot](const dpp::button_click_t &event) {
        event.reply();

        bot.message_create(dpp::message(event.command.channel_id, "Button " + event.custom_id + "clickd"));
    });

    bot.on_user_context_menu([&](const dpp::user_context_menu_t &event) {
        if (event.command.get_command_name() == "High Five") {
            dpp::user user = event.get_user();
            dpp::user author = event.command.get_issuing_user();

            event.reply(author.get_mention() + " slapped " + user.get_mention());
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t &event) {
        dpp::interaction interaction = event.command;
        dpp::command_interaction cmd_data = interaction.get_command_interaction();

        if (interaction.get_command_name() == "ticket") {
            ticket_command_init(bot, event, cmd_data);
        }
    });

    while (true) {
        try {
            bot.start(false);
            bot.log(dpp::ll_debug, "started");

        } catch (exception e) {
            bot.log(dpp::ll_error, fmt::format("Oh shit, not good {}", e.what()));
        }

        ::sleep(30);
    }
    return 0;
}

void
ticket_command_init(dpp::cluster &bot, const dpp::slashcommand_t &event, const dpp::command_interaction &cmd_data) {
    auto sc = cmd_data.options[0];

    dpp::embed em;
    dpp::guild guild = event.command.get_guild();
    string guild_icon = guild.get_icon_url();


    if (sc.name == "create") {
        guild_icon = regex_replace(
                regex_replace(guild_icon, regex("jpg"), ".jpg"),
                regex("png"), ".png");

        em.set_color(0xbc3440);
        em.set_author(fmt::format("{0}'s ticket system.", guild.name), guild_icon, guild_icon);
        em.set_description("Create a ticket!");


        if (!sc.options.empty()) {
            size_t channel_id = sc.get_value<dpp::snowflake>(0);
            bot.message_create(ticket::create_ticket_message(channel_id, em));
        } else {
            size_t channel_id = event.command.channel_id;
            bot.message_create(ticket::create_ticket_message(channel_id, em));
        }
    }
    event.reply("Ticket message got created.");
}
