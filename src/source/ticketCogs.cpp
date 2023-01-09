// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/4/22.
// When you pour water on a rock, nothing happens.

#include <regex>
#include <iostream>
#include <unistd.h>
#include <dpp/dpp.h>
#include <fmt/format.h>

#include "../include/utils.hpp"
#include "../include/ticketCogs.hpp"


void ticket::connect(mysqlpp::Connection &c, const cfg::sql &sql) {
    c.set_option(new mysqlpp::MultiStatementsOption(true));
    c.connect(sql.db, sql.host, sql.user, sql.password);
}

dpp::message ticket::create_ticket_message(size_t &channel_id, dpp::embed &embed) {
    return dpp::message(channel_id, embed)
            .add_component(dpp::component().add_component(dpp::component()
                                                                  .set_label("Create!")
                                                                  .set_type(dpp::cot_button)
                                                                  .set_emoji("📝")
                                                                  .set_id("ticket_create")));
}

void ticket::init_ticket_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config, cfg::sql &sql) {
    bot.log(dpp::ll_debug, "Initializing 'ticket_events'");

    bot.on_button_click([&bot, &c, &config, &sql](const dpp::button_click_t &event) {
        dpp::interaction event_cmd = event.command;

        if (event.custom_id == "ticket_create") {
            // start thinking and then decide what happens later, don't let the user wait
            // it's like a traffic light, you wait 2 seconds and everyone is mad at you.
            event.thinking(true);

            size_t category_id, notify_id;
            int ticket_count;
            int max_ticket_count;
            std::vector<size_t> mod_roles;

            connect(c, sql);

            mysqlpp::Query query = c.query();
            query << fmt::format("SELECT ticket.category_id, ticket.count, ticket.notify_channel, ticket.enabled, "
                                 "ticket.max_ticket FROM salty_cpp_bot.ticket WHERE ticket.server_id = '{0}';"
              "UPDATE salty_cpp_bot.ticket SET count=count + 1 WHERE ticket.server_id = {0};"
              "select count(*) from salty_cpp_bot.cur_tickets where server_id = {0} and user_id = {1};"
              "select ticket_access_roles.role_id from salty_cpp_bot.ticket_access_roles where ticket_access_roles.server_id = {0};",
                    event_cmd.guild_id, event_cmd.usr.id);

            // Do the query stuff here, so I can drop the db connection afterwards.
            mysqlpp::StoreQueryResult res = query.store();
            query.store_next();

            auto res_cur_ticket = query.store_next();
            mysqlpp::StoreQueryResult role_res = query.store_next();

            u::kill_query(query);

            c.disconnect();

            // system dribble
            if (res.empty() || !res[0]["enabled"]) {
                event.edit_original_response(dpp::message("Ticket's are not enabled, contact an Moderator when you want to create a Ticket."));
                return;
            }

            category_id = res[0]["category_id"];
            ticket_count = res[0]["count"];
            notify_id = res[0]["notify_channel"];
            max_ticket_count = res[0]["max_ticket"];


            int usr_cur_ticket = res_cur_ticket[0][0];

            if (max_ticket_count) {
                if (max_ticket_count < usr_cur_ticket) {
                    event.edit_original_response(dpp::message("You have already to many tickets open, please close one first to open another one."));
                    return;
                }
            }

            for (size_t i = 0; i < role_res.num_rows(); ++i) {
                mod_roles.push_back(role_res[i]["role_id"]);
            }

            dpp::embed em;
            dpp::channel channel;
            std::string channel_mention;
            dpp::guild guild = event_cmd.get_guild();
            dpp::user user = event_cmd.usr;

            channel.set_name(fmt::format("ticket-{0}", ticket_count + 1))
                    .set_type(dpp::channel_type::CHANNEL_TEXT)
                    .set_guild_id(event_cmd.guild_id)
                    .set_parent_id(category_id);

            channel.add_permission_overwrite(event_cmd.usr.id, dpp::overwrite_type::ot_member,
                                                dpp::permissions::p_send_messages |
                                                dpp::permissions::p_view_channel, 0)
                .add_permission_overwrite(guild.id, dpp::overwrite_type::ot_role, 0, dpp::permissions::p_view_channel);

            for (size_t role : mod_roles) {
                channel.add_permission_overwrite(role,
                                                 dpp::overwrite_type::ot_role,
                                                 dpp::permissions::p_view_channel, 0);
            }

            bot.channel_create(channel, [&, event, notify_id](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    confm_error<dpp::button_click_t>(bot, event, confm);
                    return;
                }

                auto new_ticket = confm.get<dpp::channel>();

                connect(c, sql);
                mysqlpp::Query query = c.query();

                query << fmt::format(
                        "insert into salty_cpp_bot.cur_tickets (server_id, user_id, ticket_id) values ({0}, {1}, {2})",
                        guild.id, user.id, new_ticket.id);

                query.execute();
                c.disconnect();

                event.edit_original_response(dpp::message(
                        fmt::format("Ticket got created {0}", new_ticket.get_mention())
                ).set_flags(dpp::m_ephemeral));


                em.set_title(new_ticket.name)
                  .set_color(0xbc3440)
                  .set_description(fmt::format("{0} Welcome to your ticket, a Moderator will be there shortly.", event.command.usr.get_mention()))
                  .set_timestamp(time(nullptr));

                dpp::message msg(new_ticket.id, em);
                msg.add_component(dpp::component().add_component(
                                        dpp::component()
                                                .set_type(dpp::cot_button)
                                                .set_id("ticket_close")
                                                .set_label("Close ticket")
                                                .set_emoji("🔒")
                                                .set_style(dpp::cos_danger)));

                bot.message_create(msg);

                // notify mods when id is provided
                if (notify_id > 0) {
                    dpp::embed e;
                    e.set_title(fmt::format("{0} got created by {1}", new_ticket.name, event.command.usr.format_username()))
                     .set_description(fmt::format("Claimed by: {0}\nTicket: {1}", "None", new_ticket.get_mention()))
                     .set_timestamp(time(nullptr))
                     .set_footer(fmt::format("{0}", new_ticket.id), bot.me.get_avatar_url())
                     .set_color(0xbc3440);

                    dpp::message mod_msg(notify_id, e);

                    mod_msg.add_component(dpp::component().add_component(dpp::component()
                                                            .set_type(dpp::cot_button)
                                                            .set_label("Claim")
                                                            .set_id("ticket_claim")
                                                            .set_style(dpp::cos_success)));
                    try {
                        bot.message_create(mod_msg);
                    } catch (std::exception &e) {
                        std::cout << e.what() << std::endl;
                    } // in case the bot cannot find the channel id, I will put something in there later.

                }
            });
            return;
        }

        if (event.custom_id == "ticket_delete") {
            bot.message_create(dpp::message(event_cmd.channel_id, "Ticket gets deleted in 1 sec."));
            try {
                bot.channel_delete_sync(event_cmd.channel_id);
            } catch (std::exception &e) {}
            return;
        }

        if (event.custom_id == "ticket_close") {
            auto &channel = event.command.get_channel();

            if (channel.name.find("closed-") != std::string::npos) {
                event.reply(dpp::message("Ticket is already closed.").set_flags(dpp::m_ephemeral));
                return;
            }

            dpp::embed em;

            em.set_title("Are you sure that you want to close the Ticket?")
              .set_color(0xbc3440);

            dpp::message you_sure_msg(dpp::message(event_cmd.channel_id, em)
                                        .add_component(dpp::component()
                                            .add_component(
                                                dpp::component()
                                                    .set_label("Close it")
                                                    .set_type(dpp::cot_button)
                                                    .set_style(dpp::cos_danger)
                                                    .set_id("ticket_sure"))

                                            .add_component(
                                                dpp::component()
                                                    .set_label("Nah, let it open")
                                                    .set_type(dpp::cot_button)
                                                    .set_style(dpp::cos_secondary)
                                                    .set_id("ticket_sure_nah"))));

            event.reply(you_sure_msg);
            return;
        }

        if (event.custom_id == "ticket_sure_nah") {
            bot.message_delete(event_cmd.message_id, event_cmd.channel_id);
            return;
        }

        if (event.custom_id == "ticket_sure") {
            dpp::channel channel = event_cmd.get_channel();
            size_t channel_id = channel.id;
            size_t message_id = event_cmd.message_id;

            event.reply(dpp::message("ticket gets closed"));

            // prevent rate limitation on channel edit
            if (channel.name.find("closed-") != std::string::npos)
                bot.channel_edit(channel.set_name(fmt::format("closed-{0}", channel.name)));

            connect(c, sql);

            mysqlpp::Query query = c.query();
            query << fmt::format("select user_id from cur_tickets where server_id = {0} and ticket_id = {1};",
                                 event_cmd.guild_id, event_cmd.channel_id);

            mysqlpp::StoreQueryResult res = query.store();

            // here's a segfault, but I'm not sure if it fixed yet, comes from missing data in the database.
            long user_id = res[0]["user_id"];
            u::kill_query(query);
            c.disconnect();

            bot.channel_delete_permission(channel, user_id, [&message_id, &channel_id, &bot, &config, &event](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                }

                dpp::embed em;

                bot.message_delete(message_id, channel_id);

                em.set_title("Admin Controls")
                  .set_description("Here you can do basic admin stuff.")
                  .set_color(config.b_color)
                  .set_footer("Kenexar.eu", bot.me.get_avatar_url())
                  .set_timestamp(time(nullptr));

                bot.message_create(dpp::message(channel_id, em).add_component(
                                        dpp::component().add_component(
                                        dpp::component()
                                            .set_type(dpp::cot_button)
                                            .set_id("ticket_delete")
                                            .set_emoji("🗑️")
                                            .set_style(dpp::cos_danger)
                                            .set_label("Delete ticket"))
                                        .add_component(
                                        dpp::component()
                                            .set_label("Reopen ticket")
                                            .set_id("ticket_reopen")
                                            .set_emoji("🔓")
                                            .set_type(dpp::cot_button)
                                            .set_style(dpp::cos_secondary))));

            });
            return;
        }

        if (event.custom_id == "ticket_reopen") {
            event.thinking();
            dpp::channel channel = event_cmd.get_channel();

            connect(c, sql);
            mysqlpp::Query query = c.query();

            query << fmt::format(
                    "select user_id from salty_cpp_bot.cur_tickets where server_id = {0} and ticket_id = {1};",
                        event_cmd.guild_id, event_cmd.channel_id);

            mysqlpp::StoreQueryResult res = query.store();
            size_t user_id = res[0]["user_id"];

            u::kill_query(query);
            c.disconnect();

            bot.message_delete(event_cmd.message_id, channel.id);

            // prevent rate limitation. works sometimes. but still needs some work. I think it is fixed, but the tests will show.
            sleep(5);
            try {
                bot.channel_edit_permissions(channel.id, user_id, dpp::permissions::p_view_channel, 0, true);

                event.edit_original_response(fmt::format("<@{0}> your ticket got Re-Opened.", user_id));
            } catch (std::exception &e) {
                try {
                    event.edit_original_response(dpp::message("Can't reopen ticket, try again in a few minutes"));

                // prevent edge case where the mod removes the channel before it can be re-opened
                } catch (std::exception &f) { return; }
            }

            return;
        }

        if (event.custom_id == "ticket_claim") {
            dpp::message orig_msg = event.command.msg;

            dpp::embed &orig_em = orig_msg.embeds[0];
            orig_em.set_description(
                    std::regex_replace(orig_em.description,
                                       std::regex("None"),
                                       event.command.usr.get_mention()));

            auto &orig_comp = orig_msg.components[0];
            orig_comp.components[0].set_disabled(true);

            bot.message_edit(orig_msg);
            event.reply(dpp::message("Ticket claimed").set_flags(dpp::m_ephemeral));
            return;
        }

    });

    bot.on_channel_delete([&c, &sql](const dpp::channel_delete_t &event) {
        // db cleaner, not sure yet if I want to let this in, but it's there.
        connect(c, sql);

        mysqlpp::Query query = c.query();

        query << fmt::format("if exists(select * from salty_cpp_bot.cur_tickets where ticket_id = {0} and server_id = {1}) then "
                             "delete from salty_cpp_bot.cur_tickets where ticket_id = {0} and server_id = {1}; "
                             "end if;",
                             event.deleted->id, event.deleted->guild_id);

        query.execute();
        c.disconnect();
    });
}


