#include"../headers/structs.h"

uint16_t serverStats::MAX_STAT      = 6000;
uint8_t serverStats::BASE_FLAGS     = 0b11000000;
uint8_t serverStats::START_FLAGS    = 0b11011000; // force join battle only for now
uint8_t serverStats::BADDIE_FLAGS   = 0b11111000;
int16_t serverStats::BASE_HEALTH    = 12000;
uint16_t serverStats::MAX_BADDIES   = 1000;
uint16_t serverStats::MAX_ROOMS     = 10;
uint16_t serverStats::MIN_BADDIES_PER_ROOM = 2;
uint16_t serverStats::MAX_BADDIES_PER_ROOM = 5;
uint16_t serverStats::BUFF_SIZE = std::numeric_limits<unsigned short>::max();

uint8_t LURK_TYPES::TYPE_MSG        = 1;
uint8_t LURK_TYPES::TYPE_CHANGEROOM = 2;
uint8_t LURK_TYPES::TYPE_FIGHT      = 3;
uint8_t LURK_TYPES::TYPE_PVP        = 4;
uint8_t LURK_TYPES::TYPE_LOOT       = 5;
uint8_t LURK_TYPES::TYPE_START      = 6;
uint8_t LURK_TYPES::TYPE_ERROR      = 7;
uint8_t LURK_TYPES::TYPE_ACCEPT     = 8;
uint8_t LURK_TYPES::TYPE_ROOM       = 9;
uint8_t LURK_TYPES::TYPE_CHARACTER  = 10;
uint8_t LURK_TYPES::TYPE_GAME       = 11;
uint8_t LURK_TYPES::TYPE_LEAVE      = 12;
uint8_t LURK_TYPES::TYPE_CONNECTION = 13;
uint8_t LURK_TYPES::TYPE_VERSION    = 14;
