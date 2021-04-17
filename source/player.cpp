#include"../headers/player.h"

Player::Player(int fd)
{
    socketFD = fd;
    sktAlive = true;
    started = false;
    validToon = false;
    freshSpawn = true;
    charTainer.HEALTH = serverStats::BASE_HEALTH;
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
}
void Player::quitPlayer()
{
    sktAlive = false;
    std::string m;
    if(strlen(charTainer.CHARACTER_NAME) > 0)
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
