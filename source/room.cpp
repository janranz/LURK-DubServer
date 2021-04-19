#include"../headers/room.h"

Room::Room(std::string name,std::string desc,uint16_t num)
{
    strncpy(roomTainer.ROOM_NAME,name.c_str(),32);
    roomDesc = desc;
    roomTainer.ROOM_NUMBER = num;
    std::string m = "The Mysterious Butler";
    strncpy(rmpm.SENDER_NAME,m.c_str(),32);
}

void Room::emplace_connection(std::shared_ptr<Room> r)
{
    std::lock_guard<std::mutex> lock(rLock);
    room_connections.emplace_back(r);
}

void Room::emplace_player(std::shared_ptr<Player>p)
{// consider if we need to make a copy
 // std::shared_ptr<Player> tmp = std::make_shared<Player>(p);
    {
        std::lock_guard<std::mutex> lock(rLock);
        player_list.emplace_back(p);
    }
    p.get()->charTainer.CURRENT_ROOM_NUMBER = roomTainer.ROOM_NUMBER;
    inform_connections(p);
    inform_baddies(p);
    inform_players_friendly();
    
    std::string m = fmt::format("{} has joined the room to fight by your side!\n"
        ,p.get()->charTainer.CHARACTER_NAME);
    
    int fd = p.get()->getFD();
    {
        std::lock_guard<std::mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            if((*t).get()->getFD() != fd)
            {
                (*t).get()->write_msg(rmpm,m);
            }
        }
    }
}

bool Room::isValidConnection(uint16_t r)
{
    for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
    {
        if((*t).get()->roomTainer.ROOM_NUMBER == r)
        {
            return true;
        }
    }
    return false;
}

void Room::emplace_baddie(std::shared_ptr<Baddie> b)
{
    {
        std::lock_guard<std::mutex> lock(rLock);
        baddie_list.emplace_back(b);
    }
}

void Room::remove_player(std::shared_ptr<Player>p)
{
    int pfd = p.get()->getFD();
    bool DEBUG_found = false;
    {
        std::lock_guard<std::mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            int tfd = (*t).get()->getFD();
            if(tfd == pfd)
            {
                DEBUG_found = true;
                t = player_list.erase(t);
                --t; // save time later by skipping extra loop time after found
            }
        }
    }
    
    inform_players_friendly();
    if(DEBUG_found)
    {
        std::cout << fmt::format("{0} was found in room: {1} and removed\n",
            p.get()->charTainer.CHARACTER_NAME,roomTainer.ROOM_NAME);
    }
}
void Room::inform_connections(std::shared_ptr<Player> p)
{// static no lock
    // inform room
    p.get()->write_room(roomTainer,roomDesc);
    //inform connections
    for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
    {
        p.get()->write_connection((*t).get()->roomTainer,(*t).get()->roomDesc);
    }

}

void Room::inform_players_friendly()
{
    {
        std::lock_guard<std::mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            for(auto b = player_list.begin(); b != player_list.end(); ++b)
            {
                (*t).get()->write_character((*b).get()->charTainer,(*b).get()->desc);
            }
        }
    }
}
void Room::inform_baddies(std::shared_ptr<Player> p)
{// static no rLock but baddie lock(bLock)

    for(auto t = baddie_list.begin(); t != baddie_list.end(); ++t)
    {
        std::lock_guard<std::mutex> lock((*t)->bLock);
        p.get()->write_character((*t)->bTainer,(*t)->desc);
    }
}

//debug
size_t Room::room_connection_size()
{
    return room_connections.size();
}
size_t Room::baddie_list_size()
{
    return baddie_list.size();
}
