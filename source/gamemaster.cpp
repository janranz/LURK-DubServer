#include"../headers/gamemaster.h"
/* 
    TODO:
    add a reaper()
    see if std::to_string() is neccessary.

*/

Gamemaster::Gamemaster()
{
    g_seed = static_cast<unsigned int>(std::time(NULL)); // see if that seeds.
    fmt::print("Current Seed: {0}\n",g_seed);
    vers.MAJOR = serverStats::GAME_VERSION_MAJOR;
    vers.MINOR = serverStats::GAME_VERSION_MINOR;
    vers.EXTENS_SIZE = serverStats::GAME_VERSION_EXT;
    gmInfo.INITIAL_POINTS = serverStats::PLAYER_INIT_POINTS;
    gmInfo.STAT_LIMIT = serverStats::PLAYER_MAX_STAT;
    gmInfo.DESC_LENGTH = serverStats::GAME_GREETING.length();
    //set GM's name
    strncpy(M_ToCP(gmpm.SENDER_NAME),serverStats::GM_NAME.c_str(),sizeof(gmpm.SENDER_NAME));
    
    
}

//events
// int Gamemaster::fast_rand()
// {
//     std::lock_guard<std::mutex> lock(randLock);
//     g_seed = (214013*g_seed+2531011);
//     return (g_seed>>16)&0x7FFF;
// }

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
        default:{fmt::print("Failure in buildChatter!\n");}
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
                (*t)->emplace_connection(*b);
            }
        }else if( t+1 != master_room_list.end())
        {
            (*t)->emplace_connection(*(t-1));
            (*t)->emplace_connection(*(t+1));
        }else{
            (*t)->emplace_connection(*(t-1));
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
    
    strncpy(M_ToCP(b->bTainer.CHARACTER_NAME),name.c_str(),sizeof(b->bTainer.CHARACTER_NAME));
    b->bTainer.CHARACTER_NAME[31] = 0;

    dex = (fast_rand() % c_m.baddie_desc.size());
    b->desc = c_m.baddie_desc.at(dex);
    b->bTainer.CURRENT_ROOM_NUMBER = roomNumber;
    b->bTainer.DESC_LENGTH = b->desc.length();
    
    b->respawn();
    return b;
}

void Gamemaster::populate_rooms()
{
    for(auto t = master_room_list.begin(); t != master_room_list.end(); ++t)
    {
        int roomNumber = (*t)->roomTainer.ROOM_NUMBER;
        int baddie_count = (fast_rand() %
            (serverStats::MAX_BADDIES_PER_ROOM - serverStats::MIN_BADDIES_PER_ROOM)
            + serverStats::MIN_BADDIES_PER_ROOM);
        
        for(int i = 0; i < baddie_count; i++)
            (*t)->emplace_baddie(build_a_baddie(roomNumber));
    }

}
//cleanup
void Gamemaster::ragequit(std::shared_ptr<Player> p)
{
    int fd = p->getFD();
    {
        std::lock_guard<std::shared_mutex> lock(GMLock);
        if(p->isStarted())
        {
            std::string m = fmt::format("{0} has disconnected from the server!\n",p->charTainer.CHARACTER_NAME);

            master_room_list.at(p->charTainer.CURRENT_ROOM_NUMBER)->remove_player(p);
            master_room_list.at(p->charTainer.CURRENT_ROOM_NUMBER)->inform_others_player(p);

            for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
            {
                if(fd == (*t)->getFD())
                {
                    t = master_player_list.erase(t);
                    --t;
                    break;
                    {std::lock_guard<std::mutex> lock(printLock);fmt::print("Found in master_player list. removed!\n");}
                }
            }
            for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
            {
                (*t)->write_msg(gmpm,m);
            }
            int size = master_player_list.size();
            {
                std::lock_guard<std::mutex> lock(printLock);
                fmt::print("{0} has ragequit.\n Masterlist Size: {1}\n"
                ,p->charTainer.CHARACTER_NAME,size);
            }
        }
    }
    close(fd);
}

void Gamemaster::pump_n_dump(std::shared_ptr<Player>p)
{// don't check bytes on MSG_DONTWAIT
    char dump[1024];
    for(int i = 0; i < 5; i++)
        recv(p->getFD(), dump, 1024, MSG_DONTWAIT);
}


