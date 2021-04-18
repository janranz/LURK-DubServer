#ifndef ROOM_H
#define ROOM_H
#include"../headers/structs.h"
#include"../headers/player.h"
#include<vector>
#include<string>
#include<string.h>
#include<mutex>
#include<memory>
class Room
{
    private:
        char stress_level;
        std::vector<std::shared_ptr<Room>> room_connections;
        std::vector<std::shared_ptr<Baddie>> baddie_list;
        std::vector<std::shared_ptr<Player>> player_list;
        ssize_t bytes;
    public:
        LURK_ROOM roomTainer;
        std::string roomDesc;
        std::mutex rLock;
        
        Room(std::string,std::string,uint16_t);
        void emplace_connection(std::shared_ptr<Room>);
        void emplace_baddie(std::shared_ptr<Baddie>);
        void emplace_player(std::shared_ptr<Player>);
        void remove_player(std::shared_ptr<Player>);
        void inform_connections(std::shared_ptr<Player>);    // "static"
        void inform_players_friendly();  // everyone for everyone
        void inform_baddies(std::shared_ptr<Player>); // "static"

        //debug
        size_t room_connection_size();
        size_t baddie_list_size();
        // fight logic separate threads here!
};

#endif //ROOM_H
