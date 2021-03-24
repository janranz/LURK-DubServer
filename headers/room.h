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
        std::vector<Baddie> baddieList;
        std::vector<Player*> playerList;
        char stress_level; // overall aggression of baddies;
        std::mutex innerlock;

    public:
        Room(uint16_t,std::string,uint16_t,std::string);
        ~Room();
        void lock();
        void unlock();
        void setConnectedRooms(char, uint16_t);
        void injectBaddie(Baddie*);
        void addPlayer(Player*);
        int searchPlayer(std::string const&);
        void removePlayer(Player*);
        void setStressLevel(char);
        std::vector<Baddie*> getBaddieList();
        std::vector<Player*> getPlayerList();
        std::string getRoomName();
        
        int DEBUG_getBaddieListSize();
        int DEBUG_getRoomNumber();
        std::vector<uint16_t> DEBUG_getConnected();
};
