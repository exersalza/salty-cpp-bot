//
// Created by julian on 11/17/22.
//


#include "../include/admin.hpp"

void admin::init_admin_commands(dpp::cluster &bot) {
    bot.log(dpp::ll_debug, "Initializing 'admin_commands'");

    dpp::slashcommand admin("admin", "admin base command", bot.me.id);
    admin.set_default_permissions(dpp::permissions(dpp::p_administrator));

    admin.add_option(dpp::command_option(dpp::co_sub_command, "send",
                                         "Let the bot send a message for you, useful for Rules and Announcements.")
                                            .add_option(dpp::command_option(
                                                    dpp::co_attachment, "msg",
                                                    "Please use text files that include the Message to send."))
                                            .add_option(dpp::command_option(
                                                    dpp::co_string, "title",
                                                    "Set a title, will only work when embed is on true."))
                                            .add_option(dpp::command_option(
                                                    dpp::co_boolean, "embed",
                                                    "Is the message worth to be send as an Embed?")));

    bot.global_command_create(admin);
}

// later usage
void admin::init_admin_events(dpp::cluster &bot) {

}

void admin::admin_commands(dpp::cluster &bot, const dpp::slashcommand_t &event, const dpp::command_interaction &cmd_data) {
    auto sc = cmd_data.options[0];

    dpp::guild guild = event.command.get_guild();
    short c = 0;


    if (sc.name == "send") {
        bool is_embed = false;
        std::string msg;
        std::string title;

        for (auto &i : sc.options) {
            if (i.name == "embed") {
                is_embed = sc.get_value<bool>(c);
            }

            if (i.name == "msg") {
                auto at_id = sc.get_value<dpp::snowflake>(c);
                auto at = event.command.get_resolved_attachment(at_id);

                Poco::URI uri(at.url);
                u::requests(uri);
            }

            if (i.name == "title") {
                title = sc.get_value<std::string>(c);
            }

            ++c;
        }

        if (!title.length()) title = msg.substr(0, msg.find(' '));

        if (is_embed) {
            dpp::embed em;

            em.set_title(title)
              .set_description("kommt");
        }

    }
}
