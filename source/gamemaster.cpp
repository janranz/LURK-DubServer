#include"../headers/gamemaster.h"

unsigned int Gamemaster::g_seed = 0;
uint16_t Gamemaster::MAX_BADDIES = 1000;
uint16_t Gamemaster::MAX_STAT = 4000;
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
    // delete baddie spawner
    // for(auto t : BDSpawner){delete t;}
    // BDSpawner.clear();
    //delete room list

    std::cout << "Goodbye, Gamemaster." << std::endl;
}

void Gamemaster::ragequit()
{// player has ragequit (left ungracefully)
    std::cout << "Player ragequit" << std::endl;
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
        case 0:{c_m.awaken.push_back(*m);break;}
        case 1:{c_m.baddies.push_back(*m);break;}
        case 2:{c_m.danger.push_back(*m);break;}
        case 3:{c_m.death.push_back(*m);break;}
        case 4:{c_m.error.push_back(*m);break;}
        case 5:{c_m.fightHit.push_back(*m);break;}
        case 6:{c_m.food.push_back(*m);break;}
        case 7:{c_m.health.push_back(*m);break;}
        case 8:{c_m.loot.push_back(*m);break;}
        case 9:{c_m.pvp.push_back(*m);break;}
        // case 10:{c_m.roomNames.push_back(*m);break;}
        case 10:{c_m.safeEv.push_back(*m);break;}
        case 11:{c_m.trans.push_back(*m);break;}
        case 12:{c_m.weapons.push_back(*m);break;}
        case 13:{c_m.baddie_desc.push_back(*m);break;}
        case 14:{c_m.room_desc.push_back(*m);break;}
        case 15:{c_m.adj.push_back(*m);break;}
        case 16:{c_m.noun.push_back(*m);break;}
        case 17:{c_m.roomTainer.push_back(*m);break;}
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

        Baddie* p = new Baddie(n,flags,attack,defense,regen,health,gold,0,descLength,descGrab);
        BDSpawner.push_back(p);
    }
    std::cout << "\nSpawner populated! Size of BDSpawner: " << BDSpawner.size() << std::endl;
}

void Gamemaster::craftRoomNames(int roomCount)
{
    std::string finalName;
    int adjSize = c_m.adj.size();
    int nounSize = c_m.noun.size();
    int tainerSize = c_m.roomTainer.size();
    
    for(int i = 0; i < roomCount; i++)
    {
        int dex1 = (fast_rand() % adjSize);
        int dex2 = (fast_rand() % nounSize);
        int dex3 = (fast_rand() % tainerSize);

        finalName = c_m.adj.at(dex1) + " " + c_m.noun.at(dex2) + " " + c_m.roomTainer.at(dex3);
        if(finalName.length() > 32)
            finalName = c_m.adj.at(dex1) + " " + c_m.roomTainer.at(dex3);
        c_m.roomNames.push_back(finalName);
    }
    std::cout << "\nRoom names generated! Size of room name names: " << c_m.roomNames.size();
}

void Gamemaster::buildRooms(int roomCount)
{
    uint16_t roomNumber;
    uint16_t roomDescLength;
    std::string roomName, roomDesc;
    uint8_t connectedRoom = 0;

    // build portalRoom
    roomNumber = 0;
    roomName = "The Portal Room";
    roomDesc = c_m.room_desc.at(0);
    roomDescLength = roomDesc.length();

    Room* p = new Room(roomNumber,roomName,roomDescLength,roomDesc);
    std::vector<uint16_t> connectedRoomNums;
    for(int i = 0; i < roomCount; i++)
    {
        p->setConnectedRooms('p',i); // portal room
    }
    MasterRoomList.push_back(p);

    for(int i = 1; i < roomCount; i++)
    {
        roomName = c_m.roomNames.at(i);
        roomNumber = i;
        roomDesc = c_m.room_desc.at(i);
        roomDescLength = roomDesc.length();
        Room* p = new Room(roomNumber,roomName,roomDescLength,roomDesc);
        if(i == roomCount - 1)
        {
            p->setConnectedRooms('l',i); // last room needs no +1 connected room.
        } else {
            p->setConnectedRooms('n',i); // normal room.
        }
        MasterRoomList.push_back(p);
    }
    std::cout << "\nMasterRoomList succeeds! Size: "<< MasterRoomList.size() << std::endl;
}

void Gamemaster::populateRooms()
{
    int roomCount = MasterRoomList.size();
    int min = 3;
    int max = 20;
    for(auto t: MasterRoomList)
    {
        
        int baddieCount = (fast_rand() % (max - min) + min);
        for(int i = 0; i < baddieCount; i++)
        {
            Baddie* release = BDSpawner.at((fast_rand() % (BDSpawner.size())));
            t->injectBaddie(release);
        }
    }
    for(auto t: BDSpawner)
        delete t;
    BDSpawner.clear();
    std::cout << "\nRooms have been populated! BDSpawner has been deleted. Size: " 
              << BDSpawner.size()
              << std::endl;
    std::cout << "Sanity check - Baddie List Size in a random room: "
              << MasterRoomList.at(5)->DEBUG_getBaddieListSize()
              << std::endl;
}
// END server initialization ////////

