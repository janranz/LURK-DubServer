#ifndef PLAYER_H
#define PLAYER_H
#include"../headers/structs.h"
// #include"../headers/player.h"
#include<memory>
#include<mutex>
#include<iostream>
// #include<sys/socket.h>
#include<unistd.h>
class Player
{
    private:
    int socketFD;
    bool sktAlive;
    bool started;
    bool validToon;
    // bool inMaster;
    bool freshSpawn;
    // SET INITIAL HEALTH STATS ETC
    public:
    LURK_CHARACTER charTainer;
    std::string desc;
    // std::shared_ptr<std::mutex> pLock;
    std::mutex pLock;
    
    
    
    Player(int);
    ~Player();
    
    //states
    bool isInMaster();
    bool isSktAlive();
    bool isStarted();
    bool isValidToon();
    bool isFreshSpawn();

    void startPlayer();
    void quitPlayer();
    // void setMaster();
    void setValid();
    void spawn();
    void despawn();

    int getFD();
    //read
    //write
    void writeToMe(LURK_MSG,char*);
    void reflection();
};


// class Player: public Baddie
// {
//     private:
//     int socketFD;
    
    
//     public:
//     LURK_CHARACTER charTainer;
//     std::string desc;
    

//     Player(int);
//     ~Player();
//     using Baddie::Baddie;
//     //read
//     //send
// };

#endif //PLAYER_H
