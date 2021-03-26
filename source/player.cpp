#include"../headers/player.h"

Player::Player(int s) : Baddie()
{
    std::cout << "Player created" << std::endl;
    socketFD = s;
}
Player::~Player()
{
    std::cout << "Player destroyed." << std::endl;
}

//
