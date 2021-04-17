#ifndef PLAYER_H
#define PLAYER_H

#include"../headers/structs.h"
#include"../headers/fmt/format.h"
#include<mutex>
#include<iostream>
#include<unistd.h>

class Player
{
    private:
        int socketFD;
        bool sktAlive;
        bool started;
        bool validToon;
        bool freshSpawn;
        ssize_t bytes;
    
    public:
        LURK_CHARACTER charTainer;
        std::string desc;
        std::mutex pLock;

        Player(int);

        //state check
        bool isSktAlive();
        bool isStarted();
        bool isValidToon();
        bool isFreshSpawn();

        //state set
        void startPlayer();
        void quitPlayer();
        void setValid();
        void respawn();
        void despawn();

        //getter
        int getFD();

        //writer
        void write_msg(LURK_MSG, char*);
        void write_error();
        void write_accept();
        void write_room();
        void write_character();
        void write_game();
        void write_leave();
        void write_connection();
        void write_version();
};

#endif // PLAYER_H
