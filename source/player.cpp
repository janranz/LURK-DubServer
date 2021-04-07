#include"../headers/player.h"


Player::Player(int s)
{
    std::cout << "Player created socket: "<< s << std::endl;
    socketFD = s;
    pLock = std::make_shared<std::mutex>();
}
Player::~Player()
{
    
    // std::cout << "Player destroyed." << std::endl;
}

// write funcs
void Player::writeToMe(LURK_MSG lurk_msg,char* data)
{
    std::cout << "FD: " << socketFD << std::endl;
    std::cout << "DATA: " << data << std::endl;

    {
        std::lock_guard<std::mutex> lock(*pLock);

        write(socketFD,&lurk_msg,sizeof(LURK_MSG));
        write(socketFD,data,lurk_msg.MSG_LEN);
    }
    std::cout << "message sent to: " << charTainer.CHARACTER_NAME << std::endl;
}
