#include"../headers/gamemaster.h"

Room::Room(uint16_t num,std::string name ,uint16_t roomDescLen ,std::string roomD)
{
    // roomNumber = num;
    // roomName = name;
    // roomDescLength = roomDescLen;
    
    stress_level = 0;
    rLock = std::make_shared<std::mutex>();
    strncpy(room.ROOM_NAME,name.c_str(),32);
    room.ROOM_NUMBER = num;
    room.DESC_LENGTH = roomDescLen;
    roomDesc = roomD;
}   
Room::~Room()
{

}

void Room::setConnectedRooms(LURK_CONNECTION* r)
{
    connectedRooms.emplace_back(r);    
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
    std::cout << fmt::format("<{0}> has joined {1}\n",p->charTainer.CHARACTER_NAME,room.ROOM_NAME);
    
    sendBaddieInfo();
    sendRoomInfo(p);
    // sendBaddieInfo();
}

void Room::removePlayer(Player* p)
{
    {
        std::lock_guard<std::mutex> lock(*rLock);
        int i = 0;
        for(auto t: playerList)
        {
            
            if(strcmp(p->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME) == 0)
            {
                std::cout << fmt::format("<{0}> has left {1}\n",p->charTainer.CHARACTER_NAME,room.ROOM_NAME);
                playerList.erase(playerList.begin() + i);
                break;
            }
            i++;
        }
    sendBaddieInfo();
    sendRoomInfo(p);
    // sendBaddieInfo();
    }
}

void Room::sendRoomInfo(Player* p)
{
    {
        std::lock_guard<std::mutex> lock(*p->pLock);
        for(auto t : connectedRooms)
        {
            write(p->socketFD,&t->room,sizeof(LURK_ROOM));
            write(p->socketFD,t->roomDesc.c_str(),t->room.DESC_LENGTH);
        }
    }
}

void Room::sendBaddieInfo()
{
    {
        std::lock_guard<std::mutex> lock(*rLock);
        for(auto t: playerList)
        {
            for(auto b: baddieList)
            {
                std::lock_guard<std::mutex> lock(*t->pLock);
                write(t->socketFD,&b.bTainer,sizeof(LURK_CHARACTER));
                write(t->socketFD,b.description.c_str(),b.bTainer.DESC_LENGTH);
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
