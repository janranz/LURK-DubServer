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
#include<sys/socket.h>
class Gamemaster
{
    private:
        unsigned int g_seed;
        LURK_GAME gmInfo;
        LURK_VERSION vers;
        LURK_MSG gmpm;
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
        void craft_room_names();
        void build_rooms();
        std::shared_ptr<Baddie> build_a_baddie(uint16_t);
        void populate_rooms();
        
        //cleanup
        void ragequit(std::shared_ptr<Player>);
        //network
        void GMController(int);
        uint8_t listener(std::shared_ptr<Player>);

        // type processing
        void proc_character(std::shared_ptr<Player>);

        //helper
        bool check_name(std::shared_ptr<Player>);
        bool check_stat(std::shared_ptr<Player>);
        // error handling
        void error_character(std::shared_ptr<Player>);
};

#endif //GM_H
