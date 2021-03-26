#ifndef PLAYER_H
#define PLAYER_H
#include"../headers/baddie.h"
#include"../headers/structs.h"

class Player: public Baddie
{
    private:
    int socketFD;
    
    
    public:
    LURK_CHARACTER charTainer;
    std::string desc;

    Player(int);
    ~Player();
    using Baddie::Baddie;
    void controller();
    //read
    void listenPlayer();
    //send
    void chatterPlayer(char);
};

#endif //PLAYER_H
