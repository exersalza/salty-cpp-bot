//
// Created by julian on 29.10.22.
//

#ifndef DCBOT_CONFIG_HPP
#define DCBOT_CONFIG_HPP

#define SQL_NORMAL_LENGTH 20
#define SQL_PASSWORD_LENGTH 32

#include <string>

namespace cfg {

    class Config {
    public:
        Config() = default;

        explicit Config(std::string &path);

        [[nodiscard]] const std::string &getToken() const;
        [[nodiscard]] const std::string &getToken(const std::string& token_type) const;

        // SQL - Set a to do tag here, when you insert real data. (I think only usable in Clion but not sure.)
        char sql_host[SQL_NORMAL_LENGTH] = "localhost";
        char sql_user[SQL_NORMAL_LENGTH] = "local";
        char sql_password[SQL_PASSWORD_LENGTH] = "";
        char sql_db[SQL_NORMAL_LENGTH] = "salty_cpp_bot";


        int b_color = 0xbc3440;
    private:
        // Base bot config
        std::string token;
        std::string dev_token;
        std::string path;
    };

} // Cfg

#endif //DCBOT_CONFIG_HPP
