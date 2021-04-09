#include"../headers/player.h"


Player::Player(int s)
{
    std::cout << "Player created socket: "<< s << std::endl;
    socketFD = s;
    quitter = false;
    inMaster = false;
    pLock = std::make_shared<std::mutex>();
}
Player::~Player()
{
    
    // std::cout << "Player destroyed." << std::endl;
}

// write funcs
ssize_t Player::writeToMe(LURK_MSG lurk_msg,char* data)
{
    std::cout << "FD: " << socketFD << std::endl;
    std::cout << "DATA: " << data << std::endl;
    ssize_t bytes = 0;
    {
        std::lock_guard<std::mutex> lock(*pLock);

        bytes = write(socketFD,&lurk_msg,sizeof(LURK_MSG));
        if(bytes < 0){return bytes;}
        bytes = write(socketFD,data,lurk_msg.MSG_LEN);
    }
    std::cout << "message sent to: " << charTainer.CHARACTER_NAME << std::endl;
        return bytes;
}
