#include"../headers/gamemaster.h"

Room::Room(uint16_t num,std::string name ,uint16_t roomDescLen ,std::string roomD)
{
    roomNumber = num;
    roomName = name;
    roomDescLength = roomDescLen;
    roomDesc = roomD;
    stress_level = 0;
    rLock = std::make_shared<std::mutex>();
}
Room::~Room()
{

}

std::string Room::DEBUG_getRoomname()
{
    return roomName;
}

int Room::DEBUG_getBaddieListSize()
{
    for(auto t:baddieList)
    {
        std::cout << fmt::format("\nBaddie Name: {}\n",t.getName());
    }
    return baddieList.size();
}

int Room::DEBUG_getRoomNumber()
{
    return roomNumber;
}

std::vector<uint16_t> Room::DEBUG_getConnected()
{
    return connectedRoomNums;
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

void Room::injectBaddie(Baddie s)
{// initial injection of Baddies. They are to remain here for eternity (or fatal error lol)
    baddieList.push_back(s);
}

void Room::addPlayer(Player* p)
{
    {
        std::lock_guard<std::mutex> lock(*rLock);
        playerList.emplace_back(p);
    }
    std::cout << p->charTainer.CHARACTER_NAME << " has joined Room: " << roomNumber << std::endl;
    sendRoomInfo();
}

void Room::removePlayer(Player* p)
{
    {
        std::lock_guard<std::mutex> lock(*rLock);
        int i = 0;
        for(auto& t: playerList)
        {
            
            if(strcmp(p->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME) == 0)
            {
                std::cout << fmt::format("{}: Player: {} found! Removing.",roomNumber,p->charTainer.CHARACTER_NAME);
                playerList.erase(playerList.begin() + i);
                break;
            }
            i++;
        }
    }
}

void Room::sendRoomInfo()
{
    int len = 0;
    // std::string s;
    {
        std::lock_guard<std::mutex> lock(*rLock);
        for(auto& t: playerList)
        {
            {
                std::lock_guard<std::mutex> lock(*t->pLock); // might crash
                for(auto& p: playerList)
                {
                    // s = p.get().desc.c_str();

                    write(t->socketFD,&p->charTainer,sizeof(LURK_CHARACTER));
                    write(t->socketFD,p->desc.c_str(),p->charTainer.DESC_LENGTH); // functional!
                }

                //Okay so... you might re-work baddies. check it out.
                
                // for(auto b: baddieList)
                // {
                //     write(t.get().socketFD,&b.,sizeof(LURK_CHARACTER));
                // }
            }
        }
    }
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
