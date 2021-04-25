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
    

}

void Room::calculateDiff()
{
    {
        std::shared_lock<std::shared_mutex>lock(rLock);
        if(player_list.empty())
        {
            firepower = 1;
        }else{
            firepower = player_list.size();
        }
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
    }
    p->write_reflect();
    p->write_room(roomTainer,roomDesc);
    inform_player_friendly(p);
    inform_baddies(p);
    inform_connections(p); // may be out of order with baddies. double check
    inform_others_player(p); // call from GM.
    
    std::string m = fmt::format("{} has joined the room to fight by your side!\nCurrent room strength: {1}\n"
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

void Room::remove_player(std::shared_ptr<Player>p)
{// assume this to be called after player(p) has been given a new room.
    int pfd = p->getFD();
    bool found = false;
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            int tfd = (*t)->getFD();
            if(tfd == pfd)
            {
                found = true;
                t = player_list.erase(t);
                break; // --t;
            }
        }
    }
    if(found)
    {
        
    }else{
        std::lock_guard<std::mutex>lock(printLock);std::cout << fmt::format("{0} NOT found in: {1}\n",
            p->charTainer.CHARACTER_NAME,std::to_string(roomTainer.ROOM_NUMBER));
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

void Room::inform_connections(std::shared_ptr<Player> p)
{
    {
        std::shared_lock<std::shared_mutex> lock(rLock); // TEMPORARY
        //inform connections
        for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
        {
            p->write_connection((*t)->roomTainer,(*t)->roomDesc);
        }
    }
}
void Room::inform_others_player(std::shared_ptr<Player> p)
{// inform other players, but assume player has joined the room
    if(!p->isSktAlive())
    {
        p->giveRoom(room_connections.at(0)->roomTainer.ROOM_NUMBER);
    }
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            (*t)->write_character(p->charTainer,p->desc);
        }
    }
}

void Room::initiate_fight_baddie(std::shared_ptr<Player> p)
{
    std::string m;
    {
        std::shared_lock<std::shared_mutex>lock(rLock);
        fight_in_progress = true;

        int bDex = (fast_rand() % baddie_list.size());
        m = fmt::format("{0} grows tired of {1} looking at them with googly eyes and decides to start a fight!\n",
        p->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);

        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            (*t)->write_msg(rmpm,m);
        }
    }
    fight_in_progress = false;
    // think about communicating a cleanup routine after death. How will we move them to spawn?
}

void Room::fight_controller()
{
    {
        //tally 
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
{
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
        {
            if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
                return true;
        }
    }
    return false;
}

void Room::inform_player_friendly(std::shared_ptr<Player> p)
{// lurk test may not like this REWORK
    int fd = p->getFD();
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            if(fd != (*t)->getFD())
            {
                p->write_character((*t)->charTainer,(*t)->desc);
            }
        }
    }
}
void Room::inform_baddies(std::shared_ptr<Player> p)
{// static no rLock but baddie lock(bLock)
    {
        std::shared_lock<std::shared_mutex> lock(rLock); // TEMPORARY
        for(auto t = baddie_list.begin(); t != baddie_list.end(); ++t)
        {
            // std::lock_guard<std::mutex> lock((*t)->bLock);
            p->write_character((*t)->bTainer,(*t)->desc);
        }
    }
}
