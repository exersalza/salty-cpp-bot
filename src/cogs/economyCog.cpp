//
// Created by julian on 2.11.22.
//

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

#include "../header/cogs.hpp"

using json = nlohmann::json;


economy::Shop::Shop(string &name) {
    fstream f("../shopConfig.json");
    json d = json::parse(f);

    d = d[name];

    setDispName(d["disp_name"]);
    setDesc(d["desc"]);
    setItems(d["items"]);

}

const string &economy::Shop::getIndex() const {
    return index;
}

const string &economy::Shop::getDispName() const {
    return disp_name;
}

const string &economy::Shop::getDesc() const {
    return desc;
}

const json &economy::Shop::getItems() const {
    return items;
}

void economy::Shop::setDispName(const string &dispName) {
    disp_name = dispName;
}

void economy::Shop::setDesc(const string &desc) {
    Shop::desc = desc;
}

void economy::Shop::setItems(const json &items) {
    Shop::items = items;
}