//
// Created by julian on 11/4/22.
//

#ifndef DCBOT_TICKETCOGS_HPP
#define DCBOT_TICKETCOGS_HPP

#include <mysql++/mysql++.h>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <dpp/dpp.h>

/* Todo:
 *  legend:
 *      ? -> don't know if this gonna happen
 *      ! -> necessary for something
 *      -> -> definition/description
 *      <> -> optional arguments
 *      <<>> -> default argument
 *      - -> done
 *      [] -> mandatory arguments
 *      | -> to do combiner (for those that don't use intellij based ide's, they dont recognize to do's as one, when there not merged)
 *  Global command: ticket
 *  Sub-commands:
 *      - create -> create init ticket message, enable ticket system
 *      set -> sets category for tickets
 *      ? config -> change settings
 *      disable -> disable ticket system for server, gets re-enabled with create
 *      |
 *  Command Usage:
 *   /ticket create <CHANNEL_ID/<current_channel>>
 *   /ticket set [CATEGORY_ID]
 *   /ticket config
 *   /ticket disable/enable
 *
 * */

namespace ticket {
    void init_ticket_events(dpp::cluster &bot, mysqlpp::Connection &c);

    void init_ticket_commands(dpp::cluster &bot);

    void ticket_commands(dpp::cluster &bot,
                         const dpp::slashcommand_t &event,
                         const dpp::command_interaction &cmd_data);

    dpp::message create_ticket_message(size_t &channel_id, dpp::embed &embed);

} // ticket


#endif //DCBOT_TICKETCOGS_HPP
