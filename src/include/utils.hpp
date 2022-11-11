//
// Created by julian on 1.11.22.
//
#ifndef DCBOT_UTILS_HPP
#define DCBOT_UTILS_HPP

#include <vector>
#include <string>
#include <mysql++/query.h>
#include <dpp/dpp.h>

namespace u {
    [[maybe_unused]] std::vector<std::string> split(std::string &s, const char &del);

    [[maybe_unused]] std::vector<std::string> split(std::string &s, const std::string &del);

    [[maybe_unused]] bool fileExists(const std::string &name);

    int kill_query(mysqlpp::Query &query);


    void presence_update(dpp::cluster &bot);
} // u

#endif // DCBOT_UTILS_HPP

