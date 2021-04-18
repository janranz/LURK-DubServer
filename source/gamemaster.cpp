#include"../headers/gamemaster.h"


Gamemaster::Gamemaster()
{
    gmInfo.INITIAL_POINTS = serverStats::PLAYER_INIT_POINTS;
    gmInfo.STAT_LIMIT = serverStats::PLAYER_MAX_STAT;
    gmInfo.DESC_LENGTH = serverStats::GAME_GREETING.length();
    strncpy(gmpm.SENDER_NAME,serverStats::GM_NAME.c_str(),32);


}

//events
int Gamemaster::fast_rand()
{
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
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
//cleanup
void Gamemaster::ragequit(std::shared_ptr<Player> p)
{
    std::cout << fmt::format("{} has ragequit.\n",p.get()->charTainer.CHARACTER_NAME);
}
//network
void Gamemaster::GMController(int fd)
{
    auto p = std::make_shared<Player>(fd);
    p.get()->write_version(vers);
    p.get()->write_game(gmInfo,serverStats::GAME_GREETING);

    while(p.get()->isSktAlive() && !(p.get()->isValidToon()))
    {
        uint8_t type = listener(p);
        if(type == LURK_TYPES::TYPE_CHARACTER)
        {
            proc_character(p);
        }else{
            error_character(p);
        }
    }

    //connection lost.
    ragequit(p);
}

uint8_t Gamemaster::listener(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    uint8_t dipByte;
    bytes = recv(p.get()->getFD(), &dipByte,sizeof(uint8_t),MSG_WAITALL);

    if(bytes < 0)
    {
        p.get()->quitPlayer();
        dipByte = 0;
    }
    return dipByte;
}

//type processing
void Gamemaster::proc_character(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    recv(p.get()->getFD(), &p.get()->charTainer, sizeof(LURK_CHARACTER),MSG_WAITALL);
    
    uint16_t len = p.get()->charTainer.DESC_LENGTH;
    char desc[len];
    bytes = recv(p.get()->getFD(),desc,len,MSG_WAITALL);
    desc[len] = 0;
    p.get()->desc = std::string(desc);

    if(bytes < 0)
    {
        p.get()->quitPlayer();
        return;
    }
    bool checks = false;
    checks = check_name(p);
    checks = check_stat(p);

    if(checks)
    {
        p.get()->isValidToon();
        p.get()->write_accept(LURK_TYPES::TYPE_CHARACTER);
        p.get()->write_character(p.get()->charTainer,p.get()->desc);
    }else{
        error_character(p);
    }

}

//error handling
void Gamemaster::error_character(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 4;
    std::string m;
    if(p.get()->isStarted())
    {
        m = fmt::format("{}: Join battle is always on"
            "and is not a choice (yet).\n",serverStats::GM_NAME);
    }else if(!(p.get()->isValidToon()))
    {
        m = fmt::format("{}: You attempt to set invalid stats.\n",serverStats::GM_NAME);

    }else {
        m = fmt::format("{}: You shouldn't see this. (DEBUG: error_character())\n",serverStats::GM_NAME);
    }
    pkg.MSG_LEN = m.length();
    p.get()->write_error(pkg,m);

}
//helper
bool Gamemaster::check_name(std::shared_ptr<Player> p)
{
    bool unique = true;
    if(!(master_player_list.empty()))
    {
        std::lock_guard<std::mutex> lock(GMLock);
        for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
        {
            if(strcmp((*t).get()->charTainer.CHARACTER_NAME,
               p.get()->charTainer.CHARACTER_NAME) == 0)
            {
                unique = false;
                break;
            }
        }
    }
    return unique;
}
bool Gamemaster::check_stat(std::shared_ptr<Player> p)
{
    bool good = false;
    uint16_t stat = p.get()->charTainer.ATTACK + p.get()->charTainer.DEFENSE + p.get()->charTainer.REGEN;
    p.get()->charTainer.GOLD = (fast_rand() %
        (serverStats::PLAYER_MAX_GOLD - serverStats::PLAYER_MIN_GOLD) + serverStats::PLAYER_MIN_GOLD);
    
    if(stat < serverStats::PLAYER_INIT_POINTS)
    {
        uint16_t remaining = serverStats::PLAYER_INIT_POINTS - stat;
        p.get()->charTainer.HEALTH += remaining;
        good = true;
    }
    return good;
}
