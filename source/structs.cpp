#include"../headers/structs.h"

std::string serverStats::GAME_GREETING = "Welcome to the portal room.."
    " the source of all great things!\n"
    "Go wherever your heart desires, or start off from the beginning.\n";

std::string serverStats::GM_NAME = "| Gamemaster Dubs (Server) |";
uint16_t serverStats::PLAYER_INIT_POINTS = 6000;
uint16_t serverStats::PLAYER_MAX_STAT    = std::numeric_limits<unsigned short>::max();
int16_t serverStats::PLAYER_S_MAX_STAT   = std::numeric_limits<short>::max();
uint8_t serverStats::BASE_FLAGS          = 0b11000000;
uint8_t serverStats::PLAYER_AFLAGS       = 0b11011000; // force join battle only for now
uint8_t serverStats::PLAYER_DFLAGS       = 0b01011000; // force join battle only for now
int16_t serverStats::PLAYER_BASE_HEALTH  = 12000;
uint16_t serverStats::PLAYER_MIN_GOLD    = 200;
uint16_t serverStats::PLAYER_MAX_GOLD    = 4000;

uint8_t serverStats::BADDIE_AFLAGS         = 0b11111000;
uint8_t serverStats::BADDIE_DFLAGS         = 0b01111000;
int16_t serverStats::BADDIE_MIN_HEALTH     = 100;
int16_t serverStats::BADDIE_MAX_HEALTH     = 24000;
uint16_t serverStats::BADDIE_MIN_GOLD      = 120;
uint16_t serverStats::BADDIE_MAX_GOLD      = 4000;
uint16_t serverStats::BADDIE_MIN_STAT      = 400;
uint16_t serverStats::BADDIE_MAX_STAT      = 8000;
uint16_t serverStats::MAX_BADDIES          = 1000;
uint16_t serverStats::MIN_BADDIES_PER_ROOM = 2;
uint16_t serverStats::MAX_BADDIES_PER_ROOM = 5;

uint16_t serverStats::MAX_ROOMS = 10;
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
