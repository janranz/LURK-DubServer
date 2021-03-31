#include"../headers/gamemaster.h"

unsigned int Gamemaster::g_seed = 0;
uint16_t Gamemaster::MAX_BADDIES = 1000;
uint16_t Gamemaster::MAX_STAT = 4000;
int16_t Gamemaster::BASE_HEALTH = 6000;
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
    Player p(fd);
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
    
    if(write(fd,&lurk_version,sizeof(LURK_VERSION)) == -1){ragequit();return;}

    if(write(fd,&lurk_game,sizeof(LURK_GAME)) == -1){ragequit();return;}
    if(write(fd,description.data(),description.length()) == -1){ragequit();return;}

    // wait for character message. We do this manually here
    while(bytes = recv(fd, &typeCheck,1,MSG_WAITALL|MSG_PEEK) > 0)
    {// confirm character
        bool checksOut = false;
        std::cout << "Type: " << typeCheck << std::endl;
        if(typeCheck == 10)
        {

            recv(fd,&p.charTainer,sizeof(LURK_CHARACTER),MSG_WAITALL);
            
            char tmp[p.charTainer.DESC_LENGTH];
            recv(fd,tmp,p.charTainer.DESC_LENGTH,MSG_WAITALL);
            tmp[p.charTainer.DESC_LENGTH] = 0;
            p.desc.assign(tmp);
            checksOut = checkStats(p);
            if(checksOut == false)
            {
                std::cout << "Pump n dumpin' data. Bad stats" << std::endl;
                memset(dataDump,0,BIG_BUFFER);
                recv(fd,dataDump,BIG_BUFFER,MSG_DONTWAIT);
                gatekeeper('d',p,0,4);
                m = fmt::format("Whoa let's calm down there with the stats, {0}."
                " If you wanted God Mode, all you had to do was ask. [GOD-MODE ACTIVATED]",p.charTainer.CHARACTER_NAME);
                GMPM(p,m);
            } else{break;}
        }
        std::cout << "Pump n dumpin' data. Bad Type: "<< typeCheck << std::endl;
        memset(dataDump,0,BIG_BUFFER);
        recv(fd,dataDump,BIG_BUFFER,MSG_DONTWAIT);
        gatekeeper('d',p,0,0);
        m = "Hmm... Before we get going, you mind telling me who you are? (Type: 10)";
        GMPM(p,m);

    }
    std::cout << p.charTainer.CHARACTER_NAME << " checks out!" << std::endl;
    MasterPlayerList.push_back(p);

    printf("Player successfully added to Master: %d\n",MasterPlayerList.size());
    std::cout << "Sanity check desc: " << p.desc << std::endl;

    // wait for start....
    std::string name(p.charTainer.CHARACTER_NAME);
    m = fmt::format("Welcome to these savage streets, {0}."
                    "There's nothing to be afraid about (yet),"
                    "I just need to know you're ready to START.",name);

                GMPM(p,m);
    while(bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK) > 0)
    {// confirm start
        if(typeCheck == 6)
        {
            uint8_t tmp;
            recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            std::cout << "User has started!: Type(" << typeCheck << ")" << std::endl;
            
            /*TESTING CONCAT */
            int dex = (fast_rand() % (c_m.awaken.size()));
            m = c_m.awaken.at(dex);
            GMPM(p,m);
            break;
        }
            std::cout << "User has NOT started!: Type(" << typeCheck << ")" << std::endl;
            m = "Invalid type received!";
            GMPM(p,m);
            memset(dataDump,0,BIG_BUFFER);
            recv(p.socketFD,dataDump,BIG_BUFFER,MSG_DONTWAIT);
            gatekeeper('d',p,0,5);
            m = "hmm.. I'm looking for a \"START\" message, " + name
                + " (pssst... It's TYPE 6.)";
            GMPM(p,m);
    }
    // Any new threads should be considered here.
    
    // read from client - expect to send a RESPONSE back.
    movePlayer(p,'s');
    movePlayer(p,'f');
    movePlayer(p,'b');

    while(bytes = recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK) > 0)
    {// consider how we will require a START (type 6)
        mailroom(p,fd,typeCheck);
    }
    // client most likely closed the socket or some error occured here.
    printf("Lost recv() comms with peer socket, bytes: %d\n",bytes);
}

