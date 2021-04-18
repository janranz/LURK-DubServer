#include"../headers/gamemaster.h"


Gamemaster::Gamemaster()
{
    gmInfo.INITIAL_POINTS = serverStats::PLAYER_INIT_POINTS;
    gmInfo.STAT_LIMIT = serverStats::PLAYER_MAX_STAT;
    gmInfo.DESC_LENGTH = serverStats::GAME_GREETING.length();


}

// initializers
void Gamemaster::build_chatter(int i,std::vector<std::string>::iterator m)
{
    switch(i)
    {
        case 0:{c_m.awaken.emplace_back(*m);break;}
        case 1:{c_m.baddies.emplace_back(*m);break;}
        case 2:{c_m.danger.emplace_back(*m);break;}
        case 3:{c_m.death.emplace_back(*m);break;}
        case 4:{c_m.error.emplace_back(*m);break;}
        case 5:{c_m.fightHit.emplace_back(*m);break;}
        case 6:{c_m.food.emplace_back(*m);break;}
        case 7:{c_m.health.emplace_back(*m);break;}
        case 8:{c_m.loot.emplace_back(*m);break;}
        case 9:{c_m.pvp.emplace_back(*m);break;}
        // case 10:{c_m.roomNames.emplace_back(*m);break;}
        case 10:{c_m.safeEv.emplace_back(*m);break;}
        case 11:{c_m.trans.emplace_back(*m);break;}
        case 12:{c_m.weapons.emplace_back(*m);break;}
        case 13:{c_m.baddie_desc.emplace_back(*m);break;}
        case 14:{c_m.room_desc.emplace_back(*m);break;}
        case 15:{c_m.adj.emplace_back(*m);break;}
        case 16:{c_m.noun.emplace_back(*m);break;}
        case 17:{c_m.roomType.emplace_back(*m);break;}
        default:{std::cout << fmt::format("Failure in buildChatter!\n");}
    }
}
void Gamemaster::size_vectors()
{
    vsize.awaken_VSize      = c_m.awaken.size();
    vsize.baddies_VSize     = c_m.baddies.size();
    vsize.danger_VSize      = c_m.danger.size();
    vsize.death_VSize       = c_m.death.size();
    vsize.error_VSize       = c_m.error.size();
    vsize.fightHit_VSize    = c_m.fightHit.size();
    vsize.food_VSize        = c_m.food.size();
    vsize.health_VSize      = c_m.health.size();
    vsize.loot_VSize        = c_m.loot.size();
    vsize.pvp_VSize         = c_m.pvp.size();
    vsize.safeEv_VSize      = c_m.safeEv.size();
    vsize.trans_VSize       = c_m.trans.size();
    vsize.weapons_VSize     = c_m.weapons.size();
    vsize.baddie_desc_VSize = c_m.baddie_desc.size();
    vsize.room_desc_VSize   = c_m.room_desc.size();
    vsize.adj_VSize         = c_m.adj.size();
    vsize.noun_VSize        = c_m.noun.size();
    vsize.roomType_VSize    = c_m.roomType.size();

    /*debug
    std::cout << "Awaken: "     << vsize.awaken_VSize      << std::endl
              << "Baddie: "     << vsize.baddies_VSize     << std::endl
              << "Danger: "     << vsize.danger_VSize      << std::endl
              << "death: "      << vsize.death_VSize       << std::endl
              << "error: "      << vsize.error_VSize       << std::endl
              << "fightHit: "   << vsize.fightHit_VSize    << std::endl
              << "food: "       << vsize.food_VSize        << std::endl
              << "health: "     << vsize.health_VSize      << std::endl
              << "loot: "       << vsize.loot_VSize        << std::endl
              << "pvp: "        << vsize.pvp_VSize         << std::endl
              << "safe: "       << vsize.safeEv_VSize      << std::endl
              << "trans: "      << vsize.trans_VSize       << std::endl
              << "weapons: "    << vsize.weapons_VSize     << std::endl
              << "baddieDesc: " << vsize.baddie_desc_VSize << std::endl
              << "roomDesc: "   << vsize.room_desc_VSize   << std::endl
              << "adj: "        << vsize.adj_VSize         << std::endl
              << "noun: "       << vsize.noun_VSize        << std::endl
              << "roomType: "   << vsize.roomType_VSize    << std::endl;
    */
}
void Gamemaster::craft_room_names()
{
    std::string finalName;
    int dex1,dex2,dex3;
    for(int i = 0; i < serverStats::MAX_ROOMS; i++)
    {
        do{
            dex1 = (fast_rand() % vsize.adj_VSize);
            dex2 = (fast_rand() % vsize.noun_VSize);
            dex3 = (fast_rand() % vsize.roomType_VSize);

            finalName = c_m.adj.at(dex1) + " "
                      + c_m.noun.at(dex2) + " "
                      + c_m.roomType.at(dex3);
        }while(finalName.length() > 30);

        c_m.roomNames.emplace_back(finalName);
    }
}
void Gamemaster::build_rooms()
{
    uint16_t roomNumber;
    std::string roomName,roomDesc;
    //build Portal Room
    roomName = "The Portal Room";
    roomDesc = c_m.room_desc.at(0);
    roomNumber = 0;
    master_room_list.emplace_back(std::make_shared<Room>(roomName,roomDesc,roomNumber));
    for(int i = 1; i < serverStats::MAX_ROOMS; i++)
    {
        roomName = c_m.roomNames.at(i);
        roomDesc = c_m.room_desc.at(i);
        roomNumber = i;
        master_room_list.emplace_back(std::make_shared<Room>(roomName,roomDesc,roomNumber));
    }
    // build connections
    for(auto t = master_room_list.begin(); t != master_room_list.end(); ++t)
    {
        if(t == master_room_list.begin())
        {// portal room
            for(auto b = master_room_list.begin() + 1; b != master_room_list.end(); ++b)
            {
                (*t).get()->emplace_connection(*b);
            }
        }else if( t+1 != master_room_list.end())
        {
            (*t).get()->emplace_connection(*(t-1));
            (*t).get()->emplace_connection(*(t+1));
        }else{
            (*t).get()->emplace_connection(*(t-1));
        }
    }
}
std::shared_ptr<Baddie> Gamemaster::build_a_baddie(uint16_t roomNumber)
{
    auto b = std::make_shared<Baddie>();
    std::string name;
    int dex;
    do{
        dex = (fast_rand() % c_m.baddies.size());
        name = c_m.baddies.at(dex);
    }while(name.length() > 30);
    strncpy(b.get()->bTainer.CHARACTER_NAME,name.c_str(),32);

    dex = (fast_rand() % c_m.baddie_desc.size());
    b.get()->desc = c_m.baddie_desc.at(dex);

    b.get()->bTainer.FLAGS = serverStats::BADDIE_AFLAGS;
    b.get()->bTainer.CURRENT_ROOM_NUMBER = roomNumber;
    b.get()->bTainer.DESC_LENGTH = b.get()->desc.length();

    b.get()->bTainer.GOLD = (fast_rand() %
        (serverStats::BADDIE_MAX_GOLD - serverStats::BADDIE_MIN_GOLD)
        + serverStats::BADDIE_MIN_GOLD);

    b.get()->bTainer.HEALTH = (fast_rand() %
        (serverStats::BADDIE_MAX_HEALTH - serverStats::BADDIE_MIN_HEALTH)
        + serverStats::BADDIE_MIN_HEALTH);
    
    // stat roll
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
    b.get()->bTainer.ATTACK = attack;
    b.get()->bTainer.DEFENSE = defense;
    b.get()->bTainer.REGEN = regen;
    // std::cout << fmt::format("Monster Name: {0}\nAttack: {1}\nDefense: {2}\nRegen: {3}\n"
    //     "Health: {4}\nGold: {5}\n", b.get()->bTainer.CHARACTER_NAME,
    //     std::to_string(b.get()->bTainer.ATTACK),std::to_string(b.get()->bTainer.DEFENSE),
    //     std::to_string(b.get()->bTainer.REGEN),std::to_string(b.get()->bTainer.HEALTH),
    //     std::to_string(b.get()->bTainer.GOLD));
    return b;
}

void Gamemaster::populate_rooms()
{
    for(auto t = master_room_list.begin(); t != master_room_list.end(); ++t)
    {
        int roomNumber = (*t).get()->roomTainer.ROOM_NUMBER;
        int baddie_count = (fast_rand() %
            (serverStats::MAX_BADDIES_PER_ROOM - serverStats::MIN_BADDIES_PER_ROOM)
            + serverStats::MIN_BADDIES_PER_ROOM);
        
        for(int i = 0; i < baddie_count; i++)
            (*t).get()->emplace_baddie(build_a_baddie(roomNumber));
    // std::cout << fmt::format("{0} has {1} baddies.\n",
    //     (*t).get()->roomTainer.ROOM_NAME, std::to_string((*t).get()->baddie_list_size()));
    }
    
    
}


//events
int Gamemaster::fast_rand()
{
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}
