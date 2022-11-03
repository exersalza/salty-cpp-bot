//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <fstream>
#include <utility>
#include "../header/config.h"

using namespace std;
using namespace cfg;

Config::Config(string &path) : path(path) {}

const string &Config::getPath() const {
    return path;
}
