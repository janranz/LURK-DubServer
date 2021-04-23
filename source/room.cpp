#include"../headers/room.h"

Room::Room(std::string name,std::string desc,uint16_t num)
{
    strncpy(M_ToCP(roomTainer.ROOM_NAME),name.c_str(),32);

    roomDesc = desc;
    roomTainer.ROOM_NUMBER = num;
    roomTainer.DESC_LENGTH = desc.length();
    std::string m = "The Mysterious Butler";
    

    // strlcpy(rmpm.SENDER_NAME,m.c_str(),32);
    // strlcpy(rmpm.CEIVER_NAME,n.c_str(),32);
    strncpy(M_ToCP(rmpm.SENDER_NAME),m.c_str(),32);

}

void Room::emplace_connection(std::shared_ptr<Room> r)
{
    std::lock_guard<std::recursive_mutex> lock(rLock);
    room_connections.emplace_back(r);
}

void Room::emplace_player(std::shared_ptr<Player>p)
{// consider if we need to make a copy
 // std::shared_ptr<Player> tmp = std::make_shared<Player>(p);
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        player_list.emplace_back(p);
    }
    inform_connections(p);
    inform_baddies(p);
    inform_players_friendly();
    // inform_connections(p);
    
    std::string m = fmt::format("{} has joined the room to fight by your side!\n"
        ,p->charTainer.CHARACTER_NAME);
    
    int fd = p->getFD();
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            if((*t)->getFD() != fd)
            {
                // strncpy(rmpm.CEIVER_NAME,(*t)->charTainer.CHARACTER_NAME,32);
                // rmpm.CEIVER_NAME[32] = 0;
                (*t)->write_msg(rmpm,m);
            }
        }
    }
}



bool Room::isValidConnection(uint16_t r)
{
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
        {
            if((*t)->roomTainer.ROOM_NUMBER == r)
            {
                // std::cout << "Valid connection found!: " << r << std::endl;
                return true;
            }
        }
    }
    return false;
}

void Room::emplace_baddie(std::shared_ptr<Baddie> b)
{
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        baddie_list.emplace_back(b);
    }
}

bool Room::seek_remove_player(std::shared_ptr<Player> p)
{
    bool found = false;
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            if((*t)->charTainer.CHARACTER_NAME == p->charTainer.CHARACTER_NAME)
            {
                found = true;
                break;
            }
        }

    }
    if(found)
    {
        p->charTainer.CURRENT_ROOM_NUMBER = 99; // potentially unsafe
        remove_player(p);
    }
    return found;
}

void Room::remove_player(std::shared_ptr<Player>p)
{
    int pfd = p->getFD();
    inform_players_friendly();
    // bool DEBUG_found = false;
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            int tfd = (*t)->getFD();
            if(tfd == pfd)
            {
                // DEBUG_found = true;
                t = player_list.erase(t);
                --t; // save time later by skipping extra loop time after found
            }
        }
    }
    std::string m = fmt::format("{0} has left the room.\n",p->charTainer.CHARACTER_NAME);
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            // strncpy(rmpm.CEIVER_NAME,(*t)->charTainer.CHARACTER_NAME,32);
            (*t)->write_msg(rmpm, m);
        }
    }
}

void Room::inform_connections(std::shared_ptr<Player> p)
{
    // inform room
    {
        std::lock_guard<std::recursive_mutex> lock(rLock); // TEMPORARY
        p->write_room(roomTainer,roomDesc);
        //inform connections
        for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
        {
            p->write_connection((*t)->roomTainer,(*t)->roomDesc);
        }
    }

}

void Room::inform_players_friendly()
{
    {
        std::lock_guard<std::recursive_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            for(auto b = player_list.begin(); b != player_list.end(); ++b)
            {
                (*t)->write_character((*b)->charTainer,(*b)->desc);
            }
        }
    }
}
void Room::inform_baddies(std::shared_ptr<Player> p)
{// static no rLock but baddie lock(bLock)
    {
        std::lock_guard<std::recursive_mutex> lock(rLock); // TEMPORARY
        for(auto t = baddie_list.begin(); t != baddie_list.end(); ++t)
        {
            std::lock_guard<std::mutex> lock((*t)->bLock);
            p->write_character((*t)->bTainer,(*t)->desc);
        }
    }
}

//debug
// size_t Room::room_connection_size()
// {
//     return room_connections.size();
// }
// size_t Room::baddie_list_size()
// {
//     return baddie_list.size();
// }
