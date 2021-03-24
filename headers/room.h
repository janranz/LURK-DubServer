#pragma once

#include<algorithm>
#include"baddie.h"
#include"player.h"

class Room
{
    private:
        uint16_t roomNumber;
        std::string roomName;
        uint16_t roomDescLength;
        std::string roomDesc;
        std::vector<uint16_t> connectedRoomNums;
        std::vector<Baddie*> baddieList;
        std::vector<Player*> playerList;
        std::mutex innerlock;

    public:
        Room(uint16_t,std::string,uint16_t,std::string);
        ~Room();
        void lock();
        void unlock();
        void setConnectedRooms(uint16_t);
        void addBaddie(Baddie);
        void addPlayer(Player*);
        int searchPlayer(std::string const&);
        void removePlayer(Player*);
        std::vector<Baddie*> getBaddieList();
        std::vector<Player*> getPlayerList();
};
