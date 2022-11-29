// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 29.10.22.
//

#include <fstream>
#include <iostream>
#include <dpp/nlohmann/json.hpp>

#include "../include/config.hpp"


cfg::Config::Config(std::string &&path) : path(path) {
    std::fstream f(path);
    json _data = json::parse(f);

    token = _data["token"];
    dev_token = _data["dev_token"];
    log_webhook = _data["webhook"];

    if (log_webhook.empty()) {
        std::cout << "WEBHOOK ID NOT SET, OMITTING WEBHOOK LOGING." << std::endl;
    }


    this->data = _data;
}

[[nodiscard]] cfg::sql cfg::Config::getSqlConf() {
    cfg::sql sql {};

    if (data["db"].empty()) {
        std::fstream f(path);
        json _data = json::parse(f);

        this->data = _data;
    }

    sql.db = data["db"].get_ref<const std::string&>().c_str();
    sql.host = data["host"].get_ref<const std::string&>().c_str();
    sql.user = data["user"].get_ref<const std::string&>().c_str();
    sql.password = data["password"].get_ref<const std::string&>().c_str();

    return sql;
}

const std::string &cfg::Config::getToken() const {
    return token;
}

const std::string &cfg::Config::getToken(const std::string& token_type) const {
    if (token_type == "dev")
        return dev_token;
    return token;
}
