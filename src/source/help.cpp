//
// Created by julian on 11/15/22.
//

#include "../include/help.hpp"

void cog::init_help_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config) {
    bot.log(dpp::ll_debug, "Initializing 'help_commands'");

    dpp::slashcommand help("help", "help base command", bot.me.id);

    bot.global_command_create(help);
}

void cog::init_help_commands(dpp::cluster &bot) {

}

void cog::help_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                           const dpp::command_interaction &cmd_data, const cfg::Config &conf) {
    std::vector<std::string> user_help;
    std::vector<std::string> admin_help;
}