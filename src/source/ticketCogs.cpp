//
// Created by julian on 11/4/22.
// When you pour water on a rock, nothing happens.

#include <regex>
#include <iostream>
#include <dpp/dpp.h>
#include <fmt/format.h>

#include "../include/utils.hpp"
#include "../include/ticketCogs.hpp"


void ticket::init_ticket_commands(dpp::cluster &bot) {
    bot.log(dpp::ll_debug, "Initializing 'ticket_commands'");

    dpp::slashcommand ticket("ticket", "ticket base command", bot.me.id);

    ticket.set_default_permissions(dpp::permission(dpp::p_administrator));

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "create", "Create Ticket message at current/given channel.").
                    add_option(dpp::command_option(dpp::co_channel, "channel", "Define channel", false))
    );

    ticket.add_option(dpp::command_option(dpp::co_sub_command_group, "set", "Set Category, Notify, or Roles for new tickets.")
                        .add_option(dpp::command_option(dpp::co_sub_command, "category", "set category for new tickets")
                            .add_option(dpp::command_option(dpp::co_channel, "id", "Set the category id.", false)))
                        .add_option(dpp::command_option(dpp::co_sub_command, "notify", "Set notify channel for new tickets") // set and delete
                            .add_option(dpp::command_option(dpp::co_channel, "id", "Set the notify channel id.", false)))
                        .add_option(dpp::command_option(dpp::co_sub_command, "roles", "Set Moderation roles for new tickets")
                            .add_option(dpp::command_option(dpp::co_role, "id", "Set role id", true)) // set and delete
                            .add_option(dpp::command_option(dpp::co_role, "id2", "Set role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id3", "Set role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id4", "Set role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id5", "Set role id", false))));

    ticket.add_option(dpp::command_option(dpp::co_sub_command_group, "remove", "Remove Category, Notify, or Roles for new tickets.")
                        .add_option(dpp::command_option(dpp::co_sub_command, "category", "Remove category for new tickets"))
                        .add_option(dpp::command_option(dpp::co_sub_command, "notify", "Remove notify channel for new tickets"))
                        .add_option(dpp::command_option(dpp::co_sub_command, "roles", "Remove Moderation roles for new tickets")
                            .add_option(dpp::command_option(dpp::co_role, "id", "Remove role id", true)) // set and delete
                            .add_option(dpp::command_option(dpp::co_role, "id2", "Remove role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id3", "Remove role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id4", "Remove role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id5", "Remove role id", false))));

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "init", "Start configuration wizard.")
    );

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "config", "Open config embed")
    );

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "enable", "Enable the ticket system")
    );

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "disable", "Disable the ticket system")
    );

    bot.global_command_create(ticket);
}



dpp::message ticket::create_ticket_message(size_t &channel_id, dpp::embed &embed) {
    return dpp::message(channel_id, embed)
            .add_component(dpp::component().add_component(dpp::component()
                                                                  .set_label("Create!")
                                                                  .set_type(dpp::cot_button)
                                                                  .set_emoji("📝")
                                                                  .set_id("ticket_create")));
}

