//
// Created by julian on 11/4/22.
//

#ifndef DCBOT_TICKETCOGS_HPP
#define DCBOT_TICKETCOGS_HPP

#include <string>
#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <mysql++/mysql++.h>

#include "config.hpp"

/* Todo:
 *  legend:
 *      ? -> don't know if this gonna happen
 *      ! -> necessary for something
 *      -> -> definition/description
 *      <> -> optional arguments
 *      <<>> -> default argument
 *      >> -> subcommands
 *      >>| -> options
 *      - -> done
 *      [] -> mandatory arguments
 *      | -> to do combiner (for those that don't use intellij based Ide's, they dont recognize to do's as one, when there not merged)
 *  Global command: ticket
 *  |
 *  Ideas:
 *      webhook implementation.
 *  |
 *  Sub-commands:
 *      - create -> create init ticket message, enable ticket system
 *      - set -> set channel for actions
 *          >> category -> set category for new tickets
 *          >> notify -> set mod notify channel
 *      ? config -> change settings
 *          >>| change category
 *          >>| change ticket notify for mods (channel acquired)
 *      disable -> disable ticket system for server, gets re-enabled with create
 *      rename -> rename ticket only available when tickit is not closed.
 *      |
 *  Command Usage:
 *   /ticket create <CHANNEL_ID/<<current channel>>>
 *   /ticket set [CATEGORY_ID||ROLE1-6]
 *   /ticket remove [CATEGORY_ID||ROLE1-6]
 *   /ticket init -> Start ticket wizard, maybe later on a website.
 *   /ticket config
 *   /ticket disable/enable
 *   /ticket rename -> permission mod
 *
 * */

namespace ticket {
    void init_ticket_events(dpp::cluster &bot, mysqlpp::Connection &c, cfg::Config &config);

    void init_ticket_commands(dpp::cluster &bot);

    void ticket_commands(dpp::cluster &bot,
                         const dpp::slashcommand_t &event,
                         const dpp::command_interaction &cmd_data,
                         mysqlpp::Connection &c,
                         cfg::Config &conf);

    dpp::message create_ticket_message(size_t &channel_id, dpp::embed &embed);

    template<typename T>
    void confm_error(const dpp::cluster &bot, const T &event, const dpp::confirmation_callback_t &confm);

    void change_state(const dpp::slashcommand_t &event, mysqlpp::Connection &c, const std::string& state, const int i_state);
} // ticket


#endif //DCBOT_TICKETCOGS_HPP
