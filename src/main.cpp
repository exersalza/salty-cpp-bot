// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 29.10.22.
// When the javelin hit's there, the guys are literally grown up and got a job.


#include <regex>
#include <iostream>
#include <unistd.h>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <mysql++/mysql++.h>

#include "include/help.hpp"
#include "include/user.hpp"
#include "include/admin.hpp"
#include "include/tests.hpp"
#include "include/utils.hpp"
#include "include/config.hpp"
#include "include/ticketCogs.hpp"
#include "include/initCommands.hpp"


int main(int argc, char *argv[]) {
    // Normal config shit
    std::string path = "config.json";
    cfg::Config config = cfg::Config(path);
    cfg::sql sql = config.getSqlConf();
    const std::string &token = config.getToken();

    // SQL Shit
    mysqlpp::Connection conn;
    conn.set_option(new mysqlpp::MultiStatementsOption(true));

    conn.connect(sql.db, sql.host, sql.user, sql.password);

    if (!conn.connected()) {
        std::cout << "Couldn't connect to db..." << std::endl;
        return -1;
    }

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    dpp::cache<dpp::message> bot_message_cache;

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot, &argc, &argv, &conn, &sql, &config](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            if (argc == 2 and strcmp(argv[1], "--init-commands") != 0) {
                std::thread thr_presence([&bot]() {
                    bot.log(dpp::ll_info, "Presence warmup");
                    sleep(90);
                    bot.log(dpp::ll_info, "Presence gonna start now.");

                    while (true) {
                        try {
                            bot.set_presence(dpp::presence(dpp::ps_online, dpp::activity_type::at_watching, "the development of me."));
                            sleep(120);
                        } catch (std::exception &e) {
                            bot.log(dpp::ll_error, fmt::format("FUCK, somethin went wron {0}", e.what()));
                        }
                    }
                });
                thr_presence.detach();

                std::thread thr_ping_loop1([&conn, &bot, &sql]() {
                    bot.log(dpp::ll_debug, "event_loop 'ping_loop1' started.");
                    while (true) {
                        conn.connect(sql.db, sql.host, sql.user, sql.password);
                        if (!conn.ping()) {
                            bot.log(dpp::ll_error, "Ping to db failed, tryn again in 30 seconds.");

                            conn.connect(sql.db, sql.host, sql.user, sql.password);
                            sleep(30);
                            continue;
                        }
                        conn.disconnect();
                        sleep(60 * 60 * 1);
                    }
                });

                thr_ping_loop1.detach();
                --argc;

                // IT'S NOT MANUEL ANYMORE
                createcmds(bot);

                ticket::init_ticket_events(bot, conn, config, sql);
                admin::init_admin_events(bot);
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

    bot.on_message_create([](const dpp::message_create_t &event) {
        if (event.msg.author.is_bot())
            return;


        std::string msg_content = event.msg.content;
        std::vector<std::string> splitet_msg_cont = u::split(msg_content);

        if (u::stolower(splitet_msg_cont[0]) == "hello" && u::stolower(splitet_msg_cont[1]) == "world!")
            event.reply("World Hello!");

    });

    bot.on_slashcommand([&bot, &conn, &config, &sql](const dpp::slashcommand_t &event) {
        dpp::interaction interaction = event.command;
        dpp::command_interaction cmd_data = interaction.get_command_interaction();

        if (interaction.get_command_name() == "ticket") {
            ticket::ticket_commands(bot, event, cmd_data, conn, config, sql);
        }

        if (interaction.get_command_name() == "admin") {
            admin::admin_commands(bot, event, cmd_data, config);
        }

        if (interaction.get_command_name() == "help") {
            cog::help_commands(bot, event, cmd_data, config);
        }

        if (interaction.get_command_name() == "user") {
            user::user_commands(bot, event, cmd_data);
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
            bot.log(dpp::ll_debug, "starting...");
            bot.start(false);
        } catch (std::exception &e) {
            bot.log(dpp::ll_error, fmt::format("Oh shit, not good {}", e.what()));
        }
        sleep(30);
    }
    return 0;
}

