//
// Created by julian on 2.11.22.
//

#include <string>
#include <nlohmann/json.hpp>

#include "../include/cogs.hpp"
#include "../include/economyCogs.hpp"

using json = nlohmann::json;


economy::Shop::Shop(std::string &name) {
    std::fstream f("../shopConfig.json");
    json d = json::parse(f);

    d = d[name];
}
