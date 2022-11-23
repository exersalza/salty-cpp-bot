//
// Created by julian on 29.10.22.
//

#include <fstream>
#include <dpp/nlohmann/json.hpp>

#include "../include/config.hpp"

using json = nlohmann::json;


cfg::Config::Config(std::string &path) : path(path) {
    std::fstream f(path);
    json data = json::parse(f);

    token = data["token"];
    dev_token = data["dev_token"];

    sql["db"] = data["db"].get_ref<const std::string&>().c_str();
    sql["host"] = data["host"].get_ref<const std::string&>().c_str();
    sql["user"] = data["user"].get_ref<const std::string&>().c_str();
    sql["password"] = data["password"].get_ref<const std::string&>().c_str();
}

const std::string &cfg::Config::getToken() const {
    return token;
}

const std::string &cfg::Config::getToken(const std::string& token_type) const {
    if (token_type == "dev")
        return dev_token;
    return token;
}
