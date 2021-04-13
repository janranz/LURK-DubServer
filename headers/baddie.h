#ifndef BADDIE_H
#define BADDIE_H

#include<string>
#include<cstdint>
#include<iostream>
#include<cstdint>
#include<string>
#include<vector>
#include<mutex>
#include"structs.h"

/*TODO:
    Consider if we should update players in a room w/ new char msg.
    //LEFT OFF HERE RETURN HERE << find this
 */

class Baddie
{
    // private:
    // Baddie* selfRef; // replace with this.

    // protected:

        // std::string name;
        // uint8_t flags;
        // uint16_t attack;
        // uint16_t defense;
        // uint16_t regen;
        // int16_t health;
        // uint16_t gold;
        // uint16_t roomNumber;
        // uint16_t descriptionLength;
        
        // std::mutex padlock;

    public:
        // Baddie();
        LURK_CHARACTER bTainer;
        std::string description;
        Baddie(std::string,uint8_t,uint16_t,uint16_t,uint16_t,int16_t,
                uint16_t,uint16_t,uint16_t,std::string);
        ~Baddie();
        // void lock();
        // void unlock();
};

#endif // BADDIE_H
