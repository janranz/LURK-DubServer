#include"../headers/room.h"

Room::Room(std::string name,std::string desc,uint16_t num)
{
    strncpy(roomTainer.ROOM_NAME,name.c_str(),32);
    // roomTainer.ROOM_NAME[32] = 0;
    roomDesc = desc;
    roomTainer.ROOM_NUMBER = num;
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
    inform_connections(p);
    inform_baddies(p);
    inform_players_friendly();

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
