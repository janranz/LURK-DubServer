#include"../headers/gamemaster.h"


Gamemaster::Gamemaster()
{
    gmInfo.INITIAL_POINTS = serverStats::INIT_POINTS;
    gmInfo.STAT_LIMIT = serverStats::MAX_STAT;
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
//events
int Gamemaster::fast_rand()
{
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}
