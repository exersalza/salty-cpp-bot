// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 29.10.22.
//

#ifndef DCBOT_CONFIG_HPP
#define DCBOT_CONFIG_HPP

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace cfg {
    struct sql {
        const char* user;
        const char* password;
        const char* db;
        const char* host;
    };

    struct twitch {
        const std::string id;
        const std::string secret;
        std::string oauth;
    };

    class Config {
    public:
        Config() = default;

        explicit Config(std::string &&path);

        [[nodiscard]] const std::string &getToken() const;
        [[nodiscard]] const std::string &getToken(const std::string& token_type) const;

        // SQL - Set a to do tag here, when you insert real data. (I think only usable in Clion but not sure.)

        [[nodiscard]] sql getSqlConf();
        [[nodiscard]] twitch getTwitchConf();

        int b_color = 0xbc3440;
        std::string log_webhook;
    private:
        // Base bot config
        json data;
        std::string token;
        std::string dev_token;
        std::string path;
    };


} // Cfg

#endif //DCBOT_CONFIG_HPP
