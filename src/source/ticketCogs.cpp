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

    ticket.add_option(
            dpp::command_option(dpp::co_sub_command, "set", "Set category for new tickets.")
                    .add_option(dpp::command_option(dpp::co_channel, "category", "Define category", false))
                    .add_option(dpp::command_option(dpp::co_channel, "notify", "Define notify channel", false))
                    .add_option(dpp::command_option(dpp::co_role, "role", "Define Supporter/Moderator role", false))
                    .add_option(dpp::command_option(dpp::co_role, "role2", "Define Supporter/Moderator role2", false))
                    .add_option(dpp::command_option(dpp::co_role, "role3", "Define Supporter/Moderator role3", false))
                    .add_option(dpp::command_option(dpp::co_role, "role4", "Define Supporter/Moderator role4", false))
                    .add_option(dpp::command_option(dpp::co_role, "role5", "Define Supporter/Moderator role5", false))
    );

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

                // prevent edge case where the mod removes the channel before it can be reopend
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

    if (sc.name == "set" and !sc.options.empty()) {
        event.thinking(true);
        short count = 0;
        std::string output_string;

        for (auto& i : sc.options) {
            if (i.name.find("role") != std::string::npos) {
                output_string += "Roles ";

                std::size_t role_id = sc.get_value<dpp::snowflake>(count);
                mysqlpp::Query query = c.query();

                output_string += fmt::format("<@&{0}> ", role_id);

                query << fmt::format(
                    "if not exists(select * from ticket_access_roles where role_id='{1}') then"
                        " insert into ticket_access_roles (server_id, role_id) values ('{0}', '{1}');"
                    "end if; ", event.command.guild_id, role_id);

                query.execute();

                output_string += "for support added. ";
            }

            if (i.name == "category") {
                size_t category_id = sc.get_value<dpp::snowflake>(count);

                bot.channels_get(event.command.guild_id, [&, event](const dpp::confirmation_callback_t &confm) {
                    if (confm.is_error()) {
                        confm_error<dpp::slashcommand_t>(bot, event, confm);
                        return;
                    }

                    mysqlpp::Query query = c.query();

                    auto channel = confm.get<dpp::channel_map>()[category_id];
                    if (!channel.is_category()) {
                        auto f = fmt::format("<#{0}> is not a valid category. ", category_id);
                        output_string += f;
                    } else {
                        query << fmt::format("if exists(select * from salty_cpp_bot.ticket where server_id = {0}) then"
                                             " update salty_cpp_bot.ticket set category_id = {1} where server_id = {0}; "
                                             "else"
                                             " insert into salty_cpp_bot.ticket (server_id, category_id) values ({0}, {1}); "
                                             "end if;",
                                             event.command.guild_id, category_id);

                        query.execute();
                        output_string += fmt::format("Ticket category changed to: '<#{0}>'. ", category_id);
                    }

                    event.edit_original_response(dpp::message(output_string).set_flags(dpp::m_ephemeral));
                });
                continue;
            }

            if (i.name == "notify") {
                size_t channel_id = sc.get_value<dpp::snowflake>(count);

                bot.channels_get(event.command.guild_id, [&, event](const dpp::confirmation_callback_t &confm) {
                    if (confm.is_error()) {
                        confm_error<dpp::slashcommand_t>(bot, event, confm);
                        return;
                    }

                    mysqlpp::Query query = c.query();
                    auto channel = confm.get<dpp::channel_map>()[channel_id];

                    if (!channel.is_text_channel()) {
                        output_string += fmt::format("<#{0}> is not a valid text channel. ", channel_id);

                    } else {
                        query << fmt::format("if exists(select * from salty_cpp_bot.ticket where server_id = {0}) then"
                                             " update salty_cpp_bot.ticket set notify_channel = {1} where server_id = {0}; "
                                             "else"
                                             " insert into salty_cpp_bot.ticket (server_id, notify_channel) values ({0}, {1}); "
                                             "end if;",
                                             event.command.guild_id, channel_id);

                        query.execute();
                        output_string += fmt::format("Ticket mod notify channel changed to: '<#{0}>'. ", channel_id);
                    }

                    event.edit_original_response(dpp::message(output_string).set_flags(dpp::m_ephemeral));
                });
                continue;
            }
            count++;
        }

        // event.edit_original_response(dpp::message(output_string).set_flags(dpp::m_ephemeral));
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
        size_t category_id = res[0]["category_id"];
        auto ticket_title = (std::string) res[0]["ticket_title"];
        size_t notify_channel = res[0]["notify_channel"];
        int ticket_count = res[0]["count"];
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

    if (sc.name == "change") {

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
