#ifndef PLAYER_H
#define PLAYER_H
#include"../headers/structs.h"
#include<memory>
#include<mutex>

class Player
{
    private:
    int socketFD;
    
    
    public:
    LURK_CHARACTER charTainer;
    std::string desc;
    std::shared_ptr<std::mutex> pLock;

    Player(int);
    ~Player();
    //read
    //write
    
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
