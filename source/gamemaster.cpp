#include"../headers/gamemaster.h"

unsigned int Gamemaster::g_seed = 0;
uint16_t Gamemaster::MAX_BADDIES = 1000;
uint16_t Gamemaster::MAX_STAT = 4000;
int16_t Gamemaster::BASE_HEALTH = 6000;
uint16_t Gamemaster::MAX_ROOMS = 4;
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
    for(auto p : MasterPlayerList)
    {
        if(p != nullptr)
            delete p;
    }

    MasterPlayerList.clear();

    for(auto p : MasterRoomList)
    {
        if(p != nullptr)
            delete p;
    }
        
    MasterRoomList.clear();

    std::cout << "Goodbye, Gamemaster." << std::endl;
}

void Gamemaster::ragequit(Player* p)
{// player has quit
    std::cout << fmt::format("<{0} has ragequit from room: {1}>\n",
        p->charTainer.CHARACTER_NAME,std::to_string(p->charTainer.CURRENT_ROOM_NUMBER));
    std::string m = fmt::format("Our beloved <{0}> has given up and ragequit!\n She were last seen in Room #<{1}>\n",
        p->charTainer.CHARACTER_NAME,std::to_string(p->charTainer.CURRENT_ROOM_NUMBER));
    
    printf("MasterPlayerSize (pre-removal): %lu\n",MasterPlayerList.size());

    p->quitter = true;
    int i = 0;
    {
        std::lock_guard<std::mutex>lock(GMlock);
        for(auto t:MasterPlayerList)
        {
            if(p->socketFD == t->socketFD)
            {
                MasterPlayerList.erase(MasterPlayerList.begin() + i);
                p->inMaster = false;
                break;
            }
            i++;
        }
        
        for(auto t:MasterRoomList)
        {
            i = 0;
            std::lock_guard<std::mutex>lock(*t->rLock);
            for(auto b : t->playerList)
            {
                if(p->socketFD == b->socketFD)
                {
                    t->playerList.erase(t->playerList.begin() + i);
                    break;
                }
                i++;
            }
        }
        if(p != nullptr)
            delete p;
        for(auto t:MasterPlayerList)
        {
            if(t->inMaster)
                GMPM(t,m);
        }
    }    
        std::cout << fmt::format("Current Player Count: {0}",std::to_string(MasterPlayerList.size()));
        printf("MasterPlayerSize (post-removal): %lu\n",MasterPlayerList.size());
    

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
    for(auto t : MasterRoomList)
    {
        if(t->room.ROOM_NUMBER != 0)
        {
            LURK_CONNECTION* lc = new LURK_CONNECTION;
            strncpy(lc->ROOM_NAME,t->room.ROOM_NAME,32);
            lc->ROOM_NUMBER = t->room.ROOM_NUMBER;
            lc->DESC_LENGTH = t->room.DESC_LENGTH;
            strncpy(lc->DESC,t->roomDesc.c_str(),t->room.DESC_LENGTH + 1);

            MasterRoomList.at(0)->setConnectedRooms(lc);
        }
            
    }
    for(size_t i = 1; i < MasterRoomList.size(); ++i)
    {
        LURK_CONNECTION* cPast = new LURK_CONNECTION;
        strncpy(cPast->ROOM_NAME,MasterRoomList.at(i-1)->room.ROOM_NAME,32);
        cPast->ROOM_NUMBER = MasterRoomList.at(i-1)->room.ROOM_NUMBER;
        cPast->DESC_LENGTH = MasterRoomList.at(i-1)->room.DESC_LENGTH;
        strncpy(cPast->DESC,MasterRoomList.at(i-1)->roomDesc.c_str(),MasterRoomList.at(i-1)->room.DESC_LENGTH);
        MasterRoomList.at(i)->setConnectedRooms(cPast);

        if(i != MasterRoomList.size()-1)
        {
            LURK_CONNECTION* cNext = new LURK_CONNECTION;
            strncpy(cNext->ROOM_NAME,MasterRoomList.at(i+1)->room.ROOM_NAME,32);
            cNext->ROOM_NUMBER = MasterRoomList.at(i+1)->room.ROOM_NUMBER;
            cNext->DESC_LENGTH = MasterRoomList.at(i+1)->room.DESC_LENGTH;
            strncpy(cNext->DESC,MasterRoomList.at(i+1)->roomDesc.c_str(),MasterRoomList.at(i+1)->room.DESC_LENGTH);
            MasterRoomList.at(i)->setConnectedRooms(cNext);
        }
            if(i != MasterRoomList.size() - 1)
            {
                std::cout << fmt::format("Sanity Connection Room: {0} connects with {1} and {2}\n",
                std::to_string(MasterRoomList.at(i)->room.ROOM_NUMBER),
                std::to_string(MasterRoomList.at(i)->connectedRooms.at(0)->ROOM_NUMBER),
                std::to_string(MasterRoomList.at(i)->connectedRooms.at(1)->ROOM_NUMBER));
            } else if(i == MasterRoomList.size() - 1)
            {
                std::cout << fmt::format("Sanity Connection Room: {0} connects with {1}\n",
                std::to_string(MasterRoomList.at(i)->room.ROOM_NUMBER),
                std::to_string(MasterRoomList.at(i)->connectedRooms.at(0)->ROOM_NUMBER));
            }else {
                std::cout << "Error in setting room connections!\n";
            }
    }

    std::cout << "\nMasterRoomList succeeds! Size: "<< MasterRoomList.size() << std::endl;
}

