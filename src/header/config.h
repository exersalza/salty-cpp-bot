//
// Created by julian on 29.10.22.
//

#ifndef DCBOT_CONFIG_H
#define DCBOT_CONFIG_H

#include <string>

using namespace std;

namespace cfg {

    class Config {
    public:
        Config() = default;
        explicit Config(string &path);

        string getToken();

    private:
        string token;
        string path;
    };

} // Cfg

#endif //DCBOT_CONFIG_H
