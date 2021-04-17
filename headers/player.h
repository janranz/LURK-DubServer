#ifndef PLAYER_H
#define PLAYER_H

#include"../headers/structs.h"
#include"../headers/fmt/format.h"
#include<mutex>
#include<iostream>

class Player
{
    private:
        int socketFD;
        bool sktAlive;
        bool started;
        bool validToon;
        bool freshSpawn;
    
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
};

#endif // PLAYER_H
