//
// Created by julian on 11/4/22.
//

#ifndef DCBOT_TICKETCOGS_HPP
#define DCBOT_TICKETCOGS_HPP

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
 *      [] -> mandatory arguments
 *      | -> to do combiner
 *  Global command: ticket
 *  Sub-commands:
 *      create -> create init ticket message, enable ticket system
 *      set -> sets channel group for tickets
 *      ? change -> change settings
 *      disable -> disable ticket system for server, gets re-enabled with create
 *      |
 *  Command Usage:
 *   /ticket create <CHANNEL_ID/<current_channel>>
 *   /ticket set [CATEGORY_ID]
 *   /ticket disable/enable
 *
 * */

namespace ticket {
    class Ticket {

    };
} // ticket


#endif //DCBOT_TICKETCOGS_HPP
