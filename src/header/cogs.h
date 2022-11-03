//
// Created by julian on 11/2/22.
//

#ifndef DCBOT_COGS_H
#define DCBOT_COGS_H

#include <dpp/dpp.h>
#include <vector>
#include <string>

using namespace std;

namespace cog {
    void regis_commands(dpp::cluster& bot);
} // cog

namespace economy {
    class Shop {
    public:
        Shop(string& name);

        string get_name();
        string get_itemc();

    private:
        string name;
        double itemc;
        
    };

    class Economy {
    public:
        auto get_shops();

    private:
        vector<Shop> shops;

    };
} // economy

#endif //DCBOT_COGS_H
