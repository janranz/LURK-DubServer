#include"../headers/gamemaster.h"
/* TODO:
    Figure out how to alert others of others. Think... when people join send all. but just keep it to rooms.
    Remove renaming as it's unreliable for client writers.
 */
unsigned int Gamemaster::g_seed = 0;
uint16_t Gamemaster::MAX_BADDIES = 1000;
uint16_t Gamemaster::MAX_STAT = 4000;
int16_t Gamemaster::BASE_HEALTH = 6000;
uint16_t Gamemaster::MAX_ROOMS = 29;
uint16_t Gamemaster::MIN_BADDIES_ROOM = 1;
uint16_t Gamemaster::MAX_BADDIES_ROOM = 4;
uint16_t Gamemaster::AWAKEN_VECTOR_SIZE;
uint16_t Gamemaster::BADDIES_VECTOR_SIZE;
uint16_t Gamemaster::DANGER_VECTOR_SIZE;
uint16_t Gamemaster::DEATH_VECTOR_SIZE;
uint16_t Gamemaster::ERROR_VECTOR_SIZE;
uint16_t Gamemaster::FIGHTHIT_VECTOR_SIZE;
uint16_t Gamemaster::FOOD_VECTOR_SIZE;
uint16_t Gamemaster::HEALTH_VECTOR_SIZE;
uint16_t Gamemaster::LOOT_VECTOR_SIZE;
uint16_t Gamemaster::PVP_VECTOR_SIZE;
uint16_t Gamemaster::ROOMNAMES_VECTOR_SIZE;
uint16_t Gamemaster::SAFEEV_VECTOR_SIZE;
uint16_t Gamemaster::TRANS_VECTOR_SIZE;
uint16_t Gamemaster::WEAPONS_VECTOR_SIZE;
uint16_t Gamemaster::BADDIE_DESC_SIZE;
uint16_t Gamemaster::ROOM_DESC_SIZE;

Gamemaster::Gamemaster()
{
    std::cout << "Hello Gamemaster." << std::endl;
    Gamemaster::g_seed = static_cast<unsigned int>(std::time(NULL));
    std::cout << "Gamemaster: your g_seed: " << g_seed << std::endl;
}

// destruction handling ////////
Gamemaster::~Gamemaster()
{
    for(auto p = MasterPlayerList.begin(); p != MasterPlayerList.end(); ++p)
    {
            std::cout << fmt::format("Deleting: {0}\n",(*p)->charTainer.CHARACTER_NAME);
            delete (*p);
    }
    MasterPlayerList.clear();

    for(Room* p : MasterRoomList)
    {
        for(LURK_CONNECTION* b : p->connectedRooms)
        {
            delete b;
        }
        p->connectedRooms.clear();
        delete p;
    }
    MasterRoomList.clear();
    for( auto t = MasterRoomList.begin(); t != MasterRoomList.end(); ++t)
    {
        (*t)->connectedRooms.clear();
    }
    MasterRoomList.clear();

    std::cout << "Goodbye, Gamemaster." << std::endl;
}

void Gamemaster::ragequit(Player* p)
{// player has quit
    std::string m;
    char msg[REASONABLE];

    if(p->isStarted())
    {
        m = fmt::format("Our beloved {0} has given up!\n They were last seen in Room #{1}\n",
            p->charTainer.CHARACTER_NAME,std::to_string(p->charTainer.CURRENT_ROOM_NUMBER));
        std::cout << m << std::endl;
    } else {
        m = fmt::format("An unknown player has had a change of mind prior to starting and has given up!\n");
    }

    std::cout << fmt::format("Pre Deletion Current Player Count: {0}\n",std::to_string(MasterPlayerList.size()));


    int fd = p->getFD();
    {
        std::lock_guard<std::mutex>lock(GMlock);
        for(auto t = MasterPlayerList.begin(); t != MasterPlayerList.end(); ++t)
        {
            if((*t)->getFD() == fd)
            {
                t = MasterPlayerList.erase(t);
                --t;
            }
        }
        
        for(auto t = MasterRoomList.begin(); t != MasterRoomList.end(); ++t)
        {
            std::lock_guard<std::mutex>lock((*t)->rLock);
            for(auto b = (*t)->playerList.begin(); b != (*t)->playerList.end(); ++b)
            {
                if((*b)->getFD() == fd)
                {
                    b = (*t)->playerList.erase(b);
                    --b;
                }
            }
        }
        if(p != nullptr)
        {
            delete p;
            std::cout << "Player pointer successfully deleted!\n" << std::endl;
        }
        strncpy(msg,m.c_str(),m.length());
        msg[m.length()] = 0;
        for(auto t = MasterPlayerList.begin(); t != MasterPlayerList.end(); ++t)
        {

            GMPM((*t),msg);
        }
    }
    std::cout << fmt::format("Post Deletion Current Player Count: {0}\n",std::to_string(MasterPlayerList.size()));
}

int Gamemaster::fast_rand(void)
{
    Gamemaster::g_seed = (214013*g_seed+2531011);
    return(g_seed>>16)&0x7FFF;
}
// END destruction handling ////////

