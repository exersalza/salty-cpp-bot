//
// Created by julian on 29.10.22.
// I'm driving in a cadillac, cadillac, I won't give it back, give it back.

#include <string>
#include <unistd.h>
#include <fmt/format.h>

#include "../include/utils.hpp"

using namespace Poco;

[[maybe_unused]] std::vector<std::string> u::split(std::string &s, const char &del) {
    /** String split into vector with single char delimiter
     *
     * @param s : Give the String to split.
     * @param del : Set the delimiter where to split.
     * @retuns vector : vector of strings
     *
     * **/
    std::vector<std::string> ret;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, del))
        ret.push_back(item);

    return ret;
}

[[maybe_unused]] std::vector<std::string> u::split(std::string &s, const std::string &del) {
    /** String split into vector with multi char delimiter
     *
     * @param s : Give the String to split.
     * @param del : Set the delimiter where to split.
     * @retuns vector : vector of strings
     *
     * **/
    size_t pos_start = 0, pos_end, delim_len = del.length();
    std::string token;
    std::vector<std::string> ret;

    while ((pos_end = s.find(del, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end + pos_start);
        pos_start = pos_end + delim_len;
        ret.push_back(token);
    }

    ret.push_back(s.substr(pos_start));
    return ret;
}

[[maybe_unused]] bool u::file_exists(const std::string &name) {
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
        } catch (std::exception &e) {
            bot.log(dpp::ll_error, fmt::format("FUCK, somethin went wron {0}", e.what()));
        }
    }
}

void u::requests(const Poco::URI &uri) {
    try {
        Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        std::string path = uri.getPathAndQuery();
        if (path.empty()) path = '/';

        Net::HTTPRequest req(Net::HTTPRequest::HTTP_GET, path, Net::HTTPMessage::HTTP_1_1);

        Net::HTTPResponse res;

        std::cout << res.getStatus() << " " << res.getStatus() << std::endl;


        auto &is = session.receiveResponse(res);
        StreamCopier::copyStream(is, std::cout);
    } catch (std::exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
}