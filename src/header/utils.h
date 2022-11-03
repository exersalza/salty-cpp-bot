//
// Created by julian on 1.11.22.
//
#ifndef DCBOT_UTILS_H
#define DCBOT_UTILS_H

#include <vector>
#include <string>

namespace u {
    std::vector<std::string> split(std::string& s, const char& del);
    std::vector<std::string> split(std::string& s, const std::string& del);
} // u

#endif // DCBOT_UTILS_H

