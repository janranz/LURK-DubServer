#ifndef ROOM_H
#define ROOM_H
#include<algorithm>
#include<stdio.h>
#include<string.h>
#include<vector>
class Gamemaster;
class Player;
class Room
{
    private:
        uint16_t roomNumber;
        std::string roomName;
        uint16_t roomDescLength;
        std::string roomDesc;
        std::vector<uint16_t> connectedRoomNums;

        char stress_level; // overall aggression of baddies;
        

    public:
        Room(uint16_t,std::string,uint16_t,std::string);
        ~Room();
        // std::shared_ptr<std::mutex> pLock;
        std::shared_ptr<std::mutex> rLock;
        std::vector<Baddie> baddieList;
        std::vector<Player*> playerList;
        void setConnectedRooms(char, uint16_t);
        void injectBaddie(Baddie);
        void addPlayer(Player*);
        int searchPlayer(std::string const&);
        void removePlayer(Player*);
        void setStressLevel(char);

        int DEBUG_getBaddieListSize();
        int DEBUG_getRoomNumber();
        std::vector<uint16_t> DEBUG_getConnected();

        // writing to client
        void sendRoomInfo();
};

#endif //ROOM_H
