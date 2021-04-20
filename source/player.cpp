#include"../headers/player.h"

Player::Player(int fd)
{
    socketFD = fd;
    bytes = 0;
    sktAlive = true;
    started = false;
    validToon = false;
    freshSpawn = true;
    charTainer.HEALTH = serverStats::PLAYER_BASE_HEALTH;
    charTainer.FLAGS = serverStats::BASE_FLAGS;
    desc = "";
}

//state check
bool Player::isSktAlive()
{
    return sktAlive;
}
bool Player::isStarted()
{
    return started;
}
bool Player::isValidToon()
{
    return validToon;
}
bool Player::isFreshSpawn()
{
    return freshSpawn;
}

//state set
void Player::startPlayer()
{
    started = true;
    charTainer.FLAGS = serverStats::PLAYER_AFLAGS;
}
void Player::quitPlayer()
{
    sktAlive = false;
    std::string m;
    if(started)
    {
        m = fmt::format("{} has staged to quit!\n",charTainer.CHARACTER_NAME);
    }else{
        m = "Unknown player staged to quit!\n";
    }
    std::cout << m << std::endl;
}
void Player::setValid()
{
    validToon = true;
}
void Player::respawn()
{
    freshSpawn = true;
}
void Player::despawn()
{
    freshSpawn = false;
}

//getter
int Player::getFD()
{
    return socketFD;
}

//writer
void Player::write_msg(LURK_MSG pkg, std::string msg)
{
    strlcpy(pkg.CEIVER_NAME,charTainer.CHARACTER_NAME,32);
    
    pkg.MSG_LEN = msg.length();
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD,&LURK_TYPES::TYPE_MSG,sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_MSG));
        bytes = write(socketFD,msg.c_str(), pkg.MSG_LEN);
        if(bytes < 1)
        {
            std::cout << "Called in write_msg\n";
            quitPlayer();
        }
    }
}
void Player::write_error(LURK_ERROR pkg, std::string msg)
{
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_ERROR,sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_ERROR));
        bytes = write(socketFD,msg.c_str(), pkg.MSG_LEN);
        if(bytes < 1)
        {
            std::cout << "Called in write_error\n";
            quitPlayer();
        }
    }
}
void Player::write_accept(uint8_t t)
{
    LURK_ACCEPT pkg;
    pkg.ACCEPT_TYPE = t;
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_ACCEPT,sizeof(uint8_t));
        bytes = write(socketFD,&pkg,sizeof(LURK_ACCEPT));
        if(bytes < 1)
        {
            std::cout << "Called in write_msg accept\n";
            quitPlayer();
        }
    }
    
}
void Player::write_room(LURK_ROOM pkg, std::string msg)
{
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_ROOM, sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_ROOM));
        bytes = write(socketFD,msg.c_str(), pkg.DESC_LENGTH);
        if(bytes < 1)
        {
            std::cout << "Called in write_room\n";
            quitPlayer();
        }
    }
}
void Player::write_character(LURK_CHARACTER pkg, std::string msg)
{
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_CHARACTER, sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_CHARACTER));
        bytes = write(socketFD,msg.c_str(), pkg.DESC_LENGTH);
        if(bytes < 1)
        {
            std::cout << "Called in write_character\n";
            quitPlayer();
        }
    }
}
void Player::write_game(LURK_GAME pkg, std::string msg)
{
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD,&LURK_TYPES::TYPE_GAME, sizeof(uint8_t));
        write(socketFD, &pkg, sizeof(LURK_GAME));
        bytes = write(socketFD,msg.c_str(),pkg.DESC_LENGTH);
        if(bytes < 1)
        {
            std::cout << "Called in write_game\n";
            quitPlayer();
        }
    }
}
void Player::write_version(LURK_VERSION pkg)
{
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_VERSION, sizeof(uint8_t));
        bytes = write(socketFD, &pkg, sizeof(LURK_VERSION));
        if(bytes < 1)
        {
            std::cout << "Called in write_version\n";
            quitPlayer();
        }
    }
}
void Player::write_connection(LURK_ROOM pkg,std::string msg)
{
    {
        std::lock_guard<std::mutex> lock(pLock);
        write(socketFD, &LURK_TYPES::TYPE_CONNECTION, sizeof(uint8_t));
        write(socketFD,&pkg, sizeof(LURK_ROOM));
        bytes = write(socketFD,msg.c_str(),pkg.DESC_LENGTH);
        if(bytes < 1)
        {
            std::cout << fmt::format("Called in write_connection: bytes: \n",std::to_string(bytes));
            quitPlayer();
        }
    }
}