void Gamemaster::populateRooms()
{
    
    
    
    for(auto t: MasterRoomList)
    {
        
        int baddieCount = (fast_rand() % (MAX_BADDIES_ROOM - MIN_BADDIES_ROOM) + MIN_BADDIES_ROOM);
        for(int i = 0; i < baddieCount; i++)
        {
            Baddie release = BDSpawner.at((fast_rand() % (BDSpawner.size())));
            release.bTainer.CURRENT_ROOM_NUMBER = t->room.ROOM_NUMBER;
            t->injectBaddie(release);
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
    int32_t typeCheck = 0;
    LURK_ACCEPT lurk_accept;
    LURK_ERROR lurk_error;
    LURK_GAME lurk_game;
    LURK_VERSION lurk_version;
    ssize_t bytes; // consider cnd-checking while loops

    std::string description = c_m.room_desc.at(0);
    lurk_game.DESC_LENGTH = description.length();

    // send initial messages to client
    
    if(write(fd,&lurk_version,sizeof(LURK_VERSION)) == -1){ragequit(p);return;}

    if(write(fd,&lurk_game,sizeof(LURK_GAME)) == -1){ragequit(p);return;}
    if(write(fd,description.data(),description.length()) == -1){ragequit(p);return;}

    // wait for character message. We do this manually here
    bool accepted = false; //user has quit.
    while((!accepted))
    {// confirm character
        bytes = recv(fd, &typeCheck,1,MSG_WAITALL|MSG_PEEK);
        if(bytes < 0)
        {
            if(p != nullptr)
                delete p;
            return;
        }
        bool checksOut = false;
        std::cout << "Type: " << typeCheck << std::endl;
        if(typeCheck == 10)
        {

            bytes = recv(fd,&p->charTainer,sizeof(LURK_CHARACTER),MSG_WAITALL);
            if(bytes < 0)
            {
                if(p != nullptr)
                delete p;
                return;
            }
            char tmp[p->charTainer.DESC_LENGTH];
            bytes = recv(fd,tmp,p->charTainer.DESC_LENGTH,MSG_WAITALL);
            if(bytes < 0)
            {
                if(p != nullptr)
                    delete p;
                return;
            }
            tmp[p->charTainer.DESC_LENGTH] = 0;
            p->desc.assign(tmp);
            checksOut = checkStats(p);
            if(checksOut == false)
            {
                // std::cout << "Pump n dumpin' data. Bad stats" << std::endl;
                // memset(dataDump,0,BIG_BUFFER);
                // bytes = recv(fd,dataDump,BIG_BUFFER,MSG_DONTWAIT);
                gatekeeper('d',p,0,4);
                // m = fmt::format("Whoa let's calm down there with the stats, {0}."
                // " If you wanted God Mode, all you had to do was ask. [GOD-MODE ACTIVATED]",p->charTainer.CHARACTER_NAME);
                // GMPM(p,m);
            } else
            {
                LURK_ACCEPT acpt;
                acpt.ACCEPT_TYPE = 10;
                
                if(write(fd,&acpt,sizeof(LURK_ACCEPT)) == -1){ragequit(p); return;}
                // gatekeeper('a',p,10,0);
                accepted = true;
            }
        } else {
            gatekeeper('d',p,0,0);
        }
    }
    accepted = false;
    while(!accepted)
    {// confirm start
        bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK);
        if(bytes < 0)
        {
            if(p != nullptr)
                delete p;
            return;
        }

        if(typeCheck == 6)
        {
            uint8_t tmp;
            bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            if(bytes < 0)
            {
                if(p != nullptr)
                    delete p;
                return;
            }
            std::cout << "User has started!: Type(" << typeCheck << ")" << std::endl;
            LURK_ACCEPT acpt;
            acpt.ACCEPT_TYPE = 6;
            if(write(fd,&acpt,sizeof(LURK_ACCEPT)) == -1){ragequit(p); return;}
            accepted = true;
        } else{
            std::cout << "User has NOT started!: Type(" << typeCheck << ")" << std::endl;
            gatekeeper('d',p,0,5);
        }
    }
    // Any new threads should be considered here.
    // ADD USER TO MASTERPLAYERLIST
    std::cout << p->charTainer.CHARACTER_NAME << " checks out!" << std::endl;
    {
        std::lock_guard<std::mutex> lock(GMlock);
        MasterPlayerList.emplace_back(p);
        
    }
    p->inMaster = true;
    printf("Player successfully added to Master: %lu\n",MasterPlayerList.size());

    // push them into Portal Room
    movePlayer(p,0);

    // MAIN LISTENING LOOP HERE
    while(!(p->quitter))
    {
        bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK);
        if(bytes < 0)
        {
            ragequit(p);
            continue;
        }
        std::cout << "TypeCheck: " << typeCheck << std::endl;
        mailroom(p,fd,typeCheck);
        // update client with all characters (consider noisy)
        census(p);

    }
    // client most likely closed the socket or some error occured here.
    printf("Lost bytes = recv() comms with peer socket, bytes: %lu\n",bytes);
    ragequit(p);
}

