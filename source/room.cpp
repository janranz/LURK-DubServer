#include"../headers/room.h"

Room::Room(uint16_t num,std::string name ,uint16_t roomDescLen ,std::string roomD)
{
    roomNumber = num;
    roomName = name;
    roomDescLength = roomDescLen;
    roomDesc = roomD;
    stress_level = 0;
    
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

int Room::searchPlayer(std::string const&s)
{
    int i = 0;
    lock();
    for(auto &t : playerList)
    {
        if(t->getName() == s)
        {
            unlock();
            return i;
        }
    }
    unlock();
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

void Room::setConnectedRooms(char p, uint16_t m)
{
    switch(p)
    {
        case 'p':
        {
            connectedRoomNums.push_back(m); // portal room takes all
            break;
        }
        case 'n':
        {
            connectedRoomNums.push_back(m - 1);
            connectedRoomNums.push_back(m + 1); // Normal room: to-fro
            break;
        }
        case 'l':
        {
            connectedRoomNums.push_back(m - 1); // last room takes last only.
            break;
        }
    }
    
}

void Room::setStressLevel(char s)
{// 0 - 10 [passive / torment]
    stress_level = s;
}

void Room::injectBaddie(Baddie* s)
{// initial injection of Baddies. They are to remain here for eternity (or fatal error lol)
    baddieList.push_back(*s);
}

    // std::string a = s.getName();
    // uint8_t b = s.getFlags();
    // uint16_t c = s.getAttack();
    // uint16_t d = s.getDefense();
    // uint16_t e = s.getRegen();
    // int16_t f = s.getHealth();
    // uint16_t g = s.getGold();
    // uint16_t h = s.getRoomNumber();
    // uint16_t i = s.getDescriptionLength();
    // std::string j = s.getDescription();

    // Baddie* p = new Baddie(a,b,c,d,e,f,g,h,i,j);