// START server initialization ////////
bool Gamemaster::buildChatter(int i, std::vector<std::string>::iterator m)
{
    bool checksOut = true;
    switch(i)
    {
        case 0:{c_m.awaken.emplace_back(*m);break;}
        case 1:{c_m.baddies.emplace_back(*m);break;}
        case 2:{c_m.danger.emplace_back(*m);break;}
        case 3:{c_m.death.emplace_back(*m);break;}
        case 4:{c_m.error.emplace_back(*m);break;}
        case 5:{c_m.fightHit.emplace_back(*m);break;}
        case 6:{c_m.food.emplace_back(*m);break;}
        case 7:{c_m.health.emplace_back(*m);break;}
        case 8:{c_m.loot.emplace_back(*m);break;}
        case 9:{c_m.pvp.emplace_back(*m);break;}
        // case 10:{c_m.roomNames.emplace_back(*m);break;}
        case 10:{c_m.safeEv.emplace_back(*m);break;}
        case 11:{c_m.trans.emplace_back(*m);break;}
        case 12:{c_m.weapons.emplace_back(*m);break;}
        case 13:{c_m.baddie_desc.emplace_back(*m);break;}
        case 14:{c_m.room_desc.emplace_back(*m);break;}
        case 15:{c_m.adj.emplace_back(*m);break;}
        case 16:{c_m.noun.emplace_back(*m);break;}
        case 17:{c_m.roomTainer.emplace_back(*m);break;}
        default:{checksOut = false;}
    }
    return checksOut;
}


void Gamemaster::estabSizes()
{
    Gamemaster::AWAKEN_VECTOR_SIZE = c_m.awaken.size();
    Gamemaster::BADDIES_VECTOR_SIZE = c_m.baddies.size();
    Gamemaster::DANGER_VECTOR_SIZE = c_m.danger.size();
    Gamemaster::DEATH_VECTOR_SIZE = c_m.death.size();
    Gamemaster::ERROR_VECTOR_SIZE = c_m.error.size();
    Gamemaster::FIGHTHIT_VECTOR_SIZE = c_m.fightHit.size();
    Gamemaster::FOOD_VECTOR_SIZE = c_m.food.size();
    Gamemaster::HEALTH_VECTOR_SIZE = c_m.health.size();
    Gamemaster::LOOT_VECTOR_SIZE = c_m.loot.size();
    Gamemaster::PVP_VECTOR_SIZE = c_m.pvp.size();
    // Gamemaster::ROOMNAMES_VECTOR_SIZE = c_m.roomNames.size();
    Gamemaster::SAFEEV_VECTOR_SIZE = c_m.safeEv.size();
    Gamemaster::TRANS_VECTOR_SIZE = c_m.trans.size();
    Gamemaster::WEAPONS_VECTOR_SIZE = c_m.weapons.size();
    Gamemaster::BADDIE_DESC_SIZE = c_m.baddie_desc.size();
    Gamemaster::ROOM_DESC_SIZE = c_m.room_desc.size();
}

void Gamemaster::populateSpawner()
{
    
    uint8_t flags = 248;
    uint16_t attack, defense, regen, gold;
    int16_t health;
    for(uint16_t i = 0; i < Gamemaster::MAX_BADDIES; i++)
    {
        //give stats opportunities to roll big;
        uint16_t remaining = Gamemaster::MAX_STAT;
        uint16_t MIN = 500;
        uint16_t MAX = 8000;
        int statRoll = i % 3;
        int dex = fast_rand() % (Gamemaster::BADDIES_VECTOR_SIZE);
        int roll;
        std::string n = c_m.baddies.at(dex);
        
        switch(statRoll)
        {
            case 0:
            {// attack - def - regen |  remaining = ((remaining - roll) > 0) ? (remaining - roll) : 0;
                roll = (fast_rand() % (remaining) + 1);
                attack = roll;
                remaining -= roll;
                if(!(remaining > 0))
                {
                    defense = 0;
                    regen = 0;
                    break; 
                }
                roll = (fast_rand() % (remaining) + 1);
                defense = roll;
                remaining -= roll;
                if(!(remaining > 0))
                {
                    regen = 0;
                    break;
                }
                regen = remaining;
                break;
            }
            case 1:
            {// def - regen - attack
                roll = (fast_rand() % (remaining) + 1);
                defense = roll;
                remaining -= roll;
                if(!(remaining > 0))
                {
                    regen = 0;
                    attack = 0;
                    break; 
                }
                roll = (fast_rand() % (remaining) + 1);
                regen = roll;
                remaining -= roll;
                if(!(remaining > 0))
                {
                    attack = 0;
                    break;
                }
                attack = remaining;
                break;
            }
            case 2:
            {// regen - attack - def
                roll = (fast_rand() % (remaining) + 1);
                regen = roll;
                remaining -= roll;
                if(!(remaining > 0))
                {
                    attack = 0;
                    defense = 0;
                    break; 
                }
                roll = (fast_rand() % (remaining) + 1);
                attack = roll;
                remaining -= roll;
                if(!(remaining > 0))
                {
                    defense = 0;
                    break;
                }
                defense = remaining;
                break;
            }
        }
        health = (fast_rand() % (MAX-MIN + 1) + MIN);
        MIN = 100;
        MAX = UINT16_MAX - 1;
        gold = (fast_rand() % (MAX-MIN + 1) + MIN);
        roll = (fast_rand() % Gamemaster::BADDIE_DESC_SIZE);
        std::string descGrab = c_m.baddie_desc.at(roll);
        uint16_t descLength = descGrab.length();

        Baddie p(n,flags,attack,defense,regen,health,gold,0,descLength,descGrab);
        BDSpawner.emplace_back(p);
    }
    std::cout << "\nSpawner populated! Size of BDSpawner: " << BDSpawner.size() << std::endl;
}

