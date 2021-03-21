#pragma once

#include"baddie.h"

class Player: public Baddie
{
    private:
    // Player* selfRef; this
    int socketFD;

    
    public:
    using Baddie::Baddie;
    void setSocketFD(int);
    void setGold(uint16_t);

    void controller();
    //read
    //send
};
