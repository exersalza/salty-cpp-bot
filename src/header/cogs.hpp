//
// Created by julian on 11/2/22.
//

#ifndef DCBOT_COGS_HPP
#define DCBOT_COGS_HPP

#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using namespace std;

namespace cog {
    void regis_commands(dpp::cluster &bot);
} // cog

namespace economy {
    class Shop {
    public:
        /// \param index : Give the Shop name to init from shopConfig.json
        explicit Shop(string &index);

        const string &getIndex() const;
        const string &getDispName() const;
        const string &getDesc() const;
        const json &getItems() const;

        void setDispName(const string &dispName);
        void setDesc(const string &desc);
        void setItems(const json &items);

    private:
        string index;
        string disp_name;
        string desc;
        json items;
    };

    class Economy {
    public:
        vector<Shop> getShops();

    private:
        vector<Shop> shops;

    };
} // economy

#endif //DCBOT_COGS_HPP
