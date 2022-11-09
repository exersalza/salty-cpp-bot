//
// Created by julian on 1.11.22.
//
#ifndef DCBOT_UTILS_HPP
#define DCBOT_UTILS_HPP

#include <vector>
#include <string>
#include <mysql++/query.h>

namespace u {
    std::vector<std::string> split(std::string &s, const char &del);

    std::vector<std::string> split(std::string &s, const std::string &del);

    bool fileExists(const std::string &name);

    int kill_query(mysqlpp::Query &query);
} // u

#endif // DCBOT_UTILS_HPP

