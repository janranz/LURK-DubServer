#include"../headers/gamemaster.h"

Room::Room(uint16_t num,std::string name ,uint16_t roomDescLen ,std::string roomD)
{
    // roomNumber = num;
    // roomName = name;
    // roomDescLength = roomDescLen;
    
    stress_level = 0;
    rLock = std::make_shared<std::mutex>();
    strncpy(room.ROOM_NAME,name.c_str(),sizeof(room.ROOM_NAME));
    room.ROOM_NUMBER = num;
    room.DESC_LENGTH = roomDescLen;
    roomDesc = roomD;
    std::cout << "Room created! Name: " << room.ROOM_NAME << std::endl;
}   
Room::~Room()
{
    for(auto t: connectedRooms)
    {
        delete t;
    }
    connectedRooms.clear();
    std::cout << "Connected Rooms Destructed!" << std::endl;
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
    
    sendRoomInfo(p);
    sendBaddieInfo();
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
    }
}

void Room::sendRoomInfo(Player* p)
{
    {
        std::lock_guard<std::mutex> lock(*p->pLock);
        for(auto t : connectedRooms)
        {
            // write(p->socketFD,&t,sizeof(LURK_CONNECTION));
            write(p->socketFD,&t->TYPE,sizeof(uint8_t));
            write(p->socketFD,&t->ROOM_NUMBER,sizeof(uint16_t));
            write(p->socketFD,t->ROOM_NAME,32);
            write(p->socketFD,&t->DESC_LENGTH,sizeof(uint16_t));
            write(p->socketFD,t->DESC,t->DESC_LENGTH);
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
                std::cout << fmt::format("Baddie In Room: {}\n",b.bTainer.CHARACTER_NAME);
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
