//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <sstream>
#include <fmt/format.h>

#include "../include/utils.hpp"

using namespace std;

[[maybe_unused]] vector<string> u::split(std::string &s, const char &del) {
    /** String split into vector with single char delimiter
     *
     * @param s : Give the String to split.
     * @param del : Set the delimiter where to split.
     * @retuns vector : vector of strings
     *
     * **/
    vector<string> ret;
    stringstream ss(s);
    string item;

    while (getline(ss, item, del))
        ret.push_back(item);

    return ret;
}

[[maybe_unused]] vector<string> u::split(std::string &s, const std::string &del) {
    /** String split into vector with multi char delimiter
     *
     * @param s : Give the String to split.
     * @param del : Set the delimiter where to split.
     * @retuns vector : vector of strings
     *
     * **/
    size_t pos_start = 0, pos_end, delim_len = del.length();
    string token;
    vector<string> ret;

    while ((pos_end = s.find(del, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end + pos_start);
        pos_start = pos_end + delim_len;
        ret.push_back(token);
    }

    ret.push_back(s.substr(pos_start));
    return ret;
}

[[maybe_unused]] bool u::fileExists(const string &name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

int u::kill_query(mysqlpp::Query &query) {
    /** Kills query to prevent 'command out of sync' error
     *
     * @param query : the query to kill.
     *
     * @returns int
     * **/

    try {
        for (int i = 0; query.more_results(); ++i) { query.store_next(); }
    } catch (std::exception e) {
        return 1;
    }
    return 0;
}


void u::presence_update(dpp::cluster &bot) {
    bot.log(dpp::ll_info, "Presence warmup");
    ::sleep(90);

    while (true) {
        try {
            bot.set_presence(dpp::presence(dpp::ps_online, dpp::activity_type::at_watching, "the development of me."));
            ::sleep(120);
        } catch (exception &e) {
            bot.log(dpp::ll_error, fmt::format("FUCK, somethin went wron {0}", e.what()));
        }
    }
}