void Gamemaster::census(Player* p)
{

    std::cout << fmt::format("Updating <{0}> with character list\n",p->charTainer.CHARACTER_NAME);
    {
        std::lock_guard<std::mutex> lock(GMlock);
        for(auto t: MasterPlayerList)
        {
            std::lock_guard<std::mutex> lock(*p->pLock);
            if(write(p->socketFD,&(t->charTainer),sizeof(LURK_CHARACTER)) == -1){ragequit(p);return;}
            if(write(p->socketFD,t->desc.c_str(),t->charTainer.DESC_LENGTH) == -1){ragequit(p);return;}
        }
    }
    
}

bool Gamemaster::checkStats(Player* p) //bool Gamemaster::checkStats(Player* p)
{
    // check name conflicts
    std::cout << "Checking stats" << std::endl;
    RECHECK:
    for(auto t : MasterPlayerList)
    {
        if(strcmp(t->charTainer.CHARACTER_NAME,p->charTainer.CHARACTER_NAME) == 0)
        {
            // p.charTainer.CHARACTER_NAME[32] = 0;
            std::string base(p->charTainer.CHARACTER_NAME);
            std::string suff= std::to_string(((fast_rand() % 4000)));
            std::string full = base + suff;
            strncpy(p->charTainer.CHARACTER_NAME,full.c_str(),32);
            std::cout << "Name changed to: " << p->charTainer.CHARACTER_NAME << std::endl;
            goto RECHECK;
        }
    }
    std::cout << p->charTainer.ATTACK << p->charTainer.DEFENSE << p->charTainer.REGEN << std::endl;
    uint32_t stat = p->charTainer.ATTACK + p->charTainer.DEFENSE + p->charTainer.REGEN;
    
    p->charTainer.HEALTH = BASE_HEALTH;
    if((stat > MAX_STAT))
    {
        std::cout << "Inappropriate stats: " << stat << std::endl;
        return false;
    } else if(stat < MAX_STAT)
    {
        uint32_t dif = MAX_STAT - stat;
        p->charTainer.HEALTH += dif;
    }
    p->charTainer.FLAGS = 0b11001000; // 200
    p->charTainer.GOLD = (fast_rand() & ((4200 - 69) + 69));
    p->charTainer.CURRENT_ROOM_NUMBER = 0;
    std::cout << "Appropriate stats: " << stat << std::endl;
    return true;

}

void Gamemaster::GMPM(Player* p, std::string& msg)
{
    GM_MSG gm;
    gm.MSG_LEN = msg.length();
    ssize_t bytes = 0;
    strncpy(gm.CEIVER_NAME,p->charTainer.CHARACTER_NAME,32);
    {
        std::lock_guard<std::mutex> lock(*p->pLock);
        bytes = write(p->socketFD,&gm,sizeof(gm));
        if(bytes < 0)
        {
            ragequit(p);
            return;
        }
        bytes =write(p->socketFD,msg.c_str(),gm.MSG_LEN);
        if(bytes < 0)
        {
            ragequit(p);
            return;
        }
    }
}

