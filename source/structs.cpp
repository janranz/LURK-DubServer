#include"../headers/structs.h"

uint16_t serverStats::MAX_STAT = 6000;
uint8_t serverStats::BASE_FLAGS =   0b11000000;
uint8_t serverStats::START_FLAGS =  0b11011000; // force join battle only for now
uint8_t serverStats::BADDIE_FLAGS = 0b11111000;
int16_t serverStats::BASE_HEALTH = 12000;
uint16_t serverStats::MAX_BADDIES = 1000;
uint16_t serverStats::MAX_ROOMS = 10;
uint16_t serverStats::MIN_BADDIES_PER_ROOM = 2;
uint16_t serverStats::MAX_BADDIES_PER_ROOM = 5;
uint16_t serverStats::BUFF_SIZE = std::numeric_limits<unsigned short>::max();

