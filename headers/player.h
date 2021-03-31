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
    
    
    
    public:
    LURK_CHARACTER charTainer;
    std::string desc;
    std::shared_ptr<std::mutex> pLock;
    int socketFD;
    Player(int);
    ~Player();
    //read
    //write
    void writeToMe(LURK_MSG,char*);
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
