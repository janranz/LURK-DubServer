#include"../headers/baddie.h"

// uint16_t Baddie::MAX_STAT = 4000;
// Baddie::Baddie(){}
Baddie::Baddie(std::string n,uint8_t f,uint16_t a,uint16_t d,uint16_t r,int16_t h,
                uint16_t g,uint16_t rm,uint16_t l,std::string desc)
{
    // name = n;
    // flags = f;
    // attack = a;
    // defense = d;
    // regen = r;
    // health = h;
    // gold = g;
    // roomNumber = rm;
    // descriptionLength = l;
    strncpy(bTainer.CHARACTER_NAME,n.c_str(),32);
    bTainer.FLAGS = f;
    bTainer.ATTACK = a;
    bTainer.DEFENSE = d;
    bTainer.REGEN = r;
    bTainer.HEALTH = h;
    bTainer.GOLD = g;
    bTainer.CURRENT_ROOM_NUMBER = rm;
    bTainer.DESC_LENGTH = l;
    description = desc;

}

Baddie::~Baddie()
{
    // std::cout << "baddie was casted into the lake of fire." << std::endl;
}

