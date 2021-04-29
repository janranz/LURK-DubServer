#include"../headers/room.h"

Room::Room(std::string name,std::string desc,uint16_t num)
{
    strncpy(M_ToCP(roomTainer.ROOM_NAME),name.c_str(),sizeof(roomTainer.ROOM_NAME));
    roomTainer.ROOM_NAME[31] = 0;
    roomDesc = desc;
    roomTainer.ROOM_NUMBER = num;
    roomTainer.DESC_LENGTH = desc.length();
    std::string m = "The Mysterious Butler";
    strncpy(M_ToCP(rmpm.SENDER_NAME),m.c_str(),sizeof(rmpm.SENDER_NAME));
    difficulty = 1;
    firepower = 1;
    fight_in_progress = false;

}

void Room::calculateDiff()
{// lock from outside.
    
    if(player_list.empty())
    {
        firepower = 1;
    }else{
        firepower = player_list.size();
    }
}

void Room::emplace_connection(std::shared_ptr<Room> r)
{
    std::lock_guard<std::shared_mutex> lock(rLock);
    room_connections.emplace_back(r);
}

void Room::emplace_player(std::shared_ptr<Player>p)
{
    p->write_reflect();
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        p->giveRoom(roomTainer.ROOM_NUMBER);
        player_list.emplace_back(p);
        calculateDiff();
    }
    {
        std::shared_lock<std::shared_mutex>lock(rLock);
        bundle_update(p);
    }
    std::string m = fmt::format("{0} has joined the room to fight by your side!\nCurrent room strength: {1}\n"
        ,p->charTainer.CHARACTER_NAME,std::to_string(firepower));
    int fd = p->getFD();
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            if((*t)->getFD() != fd)
            {
                (*t)->write_msg(rmpm,m);
            }
        }
    }
}

void Room::remove_player(std::shared_ptr<Player>p)
{// assume this to be called after player(p) has been given a new room.
    int pfd = p->getFD();
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            int tfd = (*t)->getFD();
            if(tfd == pfd)
            {
                
                t = player_list.erase(t);
                break; // --t;
            }
        }
        calculateDiff();
    }

    std::string m = fmt::format("{0} has left the room.\n",p->charTainer.CHARACTER_NAME);
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            (*t)->write_msg(rmpm, m);
        }
    }
}

void Room::bundle_update(std::shared_ptr<Player> p)
{// lock from the outside.
        p->write_reflect();
        p->write_room(roomTainer,roomDesc);
        inform_player_friendly(p);
        inform_baddies(p);
        inform_connections(p); // may be out of order with baddies. double check
        inform_others_player(p); // call from GM. ?
}

void Room::big_bundle_update()
{
    for(auto b = player_list.begin(); b != player_list.end(); ++b)
    {
        bundle_update((*b));
    }
}

bool Room::isValidConnection(uint16_t r)
{
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
        {
            if((*t)->roomTainer.ROOM_NUMBER == r)
            {
                return true;
            }
        }
    }
    return false;
}
void Room::emplace_baddie(std::shared_ptr<Baddie> b)
{
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        baddie_list.emplace_back(b);
    }
    if(baddie_list.empty())
    {
        difficulty = 1;
    }else{
        difficulty = baddie_list.size();
    }
}

void Room::inform_connections(std::shared_ptr<Player> p)
{// lock from outside.
    
        // std::shared_lock<std::shared_mutex> lock(rLock); // TEMPORARY
        //inform connections
    for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
    {
        p->write_connection((*t)->roomTainer,(*t)->roomDesc);
    }
    
}
void Room::inform_others_player(std::shared_ptr<Player> p)
{// inform other players, but assume player has joined the room // lock from outside.
    if(!p->isSktAlive())
    {
        p->giveRoom(room_connections.at(0)->roomTainer.ROOM_NUMBER);
    }
    // {
        // std::shared_lock<std::shared_mutex> lock(rLock);
    for(auto t = player_list.begin(); t != player_list.end(); ++t)
    {
        (*t)->write_character(p->charTainer,p->desc);
    }
    // }
}

bool Room::initiate_fight_baddie(std::shared_ptr<Player> p)
{// need a routine for those who get in here but are late to the battle. (passes check in GM, but queued to aquire lock)

    std::unique_lock<std::shared_mutex>QLock(rLock,std::defer_lock);
    std::shared_lock<std::shared_mutex>SLock(rLock,std::defer_lock);
    std::string m;

    SLock.lock();
    if(!(isValidBaddie()) || fight_in_progress)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("No baddies!\n");}
        return false;
    }
    SLock.unlock();

    QLock.lock();
    fight_in_progress = true;
    fight_controller(p);
    fight_in_progress = false;
    QLock.unlock();


    // after report

    
    return true;
    // think about communicating a cleanup routine after death. How will we move them to spawn?
}