void Gamemaster::craftRoomNames()
{
    std::string finalName;
    int adjSize = c_m.adj.size();
    int nounSize = c_m.noun.size();
    int tainerSize = c_m.roomTainer.size();
    
    for(int i = 0; i < MAX_ROOMS; i++)
    {
    REROLL:
        int dex1 = (fast_rand() % adjSize);
        int dex2 = (fast_rand() % nounSize);
        int dex3 = (fast_rand() % tainerSize);

        finalName = c_m.adj.at(dex1) + " " + c_m.noun.at(dex2) + " " + c_m.roomTainer.at(dex3);
        if(finalName.length() > 30)
            goto REROLL;
        
        c_m.roomNames.emplace_back(finalName);
    }
    std::cout << "\nRoom names generated! Size of room name names: " << c_m.roomNames.size();
}

void Gamemaster::buildRooms()
{
    

    uint16_t roomNumber;
    uint16_t roomDescLength;
    std::string roomName, roomDesc;

    // build portalRoom
    roomNumber = 0;
    roomName = "The Portal Room";
    roomDesc = c_m.room_desc.at(0);
    roomDescLength = roomDesc.length();

    Room* portal = new Room(roomNumber, roomName, roomDescLength, roomDesc);
    MasterRoomList.emplace_back(portal);
    std::cout << fmt::format("C_M.ROOMNAME SIZE: {}\n",std::to_string(c_m.roomNames.size()));
    for(int i = 1; i < MAX_ROOMS; i++)
    {
        roomNumber = i;
        roomName = c_m.roomNames.at(i);
        roomDesc = c_m.room_desc.at(i);
        roomDescLength = roomDesc.length();
        // std::cout << fmt::format("What gives: {0} {1}\n",roomName,std::to_string(roomName.length()));
        Room* r = new Room(roomNumber, roomName, roomDescLength,roomDesc);
        
        MasterRoomList.emplace_back(r);
    }
    // set connections
    char tmp[REASONABLE];
    for(auto t = MasterRoomList.begin(); t != MasterRoomList.end(); ++t)
    {
        if((*t)->room.ROOM_NUMBER != 0)
        {
            LURK_CONNECTION* lc = new LURK_CONNECTION;
            strncpy(lc->ROOM_NAME,(*t)->room.ROOM_NAME,strlen((*t)->room.ROOM_NAME));
            lc->ROOM_NAME[32] = 0;
            lc->ROOM_NUMBER = (*t)->room.ROOM_NUMBER;
            lc->DESC_LENGTH = (*t)->room.DESC_LENGTH;
            strncpy(lc->DESC,(*t)->roomDesc.c_str(),(*t)->room.DESC_LENGTH);
            tmp[(*t)->room.DESC_LENGTH] = 0;
            MasterRoomList.at(0)->setConnectedRooms(lc);
        }
            
    }
    int i = 0;
    for(auto t = MasterRoomList.begin(); t != MasterRoomList.end(); ++t)
    {//LEFT OFF HERE RETURN HERE
        ++i;
        LURK_CONNECTION* cPast = new LURK_CONNECTION;
        strncpy(cPast->ROOM_NAME,MasterRoomList.at(i-1)->room.ROOM_NAME,32);
        cPast->ROOM_NUMBER = MasterRoomList.at(i-1)->room.ROOM_NUMBER;
        cPast->DESC_LENGTH = MasterRoomList.at(i-1)->room.DESC_LENGTH;
        strncpy(cPast->DESC,MasterRoomList.at(i-1)->roomDesc.c_str(),MasterRoomList.at(i-1)->room.DESC_LENGTH);
        MasterRoomList.at(i)->setConnectedRooms(cPast);

        if(i < MasterRoomList.size()-1)
        {
            LURK_CONNECTION* cNext = new LURK_CONNECTION;
            strncpy(cNext->ROOM_NAME,MasterRoomList.at(i+1)->room.ROOM_NAME,32);
            cNext->ROOM_NUMBER = MasterRoomList.at(i+1)->room.ROOM_NUMBER;
            cNext->DESC_LENGTH = MasterRoomList.at(i+1)->room.DESC_LENGTH;
            strncpy(cNext->DESC,MasterRoomList.at(i+1)->roomDesc.c_str(),MasterRoomList.at(i+1)->room.DESC_LENGTH);
            MasterRoomList.at(i)->setConnectedRooms(cNext);
        }
            // if(i != MasterRoomList.size() - 1)
            // {
            //     std::cout << fmt::format("Sanity Connection Room: {0} connects with {1} and {2}\n",
            //     std::to_string(MasterRoomList.at(i)->room.ROOM_NUMBER),
            //     std::to_string(MasterRoomList.at(i)->connectedRooms.at(0)->ROOM_NUMBER),
            //     std::to_string(MasterRoomList.at(i)->connectedRooms.at(1)->ROOM_NUMBER));
            // } else if(i == MasterRoomList.size() - 1)
            // {
            //     std::cout << fmt::format("Sanity Connection Room: {0} connects with {1}\n",
            //     std::to_string(MasterRoomList.at(i)->room.ROOM_NUMBER),
            //     std::to_string(MasterRoomList.at(i)->connectedRooms.at(0)->ROOM_NUMBER));
            // }else {
            //     std::cout << "Error in setting room connections!\n";
            // }
    }

    std::cout << "\nMasterRoomList succeeds! Size: "<< MasterRoomList.size() << std::endl;
}

