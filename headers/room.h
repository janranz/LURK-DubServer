#ifndef ROOM_H
#define ROOM_H
#include"../headers/structs.h"
#include"../headers/baddie.h"
#include"../headers/splitter.h"
#include"../headers/player.h"
#include<vector>
#include<string>
#include<string.h>
#include<shared_mutex>
#include<memory>
#include<bsd/string.h>

class Room
{
    private:
        uint16_t difficulty;
        uint16_t firepower;
        uint16_t coffer;
        // uint16_t currentKills;
        uint16_t totalKills;
        uint16_t totalDeaths;
        std::vector<std::shared_ptr<Room>> room_connections;
        std::vector<std::shared_ptr<Baddie>> baddie_list;
        std::vector<std::shared_ptr<Player>> player_list;
        
        ssize_t bytes;
        
        bool fight_in_progress;
    public:
        LURK_MSG rmpm;
        LURK_ROOM roomTainer;
        std::string roomDesc;
        // std::mutex rLock;
        std::shared_mutex rLock;
        
        Room(std::string,std::string,uint16_t);
        void emplace_connection(std::shared_ptr<Room>);
        void emplace_baddie(std::shared_ptr<Baddie>);
        void emplace_player(std::shared_ptr<Player>);
        void remove_player(std::shared_ptr<Player>);
        

        void inform_connections(std::shared_ptr<Player>);    // "static"
        void inform_player_friendly(std::shared_ptr<Player>);  // everyone for everyone
        void inform_others_player(std::shared_ptr<Player>);
        // void inform_others_player_left(std::shared_ptr<Player>);
        void inform_baddies(std::shared_ptr<Player>); // "static"

        // initiators
        bool initiate_fight_baddie(std::shared_ptr<Player>);
        bool initiate_fight_player(std::shared_ptr<Player>,unsigned char*);
        bool initiate_loot_sequence(std::shared_ptr<Player>,unsigned char*);

        
        void slay_baddie(std::shared_ptr<Baddie>);

        // controllers
        bool pvp_controller(std::shared_ptr<Player>,unsigned char*);
        bool loot_controller(std::shared_ptr<Player>,unsigned char*);
        void fight_controller(std::shared_ptr<Player>);
        

        //helper
        bool isValidConnection(uint16_t);
        bool isValidBaddie();
        int liveBaddieCount();
        std::shared_ptr<Baddie> retrieve_a_baddie();
        bool isFightInProgress();
        void calculateDiff();
        void bundle_update(std::shared_ptr<Player>);
        void big_bundle_update();
        void spread_wealth();
        void tally_kills(uint16_t);
        void room_write(std::string);
        void respawn_baddies();
        void mass_kill_report();
        void single_kill_report(std::shared_ptr<Player>);
        void collect_donations(uint16_t);
        void inform_death(std::shared_ptr<Player>);
        uint32_t fight_roll(uint32_t,uint32_t,uint32_t);
        
        //debug
        // size_t room_connection_size();
        // size_t baddie_list_size();
        // fight logic separate threads here!
};

#endif //ROOM_H