//network**********************************************************************************************************
void Gamemaster::GMController(int fd)
{
    auto p = std::make_shared<Player>(fd);
    p->write_version(vers);
    p->write_game(gmInfo,serverStats::GAME_GREETING);

    while(p->isSktAlive() && !(p->isValidToon()))
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
    // potentially global message

    while(p->isSktAlive() && !(p->isStarted()))
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
    
    while(p->isSktAlive())
    {
        uint8_t type = listener(p);

        if(!(p->isPlayerAlive()))
        {
            spawn_player(p);
            if(type != LURK_TYPES::TYPE_MSG && type != LURK_TYPES::TYPE_LEAVE)
            {
                {std::lock_guard<std::mutex>lock(printLock);fmt::print("wtf: {}\n",type);}
                error_dead(p);
                pump_n_dump(p);
                continue;
            }
        }
        if(type == LURK_TYPES::TYPE_MSG)
        {
            proc_msg(p);
        }else if(type == LURK_TYPES::TYPE_CHANGEROOM)
        {
            proc_changeroom(p);
        }else if((type == LURK_TYPES::TYPE_LEAVE) || (type == 0))
        {
            p->quitPlayer();
        }else if(type == LURK_TYPES::TYPE_FIGHT)
        {
            proc_fight(p);
        }else{
            {
                std::lock_guard<std::mutex>lock(printLock);fmt::print("{0} SENT INVALID TYPE: {1}\n",p->charTainer.CHARACTER_NAME,type);
                error_invalid(p);
                pump_n_dump(p);
            }
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
    bytes = recv(p->getFD(), &dipByte,sizeof(uint8_t),MSG_WAITALL);
    if(p->isValidToon())
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("{0} sent type: {1}\n",p->charTainer.CHARACTER_NAME,dipByte);}
    if(bytes < 1)
    {
        dipByte = 0;
    }
    return dipByte;
}

//type processing

void Gamemaster::proc_fight(std::shared_ptr<Player> p)
{
    {
        std::shared_lock<std::shared_mutex>lock(GMLock);
        int r = p->charTainer.CURRENT_ROOM_NUMBER;
        if(!(master_room_list.at(r)->initiate_fight_baddie(p)))
        {
            error_fight(p);
        }
    }
}

void Gamemaster::proc_msg(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    int fd = p->getFD();
    LURK_MSG pkg;
    std::string m;
    bool found = true;

    recv(fd, &pkg, sizeof(LURK_MSG),MSG_WAITALL);
    pkg.CEIVER_NAME[31] = 0;
    pkg.SENDER_NAME[31] = 0;    

    char* msg = new char[pkg.MSG_LEN + 1];
    bytes = recv(fd, msg, pkg.MSG_LEN,MSG_WAITALL);
    msg[pkg.MSG_LEN] = 0;
    m = std::string(msg);
    delete msg;

    if(bytes < 1)
    {
        p->quitPlayer();
        return;
    }
    {
        std::shared_lock<std::shared_mutex> lock(GMLock);
        for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
        {
            
            if(compare_to_lowers(M_ToCP(pkg.CEIVER_NAME),M_ToCP((*t)->charTainer.CHARACTER_NAME)))
            {
                (*t)->write_accept(LURK_TYPES::TYPE_MSG);                
                (*t)->write_msg(pkg,m);
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
    uint16_t roomReq = 0;
    bytes = recv(p->getFD(),&roomReq,sizeof(uint16_t),MSG_WAITALL);
    if( bytes < 1)
    {
        p->quitPlayer();
        return;
    }
    move_player(p,roomReq);
}

void Gamemaster::proc_character(std::shared_ptr<Player> p)
{
    ssize_t bytes;
    recv(p->getFD(), &p->charTainer, sizeof(LURK_CHARACTER),MSG_WAITALL);
    p->charTainer.CHARACTER_NAME[31] = 0;
    uint16_t len = p->charTainer.DESC_LENGTH;
    
    // char desc[len];
    char* desc = new char[len + 1];
    bytes = recv(p->getFD(),desc,len,MSG_WAITALL);
    desc[len] = 0;
    p->desc = std::string(desc);
    delete desc;

    if(bytes < 1)
    {
        p->quitPlayer();
        return;
    }
    bool checks = false;
    checks = check_name(p);
    checks = check_stat(p);

    if(checks)
    {
        p->setValid();
        p->write_accept(LURK_TYPES::TYPE_CHARACTER);
        p->write_character(p->charTainer,p->desc);
    }else{
        error_character(p);
    }

}

void Gamemaster::proc_start(std::shared_ptr<Player> p)
{// ADD SEPARATE PORTAL FOR ENTRY AND REWORK CHANGEROOM
    
    int size = 0;
    {
        std::lock_guard<std::shared_mutex> lck(GMLock);
        master_player_list.emplace_back(p);
        size = master_player_list.size();
        
    }
    {
        std::lock_guard<std::mutex> lock(printLock);
        fmt::print("{0} has been added to Master: {1} (size)\n",p->charTainer.CHARACTER_NAME,size);
    }
    p->startPlayer();
    p->write_accept(LURK_TYPES::TYPE_START);
    spawn_player(p);
}

//error handling

void Gamemaster::error_dead(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 0;
    std::string m = fmt::format("{0}: Can't do that when you're dead.. but you're alive now. Do it again!\n",serverStats::GM_NAME);
    p->write_error(pkg,m);
}

void Gamemaster::error_invalid(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 0;
    std::string m = fmt::format("{0}: Under construction... try that in a bit.\n",serverStats::GM_NAME);
    p->write_error(pkg,m);
}

void Gamemaster::error_fight(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 3;
    std::string m = fmt::format("{0}: You swing out of your skull, but the baddies are already dead.\n",serverStats::GM_NAME);
    p->write_error(pkg,m);
}

void Gamemaster::error_msg(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 6;
    std::string m = fmt::format("{0}: You stick your message in the mailbox, but the mailman gets eaten by a baddie!\n",serverStats::GM_NAME);
    p->write_error(pkg, m);
}

void Gamemaster::error_changeroom(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 1;
    std::string m = fmt::format("{0}: You attempt to step through to that room, but you quickly realize that is an error in judgement!\n",
        serverStats::GM_NAME);
        pkg.MSG_LEN = m.length();
        p->write_error(pkg, m);
}

void Gamemaster::error_character(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 4;
    std::string m;
    {
        std::shared_lock<std::shared_mutex> lock(GMLock);
        if(p->isStarted())
        {
            m = fmt::format("{0}: Join battle is always on"
                "and is not a choice (yet).\n",serverStats::GM_NAME);
        }else if(!(p->isValidToon()))
        {
            m = fmt::format("{0}: You attempt to set invalid stats.\n",serverStats::GM_NAME);

        }else {
            m = fmt::format("{0}: You shouldn't see this. (DEBUG: error_character())\n",serverStats::GM_NAME);
        }
    }
    pkg.MSG_LEN = m.length();
    p->write_error(pkg,m);

}

void Gamemaster::error_start(std::shared_ptr<Player> p)
{
    LURK_ERROR pkg;
    pkg.CODE = 1;
    std::string m;
    {
        std::shared_lock<std::shared_mutex> lock(GMLock);
        if(p->isStarted())
        {
        m = fmt::format("{0}: Dearest {1}, you have already started. I love the passion, though.\n",
        serverStats::GM_NAME,p->charTainer.CHARACTER_NAME);
        }else{
            m = fmt::format("{0}: {1}, Send me a start! (Type 6)\n", serverStats::GM_NAME,p->charTainer.CHARACTER_NAME);
        }
    }
    pkg.MSG_LEN = m.length();
    p->write_error(pkg,m);
}

//helper


bool Gamemaster::check_name(std::shared_ptr<Player> p)
{
    std::shared_lock<std::shared_mutex> lock(GMLock);
    // p->charTainer.CHARACTER_NAME[32] = 0;
    bool unique = true;
    if(!(master_player_list.empty()))
    {
        
        for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
        {
            if((*t)->charTainer.CHARACTER_NAME == p->charTainer.CHARACTER_NAME)
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
    
    
    uint16_t stat = p->charTainer.ATTACK + p->charTainer.DEFENSE + p->charTainer.REGEN;
    p->charTainer.GOLD = (fast_rand() %
        (serverStats::PLAYER_MAX_GOLD - serverStats::PLAYER_MIN_GOLD) + serverStats::PLAYER_MIN_GOLD);
    
    if(stat <= serverStats::PLAYER_INIT_POINTS)
    {
        uint16_t remaining = serverStats::PLAYER_INIT_POINTS - stat;
        p->charTainer.HEALTH = (serverStats::PLAYER_BASE_HEALTH + remaining);
        good = true;
        p->charTainer.CURRENT_ROOM_NUMBER = 0;
    }
    p->charTainer.DESC_LENGTH = p->desc.length();

    {
        std::shared_lock<std::shared_mutex> lock(GMLock);
        for(auto t = master_player_list.begin(); t != master_player_list.end(); ++t)
        {
            if(compare_to_lowers(M_ToCP(p->charTainer.CHARACTER_NAME),M_ToCP((*t)->charTainer.CHARACTER_NAME)))
            {
                good = false;
                std::string m = fmt::format("Sorry, {0} is actively playing! Pick a different name and go rough them up in Room #{1}!\n",
                (*t)->charTainer.CHARACTER_NAME, std::to_string((*t)->charTainer.CURRENT_ROOM_NUMBER));
                p->write_msg(gmpm,m);
                break;
            }
        }
    }
    return good;
}

void Gamemaster::spawn_player(std::shared_ptr<Player> p)
{
    std::lock_guard<std::shared_mutex>lock(GMLock); // double check exclusiveness
    
    if(!(p->isPlayerAlive()))
    {
        uint16_t rm = p->getRoomNumber();
        p->respawn();
        master_room_list.at(rm)->remove_player(p);
        master_room_list.at(0)->emplace_player(p);
        master_room_list.at(rm)->inform_others_player(p);
    }else{
        p->respawn();
        master_room_list.at(0)->emplace_player(p);
    }
}

void Gamemaster::move_player(std::shared_ptr<Player> p, uint16_t room)
{
    {
        std::lock_guard<std::shared_mutex> lock(GMLock);
        uint16_t rm = p->getRoomNumber();
        bool found = master_room_list.at(rm)->isValidConnection(room);
        if(found)
        {
            p->write_accept(LURK_TYPES::TYPE_CHANGEROOM);
            master_room_list.at(rm)->remove_player(p);
            master_room_list.at(room)->emplace_player(p);
            
            master_room_list.at(rm)->inform_others_player(p);
            // master_room_list.at(room)->inform_others_player(p);
        }else{
            error_changeroom(p);
        }
    }
}
