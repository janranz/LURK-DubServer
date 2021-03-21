#pragma once

#include<algorithm>
#include"baddie.h"
#include"player.h"

class Room
{
    private:
        uint16_t roomNumber;
        std::string roomName;
        uint32_t roomDescLength;
        std::string roomDesc;
        std::vector<uint8_t> connectedRoomNums;
        std::vector<Baddie*> baddieList;
        std::vector<Player*> playerList;
        std::mutex innerlock;

    public:
        Room(uint16_t,std::string,uint32_t,std::string);
        ~Room();
        void lock();
        void unlock();
        void setConnectedRooms(uint8_t);
        void addBaddie(Baddie);
        void addPlayer(Player*);
        int searchPlayer(std::string);
        void removePlayer(Player*);
        std::vector<Baddie*> getBaddieList();
        std::vector<Player*> getPlayerList();
};
