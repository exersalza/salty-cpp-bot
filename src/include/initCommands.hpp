// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/24/22.
//

#ifndef DCBOT_INITCOMMANDS_HPP
#define DCBOT_INITCOMMANDS_HPP

#include <mysql++/query.h>
#include <curl/curl.h>
#include <dpp/dpp.h>
#include <string>
#include <vector>

struct cmds;
struct cmds {
    std::string desc;
    uint64_t perm;
	std::vector<dpp::command_option> p = {};
};

void createcmds(dpp::cluster& client);


inline std::map<std::string, cmds> commands {
	{
		"ticket",
			{
				"Manage your Ticket system.",                 dpp::p_administrator,
				{
					dpp::command_option(dpp::co_sub_command,
                                        "create",
                                        "Create Ticket message at current/given channel.")
                                .add_option(dpp::command_option(dpp::co_channel, "channel", "Define channel", false)),

                    dpp::command_option(dpp::co_sub_command_group, "set", "Set Category, Notify, Max Ticket Count or Roles for new tickets.")
                        .add_option(dpp::command_option(dpp::co_sub_command, "category", "set category for new tickets")
                            .add_option(dpp::command_option(dpp::co_channel, "id", "Set the category id.", false)))
                        .add_option(dpp::command_option(dpp::co_sub_command, "maxticketcount", "Set the max tickets that a user can open. You can enter 0 to deactivate the Limit.")
                            .add_option(dpp::command_option(dpp::co_integer, "id", "Set the category id.", true)))
                        .add_option(dpp::command_option(dpp::co_sub_command, "notify", "Set notify channel for new tickets") // set and delete
                            .add_option(dpp::command_option(dpp::co_channel, "id", "Set the notify channel id.", false)))
                        .add_option(dpp::command_option(dpp::co_sub_command, "roles", "Set Moderation roles for new tickets")
                            .add_option(dpp::command_option(dpp::co_role, "id", "Set role id", true)) // set and delete
                            .add_option(dpp::command_option(dpp::co_role, "id2", "Set role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id3", "Set role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id4", "Set role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id5", "Set role id", false))),

                    dpp::command_option(dpp::co_sub_command_group, "remove", "Remove Category, Notify, or Roles for new tickets.")
                        .add_option(dpp::command_option(dpp::co_sub_command, "category", "Remove category for new tickets"))
                        .add_option(dpp::command_option(dpp::co_sub_command, "notify", "Remove notify channel for new tickets"))
                        .add_option(dpp::command_option(dpp::co_sub_command, "roles", "Remove Moderation roles for new tickets")
                            .add_option(dpp::command_option(dpp::co_role, "id", "Remove role id", true)) // set and delete
                            .add_option(dpp::command_option(dpp::co_role, "id2", "Remove role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id3", "Remove role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id4", "Remove role id", false))
                            .add_option(dpp::command_option(dpp::co_role, "id5", "Remove role id", false))),

                    dpp::command_option(dpp::co_sub_command, "init", "Start configuration wizard."),
                    dpp::command_option(dpp::co_sub_command, "config", "Open config embed"),
                    dpp::command_option(dpp::co_sub_command, "enable", "Enable the ticket system"),
                    dpp::command_option(dpp::co_sub_command, "disable", "Disable the ticket system"),

				}
			}
	},
    {
        "admin", {
            "Admin base command",                             dpp::p_administrator,
            {
                dpp::command_option(dpp::co_sub_command, "send",
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
                                                    "Will set the server image as thumbnail image."))
            }
        }
    },
    {
        "ping", {"Shows bot latency, but only when you want", dpp::p_send_messages}
    },
    {
        "help", {"Shows the help page",                       dpp::p_send_messages}
    }
};

inline void createcmds(dpp::cluster& client) {
	if (dpp::run_once<struct register_commands>()) {
		std::vector<dpp::slashcommand> cmds;

		for (auto& i : commands) {
			dpp::slashcommand c;

			// Create slash command template
			c.set_name(i.first)
                .set_description(i.second.desc)
                .set_application_id(client.me.id);

            // error but don't know why, still works
            c.options = i.second.p;
            c.set_default_permissions(i.second.perm);

			// Pushing all commands
			cmds.push_back(c);
		}

		// Create a global slash commands
		client.global_bulk_command_create(cmds);
	}
}

#endif //DCBOT_INITCOMMANDS_HPP
