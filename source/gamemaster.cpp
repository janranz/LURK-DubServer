#include"../headers/gamemaster.h"


Gamemaster::Gamemaster()
{
    gmInfo.INITIAL_POINTS = serverStats::INIT_POINTS;
    gmInfo.STAT_LIMIT = serverStats::MAX_STAT;
    gmInfo.DESC_LENGTH = serverStats::GAME_GREETING.length();

    
}
