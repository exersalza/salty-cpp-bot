//
// Created by julian on 11/2/22.
//

#ifndef DCBOT_COGS_HPP
#define DCBOT_COGS_HPP

#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using namespace std;

namespace cog {
    void regis_commands(dpp::cluster &bot);
} // cog


#endif //DCBOT_COGS_HPP
