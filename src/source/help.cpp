// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/15/22.
//

#include <fmt/format.h>

#include "../include/help.hpp"


void cog::help_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                           const dpp::command_interaction &cmd_data, const cfg::Config &conf) {

    dpp::embed em;

    em.set_title("Help Site")
      .set_description("For more information press the corresponding button")
      .set_color(conf.b_color)
      .set_timestamp(time(nullptr))
      .set_footer("Kenexar.eu", bot.me.get_avatar_url());

    em.add_field("Ticket", fmt::format("`/ticket create` to enable your ticket system.\n"
                                       "`/ticket config` to lookup your ticket config.\n"
                                       "`/ticket set [roles|category|...]` set your config values.\n"
                                       "`/ticket enable/disable` enable or disable your ticket system.\n"), false);

    event.reply(dpp::message(event.command.channel_id, em));
}