//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <sstream>

#include "../include/utils.hpp"

using namespace std;

vector<string> u::split(std::string &s, const char &del) {
    /** String split into vector with single char delimiter
     *
     * @param s : Give the String to split.
     * @param del : Set the delimiter where to split.
     * @retuns vector : vector of strings
     *
     * **/
    vector<string> ret;
    stringstream ss(s);
    string item;

    while (getline(ss, item, del))
        ret.push_back(item);

    return ret;
}

vector<string> u::split(std::string &s, const std::string &del) {
    /** String split into vector with multi char delimiter
     *
     * @param s : Give the String to split.
     * @param del : Set the delimiter where to split.
     * @retuns vector : vector of strings
     *
     * **/
    size_t pos_start = 0, pos_end, delim_len = del.length();
    string token;
    vector<string> ret;

    while ((pos_end = s.find(del, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end + pos_start);
        pos_start = pos_end + delim_len;
        ret.push_back(token);
    }

    ret.push_back(s.substr(pos_start));
    return ret;
}

bool u::fileExists(const string &name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}