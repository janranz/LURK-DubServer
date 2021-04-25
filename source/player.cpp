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
    
    desc = "";
}

//state check
bool Player::isSktAlive()
{
    std::shared_lock<std::shared_mutex>lock(pLock);
    return sktAlive;
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
//state set

void Player::giveRoom(uint16_t n)
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        charTainer.CURRENT_ROOM_NUMBER = n;
    }
}

void Player::startPlayer()
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        started = true;
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
            m = fmt::format("{} has staged to quit!\n",charTainer.CHARACTER_NAME);
        }else{
            m = "Unknown player staged to quit!\n";
        }
        {std::lock_guard<std::mutex> lock(printLock);std::cout << m << std::endl;}
    }
    
}
void Player::setValid()
{
    {std::lock_guard<std::shared_mutex> lock(pLock);validToon = true;}
}
void Player::respawn()
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        charTainer.FLAGS = serverStats::PLAYER_AFLAGS;
        playerAlive = true;
    }
}
void Player::despawn()
{
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        charTainer.FLAGS = serverStats::PLAYER_DFLAGS;
        playerAlive = false;
    }
}

//getter
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
        quitPlayer();
    }
}

void Player::write_msg(LURK_MSG pkg, std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        
        strncpy(M_ToCP(pkg.CEIVER_NAME),M_ToCP(charTainer.CHARACTER_NAME),sizeof(pkg.CEIVER_NAME));
        pkg.MSG_LEN = msg.length();
        
        write(socketFD,&LURK_TYPES::TYPE_MSG,sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_MSG));
        bytes = write(socketFD,msg.c_str(), pkg.MSG_LEN);
    }
        if(bytes < 1)
        {
            quitPlayer();
        }
    
}
void Player::write_error(LURK_ERROR pkg, std::string msg)
{
    ssize_t bytes = 0;
    {
        std::lock_guard<std::shared_mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_ERROR,sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_ERROR));
        bytes = write(socketFD,msg.c_str(), pkg.MSG_LEN);

    }
    if(bytes < 1)
    {
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
        quitPlayer();
    }
}
