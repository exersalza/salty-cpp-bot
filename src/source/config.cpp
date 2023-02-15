// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 29.10.22.
//

#include <fstream>
#include <iostream>
#include <dpp/nlohmann/json.hpp>

#include "../include/config.hpp"

using json = nlohmann::json;


cfg::Config::Config(std::string &&path) : path(path) {
    std::fstream f(path);
    json _data = json::parse(f);
    f.close();

    std::fstream j("twitch_config.json");
    json _twitch = json::parse(j);
    j.close();

    t_id = _twitch["twitch_client"];
    t_secret = _twitch["twitch_secret"];
    t_oauth = _twitch["twitch_oauth"];

    token = _data["token"];
    dev_token = _data["dev_token"];

    for (auto &i : _data["webhook"]) {
        log_webhook.push_back(i);
    }


    if (log_webhook.empty()) {
        std::cout << "WEBHOOK ID NOT SET, OMITTING WEBHOOK LOGING." << std::endl;
    }

    if (!dev_token.empty()) {
        std::cout << "\u001b[31m---- USING IS_DEV TOKEN ----\u001b[0m" << std::endl;
    }


    this->data = _data;
}

[[nodiscard]] cfg::sql cfg::Config::getSqlConf() {
    cfg::sql sql{};

    if (data["db"].empty()) {
        std::fstream f(path);
        json _data = json::parse(f);

        this->data = _data;
    }

    sql.db = data["db"].get_ref<const std::string &>().c_str();
    sql.host = data["host"].get_ref<const std::string &>().c_str();
    sql.user = data["user"].get_ref<const std::string &>().c_str();
    sql.password = data["password"].get_ref<const std::string &>().c_str();

    return sql;
}

void cfg::Config::updateTwitchConfig(json &conf) {
    t_id = conf["twitch_client"];
    t_secret = conf["twitch_secret"];
    t_oauth = conf["twitch_oauth"];
}

[[nodiscard]] cfg::twitch cfg::Config::getTwitchConf() {
    return {
            t_id,
            t_secret,
            t_oauth
    };
}

const std::string &cfg::Config::getToken() const {
    return token;
}

const std::string &cfg::Config::getToken(const std::string &token_type) const {
    if (token_type == "dev")
        return dev_token;
    return token;
}
