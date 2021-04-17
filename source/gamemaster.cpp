#include"../headers/room.h"


void Room::emplace_connection(int r)
{
    connection_dex.emplace_back(r);
}
void Room::emplace_player(std::shared_ptr<Player>p)
{// consider if we need to make a copy
 // std::shared_ptr<Player> tmp = std::make_shared<Player>(p);
    std::lock_guard<std::mutex> lock(rLock);
    player_list.emplace_back(p);
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
                --t;
            }
        }
    }
    if(DEBUG_found)
    {
        std::cout << fmt::format("{0} was found in room: {1} and removed\n",
            p.get()->charTainer.CHARACTER_NAME,roomTainer.ROOM_NAME);
    }
}
