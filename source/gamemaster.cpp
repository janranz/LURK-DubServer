#include"../headers/gamemaster.h"
/* 
    TODO:
    

*/

Gamemaster::Gamemaster()
{
    g_seed = static_cast<unsigned int>(std::time(NULL));
    gmInfo.INITIAL_POINTS = serverStats::PLAYER_INIT_POINTS;
    gmInfo.STAT_LIMIT = serverStats::PLAYER_MAX_STAT;
    gmInfo.DESC_LENGTH = serverStats::GAME_GREETING.length();
    strlcpy(gmpm.SENDER_NAME,serverStats::GM_NAME.c_str(),32);

    vers.MAJOR = serverStats::GAME_VERSION_MAJOR;
    vers.MINOR = serverStats::GAME_VERSION_MINOR;
    vers.EXTENS_SIZE = serverStats::GAME_VERSION_EXT;

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
    strlcpy(b.get()->bTainer.CHARACTER_NAME,name.c_str(),32);

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
    if(p.get()->isStarted())
    {
        int fd = p.get()->getFD();
        std::string m = fmt::format("{0} has disconnected from the server!\n",p.get()->charTainer.CHARACTER_NAME);
        
        {
            std::lock_guard<std::mutex>lock(GMLock);
            for(auto t = master_room_list.begin(); t != master_room_list.end(); ++t)
            {
                if((*t).get()->seek_remove_player(p)){break;}
            }
            for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
            {
                if(fd == (*t).get()->getFD())
                {
                    t = master_player_list.erase(t);
                    --t;
                    break;
                    std::cout << "Found in master_player list. removed!\n";
                }
            }
            for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
            {
                (*t).get()->write_msg(gmpm,m);
            }
        }
    }
    std::cout << fmt::format("{0} has ragequit.\n Masterlist Size: {1}\n"
        , p.get()->charTainer.CHARACTER_NAME, std::to_string(master_player_list.size()));
}

void Gamemaster::pump_n_dump(std::shared_ptr<Player>p)
{// don't check bytes on MSG_DONTWAIT
    std::vector<char> dump(sizeof(size_t));
    recv(p.get()->getFD(), dump.data(), sizeof(size_t), MSG_DONTWAIT);
}


//network**********************************************************************************************************
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
            pump_n_dump(p);
        }
    }
    while(p.get()->isSktAlive() && !(p.get()->isStarted()))
    {
        uint8_t type = listener(p);
        if(type == LURK_TYPES::TYPE_START)
        {
            proc_start(p);
        }else{
            error_start(p);
            pump_n_dump(p);
        }
    }
    while(p.get()->isSktAlive())
    {
        uint8_t type = listener(p);
        if(type == LURK_TYPES::TYPE_MSG)
        {
            proc_msg(p);
        }else if(type == LURK_TYPES::TYPE_CHANGEROOM)
        {
            proc_changeroom(p);
        }else if(type == LURK_TYPES::TYPE_LEAVE)
        {
            std::string m = fmt::format("Walk in the light, {0}...\n",p.get()->charTainer.CHARACTER_NAME);
            p.get()->write_msg(gmpm,m);
            p.get()->quitPlayer();
        }

    }
    //connection lost.
    ragequit(p);
}
//END network******************************************************************************************************


uint8_t Gamemaster::listener(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    uint8_t dipByte;
    bytes = recv(p.get()->getFD(), &dipByte,sizeof(uint8_t),MSG_WAITALL);
    
    if(bytes < 1)
    {
        std::cout << "Bytes: " << bytes << std::endl;
        p.get()->quitPlayer();
        dipByte = 0;
    }
    return dipByte;
}

//type processing

void Gamemaster::proc_msg(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    int fd = p.get()->getFD();
    LURK_MSG pkg;
    std::string m;
    bool found = true;

    recv(fd, &pkg, sizeof(LURK_MSG),MSG_WAITALL);
    std::vector<char> message(pkg.MSG_LEN);
    bytes = recv(fd, message.data(), pkg.MSG_LEN,MSG_WAITALL);
    m = std::string(message.begin(), message.end());
    
    if(bytes < 1)
    {
        p.get()->quitPlayer();
        return;
    }
    {
        std::lock_guard<std::mutex>lock(GMLock);
        for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
        {
            if(compare_to_lowers(pkg.CEIVER_NAME,(*t).get()->charTainer.CHARACTER_NAME))
            {
                (*t).get()->write_accept(LURK_TYPES::TYPE_MSG);
                (*t).get()->write_msg(pkg,m);
                found = true;
                break;
            }
        }
    }
    if(!found)
    {
        error_msg(p);
    }
}

