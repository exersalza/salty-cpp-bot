//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <fstream>
#include <utility>
#include "../header/config.h"

using namespace std;
using namespace cfg;

bool fileExists (const string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

string Config::getToken() {
    fstream file;
    string TOKEN;

    short count = 0;

    if (!fileExists(path))
        return "Invalid File";

    file.open(path, ios::in);

    if (!file.is_open())
        return "File can't be open.";

    while (!file.eof()) {
        getline(file, TOKEN);

        if (++count >= 2 and !TOKEN.empty()) {
            file.close();
            return "Token file can't be longer than 2 lines!";
        }
    }
    file.close();

    return TOKEN;
}


Config::Config(string &path) : path(path) {}
