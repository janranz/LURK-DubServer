#include"../headers/baddie.h"

bool Baddie::hurt_baddie(int32_t h)
{
    std::lock_guard<std::shared_mutex>lock(bLock);
    if((bTainer.HEALTH - h) > 0)
    {
        bTainer.HEALTH -= h;
    }else{
        bTainer.HEALTH = 0;
        bTainer.FLAGS = serverStats::BADDIE_DFLAGS;
        alive = false;
    }
    return alive;
}

void Baddie::heal_baddie(int32_t h)
{
    std::lock_guard<std::shared_mutex>lock(bLock);
    if((bTainer.HEALTH + h) > serverStats::BADDIE_MAX_HEALTH)
    {
        bTainer.HEALTH = serverStats::BADDIE_MAX_HEALTH;
    }else{
        bTainer.HEALTH += h;
    }
}

uint16_t Baddie::getCrit()
{
    std::shared_lock<std::shared_mutex>lock(bLock);
    return critDamage;
}

bool Baddie::is_alive()
{
    std::shared_lock<std::shared_mutex>lock(bLock);
    return alive;
}

uint16_t Baddie::loot_me()
{
    uint16_t drop;
    std::lock_guard<std::shared_mutex>lock(bLock);
    if(bTainer.GOLD != 0)
    {
        drop = (fast_rand() % (bTainer.GOLD + 1) + 1);
        bTainer.GOLD -= drop;
    }else{
        drop = 0;
    }
    return drop;
}

void Baddie::respawn()
{
    std::lock_guard<std::shared_mutex>lock(bLock);
    alive = true;
    bTainer.FLAGS = serverStats::BADDIE_AFLAGS;
    bTainer.GOLD = (fast_rand() %
        ((serverStats::BADDIE_MAX_GOLD + 1) - serverStats::BADDIE_MIN_GOLD)
        + serverStats::BADDIE_MIN_GOLD);

    bTainer.HEALTH = (fast_rand() %
        ((serverStats::BADDIE_MAX_HEALTH + 1)- serverStats::BADDIE_MIN_HEALTH)
        + serverStats::BADDIE_MIN_HEALTH);
    
    bTainer.ATTACK = (fast_rand() % ((serverStats::BADDIE_MAX_STAT + 1) - serverStats::BADDIE_MIN_STAT) + serverStats::BADDIE_MIN_STAT);
    bTainer.DEFENSE = (fast_rand() % ((serverStats::BADDIE_MAX_STAT + 1) - serverStats::BADDIE_MIN_STAT) + serverStats::BADDIE_MIN_STAT);
    bTainer.REGEN = (fast_rand() % ((serverStats::BADDIE_MAX_STAT + 1) - serverStats::BADDIE_MIN_STAT) + serverStats::BADDIE_MIN_STAT);
    
    if((bTainer.ATTACK * 3) >= serverStats::BADDIE_MAX_STAT)
    {// YIKES
        critDamage = 1;
    }else{
        critDamage = bTainer.ATTACK * 3;
    }
}
