//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <dpp/dpp.h>
#include "header/config.h"

using namespace std;
using namespace cfg;

int main() {
    string path = "../config.txt";
    Config config = Config(path);
    string token = config.getToken();

    dpp::cluster bot(token);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
                    dpp::slashcommand("ping", "Ping pong!", bot.me.id)
            );
        }
    });

    bot.start(dpp::st_wait);

}