template<typename T>
void ticket::confm_error(const dpp::cluster &bot, const T &event,
                         const dpp::confirmation_callback_t &confm) {
    std::string err = confm.get_error().message;
    event.edit_original_response(
            dpp::message(
                    "Something went wrong, please try again later, or contact an Moderator.")
                    .set_flags(dpp::m_ephemeral));

    // not a typo
    bot.log(dpp::ll_error, fmt::format("FUCK Somethin went wron {}", err));
}


void ticket::ticket_commands(dpp::cluster &bot,
                             const dpp::slashcommand_t &event,
                             const dpp::command_interaction &cmd_data,
                             mysqlpp::Connection &c,
                             cfg::Config &conf,
                             cfg::sql &sql) {

    auto sc = cmd_data.options[0];

    dpp::embed em;
    dpp::guild guild = event.command.get_guild();

    // ik you could use a switch here, but I'm not a fan of indentation so no switch here.
    if (sc.name == "create") {
        connect(c, sql);
        mysqlpp::Query query = c.query();
        std::string title;

        {
            query << fmt::format("if not exists(select * from ticket where server_id='{0}') then"
                                    " insert into ticket (server_id) values ('{0}');"
                                 "else"
                                    " update ticket set ticket.enabled = 1 where server_id = {0}; "
                                 "end if; ", event.command.guild_id);

            query.execute();
        }

        query << fmt::format("select ticket.ticket_title from ticket where ticket.server_id = '{0}';",
                             event.command.guild_id);

        mysqlpp::StoreQueryResult res = query.store();
        u::kill_query(query);
        c.disconnect();

        title = (std::string) res[0]["ticket_title"];


        em.set_color(0xbc3440)
          .set_title(title)
          .set_description("Create a ticket with 📝")
          .set_footer("Kenexar.eu", bot.me.get_avatar_url())
          .set_timestamp(time(nullptr));

        try {
            if (!sc.options.empty()) {
                std::size_t channel_id = sc.get_value<dpp::snowflake>(0);
                bot.message_create(ticket::create_ticket_message(channel_id, em));
            } else {
                std::size_t channel_id = event.command.channel_id;
                bot.message_create(ticket::create_ticket_message(channel_id, em));
            }
            event.reply(dpp::message("Ticket message got created.").set_flags(dpp::m_ephemeral));

        } catch (std::exception &e) {
            bot.log(dpp::ll_error, fmt::format("Oh no, not good {}", e.what()));

            event.reply("Something went wrong, try again later.");
        }

    }

    if (sc.name == "set") {
        event.thinking(true);
        std::string output_string;

        // can only be one bc you can't chain sub command groups.
        auto sub = sc.options[0];

        // we'll use the find thingy here to prevent the edge case that somehow he can't compare a string with a string.
        // you might think "that's not possible" I had it one time, and it was not funny.
        if (sub.name.find("category") != std::string::npos) {
            dpp::channel _channel = event.command.get_channel();
            size_t category_id = _channel.parent_id;

            if (!sub.options.empty())
                category_id = sub.get_value<dpp::snowflake>(0);

            bot.channels_get(event.command.guild_id, [&bot, &c, event, category_id, &sql](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }

                auto t_channel = confm.get<dpp::channel_map>()[category_id];
                if (!t_channel.is_category()) {
                    event.edit_original_response(fmt::format("<#{0}> is not a Category.", category_id));
                    return;
                }

                connect(c, sql);
                mysqlpp::Query query = c.query();

                query << fmt::format("if exists(select * from salty_cpp_bot.ticket where server_id = {0}) then"
                                     " update salty_cpp_bot.ticket set category_id = {1} where server_id = {0}; "
                                     "else"
                                     " insert into salty_cpp_bot.ticket (server_id, category_id) values ({0}, {1}); "
                                     "end if;",
                                     event.command.guild_id, category_id);

                query.execute();
                c.disconnect();

                event.edit_original_response(fmt::format("<#{0}> was set as new Category.", category_id));
            });
        }

        if (sub.name.find("roles") != std::string::npos) {
            dpp::channel _channel = event.command.get_channel();
            short count = 0;
            std::string output = "Roles: ";

            if (!sub.options.empty()) {
                connect(c, sql);
                mysqlpp::Query query = c.query();

                for (auto& i : sub.options) {
                    size_t value = sub.get_value<dpp::snowflake>(count);

                    query << fmt::format(
                        "if not exists(select * from ticket_access_roles where role_id='{1}') then"
                            " insert into ticket_access_roles (server_id, role_id) values ('{0}', '{1}');"
                        "end if; ", event.command.guild_id, value);

                    output += fmt::format("<@&{0}> ", value);
                    ++count;
                }

                query.execute();
                c.disconnect();
            }

            output += "added for Moderation/Support.";
            event.edit_original_response(output);
        }

        if (sub.name.find("notify") != std::string::npos) {
            size_t channel_id = event.command.channel_id;

            if (!sub.options.empty())
                channel_id = sub.get_value<dpp::snowflake>(0);

            bot.channels_get(event.command.guild_id, [&bot, &c, event, channel_id, &sql](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }

                auto t_channel = confm.get<dpp::channel_map>()[channel_id];
                if (!t_channel.is_text_channel()) {
                    event.edit_original_response(fmt::format("<#{0}> is not a Text channel.", channel_id));
                    return;
                }

                connect(c, sql);
                mysqlpp::Query query = c.query();

                query << fmt::format("if exists(select * from salty_cpp_bot.ticket where server_id = {0}) then"
                                     " update salty_cpp_bot.ticket set notify_channel = {1} where server_id = {0}; "
                                     "else"
                                     " insert into salty_cpp_bot.ticket (server_id, notify_channel) values ({0}, {1}); "
                                     "end if;",
                                     event.command.guild_id, channel_id);

                query.execute();
                c.disconnect();
                event.edit_original_response(fmt::format("<#{0}> was set as new Notify channel.", channel_id));
            });
        }

        if (sub.name.find("maxticketcount") != std::string::npos) {
            long count = 0;

            //TODO: Change output on input of 0

            if (!sub.options.empty())
                count = sub.get_value<long>(0);

            if (count > INT_MAX) {
                event.edit_original_response(dpp::message(event.command.channel_id,
                                         fmt::format("Max Ticket count can't be higher then {0}", INT_MAX)).set_flags(64)); // 64 = dpp::m_ephemeral
                return;
            }

            connect(c, sql);
            mysqlpp::Query query = c.query();
            query << fmt::format("if exists(select count(*) from ticket where server_id = {0}) then"
                                 " update ticket set max_ticket = {1} where server_id = {0}; "
                                 "else"
                                 " insert into ticket (server_id, max_ticket) values ({0}, {1}); "
                                 "end if;", event.command.guild_id, count);

            query.execute();
            c.disconnect();

            std::stringstream output;

            if (count)
                output << fmt::format("{0} was setted a new max count. To deactivate the limit use 0 as count.", count);
            else
                output << "Max Ticket count per user is now Deactivated.";


            event.edit_original_response(dpp::message(event.command.channel_id, output.str()));
        }
    }

    if (sc.name == "remove" and !sc.options.empty()) {
        event.thinking(true);
        std::string output;

        auto sub = sc.options[0];

        if (sub.name.find("category") != std::string::npos
            || sub.name.find("notify") != std::string::npos) {

            std::string table_name;

            table_name = (sub.name == "category") ? "category_id" : "notify_channel";

            connect(c, sql);
            mysqlpp::Query query = c.query();
            query << fmt::format("update salty_cpp_bot.ticket set {0} = 0 where server_id = {1}", table_name, event.command.guild_id);

            query.execute();
            c.disconnect();

            event.edit_original_response(fmt::format("{0} id removed from database.", sub.name));
        }

        if (sub.name.find("roles") != std::string::npos) {
            short count = 0;

            output += "Removed: ";

            if (!sub.options.empty()){
                connect(c, sql);
                mysqlpp::Query query = c.query();

                for (auto& i : sub.options) {
                    size_t value = sub.get_value<dpp::snowflake>(count);

                    query << fmt::format(
                        "delete from salty_cpp_bot.ticket_access_roles where server_id = {0} and role_id = {1}; ",
                            event.command.guild_id, value);

                    output += fmt::format("<@&{0}> ", value);
                    ++count;
                }
                c.disconnect();
                query.execute();

                output += "from the database.";
                event.edit_original_response(output);
            }
        }
    }

    if (sc.name == "config") {
        //TODO: send empty role when no is given, otherwise error
        em.set_title("Ticket System configuration")
          .set_description(fmt::format("Config for {0}", event.command.get_guild().name))
          .set_color(conf.b_color)
          .set_footer("Kenexar.eu", bot.me.get_avatar_url())
          .set_timestamp(time(nullptr));

        connect(c, sql);
        mysqlpp::Query query = c.query();
        query << fmt::format("if not exists(select * from ticket where server_id={0}) then"
                             " insert into ticket (server_id, enabled) values ({0}, 0); "
                             "end if;"
                             "select ticket.enabled, ticket.category_id, ticket.ticket_title, "
                             "ticket.notify_channel, ticket.count, ticket.max_ticket"
                             " from salty_cpp_bot.ticket where server_id = {0};"
                             "select ticket_access_roles.role_id from ticket_access_roles where server_id = {0};",
                             event.command.guild_id);

        query.store();
        auto res = query.store_next();
        auto res2 = query.store_next();
        c.disconnect();

        bool enabled = res[0]["enabled"];
        int ticket_count = res[0]["count"];
        auto ticket_title = (std::string) res[0]["ticket_title"];
        size_t category_id = res[0]["category_id"];
        size_t notify_channel = res[0]["notify_channel"];
        int max_ticket = res[0]["max_ticket"];
        std::stringstream roles;
        std::stringstream max_ticket_out;

        for (size_t i = 0; i < res2.num_rows(); ++i) {
            size_t role = res2[i]["role_id"];
            roles << fmt::format("<@&{0}>", role) << '\n';
        }

        if (!max_ticket)
            max_ticket_out << "Disabled";
        else
            max_ticket_out << max_ticket;


        em.add_field("Enabled", enabled ? "True" : "False", true);
        em.add_field("Max Ticket for users", fmt::format("{0}", max_ticket_out.str()), true);
        em.add_field("Category for new ticket.", fmt::format("<#{0}>", category_id), true);
        em.add_field("Notify channel for new tickets.", fmt::format("<#{0}>", notify_channel), true);
        em.add_field("Created tickets on this server",  fmt::format("{0}", ticket_count), true);

        em.add_field("Support Roles:", ((roles.str()).length() != 0) ? roles.str() : "No Support roles given", false);
        event.reply(dpp::message(event.command.channel_id, em));
    }

    if (sc.name == "enable") {
        change_state(event, c, "enabled", 1, sql);
    }

    if (sc.name == "disable") {
        change_state(event, c, "disabled", 0, sql);
    }

}

void ticket::change_state(const dpp::slashcommand_t &event,
                          mysqlpp::Connection &c,
                          const std::string &state,
                          int i_state,
                          const cfg::sql &sql) {
    connect(c, sql);
    mysqlpp::Query query = c.query();

    query << fmt::format("update salty_cpp_bot.ticket set enabled={1} where server_id = {0};",
                         event.command.guild_id, i_state);

    query.execute();
    c.disconnect();

    event.reply(dpp::message(fmt::format("Ticket system got {0}.", state)).set_flags(dpp::m_ephemeral));
}
