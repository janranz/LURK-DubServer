#ifndef STRUCTS_H
#define STRUCTS_H
#include<string>
#include<cstring>
#include<vector>
#include<cstdint>

struct chatter_messages
{
    std::vector<std::string> awaken,baddies,danger,death,error,fightHit,food,health,
                             loot,pvp,roomNames,safeEv,trans,weapons,baddie_desc,room_desc,
                             adj,noun,roomTainer;
    // char awakenKEY,baddiesKEY,dangerKEY,deathKEY,errorKEY,fightHitKEY,foodKEY,healthKEY,
    //                          lootKEY,pvpKEY,roomNamesKEY,safeEvKEY,transKEY,weaponsKEY;
};

// LURK Tainers
struct LURK_MSG
{
    uint8_t TYPE = 1;
    uint16_t MSG_LEN;
    char CEIVER_NAME[32];
    char SENDER_NAME[32];
}__attribute__((packed));

struct GM_MSG
{
    uint8_t TYPE = 1;
    uint16_t MSG_LEN;
    char CEIVER_NAME[32];
    char SENDER_NAME[32] = "<MASTER DUBS>";
}__attribute__((packed));

struct LURK_CHANGEROOM
{
    uint8_t TYPE = 2;
    uint16_t ROOM_NUMBER;
}__attribute__((packed));

struct LURK_PVP
{
    uint8_t TYPE = 4;
    char TARGET[32];
}__attribute__((packed));

struct LURK_LOOT
{
    uint8_t TYPE = 5;
    char TARGET[32];
}__attribute__((packed));

struct LURK_ERROR
{
    uint8_t TYPE = 7;
    uint8_t CODE;
    uint16_t MSG_LEN;
}__attribute__((packed));

struct LURK_ACCEPT
{
    uint8_t TYPE = 8;
    uint8_t ACCEPT_TYPE;
}__attribute__((packed));

struct LURK_ROOM
{
    uint8_t TYPE = 9;
    uint16_t ROOM_NUMBER;
    char ROOM_NAME[32];
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_CHARACTER
{
    uint8_t TYPE = 10;
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
    uint8_t TYPE = 11;
    uint16_t INITIAL_POINTS = 4000;
    uint16_t STAT_LIMIT = 65535;
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_CONNECTION
{
    uint8_t TYPE = 13;
    uint16_t ROOM_NUMBER;
    char ROOM_NAME[32];
    uint16_t DESC_LENGTH;
    char DESC[1024];
}__attribute__((packed));

struct LURK_VERSION
{
    uint8_t TYPE = 14;
    uint8_t MAJOR = 2;
    uint8_t MINOR = 2;
    uint16_t EXTENS_SIZE = 0;
}__attribute__((packed));

#endif //STRUCTS_H
