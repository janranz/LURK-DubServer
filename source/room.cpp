#include"room.h"

Room::Room(uint16_t num,std::string name ,uint32_t roomDescLen ,std::string roomD)
{
    roomNumber = num;
    roomName = name;
    roomDescLength = roomDescLen;
    roomDesc = roomD;
    
}
Room::~Room()
{
    std::cout << "Room destroyed!" << std::endl;
}

void Room::lock()
{
    innerlock.lock();
}
void Room::unlock()
{
    innerlock.unlock();
}

int Room::searchPlayer(std::string s)
{
    lock();
    auto index = std::find(playerList.begin(),playerList.end(),s);
    unlock();
    if(index != playerList.end())
    {
        
        return reinterpret_cast<int>(*index);
    }
    return -1;
}

void Room::addPlayer(Player* s)
{
    lock();
    if(playerList.empty())
    {
        playerList.push_back(s);
        unlock();
        return;
    }
    
    int index = searchPlayer(s->getName());
    if(index != -1)
    {
        playerList.erase(playerList.begin() + index);
        playerList.push_back(s);
    }
    unlock();
}

