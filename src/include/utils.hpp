// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 1.11.22.
//

#ifndef DCBOT_UTILS_HPP
#define DCBOT_UTILS_HPP


#include <vector>
#include <iostream>
#include <string>
#include <dpp/dpp.h>
#include <curl/curl.h>
#include <mysql++/query.h>


namespace u {
    [[maybe_unused]] std::vector<std::string> split(std::string &s);

    [[maybe_unused]] std::vector<std::string> split(std::string &s, const char &del);

    [[maybe_unused]] std::vector<std::string> split(std::string &s, const std::string &del);

    [[maybe_unused]] bool file_exists(const std::string &name);

    [[maybe_unused]] std::string stol(const std::string &name);

    [[maybe_unused]] std::string stou(const std::string &name);

    [[maybe_unused]] int stoc(const std::string &src, char *dest);

    int kill_query(mysqlpp::Query &query);

    void presence_update(dpp::cluster &bot);

    std::string requests(const char *url);

    std::string requests(const char *url, std::vector<const char *> &header);

    std::string post(const char *url, const char *post_opt);

    std::string random_string(size_t length);

} // u



#endif // DCBOT_UTILS_HPP