bool Gamemaster::checkStats(Player& p)
{
    // check name conflicts
    std::cout << "Checking stats" << std::endl;
    RECHECK:
    for(auto t : MasterPlayerList)
    {
        if(strcmp(t.charTainer.CHARACTER_NAME,p.charTainer.CHARACTER_NAME) == 0)
        {
            // p.charTainer.CHARACTER_NAME[32] = 0;
            std::string base(p.charTainer.CHARACTER_NAME);
            std::string suff= std::to_string(((fast_rand() % 4000)));
            std::string full = base + suff;
            strncpy(p.charTainer.CHARACTER_NAME,full.c_str(),32);
            std::cout << "Name changed to: " << p.charTainer.CHARACTER_NAME << std::endl;
            goto RECHECK;
        }
    }
    std::cout << p.charTainer.ATTACK << p.charTainer.DEFENSE << p.charTainer.REGEN << std::endl;
    uint32_t stat = p.charTainer.ATTACK + p.charTainer.DEFENSE + p.charTainer.REGEN;
    
    p.charTainer.HEALTH = BASE_HEALTH;
    if((stat > MAX_STAT))
    {
        std::cout << "Inappropriate stats: " << stat << std::endl;
        return false;
    } else if(stat < MAX_STAT)
    {
        uint32_t dif = MAX_STAT - stat;
        p.charTainer.HEALTH += dif;
    }
    p.charTainer.FLAGS = 0b11001000; // 200
    p.charTainer.GOLD = (fast_rand() & (4200 - 69) + 69);
    p.charTainer.CURRENT_ROOM_NUMBER = 0;
    std::cout << "Appropriate stats: " << stat << std::endl;
    return true;

}

void Gamemaster::GMPM(Player& p, std::string& msg)
{
    GM_MSG gm;
    gm.MSG_LEN = msg.length();
    strncpy(gm.CEIVER_NAME,p.charTainer.CHARACTER_NAME,32);
    
    {
        std::lock_guard<std::mutex> lock(*p.pLock);
        write(p.socketFD,&gm,sizeof(gm));
        write(p.socketFD,msg.c_str(),gm.MSG_LEN);
    }
}

void Gamemaster::movePlayer(Player& p, char direction)
{
    switch(direction)
    {
        case 's':
        {   p.charTainer.CURRENT_ROOM_NUMBER = 0;
            MasterRoomList.at(0)->addPlayer(p);
            break;
        }
        case 'f':
        {
            p.charTainer.CURRENT_ROOM_NUMBER += 1;
            MasterRoomList.at(p.charTainer.CURRENT_ROOM_NUMBER)->addPlayer(p);
            break;
        }
        case 'b':
        {
            p.charTainer.CURRENT_ROOM_NUMBER -= 1;
            MasterRoomList.at(p.charTainer.CURRENT_ROOM_NUMBER)->addPlayer(p);
            break;
        }
    }
    std::cout << p.charTainer.CHARACTER_NAME << ": In Room: " << p.charTainer.CURRENT_ROOM_NUMBER << std::endl;
    std::cout << "Room Sanity Check: " << MasterRoomList.at(p.charTainer.CURRENT_ROOM_NUMBER)->DEBUG_getBaddieListSize() << std::endl;
}

void Gamemaster::mailroom(Player& p,int fd,int32_t type)
{// process client data (Since mutex is a shared_ptr, try and lock it via MasterPlayerList?)
    // LURK_MSG lurk_msg;
    // bool hotAccept;
    
    std::cout << "fd: " << fd << std::endl;
    switch(type)
    {
        case 1:
        {// MESSAGE (REWORK THIS)
            LURK_MSG lurk_msg;

            recv(fd,&lurk_msg,sizeof(LURK_MSG),MSG_WAITALL);
            char data[lurk_msg.MSG_LEN];
            recv(fd,data,lurk_msg.MSG_LEN,MSG_WAITALL);
            data[lurk_msg.MSG_LEN] = 0;
            postman(p,lurk_msg,data);
            // std::string s(data, lurk_msg.MSG_LEN);
            break;
        }
        case 3:
        {// FIGHT
            uint8_t tmp;
            recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            std::cout << p.charTainer.CHARACTER_NAME << " STARTS BEEF" << std::endl;
            break;
        }
        case 4:
        {// PVP
            LURK_PVP lurk_pvp;
            recv(fd,&lurk_pvp,sizeof(LURK_PVP),MSG_WAITALL);
            std::cout << p.charTainer.CHARACTER_NAME
                      << " WANTS TO FIGHT " << lurk_pvp.TARGET << std::endl;
            break;
        }
        case 5:
        {// LOOT
            LURK_LOOT lurk_loot;
            recv(fd,&lurk_loot,sizeof(LURK_LOOT),MSG_WAITALL);
            std::cout << p.charTainer.CHARACTER_NAME
                      << " WANTS TO LOOT " << lurk_loot.TARGET << std::endl;
            break;
        }
        case 6:
        {// START
            uint8_t tmp;
            recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            // do something to here to throw user into the game.

            std::cout << p.charTainer.CHARACTER_NAME << " requests a start" << std::endl;
            break;
        }
        case 7:
        {// LEAVE (Consider cleanup!)
            uint8_t tmp;
            recv(fd,&tmp,sizeof(uint8_t),MSG_WAITALL);
            std::cout << p.charTainer.CHARACTER_NAME
                      << " User has requested to leave gracefully" << std::endl;
            //clean up
            break;
        }
        case 10:
        {// CHARACTER (bitset check state of JOIN_BATTLE ONLY)
            LURK_CHARACTER lurk_char;
            recv(fd,&lurk_char,sizeof(LURK_CHARACTER),MSG_WAITALL);
            std::cout << p.charTainer.CHARACTER_NAME
                      << " sends a character message. let's check her flags for JB" << std::endl;
            break;
        }
        default:
        {
            std::string m = "Invalid type received!";
            GMPM(p,m);
            memset(dataDump,0,BIG_BUFFER);
            recv(p.socketFD,dataDump,BIG_BUFFER,MSG_DONTWAIT);
        }
    }
}

