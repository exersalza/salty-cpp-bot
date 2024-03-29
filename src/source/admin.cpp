// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/17/22.
// ⬇️ ⬆️    --




#include "../include/admin.hpp"
#include "../include/ticket.hpp"

// later usage
void admin::init_admin_events(dpp::cluster &bot) {
}

void admin::admin_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                           const dpp::command_interaction &cmd_data, const cfg::Config &conf) {
    auto sc = cmd_data.options[0];

    dpp::guild guild = event.command.get_guild();
    short c = 0;

    bot.log(dpp::ll_debug, fmt::format("Admin command invokation at {} command: {} {}", guild.name, event.command.get_command_name(), sc.name));

    if (sc.name == "send") {
        bool is_embed = false;
        bool set_server_image = false;
        bool is_reaction = false;
        bool is_verify = false;
        std::string title;
        std::string res;
        size_t res_length;
        size_t color = conf.b_color;
        dpp::message msg;

        const dpp::snowflake &channel_id = event.command.channel_id;

        for (auto &i: sc.options) {
            if (i.name == "embed") {
                is_embed = sc.get_value<bool>(c);
            }

            if (i.name == "verify") {
                is_verify = sc.get_value<bool>(c);
                color = 0x00ff00;
            }

            if (i.name == "server_image") {
                set_server_image = sc.get_value<bool>(c);
            }

            if (i.name == "reaction") {
                is_reaction = sc.get_value<bool>(c);
            }

            if (i.name == "text") {
                res = sc.get_value<std::string>(c);

                if (res.length() > 4096)
                    res_length = 4097;

            }

            if (i.name == "file") {
                auto at_id = sc.get_value<dpp::snowflake>(c);
                auto at = event.command.get_resolved_attachment(at_id);

                if (at.content_type.find("charset=utf-8") && at.content_type.find("text/plain")) {
                    event.reply("Can't use that body content, file is not UTF-8 or not a Text file.");
                    ++c;
                    return;
                }

                char url[at.url.length()];
                u::stoc(at.url, url);

                // url[strlen(url)-2] = '\0'; was a temporary bug inside the discord api, but it's not there anymore.
                res = u::requests(url);
                if (res.length() >= INT_MAX) {
                    res_length = 4097;
                    continue;
                }

                res_length = res.length();
            }

            if (i.name == "title") {
                title = sc.get_value<std::string>(c);

                if (title.length() > 256) {
                    event.reply(dpp::message(channel_id,
                                             "Message can't be created with an Title more than 256 characters"));
                    return;
                }
            }

            ++c;
        }

        if (is_reaction && is_verify) {
            event.reply(dpp::message(channel_id, "Can't create message with both `verify` and `reactions` set to true."));
            return;
        }

        if (!title.length() && !res.length()) {
            event.reply(dpp::message(channel_id, "Can't create message, title and content are missing.").set_flags(dpp::m_ephemeral));
            return;
        }

        if (!title.length()) title = res.substr(0, res.find(' '));

        std::vector<std::string> reactions;

        if (is_reaction) {
            reactions.reserve(2);
            reactions.emplace_back("⬆️");
            reactions.emplace_back("⬇️");
        }

        if (is_embed) {
            if (res_length > 4096) {
                event.reply(dpp::message(channel_id, "Message can't be created because the Text for "
                                                     "it, is too long to fit in a Embed. (Max size 4096 char.)"));
                return;
            }
            dpp::embed em;

            em.set_title(title)
                    .set_description(res)
                    .set_color(color)
                    .set_footer("Kenexar.eu", bot.me.get_avatar_url())
                    .set_timestamp(time(nullptr));

            if (set_server_image)
                em.set_thumbnail(event.command.get_guild().get_icon_url());

            msg = dpp::message(channel_id, em);

        } else {
            if ((title.length() + res_length) > 1996) {
                event.reply(dpp::message(channel_id, fmt::format(
                        "Can't create message, Content or title needs to have a lower character count. ({0}/1996)",
                        (title.length() + res_length))));
                return;
            }
            msg = dpp::message(channel_id, fmt::format("**{0}**\n\n{1}", title, res));
        }

        if (is_verify) {
            msg.add_component(
                    dpp::component().add_component(dpp::component()
                                                       .set_type(dpp::cot_button)
                                                       .set_id("verify")
                                                       .set_emoji("✔️")
                                                       .set_label("Verify")
                                                       .set_style(dpp::cos_primary)));
        }

        if (!is_reaction) {
            bot.message_create(msg);
        } else {
            bot.message_create(msg, [&bot, event, reactions](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }

                auto msg = confm.get<dpp::message>();

                for (const auto &i: reactions) {
                    bot.message_add_reaction(msg, i);
                }
            });
        }

        event.reply(dpp::message(channel_id, "Message created.").set_flags(dpp::m_ephemeral));
    }

}

