//
// Created by julian on 11/16/22.
//

#ifndef DCBOT_GAMES_HPP
#define DCBOT_GAMES_HPP

#include <vector>
#include <string>

/* Todo:
 *  Legend:
 *   Same as from the ticketCogs header
 *   |
 *  commands:
 *   admin - shop:
 *    ? /shop create <title> <desc> -> creates a new shop with given title and desc
 *   user - shop:
 *    /olaf -> opens olaf the weapon smith
 *    /herbert -> opens herbert the armor smith
 *    /nue -> opens the skill tree
 * */

class g {
    struct Player {
    public:
        explicit Player(std::string name);

        std::string name;
    };

    struct BjPlayer : Player {
    public:


    private:
        std::vector<std::string> cards;
        int card_count;
        int updateCardCount();
    };

    class BJ {
    public:
        BJ();
        explicit BJ(std::vector<BjPlayer> &player_list);

        int player_count;
        void dealCards();

    private:
        std::vector<int> possible_cards = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    };
};


#endif //DCBOT_GAMES_HPP