void Gamemaster::populateRooms()
{
    
    
    
    for(auto t = MasterRoomList.begin(); t != MasterRoomList.end(); ++t)
    {
        int baddieCount = (fast_rand() % (MAX_BADDIES_ROOM - MIN_BADDIES_ROOM) + MIN_BADDIES_ROOM);
        for(int i = 0; i < baddieCount; i++)
        {
            Baddie release = BDSpawner.at((fast_rand() % (BDSpawner.size())));
            release.bTainer.CURRENT_ROOM_NUMBER = (*t)->room.ROOM_NUMBER;
            (*t)->injectBaddie(release);
        }
    }

    BDSpawner.clear();
    std::cout << "\nRooms have been populated! BDSpawner has been deleted. Size: " 
              << BDSpawner.size()
              << std::endl;
    std::cout << "Sanity check - Baddie List Size in a random room: "
              << MasterRoomList.at(0)->baddieList.size()
              << std::endl;
}
// END server initialization ////////

//START network functions ////////
void Gamemaster::GMController(int fd)
{
    Player* p = new Player(fd);
    
    std::string m;
    uint8_t typeCheck = 0;
    LURK_ACCEPT lurk_accept;
    LURK_ERROR lurk_error;
    LURK_GAME lurk_game;
    LURK_VERSION lurk_version;
    ssize_t bytes = 0;

    std::string description = c_m.room_desc.at(0);
    lurk_game.DESC_LENGTH = description.length();

    // send initial messages to client
    
    write(fd,&lurk_version,sizeof(LURK_VERSION));
    write(fd,&lurk_game,sizeof(LURK_GAME));
    bytes = write(fd,description.data(),description.length());

    if(bytes < 0){p->quitPlayer();}

    while(!(p->isValidToon()) && p->isSktAlive())
    {// confirm character
        char tmp[BIG_SIZE];
        bytes = recv(fd, &typeCheck,1,MSG_WAITALL|MSG_PEEK);
        if(bytes < 0){p->quitPlayer();}

        bool checksOut = false;
        
        if( p->isSktAlive() && typeCheck == 10)
        {

            bytes = recv(fd,&p->charTainer,sizeof(LURK_CHARACTER),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else {
                // memset(tmp,0,p->charTainer.DESC_LENGTH + 1);
            }
            bytes = recv(fd,tmp,p->charTainer.DESC_LENGTH,MSG_WAITALL);
            
            if(bytes < 0)
            {
                
                p->quitPlayer();
            }else{
                
                p->desc.assign(tmp);
                
            }
            checksOut = checkStats(p);
            if(checksOut == false)
            {
                actionFail(p,10);
            } else
            {
                // std::cout << "Stats Check Out: " << checksOut << std::endl;
                actionPass(p,10);
                p->setValid();
                p->reflection();
            }
        } else {
                uint8_t trash;
                bytes = recv(fd,&trash,sizeof(uint8_t),MSG_WAITALL);
                if(bytes < 0)
                {
                    p->quitPlayer();
                }else{
                    actionFail(p,trash);
                }
                
        }
    }
    while(!(p->isStarted()))
    {// confirm start
        bytes = recv(fd,&typeCheck,sizeof(typeCheck),MSG_WAITALL|MSG_PEEK);
        if(bytes < 0){p->quitPlayer();}

        if(typeCheck == 6 && p->isSktAlive())
        {
            uint8_t tmp;
            bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }
            actionPass(p,typeCheck);
            p->reflection();
            p->startPlayer();
        } else{
            actionFail(p,24);
        }
    }

    // Any new threads should be considered here.
    // ADD USER TO MASTERPLAYERLIST
    {
        std::lock_guard<std::mutex> lock(GMlock);
        MasterPlayerList.emplace_back(p);
        
    }
    std::cout << fmt::format("{0} successfully added to MasterPlayerList: {1}\n",
        p->charTainer.CHARACTER_NAME,std::to_string(MasterPlayerList.size()));

    // push them into Portal Room and send a self character message
    // p->reflection();
    movePlayer(p,0);
    // MAIN LISTENING LOOP HERE
    while(p->isSktAlive())
    {
        bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK);
        if(bytes < 0){p->quitPlayer();}
        mailroom(p,fd,typeCheck);
        // update client with all characters (consider noisy)
        census(p);
        p->reflection();
    }
    // client most likely closed the socket or some error occured here.
    std::cout<<fmt::format("Lost bytes = recv() comms with peer socket, bytes: {0}\n",std::to_string(bytes));
    ragequit(p);

}
// consider how often we alert people of stuff. might be clogging us up.
void Gamemaster::census(Player* p)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::mutex> lock(GMlock);
        for(auto t = MasterPlayerList.begin(); t != MasterPlayerList.end(); ++t)
        {
            if(p->isSktAlive())
            {
                int fd = p->getFD();
                std::lock_guard<std::mutex> lock(p->pLock);
                if(strcmp((*t)->charTainer.CHARACTER_NAME,p->charTainer.CHARACTER_NAME) != 0)
                {
                    write(fd,&(*t)->charTainer,sizeof(LURK_CHARACTER));
                    bytes = write(fd,(*t)->desc.c_str(),(*t)->charTainer.DESC_LENGTH);
                    if(bytes < 0){p->quitPlayer();}
                }
            } else {break;}
        }
        for(auto t = MasterPlayerList.begin(); t != MasterPlayerList.end(); ++t)
        {
            int fd = (*t)->getFD();
            std::lock_guard<std::mutex> lock((*t)->pLock);
            for(auto b = MasterPlayerList.begin(); b != MasterPlayerList.end(); ++b)
            {
                if(strcmp((*t)->charTainer.CHARACTER_NAME,(*b)->charTainer.CHARACTER_NAME) != 0)
                {
                    write(fd,&(*b)->charTainer,sizeof(LURK_CHARACTER));
                    bytes = write(fd,(*b)->desc.c_str(),(*b)->charTainer.DESC_LENGTH);
                    if(bytes < 0 ){(*b)->quitPlayer();}
                }
            }
        }
    }
}

