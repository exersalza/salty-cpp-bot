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
        static string getToken() { return token; };
        string getToken(string &path);

    private:
        static string token;
    };

} // Cfg

#endif //DCBOT_CONFIG_H
