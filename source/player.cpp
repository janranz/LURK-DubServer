#include"../headers/player.h"


Player::Player(int s)
{
    std::cout << "Player created socket: "<< s << std::endl;
    socketFD = s;
}
Player::~Player()
{
    printf("wtf is deleting: %d\n",socketFD);
    std::cout << "Player destroyed." << std::endl;
}

// write funcs
void Player::writeToMe(LURK_MSG lurk_msg,char* data)
{
    std::cout << "Test: " << socketFD << std::endl;
    std::cout << "DATA: " << data << std::endl;
    // std::lock_guard<std::mutex> lock(*pLock);
    if(write(socketFD,&lurk_msg,sizeof(LURK_MSG) == -1)){return;}
    if(write(socketFD,&data,sizeof(lurk_msg.MSG_LEN) == -1)){return;}
    std::cout << "message sent to: " << charTainer.CHARACTER_NAME << std::endl;
}

