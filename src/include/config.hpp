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

        // SQL
        char sql_host[11] = "localhost";
        char sql_user[6] = "local";
        char sql_password[1] = "";
        char sql_db[14] = "salty_cpp_bot";


        int b_color = 0xbc3440;
    private:
        // Base bot config
        std::string token;
        std::string path;
    };

} // Cfg

#endif //DCBOT_CONFIG_HPP