void Gamemaster::proc_changeroom(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    int fd = p.get()->getFD();
    uint16_t curr = p.get()->charTainer.CURRENT_ROOM_NUMBER;
    uint16_t next;
    bytes = recv(fd,&next, sizeof(uint16_t),MSG_WAITALL);
    if(bytes < 1)
    {
        p.get()->quitPlayer();
        return;
    }
    bool c = master_room_list.at(curr).get()->isValidConnection(next);
    if(c)
    {
        p.get()->write_accept(LURK_TYPES::TYPE_CHANGEROOM);
        p.get()->charTainer.CURRENT_ROOM_NUMBER = next;
        master_room_list.at(curr).get()->remove_player(p);
        master_room_list.at(next).get()->emplace_player(p);
    }else{
        master_room_list.at(curr).get()->inform_connections(p);
        error_changeroom(p);
    }
}

void Gamemaster::proc_character(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    recv(p.get()->getFD(), &p.get()->charTainer, sizeof(LURK_CHARACTER),MSG_WAITALL);
    
    uint16_t len = p.get()->charTainer.DESC_LENGTH;
    char desc[len];
    bytes = recv(p.get()->getFD(),desc,len,MSG_WAITALL);
    desc[len] = 0;
    p.get()->desc = std::string(desc);

    if(bytes < 1)
    {
        p.get()->quitPlayer();
        return;
    }
    bool checks = false;
    checks = check_name(p);
    checks = check_stat(p);

    if(checks)
    {
        p.get()->setValid();
        p.get()->write_accept(LURK_TYPES::TYPE_CHARACTER);
        p.get()->write_character(p.get()->charTainer,p.get()->desc);
    }else{
        error_character(p);
    }

}

void Gamemaster::proc_start(std::shared_ptr<Player> p)
{
    p.get()->startPlayer();
    {
        std::lock_guard<std::mutex> lock(GMLock);
        master_player_list.emplace_back(p);
    }
    std::cout << fmt::format("{0} has been added to Master: {1} (size)\n",
        p.get()->charTainer.CHARACTER_NAME,master_player_list.size());
    
    p.get()->write_accept(LURK_TYPES::TYPE_START);
    move_player(p,0);

}

//error handling

void Gamemaster::error_msg(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 6;
    std::string m = fmt::format("{}: You stick your message in the mailbox, but the mailman gets eaten by a baddie!\n",serverStats::GM_NAME);
    p.get()->write_error(pkg, m);
}

void Gamemaster::error_changeroom(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 1;
    std::string m = fmt::format("{}: You attempt to step through to that room, but you quickly realize that is an error in judgement!\n",
        serverStats::GM_NAME);
        pkg.MSG_LEN = m.length();
        p.get()->write_error(pkg, m);
}

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

void Gamemaster::error_start(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 1;
    std::string m;
    if(p.get()->isStarted())
    {
       m = fmt::format("{0}: Dearest {1}, you have already started. I love the passion, though.\n",
       serverStats::GM_NAME, p.get()->charTainer.CHARACTER_NAME);
    }else{
        m = fmt::format("{0}: {1}, Send me a start! (Type 6)\n", serverStats::GM_NAME, p.get()->charTainer.CHARACTER_NAME);
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
    
    if(stat <= serverStats::PLAYER_INIT_POINTS)
    {
        uint16_t remaining = serverStats::PLAYER_INIT_POINTS - stat;
        p.get()->charTainer.HEALTH = (serverStats::PLAYER_BASE_HEALTH + remaining);
        good = true;
        p.get()->charTainer.CURRENT_ROOM_NUMBER = 0;
    }
    p.get()->charTainer.DESC_LENGTH = p.get()->desc.length();
    
    size_t len = strlen(p.get()->charTainer.CHARACTER_NAME);

    {
        std::lock_guard<std::mutex> lock (GMLock);
        for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
        {
            std::string nameA;
            std::string nameB;
            for(size_t i = 0; i < len; i++)
            {
                nameA += std::tolower((*t).get()->charTainer.CHARACTER_NAME[i]);
                nameB += std::tolower(p.get()->charTainer.CHARACTER_NAME[i]);
            }
            if(nameA.compare(nameB) == 0)
            {
                good = false;
                std::string m = fmt::format("Sorry, {0} is actively playing! Pick a different name and go rough them up in Room #{1}\n"
                ,(*t).get()->charTainer.CHARACTER_NAME, std::to_string((*t).get()->charTainer.CURRENT_ROOM_NUMBER));
                p.get()->write_msg(gmpm,m);
                break;
            }
        }
    }
    return good;
}

void Gamemaster::move_player(std::shared_ptr<Player> p, uint16_t room)
{
    if(p.get()->isFreshSpawn())
    {
        master_room_list.at(0).get()->emplace_player(p);
        p.get()->despawn();
    }else{
        bool found = master_room_list.at(p.get()->charTainer.CURRENT_ROOM_NUMBER).get()->isValidConnection(room);

        if(found)
        {
            master_room_list.at(p.get()->charTainer.CURRENT_ROOM_NUMBER).get()->remove_player(p);
            master_room_list.at(room).get()->emplace_player(p);
        }
    }
}