void Gamemaster::postman(Player& sender,LURK_MSG lurk_msg,char* data)
{// consider if a lock should be implemented, protecting MastPlayerList from manip
    // Player* temp = &sender;
    bool found = false;
    for(auto t:MasterPlayerList)
    {
        if(strcmp(t.charTainer.CHARACTER_NAME,lurk_msg.CEIVER_NAME) == 0)
        {
            t.writeToMe(lurk_msg,data);
            sender.writeToMe(lurk_msg,data);
            gatekeeper('a',sender,1,0);
            found = true;
            break;
        }
    }
    if(!found){gatekeeper('d',sender,0,6);}
}

void Gamemaster::gatekeeper(char ad,Player& t,uint8_t action,uint8_t err)
{
    if(ad == 'a')
    {// action accepted
        LURK_ACCEPT la;
        la.ACCEPT_TYPE = action;
        {
            std::lock_guard<std::mutex> lock(*t.pLock);
            write(t.socketFD,&la,sizeof(LURK_ACCEPT));
        }
    } else if(ad == 'd')
    {// action declined
        LURK_ERROR le;
        le.CODE = err;
        std::string desc = c_m.error.at(err);
        le.MSG_LEN = desc.length();
        {
            std::lock_guard<std::mutex> lock(*t.pLock);
            write(t.socketFD,&le,sizeof(LURK_ERROR));
            write(t.socketFD,desc.c_str(),le.MSG_LEN);
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
    //     recv(fd,&typeCheck,1,MSG_WAITALL|MSG_PEEK);
    //     std::cout << "typeCheck: " << typeCheck << std::endl;
    //     if(typeCheck != 10)
    //     {
    //         printf("Invalid type received: %d\n",typeCheck);
    //         memset(dataDump, 0, BIG_BUFFER);
    //         recv(fd,dataDump,BIG_BUFFER,MSG_DONTWAIT); // DOUBLE CHECK dataDump
    //         std::string errorMsg = "Incorrect Type. Expecting Type: 10";
    //         lurk_error.MSG_LEN = errorMsg.length();
    //         lurk_error.CODE = 0;
    //         write(fd,&lurk_error,sizeof(LURK_ERROR));
    //         write(fd,errorMsg.c_str(),lurk_error.MSG_LEN);
    //         continue;
    //     }
    //     printf("Valid type received: %d\n",typeCheck);

    //     //attempt to assemble character.
    //     recv(fd,&p.charTainer,sizeof(LURK_CHARACTER),MSG_WAITALL);

    //     if(p.charTainer.DESC_LENGTH > 1000)
    //     {
    //         printf("Invalid Description Length (too big): %d\n",p.charTainer.DESC_LENGTH);
    //         memset(dataDump,0,BIG_BUFFER);
    //         recv(fd,&dataDump,BIG_BUFFER,MSG_DONTWAIT);
    //         std::string errorMsg = "Please limit description length to 1000 characters. (You psycho.)";
    //         lurk_error.MSG_LEN = errorMsg.length();
    //         lurk_error.CODE = 0;
    //         write(fd,&lurk_error,sizeof(LURK_ERROR));
    //         write(fd,errorMsg.c_str(),lurk_error.MSG_LEN);
            
    //         continue;
    //     }

    //     char data[p.charTainer.DESC_LENGTH];
    //     recv(fd,&data,p.charTainer.DESC_LENGTH,MSG_WAITALL);
    //     data[p.charTainer.DESC_LENGTH] = 0;
        
    //     p.desc.assign(data);
    //     softStop = true;
    //     MasterPlayerList.push_back(p);
    // }
