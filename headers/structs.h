#ifndef STRUCTS_H
#define STRUCTS_H
#include<vector>
#include<string>
#include<cstdint>
#include<limits>
struct chatter_messages
{// dialogue storage
    std::vector<std::string> awaken,baddies,danger,death,error,fightHit,food,health,
                             loot,pvp,roomNames,safeEv,trans,weapons,baddie_desc,room_desc,
                             adj,noun,roomTainer;
};

struct serverStats
{
    static uint16_t MAX_STAT;
    static int16_t BASE_HEALTH;
    static uint16_t MAX_BADDIES;
    static uint16_t MAX_ROOMS;
    static uint16_t MIN_BADDIES_PER_ROOM;
    static uint16_t MAX_BADDIES_PER_ROOM;
    static uint16_t BUFF_SIZE;
};

struct cmSizes
{
    uint16_t awaken_VSize;
    uint16_t baddies_VSize;
    uint16_t danger_VSize;
    uint16_t death_VSize;
    uint16_t error_VSize;
    uint16_t fightHit_VSize;
    uint16_t health_VSize;
    uint16_t loot_VSize;
    uint16_t pvp_VSize;
    uint16_t roomNames_VSize;
    uint16_t safeEv_VSize;
    uint16_t trans_VSize;
    uint16_t weapons_VSize;
    uint16_t baddie_desc_VSize;
    uint16_t room_desc_VSize;
    uint16_t adj_VSize;
    uint16_t noun_VSize;
    uint16_t roomTainer_VSize;
};

// LURK Tainers
struct LURK_MSG
{
    // uint8_t TYPE = 1;
    uint16_t MSG_LEN;
    char CEIVER_NAME[32];
    char SENDER_NAME[32];
    char *msg;
}__attribute__((packed));

struct GM_MSG
{
    // uint8_t TYPE = 1;
    uint16_t MSG_LEN;
    char CEIVER_NAME[32];
    char SENDER_NAME[32];
}__attribute__((packed));

struct LURK_CHANGEROOM
{
    // uint8_t TYPE = 2;
    uint16_t ROOM_NUMBER;
}__attribute__((packed));

struct LURK_PVP
{
    // uint8_t TYPE = 4;
    char TARGET[32];
}__attribute__((packed));

struct LURK_LOOT
{
    // uint8_t TYPE = 5;
    char TARGET[32];
}__attribute__((packed));

struct LURK_ERROR
{
    // uint8_t TYPE = 7;
    uint8_t CODE;
    uint16_t MSG_LEN;
}__attribute__((packed));

struct LURK_ACCEPT
{
    // uint8_t TYPE = 8;
    uint8_t ACCEPT_TYPE;
}__attribute__((packed));

struct LURK_ROOM
{
    // uint8_t TYPE = 9;
    uint16_t ROOM_NUMBER;
    char ROOM_NAME[32];
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_CHARACTER
{
    // uint8_t TYPE = 10;
    char CHARACTER_NAME[32];
    uint8_t FLAGS;
    uint16_t ATTACK;
    uint16_t DEFENSE;
    uint16_t REGEN;
    int16_t HEALTH; // stay under 32,760;
    uint16_t GOLD;
    uint16_t CURRENT_ROOM_NUMBER;
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_GAME
{
    // uint8_t TYPE = 11;
    uint16_t INITIAL_POINTS;
    uint16_t STAT_LIMIT;
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_CONNECTION
{
    // uint8_t TYPE = 13;
    uint16_t ROOM_NUMBER;
    char ROOM_NAME[32];
    uint16_t DESC_LENGTH;
    char DESC[];
}__attribute__((packed));

struct LURK_VERSION
{
    // uint8_t TYPE = 14;
    uint8_t MAJOR = 2;
    uint8_t MINOR = 2;
    uint16_t EXTENS_SIZE;
}__attribute__((packed));


#endif // STRUCTS_H

