#include"../headers/gamemaster.h"

Room::Room(uint16_t num,std::string name ,uint16_t roomDescLen ,std::string roomD)
{
    
    stress_level = 0;
    // rLock = std::make_shared<std::mutex>();
    // strncpy(room.ROOM_NAME,name.c_str(),sizeof(room.ROOM_NAME));
    strncpy(room.ROOM_NAME,name.c_str(),32);
    room.ROOM_NUMBER = num;
    room.DESC_LENGTH = roomDescLen;
    roomDesc = roomD;
}   
Room::~Room()
{
    // for(auto t: connectedRooms) Possibly GM's responsibility
    // {
    //     delete t;
    // }
    // connectedRooms.clear();
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
        std::lock_guard<std::mutex> lock(rLock);
        playerList.emplace_back(p);
    }
    std::cout << fmt::format("<{0}> has joined {1}\n",p->charTainer.CHARACTER_NAME,room.ROOM_NAME);
    
    sendRoomInfo(p);
    if(p->isSktAlive())
        sendBaddieInfo();
    
}

void Room::removePlayer(Player* p)
{
    ssize_t bytes = 0;
    
    std::string m = fmt::format("A bright light flashes as {0} has stepped into a portal left the room..\nWill you follow them?\n",p->charTainer.CHARACTER_NAME);

    GM_MSG pkg;
    pkg.MSG_LEN = m.length();

    {
        std::lock_guard<std::mutex> lock(rLock);
        int i = 0;
        for(auto t: playerList)
        {
            
            if(strcmp(p->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME) == 0)
            {
                std::cout << fmt::format("<{0}> has left {1}\n",p->charTainer.CHARACTER_NAME,room.ROOM_NAME);
                playerList.erase(playerList.begin() + i);
            }
            
            strncpy(pkg.CEIVER_NAME,t->charTainer.CHARACTER_NAME,32);
            {
                int fd = t->getFD();
                std::lock_guard<std::mutex> lock(t->pLock);
                write(fd,&pkg,sizeof(GM_MSG));
                bytes = write(fd,m.c_str(),pkg.MSG_LEN);
                if(bytes < 0){t->quitPlayer();}
            }
            i++;
        }
    }
}

// void Room::sendFriendInfo()
// {
//     ssize_t bytes;
//     {
//         std::lock_guard<std::mutex> lock(*rLock);
//         for(auto t : playerList)
//         {
//             for(auto b: playerList)
//             {
//                 std::lock_guard<std::mutex> lock(t->pLock);
//                 if(bytes = write(t->socketFD,&b->charTainer,sizeof(LURK_CHARACTER)) < 0){return bytes;}
//                 if(bytes =write(t->socketFD,b->desc.c_str() ,b->charTainer.DESC_LENGTH) < 0){return bytes;}
//             }
//         }
//     }
// }

void Room::sendRoomInfo(Player* p)
{
    ssize_t bytes = 0;
    int fd = p->getFD();
    {
        std::lock_guard<std::mutex> lock(p->pLock);
        for(auto t : connectedRooms)
        {
            std::cout << fmt::format("DEBUG: Connected Room Name: {0} {1}\n",t->ROOM_NAME,std::to_string(t->ROOM_NUMBER));
            write(fd,&t->TYPE,sizeof(uint8_t));
            write(fd,&t->ROOM_NUMBER,sizeof(uint16_t));
            write(fd,&t->ROOM_NAME,32);
            write(fd,&t->DESC_LENGTH,sizeof(uint16_t));
            bytes = write(fd,t->DESC,t->DESC_LENGTH);
            if(bytes < 0){p->quitPlayer();break;}
        }
        if(p->isSktAlive())
        {
            write(fd,&room,sizeof(LURK_ROOM));
            bytes = write(fd,roomDesc.c_str(),room.DESC_LENGTH);
            if(bytes < 0){p->quitPlayer();}
        }
    }
}

void Room::sendBaddieInfo()
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::mutex> lock(rLock);
        for(auto t = playerList.begin(); t != playerList.end(); ++t)
        {
            for(auto b: baddieList)
            {
                int fd = (*t)->getFD();
                std::cout << fmt::format("Baddie In Room: {}\n",b.bTainer.CHARACTER_NAME);
                std::lock_guard<std::mutex> lock((*t)->pLock);
                write(fd,&b.bTainer,sizeof(LURK_CHARACTER));
                bytes = write(fd,b.description.c_str(),b.bTainer.DESC_LENGTH);
                if(bytes < 0){(*t)->quitPlayer();}
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
