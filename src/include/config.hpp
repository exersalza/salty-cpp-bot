//
// Created by julian on 29.10.22.
//

#ifndef DCBOT_CONFIG_HPP
#define DCBOT_CONFIG_HPP

#include <string>

namespace cfg {

    class Config {
    public:
        Config() = default;

        explicit Config(std::string &path);

        [[nodiscard]] const std::string &getToken() const;
        [[nodiscard]] const std::string &getToken(const std::string& token_type) const;

        // SQL - Set a to do tag here, when you insert real data. (I think only usable in Clion but not sure.)

        std::map<std::string, const char*> sql;

        int b_color = 0xbc3440;
    private:
        // Base bot config
        std::string token;
        std::string dev_token;
        std::string path;
    };

} // Cfg

#endif //DCBOT_CONFIG_HPP