void Room::fight_controller(std::shared_ptr<Player> inst)
{// locked by unique room lock
    
    std::string m;
    bool next = false;
    uint32_t crit;
    uint16_t base;
    int roll;
    

    int bDex;
    
    bDex = LiveBaddieDex();
    
    m = fmt::format("{0} grows tired of {1} looking at them with googly eyes and decides to start a fight!\n",
    inst->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME); // potential UB.

    room_write(m);
    // for(auto t = player_list.begin(); t != player_list.end(); ++t)
    // {
    //     (*t)->write_msg(rmpm,m);
    // }

    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if(!isValidBaddie())
        {
            m = fmt::format("{0} and their homies have successfully slain all baddies in the room!\n",(*p)->charTainer.CHARACTER_NAME);
            room_write(m);
            break;
        }else if(next)
        {
            next = false;
            bDex = LiveBaddieDex();
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("bDex sanity check: {0}\n",bDex);}
            m = fmt::format("{0} sees their homie deliver the final blow on the last baddie, and locks eyes with {1}!\n"
                ,(*p)->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
            room_write(m);
        }
        crit = (*p)->getCrit();
        base = (*p)->charTainer.ATTACK;

        if(baddie_list.at(bDex)->is_alive())
        {
            roll = ((fast_rand() % (crit + 1) - base) + base);
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("{0} rolls: {1} damage!\n",(*p)->charTainer.CHARACTER_NAME,roll);}
            if(!(baddie_list.at(bDex)->hurt_baddie(roll)))
            {// final blow
                m = fmt::format("{0} delivers the most savage, and ruthless blow of {1} damage to {2}'s goofy looking head,\ncasting him into the lake of fire with the quickness!\n",
                (*p)->charTainer.CHARACTER_NAME,roll,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
                big_bundle_update();
                room_write(m);
                next = true;
            }else{
                m = fmt::format("{0} delivers a striking blow, dealing {1} damage to {2}!\n",
                (*p)->charTainer.CHARACTER_NAME,std::to_string(roll),baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
                big_bundle_update();
                room_write(m);
            }
        }else{
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("Baddie dead?: {0}\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME);}
        }
    }
    int i = liveBaddieCount();
    if(i)
    {
        m = fmt::format("{0} baddies remaining!\n",std::to_string(i));
    }else{
        m = fmt::format("No more baddies remain!\n",std::to_string(i));
    }
    room_write(m);

}

void Room::room_write(std::string m)
{
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        (*p)->write_msg(rmpm,m);
    }
}

bool Room::isFightInProgress()
{
    {
        std::shared_lock<std::shared_mutex>lock(rLock);
        return fight_in_progress;
    }
}

bool Room::isValidBaddie()
{// locked by initiate_fight

    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
            return true;
    }
    return false;
}

int Room::LiveBaddieDex()
{
    liveDex.clear();
    int i = 0;
    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
        {
            liveDex.emplace_back(i); // value of i DOES matter here.
        }
        i++;
    }
    {std::lock_guard<std::mutex>lock(printLock);fmt::print("Size of live baddies: {0}\n",liveDex.size());}
    if(liveDex.size() > 0)
    {
        return liveDex.at((fast_rand() % liveDex.size()));
    }else{
        return -1; // should never hit.
    }
}

int Room::liveBaddieCount()
{
    int i = 0;
    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
        {
            i++;
        }
    }
    return i;
}

void Room::inform_player_friendly(std::shared_ptr<Player> p)
{// lock from outside.
    int fd = p->getFD();
    
        // std::shared_lock<std::shared_mutex> lock(rLock);
    for(auto t = player_list.begin(); t != player_list.end(); ++t)
    {
        if(fd != (*t)->getFD())
        {
            p->write_character((*t)->charTainer,(*t)->desc);
        }
    }
}
void Room::inform_baddies(std::shared_ptr<Player> p)
{// static no rLock but baddie lock(bLock)
    
    for(auto t = baddie_list.begin(); t != baddie_list.end(); ++t)
    {
        // std::lock_guard<std::mutex> lock((*t)->bLock);
        p->write_character((*t)->bTainer,(*t)->desc);
    }
}
