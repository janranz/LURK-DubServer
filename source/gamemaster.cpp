#include"../headers/gamemaster.h"

unsigned int Gamemaster::g_seed = 0;
uint16_t Gamemaster::MAX_BADDIES = 1000;
uint16_t Gamemaster::AWAKEN_VECTOR_SIZE;
uint16_t Gamemaster::BADDIES_VECTOR_SIZE;
uint16_t Gamemaster::DANGER_VECTOR_SIZE;
uint16_t Gamemaster::DEATH_VECTOR_SIZE;
uint16_t Gamemaster::ERROR_VECTOR_SIZE;
uint16_t Gamemaster::FIGHTHIT_VECTOR_SIZE;
uint16_t Gamemaster::FOOD_VECTOR_SIZE;
uint16_t Gamemaster::HEALTH_VECTOR_SIZE;
uint16_t Gamemaster::LOOT_VECTOR_SIZE;
uint16_t Gamemaster::PVP_VECTOR_SIZE;
uint16_t Gamemaster::ROOMNAMES_VECTOR_SIZE;
uint16_t Gamemaster::SAFEEV_VECTOR_SIZE;
uint16_t Gamemaster::TRANS_VECTOR_SIZE;
uint16_t Gamemaster::WEAPONS_VECTOR_SIZE;

Gamemaster::Gamemaster()
{
    std::cout << "Hello Gamemaster." << std::endl;
    Gamemaster::g_seed = static_cast<unsigned int>(std::time(NULL));
    std::cout << "Gamemaster: your g_seed: " << g_seed << std::endl;
}

Gamemaster::~Gamemaster()
{
    std::cout << "Goodbye, Gamemaster." << std::endl;
}
int Gamemaster::fast_rand(void)
{
    Gamemaster::g_seed = (214013*g_seed+2531011);
    return(g_seed>>16)&0x7FFF;
}

bool Gamemaster::buildChatter(int i, std::vector<std::string>::iterator m)
{
    bool checksOut = true;
    switch(i)
    {
        case 0:{c_m.awaken.push_back(*m);c_m.awakenKEY = i;break;}
        case 1:{c_m.baddies.push_back(*m);c_m.baddiesKEY = i;break;}
        case 2:{c_m.danger.push_back(*m);c_m.dangerKEY = i;break;}
        case 3:{c_m.death.push_back(*m);c_m.deathKEY = i;break;}
        case 4:{c_m.error.push_back(*m);c_m.errorKEY = i;break;}
        case 5:{c_m.fightHit.push_back(*m);c_m.fightHitKEY = i;break;}
        case 6:{c_m.food.push_back(*m);c_m.foodKEY = i;break;}
        case 7:{c_m.health.push_back(*m);c_m.healthKEY = i;break;}
        case 8:{c_m.loot.push_back(*m);c_m.lootKEY = i;break;}
        case 9:{c_m.pvp.push_back(*m);c_m.pvpKEY = i;break;}
        case 10:{c_m.roomNames.push_back(*m);c_m.roomNamesKEY = i;break;}
        case 11:{c_m.safeEv.push_back(*m);c_m.safeEvKEY = i;break;}
        case 12:{c_m.trans.push_back(*m);c_m.transKEY = i;break;}
        case 13:{c_m.weapons.push_back(*m);c_m.weaponsKEY = i;break;}
        default:{checksOut = false;}
    }
    return checksOut;
}

void Gamemaster::estabSizes()
{
    Gamemaster::AWAKEN_VECTOR_SIZE = c_m.awaken.size();
    Gamemaster::BADDIES_VECTOR_SIZE = c_m.baddies.size();
    Gamemaster::DANGER_VECTOR_SIZE = c_m.danger.size();
    Gamemaster::DEATH_VECTOR_SIZE = c_m.death.size();
    Gamemaster::ERROR_VECTOR_SIZE = c_m.error.size();
    Gamemaster::FIGHTHIT_VECTOR_SIZE = c_m.fightHit.size();
    Gamemaster::FOOD_VECTOR_SIZE = c_m.food.size();
    Gamemaster::HEALTH_VECTOR_SIZE = c_m.health.size();
    Gamemaster::LOOT_VECTOR_SIZE = c_m.loot.size();
    Gamemaster::PVP_VECTOR_SIZE = c_m.pvp.size();
    Gamemaster::ROOMNAMES_VECTOR_SIZE = c_m.roomNames.size();
    Gamemaster::SAFEEV_VECTOR_SIZE = c_m.safeEv.size();
    Gamemaster::TRANS_VECTOR_SIZE = c_m.trans.size();
    Gamemaster::WEAPONS_VECTOR_SIZE = c_m.weapons.size();

    std::cout << Gamemaster::FOOD_VECTOR_SIZE << std::endl;
}

void Gamemaster::populateSpawner()
{
    uint8_t flags = 248;
    uint16_t attack, defense, regen, gold;
    int16_t health;
    for(uint16_t i; i < Gamemaster::MAX_BADDIES; i++)
    {
        int statRoll = i % 5;
        int dex = fast_rand() % (Gamemaster::BADDIES_VECTOR_SIZE);
        std::string n = c_m.baddies.at(dex);

        
        // switch(statRoll)
        // {
        //     case 0:
        //     {

        //     }
        // }
        
    }
}
