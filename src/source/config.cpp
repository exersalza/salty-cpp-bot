//
// Created by julian on 29.10.22.
//

#include <fstream>
#include <nlohmann/json.hpp>

#include "../include/config.hpp"

using json = nlohmann::json;


cfg::Config::Config(std::string &path) : path(path) {
    std::fstream f(path);
    json data = json::parse(f);

    token = data["token"];
}

const std::string &cfg::Config::getToken() const {
    return token;
}
