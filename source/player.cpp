#include"../headers/player.h"

Player::Player(int fd)
{
    std::string m = "unknown Player";
    strncpy(M_ToCP(charTainer.CHARACTER_NAME),m.c_str(),sizeof(charTainer.CHARACTER_NAME));
    socketFD = fd;
    sktAlive = true;
    playerAlive = false;
    started = false;
    validToon = false;
    freshSpawn = true;
    charTainer.HEALTH = serverStats::PLAYER_BASE_HEALTH;
    charTainer.FLAGS = serverStats::BASE_FLAGS;
    charTainer.ATTACK = 0;
    charTainer.DEFENSE = 0;
    charTainer.REGEN = 0;
    charTainer.GOLD = 0;
    charTainer.CURRENT_ROOM_NUMBER = 0;
    gender = "a";
    genderPos = "a";
    genderHeShe = "a";
    desc = "";
    pvpKills = 0;
    totalDeaths = 0;
    currPVEScore = 0;
    highPVEScore = 0;
    flagHS = false;
}

//state check
bool Player::isSktAlive()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return sktAlive;
}
bool Player::isHighScore()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return !flagHS;
}
bool Player::isStarted()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return started;
}
bool Player::isValidToon()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return validToon;
}
bool Player::isFreshSpawn()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return freshSpawn;
}
bool Player::isPlayerAlive()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return playerAlive;
}

uint16_t Player::getPVPKills()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return pvpKills;
}
//state set

void Player::HSchecked()
{
    std::lock_guard<std::shared_mutex>lock(pLock);
    flagHS = false;
}

void Player::full_restore_health()
{
    std::lock_guard<std::shared_mutex>lock(pLock);
    charTainer.HEALTH = serverStats::PLAYER_BASE_HEALTH;
}

void Player::tally_pvp()
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        if((pvpKills + 1) > serverStats::PLAYER_MAX_STAT)
        {
            pvpKills = serverStats::PLAYER_MAX_STAT;
        }else{
            pvpKills++;
        }
        flagHS = true;
    }
}

void Player::giveRoom(uint16_t n)
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        charTainer.CURRENT_ROOM_NUMBER = n;
    }
}

void Player::startPlayer(bool g)
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        started = true;
        playerAlive = true;
        charTainer.FLAGS = serverStats::PLAYER_AFLAGS;
        
        critDamage = (charTainer.ATTACK * 3);
        if(g)
        {
            gender = "her";
            genderPos = "her";
            genderHeShe = "she";
        }else{
            gender = "him";
            genderPos = "his";
            genderHeShe = "he";
        }
    }
}
void Player::quitPlayer()
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        sktAlive = false;
        std::string m;
        if(started)
        {
            m = fmt::format("{0} has staged to quit!\n",charTainer.CHARACTER_NAME);
        }else{
            m = "Unknown player staged to quit!\n";
        }
        {std::lock_guard<std::mutex> lock(printLock);fmt::print("{}\n",m);}
    }
    
}
void Player::setValid()
{
    {std::lock_guard<std::shared_mutex> lock(pLock);validToon = true;}
}

void Player::tally_PVE_kill()
{
    std::lock_guard<std::shared_mutex>lock(pLock);
    if((currPVEScore + 1) < std::numeric_limits<uint16_t>::max())
    {
        currPVEScore++;
    }else{
        currPVEScore = std::numeric_limits<uint16_t>::max();
    }
    if(currPVEScore > highPVEScore)
    {
        highPVEScore = currPVEScore;
    }
    flagHS = true;
}

void Player::respawn()
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        charTainer.HEALTH = serverStats::PLAYER_BASE_HEALTH;
        charTainer.FLAGS = serverStats::PLAYER_AFLAGS;
        playerAlive = true;
        currPVEScore = 0;
    }
}

bool Player::hurt_player(int32_t h)
{
    // bool dead = false;
    {
        std::lock_guard<std::shared_mutex>lock(pLock);
        if((charTainer.HEALTH - h) > 0)
        {
            charTainer.HEALTH -= h;
        }else{
            charTainer.HEALTH = 0;
            // currScore = 0;
            charTainer.FLAGS = serverStats::PLAYER_DFLAGS;
            totalDeaths++;
            playerAlive = false;
            
        }
    }
    return playerAlive;
}

void Player::heal_player(int32_t h)
{
    {
        std::lock_guard<std::shared_mutex>lock(pLock);
        if((charTainer.HEALTH + h) <= serverStats::PLAYER_BASE_HEALTH)
        {
            charTainer.HEALTH += h;
        }else{
            charTainer.HEALTH = serverStats::PLAYER_BASE_HEALTH;
        }
    }
}

void Player::give_gold(uint32_t g)
{
    std::lock_guard<std::shared_mutex>lock(pLock);
    if((charTainer.GOLD + g ) > serverStats::PLAYER_MAX_STAT)
    {
        charTainer.GOLD = serverStats::PLAYER_MAX_STAT;
    }else{
        charTainer.GOLD += g;
    }
}

