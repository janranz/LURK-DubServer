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
        std::vector<std::shared_ptr<Room>> room_connections;
        std::vector<std::shared_ptr<Baddie>> baddie_list;
        std::vector<std::shared_ptr<Player>> player_list;
        std::vector<int> liveDex;
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

        // fight logic
        bool initiate_fight_baddie(std::shared_ptr<Player>);
        void fight_controller(std::shared_ptr<Player>);
        void slay_baddie(std::shared_ptr<Baddie>);
        //helper
        bool isValidConnection(uint16_t);
        bool isValidBaddie();
        int liveBaddieCount();
        int LiveBaddieDex();
        bool isFightInProgress();
        void calculateDiff();
        void bundle_update(std::shared_ptr<Player>);
        void big_bundle_update();
        void room_write(std::string);
        void respawn_baddies();
        
        //debug
        // size_t room_connection_size();
        // size_t baddie_list_size();
        // fight logic separate threads here!
};

#endif //ROOM_H
