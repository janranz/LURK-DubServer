#include"../headers/player.h"
#include<iostream>

Player::Player(int s)
{
    std::cout << "Player created" << std::endl;
    socketFD = s;
}
Player::~Player()
{
    std::cout << "Player destroyed." << std::endl;
}

// write funcs
void Player::writeResponse(int32_t)
{
    
}

