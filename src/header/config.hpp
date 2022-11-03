//
// Created by julian on 29.10.22.
//

#ifndef DCBOT_CONFIG_HPP
#define DCBOT_CONFIG_HPP

#include <string>

using namespace std;

namespace cfg {

    class Config {
    public:
        Config() = default;

        explicit Config(string &path);

        [[nodiscard]] const string &getToken() const;


    private:
        string token;
        string path;
    };

} // Cfg

#endif //DCBOT_CONFIG_HPP
