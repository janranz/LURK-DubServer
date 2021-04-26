#include"../headers/baddie.h"

void Baddie::hurt_baddie(int16_t h)
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
}

void Baddie::heal_baddie(int16_t h)
{
    std::lock_guard<std::shared_mutex>lock(bLock);
    if((bTainer.HEALTH + h) > serverStats::BADDIE_MAX_HEALTH)
    {
        bTainer.HEALTH = serverStats::BADDIE_MAX_HEALTH;
    }else{
        bTainer.HEALTH += h;
    }
}

bool Baddie::is_alive()
{
    std::shared_lock<std::shared_mutex>lock(bLock);
    return alive;
}

void Baddie::respawn()
{
    std::lock_guard<std::shared_mutex>lock(bLock);
    alive = true;
    bTainer.FLAGS = serverStats::BADDIE_AFLAGS;
    bTainer.GOLD = (fast_rand() %
        (serverStats::BADDIE_MAX_GOLD - serverStats::BADDIE_MIN_GOLD)
        + serverStats::BADDIE_MIN_GOLD);

    bTainer.HEALTH = (fast_rand() %
        (serverStats::BADDIE_MAX_HEALTH - serverStats::BADDIE_MIN_HEALTH)
        + serverStats::BADDIE_MIN_HEALTH);
    
    int remaining = serverStats::PLAYER_INIT_POINTS;
    uint16_t attack = 0;
    uint16_t defense = 0;
    uint16_t regen = 0;
    int roll;
    int i = 0;
    while(remaining != 0)
    {
        switch(i % 3)
        {
            case 0:
            {
                roll = (fast_rand() % (remaining) + 1);
                attack += roll;
                remaining -= roll;
                break;                
            }
            case 1:
            {
                roll = (fast_rand() % (remaining) + 1);
                defense += roll;
                remaining -= roll;
                break;
            }
            case 2:
            {
                roll = (fast_rand() % (remaining) + 1);
                regen += roll;
                remaining -= roll;
                break;
            }
        }
        i++;
    }
    bTainer.ATTACK = attack;
    bTainer.DEFENSE = defense;
    bTainer.REGEN = regen;
}