bool Gamemaster::checkStats(Player* p) //bool Gamemaster::checkStats(Player* p)
{
    /*
        Flags: uint8_t [Alive | J_B | Monster | Started | Read | RES | RES | RES]
        Player (fresh spawn): 0b11001000

    */
    // check name conflicts
    if(!p->isSktAlive())
    {
        return false;
    }
    {
        std::lock_guard<std::mutex>lock(GMlock);
        for(auto t = MasterPlayerList.begin(); t != MasterPlayerList.end(); ++t)
        {
            if(strcmp((*t)->charTainer.CHARACTER_NAME,p->charTainer.CHARACTER_NAME) == 0)
            {
                t = MasterPlayerList.begin();
                std::cout << fmt::format("Naming conflict: {}\n",p->charTainer.CHARACTER_NAME);
                return false;
            }
        }
    }
    uint32_t stat = p->charTainer.ATTACK + p->charTainer.DEFENSE + p->charTainer.REGEN;
    uint16_t a = p->charTainer.ATTACK;
    uint16_t d = p->charTainer.DEFENSE;
    uint16_t r = p->charTainer.REGEN;

    p->charTainer.HEALTH = BASE_HEALTH;
    uint16_t dif = MAX_STAT - stat;
    if((stat > MAX_STAT))
    {
        std::cout << fmt::format("Inappropriate stat: {}\n",stat);
        return false;
    }else if((a < MAX_STAT) && ((a <= d) && (a <= r)))
    {
        a = dif;
    }else if((a < MAX_STAT) && ((d <= a) && (d <= r)))
    {
        d = dif;
    }
    else{
        r = dif;
    }

    p->charTainer.FLAGS = 0b11001000;
    p->charTainer.GOLD = (fast_rand() & ((4200 - 69) + 69));
    p->charTainer.CURRENT_ROOM_NUMBER = 0;
    return true;

}

void Gamemaster::GMPM(Player* p, char* m)
{
    GM_MSG gm;
    int fd = p->getFD();
    gm.MSG_LEN = strlen(m);
    ssize_t bytes = 0;
    strncpy(gm.CEIVER_NAME,p->charTainer.CHARACTER_NAME,32);
    {
        std::lock_guard<std::mutex> lock(p->pLock);
        write(fd,&gm,sizeof(GM_MSG));
        bytes = write(fd,m,gm.MSG_LEN);
        if(bytes < 0){p->quitPlayer();}
    }
}

void Gamemaster::movePlayer(Player* p, int newRoom)
{
    if((p->isFreshSpawn()))
    {
        {
            std::lock_guard<std::mutex>lock(MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->rLock);
            MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->removePlayer(p);
        }
        p->despawn();
    }
    p->charTainer.CURRENT_ROOM_NUMBER = newRoom;
    {
        std::lock_guard<std::mutex>lock(MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->rLock);
        MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->addPlayer(p);
    }
}

