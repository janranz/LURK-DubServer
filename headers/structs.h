#ifndef STRUCTS_H
#define STRUCTS_H
#define M_ToCP(p) reinterpret_cast<char*>(p)
#include<vector>
#include<string>
#include<cstdint>
#include<limits>
#include<mutex>
#include<array>

struct chatter_messages
{// dialogue storage
    std::vector<std::string> awaken,baddies,danger,death,error,fightHit,food,health,
                             loot,pvp,roomNames,safeEv,trans,weapons,baddie_desc,room_desc,
                             adj,noun,roomType;
};

//global mutex
extern std::mutex printLock;
extern std::mutex randLock;
struct serverStats
{
    static const  uint8_t GAME_VERSION_MAJOR;
    static const  uint8_t GAME_VERSION_MINOR;
    static const  uint8_t GAME_VERSION_EXT;
    static const  std::string GAME_GREETING;
    static const  std::string GM_NAME;
    
    static const  uint16_t PLAYER_MAX_STAT;
    static const  int16_t PLAYER_S_MAX_STAT;
    static const  uint16_t PLAYER_INIT_POINTS;
    static const  uint8_t PLAYER_AFLAGS;
    static const  uint8_t PLAYER_DFLAGS;
    static const  uint16_t PLAYER_MIN_GOLD;
    static const  uint16_t PLAYER_MAX_GOLD;
    static const  uint16_t CRUTCH_BUMP;
    static const  int16_t PLAYER_BASE_HEALTH;
    static const  uint8_t BASE_FLAGS;

    static const  uint8_t BADDIE_AFLAGS;
    static const  uint8_t BADDIE_DFLAGS;
    static const  int16_t BADDIE_MIN_HEALTH;
    static const  int16_t BADDIE_MAX_HEALTH;
    static const  uint16_t BADDIE_MIN_GOLD;
    static const  uint16_t BADDIE_MAX_GOLD;
    static const  uint16_t BADDIE_MIN_STAT;
    static const  uint16_t BADDIE_MAX_STAT;
    static const  uint16_t MAX_BADDIES;
    static const  uint16_t MAX_ROOMS;
    static const  uint16_t MIN_BADDIES_PER_ROOM;
    static const  uint16_t MAX_BADDIES_PER_ROOM;
    static const  uint16_t BUFF_SIZE;
    
    
};

struct LURK_TYPES
{
    static const  uint8_t TYPE_MSG;
    static const  uint8_t TYPE_CHANGEROOM;
    static const  uint8_t TYPE_FIGHT;
    static const  uint8_t TYPE_PVP;
    static const  uint8_t TYPE_LOOT;
    static const  uint8_t TYPE_START;
    static const  uint8_t TYPE_ERROR;
    static const  uint8_t TYPE_ACCEPT;
    static const  uint8_t TYPE_ROOM;
    static const  uint8_t TYPE_CHARACTER;
    static const  uint8_t TYPE_GAME;
    static const  uint8_t TYPE_LEAVE;
    static const  uint8_t TYPE_CONNECTION;
    static const  uint8_t TYPE_VERSION;
};

struct cm_sizes
{
    uint16_t awaken_VSize;
    uint16_t baddies_VSize;
    uint16_t danger_VSize;
    uint16_t death_VSize;
    uint16_t error_VSize;
    uint16_t fightHit_VSize;
    uint16_t food_VSize;
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
    uint16_t roomType_VSize;
};

// LURK Tainers
struct LURK_MSG
{
    uint16_t MSG_LEN;
    unsigned char CEIVER_NAME[32];
    unsigned char SENDER_NAME[32];
}__attribute__((packed));

struct LURK_CHANGEROOM
{
    uint16_t ROOM_NUMBER;
}__attribute__((packed));

struct LURK_PVP
{
    unsigned char TARGET[32];
}__attribute__((packed));

struct LURK_LOOT
{
    unsigned char TARGET[32];
}__attribute__((packed));

struct LURK_ERROR
{
    uint8_t CODE;
    uint16_t MSG_LEN;
}__attribute__((packed));

struct LURK_ACCEPT
{
    uint8_t ACCEPT_TYPE;
}__attribute__((packed));

struct LURK_ROOM
{
    uint16_t ROOM_NUMBER;
    unsigned char ROOM_NAME[32];
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_CHARACTER
{
    unsigned char CHARACTER_NAME[32];
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
    uint16_t INITIAL_POINTS;
    uint16_t STAT_LIMIT;
    uint16_t DESC_LENGTH;
}__attribute__((packed));

struct LURK_VERSION
{
    uint8_t MAJOR = 2;
    uint8_t MINOR = 2;
    uint16_t EXTENS_SIZE;
}__attribute__((packed));



#endif // STRUCTS_H

