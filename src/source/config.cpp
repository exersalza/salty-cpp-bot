//
// Created by julian on 29.10.22.
//

#include "../include/config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

using json = nlohmann::json;


cfg::Config::Config(string &path) : path(path) {
    fstream f(path);
    json data = json::parse(f);

    token = data["token"];

    // convert string to char[]

    vector<string> sql_data = {
            "host", "user", "password", "db"
    };

    vector<char *> test = {
            sql_host, sql_user, sql_db, sql_password
    };

    cout << test[1] << endl;
    cout << "f" << endl;


}

const string &cfg::Config::getToken() const {
    return token;
}
