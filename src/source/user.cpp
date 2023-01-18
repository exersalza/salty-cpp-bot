// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/24/22.
//

#include "../include/user.hpp"

// todo: welcome/bye thingi

void user::init_user_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config) {

}

void
user::user_commands(dpp::cluster &bot, const dpp::slashcommand_t &event, const dpp::command_interaction &cmd_data) {
    auto sc = cmd_data.options[0];

    if (sc.name == "avatar" && sc.type == dpp::co_sub_command) {
        size_t uid;

        if (!sc.options.empty()) {
            uid = sc.get_value<dpp::snowflake>(0);
        } else {
            uid = event.command.usr.id;
        }

        try { // sometimes it throws errors, but idk why.
            bot.user_get(uid, [event, &bot](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }
                std::string username;
                std::stringstream avatar;
                dpp::embed em;

                auto usr = confm.get<dpp::user_identified>();

                avatar << usr.get_avatar_url() << "?size=480";
                username = usr.username;

                em.set_title(fmt::format("Avatar from {0}", username))
                        .set_description(fmt::format("[Avatar]({0})", avatar.str()))
                        .set_color(usr.accent_color)
                        .set_image(avatar.str());

                event.reply(dpp::message(event.command.channel_id, em));
            });
        } catch (std::exception &e) {
            bot.log(dpp::ll_error, e.what());
            event.reply(dpp::message(event.command.channel_id,
                                     "Something went wrong on your request, please try again later.")
                                .set_flags(dpp::m_ephemeral));
        }
    }

    if (sc.name == "banner" && sc.type == dpp::co_sub_command) {
        size_t uid;

        if (!sc.options.empty()) {
            uid = sc.get_value<dpp::snowflake>(0);
        } else {
            uid = event.command.usr.id;
        }

        try {
            bot.user_get(uid, [event, &bot](const dpp::confirmation_callback_t &confm) {
                if (confm.is_error()) {
                    ticket::confm_error(bot, event, confm);
                    return;
                }
                std::string username;
                std::stringstream banner;
                dpp::embed em;

                auto usr = confm.get<dpp::user_identified>();

                banner << usr.get_banner_url() << ((!usr.get_banner_url().empty()) ? "?size=480" : "");
                username = usr.username;


                em.set_title(fmt::format("Banner from {0}", username))
                        .set_description(fmt::format("[{1}]({0})", banner.str(),
                                                     ((!banner.str().empty()) ? "Banner" : "User has no banner.")))
                        .set_color(usr.accent_color)
                        .set_image((!banner.str().empty()) ? banner.str() : "");

                event.reply(dpp::message(event.command.channel_id, em));
            });
        } catch (std::exception &e) {
            bot.log(dpp::ll_error, e.what());
            event.reply(dpp::message(event.command.channel_id,
                                     "Something went wrong on your request, please try again later.")
                                .set_flags(dpp::m_ephemeral));
        }
    }

    if (sc.name == "info" && sc.type == dpp::co_sub_command) {
        size_t uid;

        if (!sc.options.empty()) {
            uid = sc.get_value<dpp::snowflake>(0);
        } else {
            uid = event.command.usr.id;
        }

        bot.user_get(uid, [&bot, &event](const dpp::confirmation_callback_t &confm) {
            if (confm.is_error()) {
                ticket::confm_error(bot, event, confm);
                return;
            }

            auto usr = confm.get<dpp::user_identified>();

        });
    }
}