void Gamemaster::mailroom(Player* p,int fd,int32_t type)
{// process client data (Since mutex is a shared_ptr, try and lock it via MasterPlayerList?)
    // LURK_MSG lurk_msg;
    ssize_t bytes;
    char data[BIG_SIZE];
    // bool leaver = false;
    switch(type)
    {
        case 1:
        {
            LURK_MSG lurk_msg;
            bytes = recv(fd,&lurk_msg,sizeof(LURK_MSG),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else{
                memset(data,0,lurk_msg.MSG_LEN);
            }

            bytes = recv(fd,data,lurk_msg.MSG_LEN,MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else{
                postman(p,lurk_msg,data);
            }
            break;
        }
        case 2:
        {// CHANGEROOM
            LURK_CHANGEROOM changeRoom;
            bool found = false;
            bytes = recv(fd,&changeRoom,sizeof(LURK_CHANGEROOM),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else{
                for(auto t = MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->connectedRooms.begin();
                    t != MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->connectedRooms.end(); ++t)
                {
                    if((*t)->ROOM_NUMBER == changeRoom.ROOM_NUMBER)
                    {
                        actionPass(p,type);
                        movePlayer(p,changeRoom.ROOM_NUMBER);
                        found = true;
                        t = (MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->connectedRooms.end() - 1);
                    }
                }
                if(!found)
                {
                    actionFail(p,type);
                }
                break;
            }
                // std::lock_guard<std::mutex>lock(GMlock);

            //     for(auto t = MasterRoomList.begin(); t != MasterRoomList.end(); ++t)
            //     {
            //         for(auto b = (*t)->connectedRooms.begin(); b != (*t)->connectedRooms.end(); ++b)
            //         {
            //             if((*b)->ROOM_NUMBER == changeRoom.ROOM_NUMBER)
            //             {
            //                 actionPass(p,type);
            //                 movePlayer(p,(*b)->ROOM_NUMBER);
            //                 found = true;
            //                 b = ((*t)->connectedRooms.end() - 1);
            //             }
            //         }
            //         if(found)
            //             t = (MasterRoomList.end() - 1);
            //     }
            // }
            // if(!found){actionFail(p,type);}
        //     break;
        // }
        }
        case 3:
        {// FIGHT
            uint8_t tmp;
            bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            } else {
                // some fight functionality
                int rm = p->charTainer.CURRENT_ROOM_NUMBER;
                int bDex,cDex;
                std::string m;
                {
                    std::lock_guard<std::mutex>lock(MasterRoomList.at(rm)->rLock); // consider where this lock belongs

                    bDex = (fast_rand() % c_m.weapons.size());
                    cDex = (fast_rand() % c_m.food.size());
                    if(MasterRoomList.at(rm)->baddieList.size() > 0)
                    {
                        actionPass(p,type);
                        bDex = (fast_rand() % MasterRoomList.at(rm)->baddieList.size());
                        std::string baddie = MasterRoomList.at(rm)->baddieList.at(bDex).bTainer.CHARACTER_NAME;
                        m = fmt::format("{0} grabs a {1} and pummels {2} over the head, dealing damage!"
                        " as the baddie explodes, he drops some {3}\n",
                        p->charTainer.CHARACTER_NAME ,c_m.weapons.at(bDex),baddie, c_m.food.at(cDex));
                    } else{
                        actionFail(p,type);
                        m = fmt::format("{0}, starts swinging in a fit of rage, but targets absolutely nothing..."
                        " How embarrassing! They calm down and snack on some {1} instead\n",
                        p->charTainer.CHARACTER_NAME,c_m.food.at(cDex));
                    }
                    for(auto t = MasterRoomList.at(rm)->playerList.begin(); t != MasterRoomList.at(rm)->playerList.end(); ++t)
                    {
                        GMPM((*t),m);
                    }
                }
                
            }
            break;
        }
        case 4:
        {// PVP
            LURK_PVP lurk_pvp;
            std::string m;
            bytes = recv(fd,&lurk_pvp,sizeof(LURK_PVP), MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else{
                int rm = p->charTainer.CURRENT_ROOM_NUMBER;
                // int aDex, bDex, cDex;
                bool found = false;
                {
                    std::lock_guard<std::mutex>lock(MasterRoomList.at(rm)->rLock);
                        for(auto t = MasterRoomList.at(rm)->playerList.begin(); t != MasterRoomList.at(rm)->playerList.end(); ++t)
                        {
                            if(strcmp((*t)->charTainer.CHARACTER_NAME,lurk_pvp.TARGET) == 0)
                            { // TARGET AQUIRED
                                m = fmt::format("{0} bops {1} in the head with a {2}!\n",
                                p->charTainer.CHARACTER_NAME,(*t)->charTainer.CHARACTER_NAME,
                                c_m.weapons.at(fast_rand() % c_m.weapons.size()));
                                found = true;
                                break;
                            }
                        }
                        if(found)
                        {
                            actionPass(p,type);
                        } else {
                            actionFail(p,type);
                            m = fmt::format("{0} swings at absolutely nothing because that player doesn't exist!\n",
                            p->charTainer.CHARACTER_NAME);
                        }
                        for(auto t = MasterRoomList.at(rm)->playerList.begin(); t != MasterRoomList.at(rm)->playerList.end(); ++t)
                        {
                            GMPM((*t),m);
                        }
                }
            }
            break;
        }
        case 5:
        {// LOOT Player only for now, needs baddies.
            LURK_LOOT lurk_loot;
            std::string m;
            bytes = recv(fd,&lurk_loot,sizeof(LURK_LOOT), MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else{
                int rm = p->charTainer.CURRENT_ROOM_NUMBER;
                // int aDex, bDex, cDex;
                bool found = false;
                {
                    std::lock_guard<std::mutex>lock(MasterRoomList.at(rm)->rLock);
                        for(auto t =  MasterRoomList.at(rm)->playerList.begin(); t != MasterRoomList.at(rm)->playerList.end(); ++t)
                        {
                            if(strcmp((*t)->charTainer.CHARACTER_NAME,lurk_loot.TARGET) == 0)
                            { // TARGET AQUIRED
                                m = fmt::format("{0} dips their hands into {1}'s pockets! {1} reaches for a {2} and"
                                " swings at {0}, but misses miserably.\n",
                                p->charTainer.CHARACTER_NAME,(*t)->charTainer.CHARACTER_NAME,
                                c_m.weapons.at(fast_rand() % c_m.weapons.size()));
                                found = true;
                                t = (MasterRoomList.at(rm)->playerList.end() - 1);
                            }
                        }
                        if(found)
                        {
                            actionPass(p,type);
                        } else {
                            actionFail(p,type);
                            m = fmt::format("{0} dips their hands into their own pockets as they fail to find {1}!\n",
                            p->charTainer.CHARACTER_NAME,lurk_loot.TARGET);
                        }
                        for(auto t = MasterRoomList.at(rm)->playerList.begin(); t != MasterRoomList.at(rm)->playerList.end(); ++t)
                        {
                            GMPM((*t),m);
                        }
                }
            }
            break;
        }
        case 6:
        {// START
            uint8_t tmp;
            bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            }else{
                actionPass(p,type);
                std::string m = fmt::format("You reach into your pocket and pull out a glowy orb...\n"
                "As you smash it on the ground, a portal opens and you return to the Portal room!\n");
                GMPM(p,m);
                movePlayer(p,0);
            }
            break;
        }
        case 10:
        {// CHARACTER (bitset check state of JOIN_BATTLE ONLY)
            LURK_CHARACTER lurk_char;
            bytes = recv(fd,&lurk_char,sizeof(LURK_CHARACTER),MSG_WAITALL);
            if(bytes < 0)
            {
                p->quitPlayer();
            } else {
                std::bitset<8>newFlag{lurk_char.FLAGS};
                std::bitset<8>currFlag{p->charTainer.FLAGS};
                
                (newFlag[6]) ? currFlag.set(6,true) : currFlag.set(6,false);
                
                std::cout << fmt::format("{0} Is attempting to set their JB flag (before): {1}",
                p->charTainer.CHARACTER_NAME,p->charTainer.FLAGS);
                p->charTainer.FLAGS = static_cast<uint8_t>(currFlag.to_ulong());
                std::cout << fmt::format("{0} Has successfully set their JB Flag: {1}",
                p->charTainer.CHARACTER_NAME,p->charTainer.FLAGS);
                actionPass(p,type);
            }
            break;
        }
        case 12:
        {// LEAVE (graceful)
            uint8_t tmp;
            bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            actionPass(p,type);
            if(p->isSktAlive())
            {
                std::string m = fmt::format("You have chosen to leave gracefully, and as the server owner, I salute you, {0}\n"
                "A portal opens up, a bright light blasts you in the face, and you make good your escape. Goodbye!\n",
                p->charTainer.CHARACTER_NAME);
                GMPM(p,m);
            }
            p->quitPlayer();
            break;
        }
        default:
        {
            uint8_t tossByte;
            bytes = recv(fd,&tossByte,sizeof(uint8_t),MSG_WAITALL);
            std::cout << fmt::format("{0} sent a bad type: {1}",p->charTainer.CHARACTER_NAME,std::to_string(tossByte));
            
            actionFail(p,tossByte);
        }
    }
}

void Gamemaster::postman(Player* p,LURK_MSG lurk_msg,char* data)
{// consider if a lock should be implemented, protecting MastPlayerList from manip
    ssize_t bytes = 0;
    {
        std::lock_guard<std::mutex> lock(GMlock);
        for(auto t = MasterPlayerList.begin(); t != MasterPlayerList.end(); ++t)
        {
            if(strcmp((*t)->charTainer.CHARACTER_NAME,lurk_msg.CEIVER_NAME) == 0)
            {
                {
                    std::lock_guard<std::mutex>lock((*t)->pLock);
                    write((*t)->getFD(),&lurk_msg,sizeof(LURK_MSG));
                    bytes = write((*t)->getFD(),data,lurk_msg.MSG_LEN);
                }
                actionPass(p,1);
                if(bytes < 0){(*t)->quitPlayer();}
                t = (MasterPlayerList.end() - 1);
            } else {
                actionFail(p,26);
            }
        }
    }
    
}

void Gamemaster::actionFail(Player* p, uint8_t type)
{
    std::string m;
    ssize_t bytes = 0;
    LURK_ERROR pkg;
    if(!p->isSktAlive())
        return;

    switch(type)
    {
        case 2:
        {// failed changeRoom
            pkg.CODE = 1;
            int dex = (fast_rand() % c_m.weapons.size());
            m = fmt::format("\nYou attempt to pass through to the next room, but you trip over a {0} instead!\n",c_m.weapons.at(dex));
            pkg.MSG_LEN = m.length();
            {
                std::lock_guard<std::mutex>lock(p->pLock);
                write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
            }
            if(bytes < 0){p->quitPlayer();}
            break;
        }
        case 3:
        {// failed fight
            pkg.CODE = 7;
            int dex = (fast_rand() % c_m.food.size());
            m = fmt::format("\nYou swing with all your might, but nobody is here! You eat some {0} instead! Tasty.\n",c_m.food.at(dex));
            pkg.MSG_LEN = m.length();
            {
                std::lock_guard<std::mutex>lock(p->pLock);
                write(p->getFD(), &pkg,sizeof(LURK_ERROR));
                bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
            }
            if(bytes < 0){p->quitPlayer();}
            break;
        }
        case 5:
        {// failed loot monster
            pkg.CODE = 3;
            int size = MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->baddieList.size(); // will need lock if spawning
            if(p->isStarted() && size != 0)
            {
                std::string baddie = MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->baddieList
                    .at(fast_rand() % size)
                    .bTainer.CHARACTER_NAME;
                m = fmt::format("\nYou attempt to loot that baddie, but {0} catches you in the act. YIKES!\n",baddie);
                pkg.MSG_LEN = m.length();
                {
                    std::lock_guard<std::mutex>lock(p->pLock);
                    write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                    bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
                }
                
            } else {
                m = fmt::format("\nYou attempt to loot that baddie, but have a change of mind... probably because he isn't there.\n");
                pkg.MSG_LEN = m.length();
                {
                    std::lock_guard<std::mutex>lock(p->pLock);
                    write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                    bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
                }
            }
            if(bytes < 0){p->quitPlayer();}
            break;
        }
        case 10:
        {// failed stat check
            pkg.CODE = 4;
            int dexOne = (fast_rand() % c_m.weapons.size());
            int dexTwo = (fast_rand() % c_m.food.size());
            m = fmt::format("\nYou successfully get away with inappropriate stats," 
                "but you trip on a {0} while you gobble down some {1}.. resetting your stats to 0!\n",
                c_m.weapons.at(dexOne),c_m.food.at(dexTwo));
            pkg.MSG_LEN = m.length();

            {
                std::lock_guard<std::mutex>lock(p->pLock);
                write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
            }
            if(bytes < 0){p->quitPlayer();}
            break;
        }
        case 24:
        {// failed not ready
            pkg.CODE = 5;
            m = fmt::format("\nYou attempt to do something wild, but you're just not ready yet.\n");
            pkg.MSG_LEN = m.length();
            {
                std::lock_guard<std::mutex>lock(p->pLock);
                write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
            }
            if(bytes < 0){p->quitPlayer();}
            break;
        }
        case 26:
        {// failed player target
            pkg.CODE = 6;
            m = fmt::format("\nYou try to call out a name, but either you're mistaken... or nobody is here.\n");
            pkg.MSG_LEN = m.length();
            {
                std::lock_guard<std::mutex>lock(p->pLock);
                write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
            }
            if(bytes < 0){p->quitPlayer();}
            break;    
        }
        default:
        {
            pkg.CODE = 0;
            m = fmt::format("\nYou attempt something silly, and silly you did..."
                "but checking the protocol, you see the error of your ways <Sent Type: {0}>.\n",std::to_string(type));
            pkg.MSG_LEN = m.length();
            {
                std::lock_guard<std::mutex>lock(p->pLock);
                write(p->getFD(),&pkg,sizeof(LURK_ERROR));
                bytes = write(p->getFD(),m.c_str(),pkg.MSG_LEN);
            }
            if(bytes < 0){p->quitPlayer();}
        }
        
    }
}

void Gamemaster::actionPass(Player* p, uint8_t type)
{
    if(p->isSktAlive())
    {
        ssize_t bytes = 0;
        LURK_ACCEPT pkg;
        pkg.ACCEPT_TYPE = type;
        std::lock_guard<std::mutex>lock(p->pLock);
        bytes = write(p->getFD(),&pkg,sizeof(LURK_ACCEPT));
        if(bytes < 0){p->quitPlayer();}
    }

}

//END network functions ////////

