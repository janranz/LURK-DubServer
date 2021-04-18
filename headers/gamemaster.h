#ifndef GM_H
#define GM_H
#include"../headers/structs.h"
#include"../headers/fmt/format.h"
#include"../headers/room.h"
#include"../headers/player.h"
#include<string>
#include<vector>
#include<memory>

class Gamemaster
{
    private:
        LURK_GAME gmInfo;
        LURK_VERSION vers;
        std::mutex GMLock;
        std::vector<std::shared_ptr<Room>> master_room_list;
        std::vector<std::shared_ptr<Player>> master_player_list;
    public:
        chatter_messages c_m;
        Gamemaster();
};

#endif //GM_H
