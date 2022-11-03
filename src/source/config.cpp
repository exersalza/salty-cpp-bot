//
// Created by julian on 29.10.22.
//

#include "../header/config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using namespace std;

using json = nlohmann::json;


cfg::Config::Config(string &path) : path(path) {
    fstream f(path);
    json data = json::parse(f);

    token = data["token"];
}

const string &cfg::Config::getToken() const {
    return token;
}
