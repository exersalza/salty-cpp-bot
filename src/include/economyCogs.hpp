//
// Created by julian on 11/4/22.
//

#ifndef DCBOT_ECONOMYCOGS_HPP
#define DCBOT_ECONOMYCOGS_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <dpp/dpp.h>


namespace ec {
    class Shop {
    public:

    private:
        explicit Shop(std::string &name);

        std::string name;
        std::string desc;

    };

} // economy




#endif //DCBOT_ECONOMYCOGS_HPP