void Gamemaster::movePlayer(Player* p, int newRoom)
{
    MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->removePlayer(p);
    p->charTainer.CURRENT_ROOM_NUMBER = newRoom;
    MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->addPlayer(p);
}

void Gamemaster::mailroom(Player* p,int fd,int32_t type)
{// process client data (Since mutex is a shared_ptr, try and lock it via MasterPlayerList?)
    // LURK_MSG lurk_msg;
    ssize_t bytes;
    uint8_t trashBit;
    // bool leaver = false;
    switch(type)
    {
        case 1:
        {// MESSAGE (REWORK THIS)
            LURK_MSG lurk_msg;
           if(bytes = recv(fd,&lurk_msg,sizeof(LURK_MSG),MSG_WAITALL) < 0){ragequit(p);break;}
            char data[lurk_msg.MSG_LEN];
            if(bytes = recv(fd,data,lurk_msg.MSG_LEN,MSG_WAITALL) < 0){ragequit(p);break;}
            data[lurk_msg.MSG_LEN] = 0;
            postman(p,lurk_msg,data);
            // std::string s(data, lurk_msg.MSG_LEN);
            break;
        }
        case 2:
        {// CHANGEROOM
            LURK_CHANGEROOM changeRoom;
            if(bytes = recv(fd,&changeRoom,sizeof(LURK_CHANGEROOM),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << fmt::format("Changeroom: {0} | Current: {1}\n",std::to_string(changeRoom.ROOM_NUMBER),std::to_string(p->charTainer.CURRENT_ROOM_NUMBER));
            ;
            for(auto t : MasterRoomList.at(p->charTainer.CURRENT_ROOM_NUMBER)->connectedRooms)
            {
                std::cout << fmt::format("Changeroom: {0} | {1}\n",std::to_string(changeRoom.ROOM_NUMBER),std::to_string(t->ROOM_NUMBER));
                if(t->ROOM_NUMBER == changeRoom.ROOM_NUMBER)
                {
                    movePlayer(p,changeRoom.ROOM_NUMBER);
                    break;
                } else {
                    gatekeeper('d',p,type,1);
                }
            }
            break;
        }
        case 3:
        {// FIGHT
            uint8_t tmp;
            if(bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << p->charTainer.CHARACTER_NAME << " STARTS BEEF" << std::endl;
            gatekeeper('a',p,type,0);
            break;
        }
        case 4:
        {// PVP
            LURK_PVP lurk_pvp;
            if(bytes = recv(fd,&lurk_pvp,sizeof(LURK_PVP),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << p->charTainer.CHARACTER_NAME
                      << " WANTS TO FIGHT " << lurk_pvp.TARGET << std::endl;
            gatekeeper('a',p,type,0);
            break;
        }
        case 5:
        {// LOOT
            LURK_LOOT lurk_loot;
            if(bytes = recv(fd,&lurk_loot,sizeof(LURK_LOOT),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << p->charTainer.CHARACTER_NAME
                      << " WANTS TO LOOT " << lurk_loot.TARGET << std::endl;
            gatekeeper('a',p,type,0);
            break;
        }
        case 6:
        {// START
            uint8_t tmp;
            if(bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL) < 0){ragequit(p);break;}
            // do something to here to throw user into the game.
            std::cout << p->charTainer.CHARACTER_NAME << " requests a start" << std::endl;
            gatekeeper('a',p,type,0);
            break;
        }
        case 12:
        {// LEAVE (Consider cleanup!)
            uint8_t tmp;
            if(bytes = recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << p->charTainer.CHARACTER_NAME
                      << " User has requested to leave gracefully" << std::endl;
            gatekeeper('a',p,type,0);
            //clean up... try
            ragequit(p);
            break;
        }
        case 10:
        {// CHARACTER (bitset check state of JOIN_BATTLE ONLY)
            LURK_CHARACTER lurk_char;
            if(bytes = recv(fd,&lurk_char,sizeof(LURK_CHARACTER),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << p->charTainer.CHARACTER_NAME
                      << " sends a character message. let's check her flags for JB" << std::endl;
            gatekeeper('a',p,type,0);
            break;
        }
        default:
        {
            uint8_t tossByte;
            if(bytes = recv(fd,&tossByte,sizeof(uint8_t),MSG_WAITALL) < 0){ragequit(p);break;}
            std::cout << fmt::format("Received bad Type from: {0} Type: {1}",p->charTainer.CHARACTER_NAME, std::to_string(tossByte));
            gatekeeper('d',p,type,0);
        }
    }
}

void Gamemaster::postman(Player* sender,LURK_MSG lurk_msg,char* data)
{// consider if a lock should be implemented, protecting MastPlayerList from manip
    bool found = false;

    {
        std::lock_guard<std::mutex> lock(GMlock);
        for(auto t:MasterPlayerList)
        {
            if(strcmp(t->charTainer.CHARACTER_NAME,lurk_msg.CEIVER_NAME) == 0)
            {
                if(t->writeToMe(lurk_msg,data) < 0){ragequit(t);return;}
                if(sender->writeToMe(lurk_msg,data) < 0){ragequit(sender);return;}
                gatekeeper('a',sender,1,0);
                found = true;
                break;
            }
        }
    }
    if(!found){gatekeeper('d',sender,0,6);}
}

void Gamemaster::gatekeeper(char ad,Player* t,uint8_t action,uint8_t err)
{
    if(ad == 'a')
    {// action accepted
        LURK_ACCEPT la;
        la.ACCEPT_TYPE = action;
        {
            std::lock_guard<std::mutex> lock(*t->pLock);
            if(write(t->socketFD,&la,sizeof(LURK_ACCEPT)) < 0){ragequit(t);return;}
        }
    } else if(ad == 'd')
    {// action declined
        LURK_ERROR le;
        le.CODE = err;
        std::string desc = c_m.error.at(err);
        le.MSG_LEN = desc.length();
        {
            std::lock_guard<std::mutex> lock(*t->pLock);
            if(write(t->socketFD,&le,sizeof(LURK_ERROR)) < 0){ragequit(t);return;}
            if(write(t->socketFD,desc.c_str(),le.MSG_LEN) < 0){ragequit(t);return;}
        }
    }
}
//END network functions ////////


/* 
USEFUL CLIPS:
std::lock_guard<std::mutex> lock(*t->pLock);
memset(dataDump, 0, BIG_BUFFER);
ERROR CODES:
0	Other (not covered by any below error code)
1	Bad room
2	Player Exists
3	Bad Monster
4	Stat error
5	Not Ready
6	No target
7	No fight
8	No player vs. player

*/

    // while(!softStop)
    // {
    //     bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK);
    //     std::cout << "typeCheck: " << typeCheck << std::endl;
    //     if(typeCheck != 10)
    //     {
    //         printf("Invalid type received: %d\n",typeCheck);
    //         memset(dataDump, 0, BIG_BUFFER);
    //         bytes = recv(fd,dataDump,BIG_BUFFER,MSG_DONTWAIT); // DOUBLE CHECK dataDump
    //         std::string errorMsg = "Incorrect Type. Expecting Type: 10";
    //         lurk_error.MSG_LEN = errorMsg.length();
    //         lurk_error.CODE = 0;
    //         write(fd,&lurk_error,sizeof(LURK_ERROR));
    //         write(fd,errorMsg.c_str(),lurk_error.MSG_LEN);
    //         continue;
    //     }
    //     printf("Valid type received: %d\n",typeCheck);

    //     //attempt to assemble character.
    //     bytes = recv(fd,&p.charTainer,sizeof(LURK_CHARACTER),MSG_WAITALL);

    //     if(p.charTainer.DESC_LENGTH > 1000)
    //     {
    //         printf("Invalid Description Length (too big): %d\n",p.charTainer.DESC_LENGTH);
    //         memset(dataDump,0,BIG_BUFFER);
    //         bytes = recv(fd,&dataDump,BIG_BUFFER,MSG_DONTWAIT);
    //         std::string errorMsg = "Please limit description length to 1000 characters. (You psycho.)";
    //         lurk_error.MSG_LEN = errorMsg.length();
    //         lurk_error.CODE = 0;
    //         write(fd,&lurk_error,sizeof(LURK_ERROR));
    //         write(fd,errorMsg.c_str(),lurk_error.MSG_LEN);
            
    //         continue;
    //     }

    //     char data[p.charTainer.DESC_LENGTH];
    //     bytes = recv(fd,&data,p.charTainer.DESC_LENGTH,MSG_WAITALL);
    //     data[p.charTainer.DESC_LENGTH] = 0;
        
    //     p.desc.assign(data);
    //     softStop = true;
    //     MasterPlayerList.emplace_back(p);
    // }
