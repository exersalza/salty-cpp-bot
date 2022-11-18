//
// Created by julian on 29.10.22.
// I'm driving in a cadillac, cadillac, I won't give it back, give it back.

#include <string>
#include <unistd.h>
#include <fmt/format.h>

#include "../include/utils.hpp"


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

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string u::requests(const char* url) {
    CURL *curl;
    CURLcode cres;
    std::string readBuffer;


    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        cres = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

int u::stoc(const std::string &src, char* dest) {
    try {
        for (int i = 0; i < src.length(); ++i) {
            dest[i] = src[i];
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
