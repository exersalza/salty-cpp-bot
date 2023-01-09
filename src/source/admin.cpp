// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/17/22.
//


#include "../include/admin.hpp"
#include "../include/ticketCogs.hpp"

// later usage
void admin::init_admin_events(dpp::cluster &bot) {
}

void admin::admin_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                           const dpp::command_interaction &cmd_data, const cfg::Config &conf) {
    auto sc = cmd_data.options[0];

    dpp::guild guild = event.command.get_guild();
    short c = 0;


    if (sc.name == "send") {
        bool is_embed = false;
        bool set_server_image = false;
        std::string title;
        std::string res;

        for (auto &i : sc.options) {
            if (i.name == "embed") {
                is_embed = sc.get_value<bool>(c);
            }

            if (i.name == "server_image") {
                set_server_image = sc.get_value<bool>(c);
            }

            if (i.name == "file") {
                auto at_id = sc.get_value<dpp::snowflake>(c);
                auto at = event.command.get_resolved_attachment(at_id);

                if (at.content_type.find("charset=utf-8") && at.content_type.find("text/plain")) {
                    event.reply("Can't use that body content, file is not UTF-8 or not a Text file.");
                    ++c;
                    continue;
                }

                char url[at.url.length()];
                u::stoc(at.url, url);

                url[strlen(url)-2] = '\0';
                res = u::requests(url);
            }

            if (i.name == "title") {
                title = sc.get_value<std::string>(c);
            }

            ++c;
        }

        if (!title.length()) title = res.substr(0, res.find(' '));

        if (is_embed) {
            dpp::embed em;

            em.set_title(title)
              .set_description(res)
              .set_color(conf.b_color)
              .set_footer("Kenexar.eu", bot.me.get_avatar_url())
              .set_timestamp(time(nullptr));

            if (set_server_image)
                em.set_thumbnail(event.command.get_guild().get_icon_url());

            bot.message_create(dpp::message(event.command.channel_id, em));
        } else {
            bot.message_create(dpp::message(event.command.channel_id,
                                            fmt::format("**{0}**\n\n{1}", title, res)));
        }

        event.reply(dpp::message(event.command.channel_id, "Message created.").set_flags(dpp::m_ephemeral));
    }

}

void admin::init_verify_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::sql &sql) {
    bot.log(dpp::ll_debug, "Initializing 'verify_events'");

    bot.on_button_click([&bot, &c, &sql](const dpp::button_click_t &event) {
        auto event_cmd = event.command;

        if (event.custom_id == "verify") {
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
            role_id = res[0]["role_id"];
            std::cout << role_id << '\n';

            bot.guild_member_add_role(event.command.guild_id, user.id, role_id);
        }
    });
}

void admin::verify_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                           const dpp::command_interaction &cmd_data, const cfg::Config &conf,
                           mysqlpp::Connection &c, const cfg::sql &sql) {

    auto sc = cmd_data.options[0];

    if (sc.name == "role") {
        event.thinking(true);
        auto sub = sc.options[0];
        auto _role = sc.get_value<dpp::snowflake>(0);// 763183221209956362
        size_t role = 0;

        return;

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

            event.edit_response(fmt::format("<@&{0}> was set as an Verified role.", role));
        } catch (std::exception &e) {
            bot.log(dpp::ll_error, fmt::format("Can't create Verify Role, idk why {0}", e.what()));
            event.edit_response("Can't do that right now, please try again later.");
        }
    }

    if (sc.name == "send") {
        dpp::embed em;

        em.set_title("Verify")
            .set_description("Please verify yourself with a press on the button below.")
            .set_color(conf.b_color)
            .set_footer("Kenexar.eu - Verify", bot.me.get_avatar_url());

        event.reply(dpp::message("Verify message created.").set_flags(dpp::m_ephemeral));
        bot.message_create(dpp::message(event.command.channel_id, em).add_component(
                    dpp::component().add_component(dpp::component()
                                                    .set_type(dpp::cot_button)
                                                    .set_id("verify")
                                                    .set_emoji("✔️")
                                                    .set_label("Verify")
                                                    .set_style(dpp::cos_primary))));
    }
}
