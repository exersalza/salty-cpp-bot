// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/17/22.
//

#include <fmt/format.h>

#include "../include/twitchIntegration.hpp"
#include "../include/config.hpp"
#include "../include/utils.hpp"

void twitch::generateNewToken(cfg::twitch& twitch_config) {
    auto f = fmt::format("client_id={0}&client_secret={1}&grant_type=client_credentials",
                                twitch_config.id, twitch_config.secret);

    auto res = u::post("https://id.twitch.tv/oauth2/token", f.c_str());

    // define parser callback, stole it from https://json.nlohmann.me/api/basic_json/parse/#examples
    json::parser_callback_t cb = [](int depth, json::parse_event_t event, json& parsed) {
        if (event == json::parse_event_t::key &&
                (parsed == json("expires_in") | parsed == json("token_type"))) {
            return false;
        }
        return true;
    };

    // parse (with callback) and serialize JSON
    json j_filtered = json::parse(res, cb);
    twitch_config.oauth = j_filtered["access_token"];
}

