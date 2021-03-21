#pragma once

#include<string>
#include<cstdint>
#include<iostream>
#include<cstdint>
#include<string>
#include<vector>
#include<mutex>

class Baddie
{
    private:
    Baddie* selfRef;

    protected:
        std::string name;
        uint8_t flags;
        uint16_t attack;
        uint16_t defense;
        uint16_t regen;
        int16_t health;
        uint16_t gold;
        uint16_t roomNumber;
        uint16_t descriptionLength;
        std::string description;
        std::mutex padlock;

    public:
        Baddie(std::string,uint8_t,uint16_t,uint16_t,uint16_t,int16_t,
                uint16_t,uint16_t,uint16_t,std::string);
        ~Baddie();
        void lock();
        void unlock();
        std::string getName();
        uint8_t getFlags();
        uint16_t getAttack();
        uint16_t getDefense();
        uint16_t getRegen();
        int16_t getHealth();
        uint16_t getGold();
        uint16_t getRoomNumber();
        uint16_t getDescriptionLength();
        char* getDescription();

        void setFlags(uint8_t);
        void setHealth(int16_t);
};
