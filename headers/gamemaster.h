#ifndef GM_H
#define GM_H
#include"../headers/structs.h"
#include"../headers/fmt/format.h"
#include"../headers/room.h"
#include"../headers/player.h"
#include<string>
#include<vector>
#include<memory>
#include<ctime>

class Gamemaster
{
    private:
        unsigned int g_seed;
        LURK_GAME gmInfo;
        LURK_VERSION vers;
        std::mutex GMLock;
        std::vector<std::shared_ptr<Room>> master_room_list;
        std::vector<std::shared_ptr<Player>> master_player_list;
        chatter_messages c_m;
        cm_sizes vsize;
    public:
        //events
        int fast_rand();
        //initializers
        Gamemaster();
        void build_chatter(int, std::vector<std::string>::iterator);
        void size_vectors();
};

#endif //GM_H
