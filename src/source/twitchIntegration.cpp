// Copyright (c) 2022 exersalza
// This project is licensed under the GNU 3 license. more information https://github.com/exersalza/salty-cpp-bot/blob/main/LICENSE

//
// Created by julian on 11/17/22.
//

#include <fmt/format.h>

#include "../include/twitchIntegration.hpp"
#include "../include/config.hpp"
#include "../include/utils.hpp"

void twitch::generateNewToken(cfg::twitch& twitch_config) {
    auto f = fmt::format("client_id={0}&client_secret={1}&grant_type=client_credentials",
                                twitch_config.id, twitch_config.secret);

    auto res = u::post("https://id.twitch.tv/oauth2/token", f.c_str());

    // define parser callback, stole it from https://json.nlohmann.me/api/basic_json/parse/#examples
    json::parser_callback_t cb = [](int depth, json::parse_event_t event, json& parsed) {
        if (event == json::parse_event_t::key &&
                (parsed == json("expires_in") | parsed == json("token_type"))) {
            return false;
        }
        return true;
    };

    // parse (with callback) and serialize JSON
    json j_filtered = json::parse(res, cb);
    twitch_config.oauth = j_filtered["access_token"];
}

void twitch::init(cfg::Config& config, mysqlpp::Connection& conn, dpp::cluster& bot) {
    cfg::twitch twitch_config = config.getTwitchConf();
    json twitch_content_map; // twitch channel info thingi
    // Twitch integration
    std::fstream file("CACHE_twitch_content_map.json");

    if (!file.fail())
        twitch_content_map = json::parse(file);

    file.close();


    std::string bearer {fmt::format("Authorization: Bearer {0}", twitch_config.oauth)};
    std::string client_id {fmt::format("Client-Id: {0}", twitch_config.id)};
    std::vector<const char *> helix_header;
    json twitch_channel_map; // channel_id and twitch_channel id
    std::unordered_set<size_t> ids; // add all streamer ids from the db and remove dupes
    std::vector<std::string> query_strings;
    std::vector<std::string> active_streams;

    helix_header.push_back(bearer.c_str());
    helix_header.push_back(client_id.c_str());



    mysqlpp::Query query = conn.query();
    query << fmt::format("select * from salty_cpp_bot.twitch");
    mysqlpp::StoreQueryResult res = query.store();

    twitch_channel_map.clear();

    for (auto &i : res) {
        if (!twitch_channel_map.contains((std::string)i["stream_id"]))
            twitch_channel_map[(std::string)i["stream_id"]] = {};


        twitch_channel_map[(std::string)i["stream_id"]] += (size_t)i["channel_id"];
        ids.insert(i["stream_id"]);
    }

    conn.disconnect();

    short counter = 0;
    std::stringstream query_str;

    for (auto &i : ids) {
        if (counter >= 100) {
            query_str << "first=100";
            query_strings.push_back(query_str.str());
            query_str.clear();
        }

        query_str << "user_id=" << i << "&";
        ++counter;
    }
    query_str << "first=100";
    query_strings.push_back(query_str.str());
    active_streams.clear();


    // https://dev.twitch.tv/docs/api/reference/#get-streams
    for (auto &i : query_strings) {
        json response = json::parse(u::requests(fmt::format("https://api.twitch.tv/helix/streams?{0}", i).c_str(), helix_header)); // {"error":"Unauthorized","status":401,"message":"Invalid OAuth token"}

        if (response.find("status") != response.end()) {
            twitch::generateNewToken(twitch_config);
            response = json::parse(u::requests(fmt::format("https://api.twitch.tv/helix/streams?{0}", i).c_str(), helix_header));
        }

        json &data = response["data"];

        for (auto &j : data) {
            active_streams.push_back((std::string)j["user_id"]);

            if (twitch_content_map.contains(j["user_id"]))
                continue;

            twitch_content_map[(std::string)j["user_id"]] = {
                    {"user_name", j["user_name"]},
                    {"title", j["title"]},
                    {"game_name", j["game_name"]},
                    {"thumbnail_url", j["thumbnail_url"]},
                    {"viewer_count", j["viewer_count"]},
                    {"send", false}
            };
        }

        // yeet offline channels

        auto iter = twitch_content_map.begin();
        for(; iter != twitch_content_map.end(); ) {
            if (std::find(active_streams.begin(), active_streams.end(), iter.key()) == active_streams.end()
                    || iter.value().is_null()) {

                iter = twitch_content_map.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    for (auto &i : twitch_channel_map.items()) {
        if (!twitch_content_map.contains(i.key()))
            continue;

        auto& streamer = twitch_content_map[i.key()];

        for (auto &j : twitch_channel_map[i.key()]) {
            dpp::embed em;

            auto name = streamer["user_name"];
            std::stringstream Turl;
            Turl << fmt::format("{0}", streamer["thumbnail_url"]);
            auto url = Turl.str();

            em.set_author(fmt::format("{0} is now Live!", name),
                          fmt::format("https://twitch.tv/{0}", name),
                          bot.me.get_avatar_url())
              .set_title(streamer["title"])
              .set_url(fmt::format("https://twitch.tv/{0}", name))
              .set_image(fmt::format("https://static-cdn.jtvnw.net/previews-ttv/live_user_{0}-{width}x{height}.jpg",
                                     u::stol(name), fmt::arg("width", 1920), fmt::arg("height", 1080)));

            bot.message_create(dpp::message((size_t)j, em));
        }

    }

    // create cache
    std::ofstream TMP_twitch_content_map("CACHE_twitch_content_map.json");
    TMP_twitch_content_map << twitch_content_map;
    TMP_twitch_content_map.close();
}

