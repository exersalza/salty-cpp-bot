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

        const std::string &getIndex() const;

        const std::string &getDispName() const;

        const std::string &getDesc() const;

        const json &getItems() const;

        const std::string &getColor() const;

        void setDispName(const std::string &dispName);

        void setDesc(const std::string &desc);

        void setItems(const json &items);

        void setColor(const std::string &color);

        dpp::message createEmbed(std::size_t channel_id);

    private:
        std::string index;
        std::string disp_name;
        std::string desc;
        json items;
        std::string color = "0xffffff";
    };

    class Economy {
    public:
        std::vector<Shop> getShops();

    private:
        std::vector<Shop> shops;

    };

} // economy




#endif //DCBOT_ECONOMYCOGS_HPP
