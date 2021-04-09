#ifndef ROOM_H
#define ROOM_H
#include<algorithm>
#include<stdio.h>
#include<string.h>
#include<vector>
#include"structs.h"

class Gamemaster;
class Player;
class Room
{
    private:
        
        // std::vector<uint16_t> connectedRoomNums;
        


        char stress_level; // overall aggression of baddies;
        

    public:
        LURK_ROOM room;
        std::string roomDesc;
        std::vector<LURK_CONNECTION*> connectedRooms;
        Room(uint16_t,std::string,uint16_t,std::string);
        ~Room();
        // std::shared_ptr<std::mutex> pLock;
        std::shared_ptr<std::mutex> rLock;
        std::vector<Baddie> baddieList;
        std::vector<Player*> playerList;
        void setConnectedRooms(LURK_CONNECTION*);
        void injectBaddie(Baddie);
        ssize_t addPlayer(Player*);
        int searchPlayer(std::string const&);
        void removePlayer(Player*);
        void setStressLevel(char);

        // writing to client
        ssize_t sendBaddieInfo();
        ssize_t sendRoomInfo(Player*);
        ssize_t sendFriendInfo();
};

#endif //ROOM_H
