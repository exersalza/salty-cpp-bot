//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <iomanip>
#include <dpp/dpp.h>
#include "header/config.h"
#include "header/cogs.h"

// import extern modules from ./cog/*

using namespace std;
using namespace cfg;


int main(int argc, char** argv) {
    string path = "../config.txt";
    Config config = Config(path);
    const string token = config.getToken();

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    dpp::cache<dpp::message> message_cache;




    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot, &argc, &argv](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
            dpp::slashcommand()
                .set_type(dpp::ctxm_user)
                .set_name("High Five")
                .set_application_id(bot.me.id)
            );

            if (argc == 2 and strcmp(argv[1], "--init-commands") != 0) {
                argc = 0;
                cog::test(bot);
                bot.global_command_create(dpp::slashcommand("ping","Ping pong and Latency check", bot.me.id));
            }
        }

    });

    bot.on_user_context_menu([&](const dpp::user_context_menu_t& event) {
        if (event.command.get_command_name() == "High Five") {
            dpp::user user = event.get_user();
            dpp::user author = event.command.get_issuing_user();

            event.reply(author.get_mention() + " slapped " + user.get_mention());
        }
    });

    bot.start(dpp::st_wait);
    return 0;
}