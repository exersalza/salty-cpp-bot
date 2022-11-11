//
// Created by julian on 2.11.22.
//

#include <string>
#include <nlohmann/json.hpp>

#include "../include/cogs.hpp"
#include "../include/economyCogs.hpp"

using json = nlohmann::json;


economy::Shop::Shop(std::string &name) {
    std::fstream f("../shopConfig.json");
    json d = json::parse(f);

    d = d[name];

    setDispName(d["disp_name"]);
    setDesc(d["desc"]);
    setItems(d["items"]);

    if (!d["color"].empty())
        setColor(d["color"]);

}


dpp::message economy::Shop::createEmbed(const size_t channel_id) {
    dpp::embed embed;

    uint32_t col = std::stoul(color, nullptr, 16);

    embed.set_author(getDispName(), "", "attachment://kermit.jpg")
            .set_color(col);
    dpp::message m(channel_id, embed);

    m.add_file("kermit.jpg", dpp::utility::read_file("resources/pic/kermit.jpg"));

    m.add_component(
            dpp::component().add_component(dpp::component()
                                                   .set_label("Exit")
                                                   .set_type(dpp::cot_button)
                                                   .set_style(dpp::cos_danger)
                                                   .set_id("exit"))
    );

    return m;
}


std::vector<economy::Shop> economy::Economy::getShops() {
    return shops;
}

const std::string &economy::Shop::getIndex() const {
    return index;
}

const std::string &economy::Shop::getDispName() const {
    return disp_name;
}

const std::string &economy::Shop::getDesc() const {
    return desc;
}

const json &economy::Shop::getItems() const {
    return items;
}

const std::string &economy::Shop::getColor() const {
    return color;
}

void economy::Shop::setDispName(const std::string &dispName) {
    disp_name = dispName;
}

void economy::Shop::setDesc(const std::string &desc) {
    this->desc = desc;
}

void economy::Shop::setItems(const json &items) {
    this->items = items;
}

void economy::Shop::setColor(const std::string &color) {
    this->color = color;
}
