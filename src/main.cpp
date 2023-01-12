// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 29.10.22.
// When the javelin hit's there, the guys are literally grown up and got a job.

#include <map>
#include <regex>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <bits/stdc++.h>
#include <mysql++/mysql++.h>

#include "include/help.hpp"
#include "include/user.hpp"
#include "include/admin.hpp"
#include "include/tests.hpp"
#include "include/utils.hpp"
#include "include/config.hpp"
#include "include/ticket.hpp"
#include "include/initCommands.hpp"
#include "include/twitchIntegration.hpp"


int main(int argc, char *argv[]) {
    // Normal config shit
    cfg::Config config = cfg::Config("config.json");
    cfg::sql sql = config.getSqlConf();

    // SQL Shit
    mysqlpp::Connection conn;
    ticket::connect(conn, sql);

    if (!conn.connected()) {
        std::cout << "Couldn't connect to db..." << std::endl;
        return -1;
    }

    cfg::twitch twitch_config = config.getTwitchConf();

    std::vector<const char *> helix_header;
    std::multimap<size_t, size_t> twitch_channel_map; // channel_id and twitch_channel id
    std::map<size_t, u::twitch_channel> twitch_content_map; // twitch channel info thingi
    std::unordered_set<size_t> ids; // add all streamer ids from the db and remove dupes
    std::vector<std::string> query_strings;
    std::string bearer {fmt::format("Authorization: Bearer {0}", twitch_config.oauth)};
    std::string client_id {fmt::format("Client-Id: {0}", twitch_config.id)};

    helix_header.push_back(bearer.c_str());
    helix_header.push_back(client_id.c_str());


    // Twitch integration
    mysqlpp::Query query = conn.query();
    query << fmt::format("select * from salty_cpp_bot.twitch");
    mysqlpp::StoreQueryResult res = query.store();

    twitch_channel_map.clear();

    for (auto &i : res) {
        twitch_channel_map.insert(std::pair(i["channel_id"], i["stream_id"]));
        ids.insert(i["stream_id"]);
    }

    conn.disconnect();

    short counter = 0;
    std::stringstream query_str;

    for (auto &i : ids) {
        if (counter >= 100) {
            query_str << "first=100";
            query_strings.push_back(query_str.str());
            query_str.clear();
        }
        query_str << "user_id=" << i << "&";
        ++counter;
    }
    query_strings.push_back(query_str.str());


    // https://dev.twitch.tv/docs/api/reference/#get-streams
    for (auto &i : query_strings) {
        json response = json::parse(u::requests(fmt::format("https://api.twitch.tv/helix/streams?{0}", i).c_str(), helix_header)); // {"error":"Unauthorized","status":401,"message":"Invalid OAuth token"}

        if (response.find("status") != response.end()) {
            twitch::generateNewToken(twitch_config);
            response = json::parse(u::requests(fmt::format("https://api.twitch.tv/helix/streams?{0}", i).c_str(), helix_header));
        }

        json &data = response["data"];
        for (json &j : data) {
            if (twitch_content_map.find(data["user_id"]) != twitch_content_map.end())
                continue;

            twitch_content_map[data["user_id"]] = {
                data["user_name"],
                data["title"],
                data["game_name"],
                data["thumbnail_url"],
                data["viewer_count"]
            };

        }
    }



    return 0;


    const std::string &token = config.getToken("dev"); // todo: remove dev
    long uptime = time(nullptr);

    std::map<int, std::string> ll_map {
            {0, "trace"},
            {1, "debug"},
            {2, "info"},
            {3, "warning"},
            {4, "error"},
            {5, "critical"}
    };

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    dpp::cache<dpp::message> bot_message_cache;

    bot.on_log([&bot, &config, &ll_map](const dpp::log_t &lt) {
        std::time_t ct = std::time(nullptr);

        std::string t = std::ctime(&ct);
        t.erase(t.end() - 1);

        std::string output_str = fmt::format("[{0}][{1}]: {2}", t, ll_map[lt.severity], lt.message);
        std::cout << output_str << '\n';

        if (!config.log_webhook.empty()) {
            if (lt.message.find("Initial") != std::string::npos) {
                return;
            }

            dpp::webhook wh(config.log_webhook);
            bot.execute_webhook(wh, dpp::message(output_str));
        }

    });

    bot.on_ready([&bot, &argc, &argv, &conn, &sql, &config](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            if (argc == 2 && !strcmp(argv[1], "--init")) {
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
                admin::init_verify_events(bot, conn, sql);
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

        if (u::stol(splitet_msg_cont[0]) == "hello" && u::stol(splitet_msg_cont[1]) == "world!")
            event.reply("World Hello!");

    });

    bot.on_slashcommand([&bot, &conn, &config, &sql, &uptime](const dpp::slashcommand_t &event) {
        dpp::interaction interaction = event.command;
        dpp::command_interaction cmd_data = interaction.get_command_interaction();


        // todo: use map for commands.
        if (interaction.get_command_name() == "ticket") {
            ticket::ticket_commands(bot, event, cmd_data, conn, config, sql);
        }

        if (interaction.get_command_name() == "verify") {
            admin::verify_commands(bot, event, cmd_data, config, conn, sql);
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

        if (interaction.get_command_name() == "uptime") {
            event.thinking(true);

            dpp::embed em;
            em.set_title("Uptime");
            em.add_field("The Bot is online since", fmt::format("<t:{0}>", uptime), false);
            em.set_footer("Kenexar.eu", bot.me.get_avatar_url());
            em.set_color(config.b_color);

            event.edit_response(dpp::message(event.command.channel_id, em));
        }

        if (interaction.get_command_name() == "credits") {
            event.thinking(true);

            dpp::embed em;

            em.set_title("Credits")
              .set_description("Here can you see, who to blame.\n\n**Bot Creator**: "
                               "[exersalza[>'-']>](https://github.com/exersalza)\n"
                               "**Website Creator**: [ZerXDE](https://github.com/zerxgit)\n\n"
                               "[Our Discord](https://discord.gg/W3Yf53dBMH) "
                               "**|** [GitHub](https://github.com/exersalza/salty-cpp-bot)")
              .set_color(config.b_color)
              .set_footer("Kenexar.eu", bot.me.get_avatar_url());

            event.edit_response(dpp::message(event.command.channel_id, em));
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

