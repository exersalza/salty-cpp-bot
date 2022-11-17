//
// Created by julian on 1.11.22.
//
#ifndef DCBOT_UTILS_HPP
#define DCBOT_UTILS_HPP

#include <vector>
#include <string>
#include <dpp/dpp.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/URI.h>
#include <Poco/StreamCopier.h>
#include <mysql++/query.h>


namespace u {
    [[maybe_unused]] std::vector<std::string> split(std::string &s, const char &del);

    [[maybe_unused]] std::vector<std::string> split(std::string &s, const std::string &del);

    [[maybe_unused]] bool file_exists(const std::string &name);

    int kill_query(mysqlpp::Query &query);


    void presence_update(dpp::cluster &bot);

    void requests(const Poco::URI &uri);

    int do_request(Poco::Net::HTTPClientSession &session, Poco::Net::HTTPRequest &request);
} // u

#endif // DCBOT_UTILS_HPP