void admin::init_verify_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::sql &sql) {
    bot.log(dpp::ll_debug, "Initializing 'verify_events'");

    bot.on_button_click([&bot, &c, &sql](const dpp::button_click_t &event) {
        auto event_cmd = event.command;

        if (event.custom_id == "verify") {
            bot.log(dpp::ll_debug, fmt::format("Verified user: {}", event.command.get_guild().name));
            event.thinking(true);

            dpp::user user = event_cmd.usr;
            size_t role_id;

            ticket::connect(c, sql);
            mysqlpp::Query query = c.query();
            query << fmt::format("select role_id from salty_cpp_bot.verify where server_id = {0}",
                                 event.command.guild_id);

            mysqlpp::StoreQueryResult res = query.store();
            u::kill_query(query);
            c.disconnect();

            if (res.empty()) {
                event.edit_response("Verify system is not active, please contact a moderator.");
                return;
            }

            role_id = res[0]["role_id"];

            bot.guild_get_member(event.command.guild_id, event.command.usr.id,
                                 [&bot, role_id, event](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }

                auto member = confm.get<dpp::guild_member>();

                if (std::count(member.roles.begin(), member.roles.end(), role_id)) {
                    event.edit_response("You cannot verify yourself twice.");
                    return;
                }

                bot.guild_member_add_role(event.command.guild_id, member.user_id, role_id);
                event.edit_response("You are Verified now. :)");
            });

        }
    });
}

void admin::verify_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                            const dpp::command_interaction &cmd_data, const cfg::Config &conf,
                            mysqlpp::Connection &c, const cfg::sql &sql) {

    auto sc = cmd_data.options[0];
    auto guild = event.command.get_guild();

    bot.log(dpp::ll_debug, fmt::format("Verify command invokation at {} command: {} {}", guild.name, event.command.get_command_name(), sc.name));

    if (sc.name == "role") {
        set_verify_role(bot, event, c, sql, sc);
    }

    if (sc.name == "send") {
        dpp::embed em;

        em.set_title("Verify")
                .set_description("Please verify yourself with a press on the button below.")
                .set_color(0x00ff00)
                .set_footer("Kenexar.eu - Verify", bot.me.get_avatar_url());

        event.reply(dpp::message(fmt::format("Verify message created. {0} please don't forget to set a role "
                                             "with `/verify role` otherwise will the verify system not work.",
                                             event.command.usr.get_mention())).set_flags(dpp::m_ephemeral));

        bot.message_create(dpp::message(event.command.channel_id, em).add_component(
                dpp::component().add_component(dpp::component()
                                                       .set_type(dpp::cot_button)
                                                       .set_id("verify")
                                                       .set_emoji("✔️")
                                                       .set_label("Verify")
                                                       .set_style(dpp::cos_primary))));

        ticket::connect(c, sql);

        mysqlpp::Query query = c.query();
        query << fmt::format("if not exists(select server_id from salty_cpp_bot.verify where server_id='{0}') then"
                             "  insert into salty_cpp_bot.verify (server_id) values ('{0}');"
                             "end if;", guild.id);

        // Do the query stuff here, so I can drop the db connection afterwards. copy & paste monkey here
        query.execute();
        c.disconnect();
    }
}

void admin::set_verify_role(const dpp::cluster &bot, const dpp::slashcommand_t &event, mysqlpp::Connection &c,
                            const cfg::sql &sql, dpp::command_data_option &sc) {
    event.thinking(true);
    auto sub = sc.options[0];
    size_t role = sc.get_value<dpp::snowflake>(0);


    try {
        ticket::connect(c, sql);

        mysqlpp::Query query = c.query();
        query << fmt::format("if not exists(select server_id from salty_cpp_bot.verify where server_id='{0}') then"
                             "  insert into salty_cpp_bot.verify (server_id, role_id) values ({0}, {1});"
                             "else"
                             "  update salty_cpp_bot.verify set role_id = {1} where server_id = {0};"
                             "end if;", event.command.guild_id, role);

        // Do the query stuff here, so I can drop the db connection afterwards. copy & paste monkey here
        query.execute();
        u::kill_query(query);
        c.disconnect();

        event.edit_response(fmt::format("<@&{0}> is the new Verified role.", role));
    } catch (std::exception &e) {
        bot.log(dpp::ll_error, fmt::format("Can't create Verify Role, idk why {0}", e.what()));
        event.edit_response("Can't do that right now, please try again later.");
    }
}
