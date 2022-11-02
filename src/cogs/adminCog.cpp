//
// Created by julian on 11/2/22.
//

#include <dpp/dpp.h>
#include "../header/cogs.h"

void cog::test(dpp::cluster& bot) {
    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        if (event.msg.content == "!melm") {
            bot.message_create(dpp::message(event.msg.channel_id, "POGGERS")
                                                .set_reference(event.msg.id));
        }
    });
}