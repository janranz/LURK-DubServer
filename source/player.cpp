#include"../headers/player.h"


Player::Player(int s)
{
    std::cout << "Player created socket: "<< s << std::endl;
    socketFD = s;
    sktAlive = true;
    started = false;
    validToon = false;
    freshSpawn = false;
}
Player::~Player()
{
    
    // std::cout << "Player destroyed." << std::endl;
}

int Player::getFD()
{
    return socketFD;
}

// set states
void Player::startPlayer()
{
    std:: cout << "User has selected to start!" << std::endl;
    started = true;
}

// void Player::setMaster()
// {
//     inMaster = true;
// }

void Player::quitPlayer()
{
    std::cout << "Connection lost! User staged to quit." << std::endl;
    sktAlive = false;
}

void Player::setValid()
{
    validToon = true;
}

void Player::spawn()
{
    freshSpawn = true;
}

void Player::despawn()
{
    freshSpawn = false;
}
// Bools
bool Player::isValidToon()
{
    return validToon;
}

bool Player::isSktAlive()
{
    return sktAlive;
}

bool Player::isFreshSpawn()
{
    return freshSpawn;
}

// bool Player::isInMaster()
// {
//     return inMaster;
// }

bool Player::isStarted()
{
    return started;
}

// write funcs
void Player::writeToMe(LURK_MSG lurk_msg,char* data)
{
    if(!sktAlive)
        return;
    std::cout << "FD: " << socketFD << std::endl;
    std::cout << "DATA: " << data << std::endl;
    ssize_t bytes = 0;
    {
        std::lock_guard<std::mutex> lock(pLock);

        bytes = write(socketFD,&lurk_msg,sizeof(LURK_MSG));
        bytes = write(socketFD,data,lurk_msg.MSG_LEN);
    }
    if(bytes < 0){quitPlayer();}
    std::cout << "message sent to: " << charTainer.CHARACTER_NAME << std::endl;
}

void Player::reflection()
{
    if(sktAlive)
    {
        std::lock_guard<std::mutex>lock(pLock);
        write(socketFD,&charTainer,sizeof(LURK_CHARACTER));
        write(socketFD,desc.c_str(),charTainer.DESC_LENGTH);
    }
}
