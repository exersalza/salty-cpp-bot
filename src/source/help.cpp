//
// Created by julian on 11/15/22.
//

#include "../include/help.hpp"


void cog::help_commands(dpp::cluster &bot, const dpp::slashcommand_t &event,
                           const dpp::command_interaction &cmd_data, const cfg::Config &conf) {

    dpp::embed em;

    em.set_title("Help Site")
      .set_description("For more information press the corresponding button")
      .set_color(conf.b_color)
      .set_timestamp(time(nullptr))
      .set_footer("Kenexar.eu", bot.me.get_avatar_url());

    em.add_field("Ticket", "`/ticket create` to enable your ticket system", true);

    event.reply(dpp::message(event.command.channel_id, em));
}