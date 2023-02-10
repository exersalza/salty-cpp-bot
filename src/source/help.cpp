// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/15/22.
//

#include <fmt/format.h>

#include "../include/help.hpp"
#include "../include/utils.hpp"


void cog::help_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                        const dpp::command_interaction &cmd_data, const cfg::Config &conf) {

    dpp::embed em;
    em.set_title("Help Site")
            .set_color(conf.b_color)
            .set_timestamp(time(nullptr))
            .set_footer("Kenexar.eu", bot.me.get_avatar_url());

    if (!cmd_data.options.empty()) {
        auto site = std::get<std::string>(event.get_parameter("site"));

        if (u::stol(site) == "admin") {
            em.set_title(fmt::format("{} - Admin", em.title));
            em.set_description("Syntax: `/admin send <args>`\n"
                                  "Args: \n"
                                  "`file` -> Attach file for longer texts.\n"
                                  "`title` -> Set a Title, when none is given, it will take the first word of the body text.\n"
                                  "`text` -> set a small text. For longer and formatted texts, please consider using the `file` option.\n"
                                  "`server_image` -> Will set the server image as thumbnail (the little image top right)\n"
                                  "`reaction` -> Add the arrows from reddit to the message.\n"
                                  "`verify` -> Will create the verify message with a custom text in the current channel.");
        }

        event.reply(dpp::message(event.command.channel_id, em));
        return;
    }

    em.set_description("For more information type `/help <command name>`");

    em.add_field("Ticket", "`/ticket create` to enable your ticket system.\n"
                           "`/ticket config` to lookup your ticket config.\n"
                           "`/ticket set [roles|category|...]` set your config values.\n"
                           "`/ticket enable/disable` enable or disable your ticket system.\n", false);

    em.add_field("Information", "`/help` shows this page.\n"
                                "`/uptime` tells you the uptime from the Bot.\n"
                                "`/ping` just a ping command.\n"
                                "`/user [avatar|banner]` get the avatar/banner from a user.\n"
                                "`/credits` everyone should know what that command does.\n", false);

    em.add_field("Admin commands", "`/admin send` let the bot send a text for you.\n"
                                   "`/verify` setup your verify system", false);

    event.reply(dpp::message(event.command.channel_id, em));
}