//START network functions ////////
void Gamemaster::GMController(int fd)
{
    int32_t typeCheck;
    LURK_ACCEPT lurk_accept;
    LURK_ERROR lurk_error;
    LURK_GAME lurk_game;
    LURK_VERSION lurk_version;
    ssize_t bytes; // consider cnd-checking while loops

    
    std::string description = c_m.room_desc.at(0);
    lurk_game.DESC_LENGTH = description.length();

    // send initial messages to client
    
    if(write(fd,&lurk_version,sizeof(LURK_VERSION)) == -1){ragequit();return;}

    if(write(fd,&lurk_game,sizeof(LURK_GAME)) == -1){ragequit();return;}
    if(write(fd,description.data(),description.length()) == -1){ragequit();return;}

    // wait for character message. We do this manually here not to disrupt other valid players
    bool softStop = false;
    char dataDump[BIG_BUFFER];
    Player p(fd);
    while(!softStop)
    {
        recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK);
        std::cout << "typeCheck: " << typeCheck << std::endl;
        if(typeCheck != 10)
        {
            printf("Invalid type received: %d\n",typeCheck);
            memset(dataDump, 0, BIG_BUFFER);
            recv(fd,&dataDump,BIG_BUFFER,MSG_DONTWAIT);
            std::string errorMsg = "Incorrect Type. Expecting Type: 10";
            lurk_error.MSG_LEN = errorMsg.length();
            lurk_error.CODE = 0;
            write(fd,&lurk_error,sizeof(LURK_ERROR));
            write(fd,errorMsg.c_str(),lurk_error.MSG_LEN);
            continue;
        }
        printf("Valid type received: %d\n",typeCheck);

        //attempt to assemble character.
        recv(fd,&p.charTainer,sizeof(LURK_CHARACTER),MSG_WAITALL);

        if(p.charTainer.DESC_LENGTH > 1000)
        {
            printf("Invalid Description Length (too big): %d\n",p.charTainer.DESC_LENGTH);
            memset(dataDump,0,BIG_BUFFER);
            recv(fd,&dataDump,BIG_BUFFER,MSG_DONTWAIT);
            std::string errorMsg = "Please limit description length to 1000 characters. (You psycho.)";
            lurk_error.MSG_LEN = errorMsg.length();
            lurk_error.CODE = 0;
            write(fd,&lurk_error,sizeof(LURK_ERROR));
            write(fd,errorMsg.c_str(),lurk_error.MSG_LEN);
            
            continue;
        }

        // printf("\nName:%s\nFlags: %d\nAttack: %d\nDefense: %d\nRegen: %d\nHealth: %d\
        //         \nGold: %d\nCurrent Room: %d\nDescription Length: %d\n",\
        //         p->charTainer.CHARACTER_NAME,p->charTainer.FLAGS,p->charTainer.ATTACK,\
        //         p->charTainer.DEFENSE,p->charTainer.REGEN,p->charTainer.HEALTH,p->charTainer.GOLD,\
        //         p->charTainer.CURRENT_ROOM_NUMBER,p->charTainer.DESC_LENGTH);

        char data[p.charTainer.DESC_LENGTH];
        recv(fd,&data,p.charTainer.DESC_LENGTH,MSG_WAITALL);
        data[p.charTainer.DESC_LENGTH] = 0;
        
        p.desc.assign(data);
        softStop = true;
        MasterPlayerList.push_back(p);
    }
    printf("Player successfully added to Master: %d\n",MasterPlayerList.size());
    // Any new threads should be considered here.
    
    // read from client - expect to send a RESPONSE back.

    while(bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK) > 0)
    {
        mailroom(fd,typeCheck);
    }
    // client most likely closed the socket or some error occured here.
    printf("Lost recv() comms with peer socket, bytes: %d\n",bytes);
}

void Gamemaster::mailroom(int fd,int32_t type)
{// process client data (Since mutex is a shared_ptr, try and lock it via MasterPlayerList?)
    LURK_MSG lurk_msg;
    bool hotAccept;

    std::cout << "fd: " << fd << std::endl;
    switch(type)
    {
        case 1:
        {// MESSAGE
            recv(fd,&lurk_msg,sizeof(LURK_MSG),MSG_WAITALL);
            char data[lurk_msg.MSG_LEN];
            recv(fd,&data,lurk_msg.MSG_LEN,MSG_WAITALL);
            data[lurk_msg.MSG_LEN] = 0;
            postman(lurk_msg,data);
            // std::string s(data, lurk_msg.MSG_LEN);
            break;
        }
    }
}

void Gamemaster::postman(LURK_MSG lurk_msg,char* data)
{// consider if a lock should be implemented, protecting MastPlayerList from manip
    // for(auto t:MasterPlayerList)
    // {
    //     std::cout << "In postman: " << t.socketFD << std::endl;
    //     t.writeToMe(lurk_msg,data);
    // }
    int theFD = MasterPlayerList.at(0).socketFD;
    MasterPlayerList.at(0).writeToMe(lurk_msg,data);
}
//END network functions ////////
