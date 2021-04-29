#ifndef BADDIE_H
#define BADDIE_H

#include"../headers/structs.h"
#include"../headers/splitter.h" // fast_rand
#include"../headers/fmt/format.h"
#include<shared_mutex>
#include<iostream>
#include<unistd.h>

class Baddie
{
    private:

        std::shared_mutex bLock;
        bool alive;
        uint16_t critDamage;

    public:
        LURK_CHARACTER bTainer;
        std::string desc;
        bool hurt_baddie(int16_t);
        void heal_baddie(int16_t);
        uint16_t getCrit();
        bool is_alive();
        void respawn();
    

};

#endif // BADDIE_H
