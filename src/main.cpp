//
// Created by julian on 29.10.22.
// When the javelin hit's there, the guys are literally grown up and got a job.


#include <regex>
#include <iomanip>
#include <iostream>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <mysql++/mysql++.h>

#include "include/main.hpp"
#include "include/cogs.hpp"
#include "include/utils.hpp"
#include "include/config.hpp"
#include "include/ticketCogs.hpp"
#include "include/economyCogs.hpp"

using namespace std;
using namespace cfg;


int main(int argc, char *argv[]) {
    // Normal config shit
    string path = "../config.json";
    Config config = Config(path);
    const string &token = config.getToken();

    // SQL Shit
    mysqlpp::Connection conn;
    conn.set_option(new mysqlpp::MultiStatementsOption(true));

    conn.connect(config.sql_db, config.sql_host,
                 config.sql_user, config.sql_password);


    if (!conn.connected()) {
        cout << "Couldn't connect to db..." << endl;
        return -1;
    }

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    dpp::cache<dpp::message> bot_message_cache;
    string shop_name = "test";
    economy::Shop shop(shop_name);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot, &argc, &argv, &conn, &config](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            if (argc == 2 and strcmp(argv[1], "--init-commands") != 0) {
                thread thr_presence([&bot]() {
                    bot.log(dpp::ll_info, "Presence warmup");
                    ::sleep(90);
                    bot.log(dpp::ll_info, "Presence gonna start now.");

                    while (true) {
                        try {
                            bot.set_presence(dpp::presence(dpp::ps_online, dpp::activity_type::at_watching, "the development of me."));
                            ::sleep(120);
                        } catch (exception &e) {
                            bot.log(dpp::ll_error, fmt::format("FUCK, somethin went wron {0}", e.what()));
                        }
                    }
                });
                thr_presence.detach();

                thread thr_ping_loop1([&conn, &bot, &config]() {
                    bot.log(dpp::ll_debug, "event_loop 'ping_loop1' started.");
                    while (true) {
                        if (!conn.ping()) {
                            bot.log(dpp::ll_error, "Ping to db failed, tryn to reconnect.");

                            conn.connect(config.sql_db, config.sql_host,
                                         config.sql_user, config.sql_password);
                            sleep(30);
                            continue;
                        }
                        sleep(60 * 60 * 1);
                    }
                });

                thr_ping_loop1.detach();
                argc = 0;

                cog::regis_commands(bot);
                ticket::init_ticket_events(bot, conn, config);
                ticket::init_ticket_commands(bot);
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

    bot.on_slashcommand([&bot, &conn](const dpp::slashcommand_t &event) {
        dpp::interaction interaction = event.command;
        dpp::command_interaction cmd_data = interaction.get_command_interaction();

        if (interaction.get_command_name() == "ticket") {
            ticket::ticket_commands(bot, event, cmd_data, conn);
        }

        if (interaction.get_command_name() == "ping") {
            event.thinking(true);
            double ws_ping = bot.get_shard(0)->websocket_ping;

            event.edit_response(dpp::message(
                    fmt::format("I'm still alive. Latency: {0:.02f}ms", (bot.rest_ping + ws_ping) * 1000))
                                        .set_flags(dpp::m_ephemeral));
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

