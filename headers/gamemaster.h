#ifndef GM_H
#define GM_H
#include"../headers/structs.h"
#include"../headers/baddie.h"
#include"../headers/splitter.h"
#include"../headers/fmt/format.h"
#include"../headers/room.h"
#include"../headers/player.h"
#include<string>
#include<vector>
#include<ctime>
#include<sys/socket.h>
#include<shared_mutex>

class Gamemaster
{
    private:
        // unsigned int g_seed;
        LURK_GAME gmInfo;
        LURK_VERSION vers;
        LURK_MSG gmpm;
        //std::mutex GMLock;
        std::shared_mutex GMLock;
        std::vector<std::shared_ptr<Room>> master_room_list;
        std::vector<std::shared_ptr<Player>> master_player_list;
        chatter_messages c_m;
        cm_sizes vsize;
        bool genderBender;
        std::string pveLeader;
        std::string pvpLeader;
        uint32_t pveHighScore;
        uint32_t pvpHighScore;

    public:
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
        void pump_n_dump(std::shared_ptr<Player>);
        //network
        void GMController(int);
        uint8_t listener(std::shared_ptr<Player>);
        // type processing
        void proc_character(std::shared_ptr<Player>);
        void proc_start(std::shared_ptr<Player>);
        void proc_changeroom(std::shared_ptr<Player>);
        void proc_msg(std::shared_ptr<Player>);
        void proc_leave(std::shared_ptr<Player>);
        void proc_fight(std::shared_ptr<Player>);
        void proc_pvp(std::shared_ptr<Player>);
        void proc_loot(std::shared_ptr<Player>);
        //helper
        bool check_name(std::shared_ptr<Player>);
        bool check_stat(std::shared_ptr<Player>);
        void move_player(std::shared_ptr<Player>,uint16_t);
        void spawn_player(std::shared_ptr<Player>);
        void check_pveHighScore(char*,uint32_t);
        void check_pvpHighScore(char*,uint32_t);
        void write_global(std::string);
        // error handling
        void error_character(std::shared_ptr<Player>);
        void error_start(std::shared_ptr<Player>);
        void error_changeroom(std::shared_ptr<Player>);
        void error_msg(std::shared_ptr<Player>);
        void error_fight(std::shared_ptr<Player>);
        void error_invalid(std::shared_ptr<Player>);
        void error_dead(std::shared_ptr<Player>);
        void error_pvp(std::shared_ptr<Player>);
        void error_loot(std::shared_ptr<Player>);
};

#endif //GM_H