uint16_t Player::drop_gold()
{
    uint16_t drop = 0;
    std::lock_guard<std::shared_mutex>lock(pLock);
    if(charTainer.GOLD > 0)
    {
        drop = (fast_rand() % ((charTainer.GOLD + 1) - 1) + 1);
        charTainer.GOLD -= drop;
    }
    
    return drop;
}

void Player::take_gold(uint16_t g)
{// for refunding
    std::lock_guard<std::shared_mutex>lock(pLock);
    if((charTainer.GOLD - g) < 0)
    {
        charTainer.GOLD = 0;
    }else{
        charTainer.GOLD -= g;
    }
}

//getter

uint16_t Player::get_deaths()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return totalDeaths;
}

uint16_t Player::getHighScore()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return highPVEScore;
}

uint16_t Player::get_gold()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return charTainer.GOLD;
}

uint16_t Player::getCurrScore()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return currPVEScore;
}

int Player::getFD()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return socketFD;
}
uint16_t Player::getRoomNumber()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return charTainer.CURRENT_ROOM_NUMBER;
}
uint16_t Player::getCrit()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return critDamage;
}

uint16_t Player::loot_me()
{
    uint16_t drop = 0;
    std::lock_guard<std::shared_mutex>lock(pLock);
    if(charTainer.GOLD > 0)
    {
        drop = (fast_rand() % (charTainer.GOLD) + 1);
        charTainer.GOLD -= drop;
    }else if(charTainer.GOLD == 1){
        drop = 1;
        charTainer.GOLD -= drop;
    }
    return drop;
}

//writer

void Player::write_reflect()
{// self character message
    ssize_t bytes = 0;
    {
        std::shared_lock<std::shared_mutex>lock(pLock);
        write(socketFD,&LURK_TYPES::TYPE_CHARACTER, sizeof(uint8_t));
        write(socketFD,&charTainer,sizeof(LURK_CHARACTER));
        bytes = write(socketFD,desc.c_str(),charTainer.DESC_LENGTH);
    }
    if( bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}

void Player::write_msg(LURK_MSG pkg, std::string msg,int called)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        
        strncpy(M_ToCP(pkg.CEIVER_NAME),M_ToCP(charTainer.CHARACTER_NAME),sizeof(pkg.CEIVER_NAME));
        pkg.MSG_LEN = msg.length();
        
        write(socketFD,&LURK_TYPES::TYPE_MSG,sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_MSG));
        bytes = write(socketFD,msg.c_str(), pkg.MSG_LEN);
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("PLAYER: {0} BYTES:{1} -> Line {2} - {3}\nMESSAGE:{4}\nCALLED FROM LINE: {5}\n",charTainer.CHARACTER_NAME,bytes,__LINE__,__FILE__,msg,called);}
    }
        if(bytes < 1)
        {
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
            quitPlayer();
        }
    
}
void Player::write_error(LURK_ERROR pkg, std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        pkg.MSG_LEN = msg.length();
        write(socketFD, &LURK_TYPES::TYPE_ERROR,sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_ERROR));
        bytes = write(socketFD,msg.c_str(), pkg.MSG_LEN);
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
void Player::write_accept(uint8_t t)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        LURK_ACCEPT pkg;
        pkg.ACCEPT_TYPE = t;
        write(socketFD, &LURK_TYPES::TYPE_ACCEPT,sizeof(uint8_t));
        bytes = write(socketFD,&pkg.ACCEPT_TYPE,sizeof(uint8_t));
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
void Player::write_room(LURK_ROOM pkg, std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_ROOM, sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_ROOM));
        bytes = write(socketFD,msg.c_str(), pkg.DESC_LENGTH);
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
void Player::write_character(LURK_CHARACTER pkg, std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_CHARACTER, sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_CHARACTER));
        bytes = write(socketFD,msg.c_str(), pkg.DESC_LENGTH);
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
void Player::write_game(LURK_GAME pkg, std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        write(socketFD,&LURK_TYPES::TYPE_GAME, sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_GAME));
        bytes = write(socketFD,msg.c_str(),pkg.DESC_LENGTH);
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
void Player::write_version(LURK_VERSION pkg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_VERSION, sizeof(uint8_t));
        bytes = write(socketFD, &pkg, sizeof(LURK_VERSION));
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
void Player::write_connection(LURK_ROOM pkg,std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_CONNECTION, sizeof(uint8_t));
        write(socketFD,&pkg, sizeof(LURK_ROOM));
        bytes = write(socketFD,msg.c_str(),pkg.DESC_LENGTH);
    }
    if(bytes < 1)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG: Line {0} - {1}\n",__LINE__,__FILE__);}
        quitPlayer();
    }
}
