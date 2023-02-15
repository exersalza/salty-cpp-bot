// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/4/22.
//

#ifndef DCBOT_ECONOMY_HPP
#define DCBOT_ECONOMY_HPP

#include <string>
#include <vector>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>


namespace ec {
    class Shop {
    public:

    private:
        explicit Shop(std::string &name);

        std::string name;
        std::string desc;

    };

} // economy




#endif //DCBOT_ECONOMY_HPP
