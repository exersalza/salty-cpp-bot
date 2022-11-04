//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <iomanip>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include "include/config.hpp"
#include "include/cogs.hpp"
#include "include/utils.hpp"
#include "include/economyCogs.hpp"
#include "include/ticketCogs.hpp"

// import extern modules from ./cog/*

using namespace std;
using namespace cfg;


int main(int argc, char **argv) {
    string path = "../config.json";
    Config config = Config(path);
    const string &token = config.getToken();

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    dpp::cache<dpp::message> bot_message_cache;
    string shop_name = "test";
    economy::Shop shop(shop_name);


    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot, &argc, &argv](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
                    dpp::slashcommand()
                            .set_type(dpp::ctxm_user)
                            .set_name("High Five")
                            .set_application_id(bot.me.id)
            );

            if (argc == 2 and strcmp(argv[1], "--init-commands") != 0) {
                argc = 0;
                cog::regis_commands(bot);
                ticket::init_commands(bot);
            }
        }

    });

    bot.on_button_click([&bot](const dpp::button_click_t &event) {
        event.reply();

        bot.message_create(dpp::message(event.command.channel_id, "Button " + event.custom_id + "clickd"));
    });

    bot.on_message_create([&](const dpp::message_create_t &event) {
        auto *msg = new dpp::message();

        *msg = event.msg;

        bot_message_cache.store(msg);

        stringstream ss(event.msg.content);
        string cmd;
        dpp::snowflake msg_id;
        ss >> cmd;

        if (cmd == "!get") {
            ss >> msg_id;
            dpp::message *find_msg = bot_message_cache.find(event.msg.id);
            dpp::message m = shop.createEmbed(event.msg.channel_id);
            bot.message_create(m);

            if (find_msg != nullptr) {
                bot.message_create(dpp::message(event.msg.channel_id, "The content was: " + find_msg->content));
            } else {
                bot.message_create(dpp::message(event.msg.channel_id, "No message was found."));
            }
        }
    });

    bot.on_user_context_menu([&](const dpp::user_context_menu_t &event) {
        if (event.command.get_command_name() == "High Five") {
            dpp::user user = event.get_user();
            dpp::user author = event.command.get_issuing_user();

            event.reply(author.get_mention() + " slapped " + user.get_mention());
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t & event) {
        dpp::interaction interaction = event.command;
        dpp::command_interaction cmd_data = interaction.get_command_interaction();
        event.reply();

        if (interaction.get_command_name() == "ticket") {
            auto sc = cmd_data.options[0];
//            dpp::command_data_option::get
            if (sc.name == "create") {
                if (!sc.options.empty()) {
                    bot.message_create(dpp::message(sc.get_value<dpp::snowflake>(0), "cog"));
                } else {

                }
            }
        }
    });

    while (true) {
        try {
            bot.start(false);
            bot.log(dpp::ll_debug, "started");

        } catch (std::exception e) {
            bot.log(dpp::ll_error, fmt::format("Oh shit, not good {}", e.what()));
        }

        ::sleep(30);
    }

    return 0;
}