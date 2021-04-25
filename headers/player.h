#ifndef PLAYER_H
#define PLAYER_H

#include"../headers/structs.h"
#include"../headers/fmt/format.h"
#include<shared_mutex>
#include<iostream>
#include<unistd.h>
#include<bsd/string.h>
class Player
{
    private:
        int socketFD;
        bool sktAlive;
        bool started;
        bool validToon;
        bool freshSpawn;
        bool playerAlive;
    
    public:
        LURK_CHARACTER charTainer;
        std::string desc;
        std::shared_mutex pLock;

        Player(int);

        //state check
        bool isSktAlive();
        bool isStarted();
        bool isValidToon();
        bool isFreshSpawn();
        bool isPlayerAlive();

        //state set
        void startPlayer();
        void quitPlayer();
        void setValid();
        void giveRoom(uint16_t);
        void respawn();
        void despawn();

        //getter
        int getFD();
        uint16_t getRoomNumber();

        //writer
        void write_msg(LURK_MSG, std::string);
        void write_error(LURK_ERROR, std::string);
        void write_accept(uint8_t);
        void write_room(LURK_ROOM,std::string);
        void write_character(LURK_CHARACTER, std::string);
        void write_game(LURK_GAME, std::string);
        void write_connection(LURK_ROOM,std::string);
        void write_version(LURK_VERSION);
        void write_reflect();
};

#endif // PLAYER_H
