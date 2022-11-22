//
// Created by julian on 11/17/22.
//


#include "../include/admin.hpp"

void admin::init_admin_commands(dpp::cluster &bot) {
    bot.log(dpp::ll_debug, "Initializing 'admin_commands'");

    dpp::slashcommand admin("admin", "admin base command", bot.me.id);
    admin.set_default_permissions(dpp::permissions(dpp::p_administrator));

    admin.add_option(dpp::command_option(dpp::co_sub_command, "send",
                                         "Let the bot send a message for you, useful for Rules and other kinds of stuff.")
                                            .add_option(dpp::command_option(
                                                    dpp::co_attachment, "file",
                                                    "Please use text files that include the Message to send. (*.txt)",
                                                    true)) // force the user to supply a file.
                                            .add_option(dpp::command_option(
                                                    dpp::co_string, "title",
                                                    "Set a title."))
                                            .add_option(dpp::command_option(
                                                    dpp::co_boolean, "embed",
                                                    "Is the message worth to be send as an Embed? Standard is false."))
                                            .add_option(dpp::command_option(
                                                    dpp::co_boolean, "server_image",
                                                    "Will set the server image as thumbnail image.")));

    bot.global_command_create(admin);
}

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
