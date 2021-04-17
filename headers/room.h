#ifndef ROOM_H
#define ROOM_H
#include"../headers/structs.h"
#include"../headers/player.h"
#include<vector>
#include<string>
#include<string.h>
#include<mutex>
class Room
{
    private:
        char stress_level;
        std::vector<int> connection_dex;
        std::vector<Player*> player_list;
    public:
        LURK_ROOM roomTainer;
        std::string roomDesc;
        std::mutex rLock;

        void emplace_connection(int);
        void emplace_player(Player*);
};

#endif //ROOM_H
