//
// Created by julian on 11/4/22.
//

#ifndef DCBOT_ECONOMYCOGS_HPP
#define DCBOT_ECONOMYCOGS_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <dpp/dpp.h>

namespace economy {
    class Shop {
    public:
        /// \param index : Give the Shop name to init from shopConfig.json
        explicit Shop(std::string &index);

    private:
        std::string index;
        std::string disp_name;
        std::string desc;
        json items;
        std::string color = "0xffffff";
    };

    class Economy {

    private:
        std::vector<Shop> shops;

    };

} // economy




#endif //DCBOT_ECONOMYCOGS_HPP
