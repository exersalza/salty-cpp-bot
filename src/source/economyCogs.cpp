// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 2.11.22.
//

#include <string>
#include <nlohmann/json.hpp>

#include "../include/cogs.hpp"
#include "../include/economyCogs.hpp"

using json = nlohmann::json;


ec::Shop::Shop(std::string &name) {
    std::fstream f("../shopConfig.json");
    json d = json::parse(f);

    d = d[name];
}