void ticket::init_ticket_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config) {
    bot.log(dpp::ll_debug, "Initializing 'ticket_events'");

    bot.on_button_click([&bot, &c, &config](const dpp::button_click_t &event) {
        dpp::interaction event_cmd = event.command;

        if (event.custom_id == "ticket_create") {
            // start thinking and then decide what happens later, don't let the user wait
            // it's like a traffic light, you wait 2 seconds and everyone is mad at you.
            event.thinking(true);

            size_t category_id, notify_id;
            int ticket_count;
            std::vector<size_t> mod_roles;

            mysqlpp::Query query = c.query();
            query << fmt::format("SELECT ticket.category_id, ticket.count, ticket.notify_channel, ticket.enabled FROM ticket WHERE ticket.server_id = '{0}'; "
              "UPDATE salty_cpp_bot.ticket SET count=count + 1 WHERE ticket.server_id = '{0}';"
              "select ticket_access_roles.role_id from ticket_access_roles where ticket_access_roles.server_id = {0};",
                    event_cmd.guild_id);

            mysqlpp::StoreQueryResult res = query.store();

            category_id = res[0]["category_id"];
            ticket_count = res[0]["count"];
            notify_id = res[0]["notify_channel"];

            short enabled = res[0]["enabled"];

            query.store_next();
            mysqlpp::StoreQueryResult res2 = query.store_next();

            for (size_t i = 0; i < res2.num_rows(); ++i) {
                mod_roles.push_back(res2[i]["role_id"]);
            }

            u::kill_query(query);

            if (!enabled) {
                event.edit_original_response(dpp::message("Ticket's are not enabled, contact an Moderator when you want to create a Ticket."));
                return;
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

                mysqlpp::Query query = c.query();

                query << fmt::format(
                        "insert into salty_cpp_bot.cur_tickets (server_id, user_id, ticket_id) values ({0}, {1}, {2})",
                        guild.id, user.id, new_ticket.id);

                query.execute();

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
                    } catch (std::exception e) {} // in case the bot cannot find the channel id, I will put something in there later.

                }
            });
            return;
        }

        if (event.custom_id == "ticket_delete") {
            bot.message_create(dpp::message(event_cmd.channel_id, "Ticket gets deleted in 1 sec."));
            try {
                bot.channel_delete_sync(event_cmd.channel_id);
            } catch (std::exception e) {}
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

            bot.channel_edit(channel.set_name(fmt::format("closed-{0}", channel.name)));

            mysqlpp::Query query = c.query();
            query << fmt::format("select user_id from cur_tickets where server_id = {0} and ticket_id = {1};",
                                 event_cmd.guild_id, event_cmd.channel_id);

            mysqlpp::StoreQueryResult res = query.store();

            long user_id = res[0]["user_id"];
            u::kill_query(query);

            bot.channel_delete_permission(channel, user_id, [&](const dpp::confirmation_callback_t &confm) {
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
                                            .set_style(dpp::cos_danger)
                                            .set_label("Delete ticket"))
                                        .add_component(
                                        dpp::component()
                                            .set_label("Reopen ticket")
                                            .set_id("ticket_reopen")
                                            .set_type(dpp::cot_button)
                                            .set_style(dpp::cos_secondary))));

            });
            return;
        }

        if (event.custom_id == "ticket_reopen") {
            event.thinking();
            dpp::channel channel = event_cmd.get_channel();
            mysqlpp::Query query = c.query();

            query << fmt::format(
                    "select user_id from salty_cpp_bot.cur_tickets where server_id = {0} and ticket_id = {1};",
                        event_cmd.guild_id, event_cmd.channel_id);

            mysqlpp::StoreQueryResult res = query.store();
            size_t user_id = res[0]["user_id"];

            u::kill_query(query);

            bot.message_delete(event_cmd.message_id, channel.id);
            bot.channel_edit_permissions(channel.id, user_id, dpp::permissions::p_view_channel, 0, true);

            // prevent rate limitation. works sometimes.
            ::sleep(5);
            try {
                bot.channel_edit(channel.set_name(std::regex_replace(channel.name, std::regex("closed-"), "")));
                event.edit_original_response(fmt::format("<@{0}> your ticket got Re-Opened.", user_id));
            } catch (std::exception e) {
                try {
                    event.edit_original_response(dpp::message("Can't reopen ticket, try again in a few minutes"));

                // prevent edge case where the mod removes the channel before it can be re-opened
                } catch (std::exception f) { return; }
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

    bot.on_channel_delete([&c](const dpp::channel_delete_t &event) {
        // db cleaner, not sure yet if I want to let this in, but it's there.
        mysqlpp::Query query = c.query();

        query << fmt::format("if exists(select * from salty_cpp_bot.cur_tickets where ticket_id = {0} and server_id = {1}) then "
                             "delete from salty_cpp_bot.cur_tickets where ticket_id = {0} and server_id = {1}; "
                             "end if;",
                             event.deleted->id, event.deleted->guild_id);

        query.execute();
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
                             cfg::Config &conf) {

    auto sc = cmd_data.options[0];

    dpp::embed em;
    dpp::guild guild = event.command.get_guild();

    // ik you could use a switch here, but I'm not a fan of indentation so no switch here.
    if (sc.name == "create") {
        mysqlpp::Query query = c.query();
        std::string title;

        {
            query << fmt::format("if not exists(select * from ticket where server_id='{0}') then"
                                    " insert into ticket (server_id) values ('{0}');"
                                 "end if; ", event.command.guild_id);

            query.execute();
        }

        query << fmt::format("select ticket.ticket_title from ticket where ticket.server_id = '{0}';",
                             event.command.guild_id);

        mysqlpp::StoreQueryResult res = query.store();

        title = (std::string) res[0]["ticket_title"];

        u::kill_query(query);

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

        } catch (std::exception e) {
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

            bot.channels_get(event.command.guild_id, [&bot, &c, event, category_id](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }

                auto t_channel = confm.get<dpp::channel_map>()[category_id];
                if (!t_channel.is_category()) {
                    event.edit_original_response(fmt::format("<#{0}> is not a Category.", category_id));
                    return;
                }

                mysqlpp::Query query = c.query();

                query << fmt::format("if exists(select * from salty_cpp_bot.ticket where server_id = {0}) then"
                                     " update salty_cpp_bot.ticket set category_id = {1} where server_id = {0}; "
                                     "else"
                                     " insert into salty_cpp_bot.ticket (server_id, category_id) values ({0}, {1}); "
                                     "end if;",
                                     event.command.guild_id, category_id);

                query.execute();

                event.edit_original_response(fmt::format("<#{0}> was set as new Category.", category_id));
            });
        }

        if (sub.name.find("roles") != std::string::npos) {
            dpp::channel _channel = event.command.get_channel();
            short count = 0;
            std::string output = "Roles: ";

            if (!sub.options.empty()) {
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
            }

            output += "added for Moderation/Support.";
            event.edit_original_response(output);
        }

        if (sub.name.find("notify") != std::string::npos) {
            size_t channel_id = event.command.channel_id;

            if (!sub.options.empty())
                channel_id = sub.get_value<dpp::snowflake>(0);

            bot.channels_get(event.command.guild_id, [&bot, &c, event, channel_id](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }

                auto t_channel = confm.get<dpp::channel_map>()[channel_id];
                if (!t_channel.is_text_channel()) {
                    event.edit_original_response(fmt::format("<#{0}> is not a Text channel.", channel_id));
                    return;
                }

                mysqlpp::Query query = c.query();

                query << fmt::format("if exists(select * from salty_cpp_bot.ticket where server_id = {0}) then"
                                     " update salty_cpp_bot.ticket set notify_channel = {1} where server_id = {0}; "
                                     "else"
                                     " insert into salty_cpp_bot.ticket (server_id, notify_channel) values ({0}, {1}); "
                                     "end if;",
                                     event.command.guild_id, channel_id);

                query.execute();
                event.edit_original_response(fmt::format("<#{0}> was set as new Notify channel.", channel_id));
            });
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

            mysqlpp::Query query = c.query();
            query << fmt::format("update salty_cpp_bot.ticket set {0} = 0 where server_id = {1}", table_name, event.command.guild_id);

            query.execute();

            event.edit_original_response(fmt::format("{0} id removed from database.", sub.name));
        }

        if (sub.name.find("roles") != std::string::npos) {
            short count = 0;

            output += "Removed: ";

            if (!sub.options.empty()){
                mysqlpp::Query query = c.query();

                for (auto& i : sub.options) {
                    size_t value = sub.get_value<dpp::snowflake>(count);

                    query << fmt::format(
                        "delete from salty_cpp_bot.ticket_access_roles where server_id = {0} and role_id = {1}; ",
                            event.command.guild_id, value);

                    output += fmt::format("<@&{0}> ", value);
                    ++count;
                }
                query.execute();

                output += "from the database.";
                event.edit_original_response(output);
            }
        }
    }

    if (sc.name == "config") {
        em.set_title("Ticket System configuration")
          .set_description(fmt::format("Config for {0}", event.command.get_guild().name))
          .set_color(conf.b_color)
          .set_footer("Kenexar.eu", bot.me.get_avatar_url())
          .set_timestamp(time(nullptr));

        mysqlpp::Query query = c.query();
        query << fmt::format("select ticket.enabled, ticket.category_id, ticket.ticket_title, "
                 "ticket.notify_channel, ticket.count from salty_cpp_bot.ticket where server_id = {0};"
                 "select ticket_access_roles.role_id from ticket_access_roles where server_id = {0};", event.command.guild_id);

        mysqlpp::StoreQueryResult res = query.store();

        bool enabled = res[0]["enabled"];
        int ticket_count = res[0]["count"];
        auto ticket_title = (std::string) res[0]["ticket_title"];
        size_t category_id = res[0]["category_id"];
        size_t notify_channel = res[0]["notify_channel"];
        std::stringstream roles;


        auto res2 = query.store_next();
        for (size_t i = 0; i < res2.num_rows(); ++i) {
            size_t role = res2[i]["role_id"];
            roles << fmt::format("<@&{0}>", role) << '\n';
        }

        em.add_field("Enabled", enabled ? "True" : "False", true);
        em.add_field("Category for new ticket.", fmt::format("<#{0}>", category_id), true);
        em.add_field("Notify channel for new tickets.", fmt::format("<#{0}>", notify_channel), true);
        em.add_field("Created tickets on this server",  fmt::format("{0}", ++ticket_count), true);

        em.add_field("Support Roles:", roles.str(), false);
        event.reply(dpp::message(event.command.channel_id, em));
    }

    if (sc.name == "enable") {
        change_state(event, c, "enabled", 1);
    }

    if (sc.name == "disable") {
        change_state(event, c, "disabled", 0);
    }

    if (sc.name == "init") {

    }
}

void ticket::change_state(const dpp::slashcommand_t &event,
                          mysqlpp::Connection &c,
                          const std::string &state,
                          const int i_state) {
    mysqlpp::Query query = c.query();

    query << fmt::format("update salty_cpp_bot.ticket set enabled={1} where server_id = {0};",
                         event.command.guild_id, i_state);

    query.execute();

    event.reply(dpp::message(fmt::format("Ticket system got {0}.", state)).set_flags(dpp::m_ephemeral));
}
