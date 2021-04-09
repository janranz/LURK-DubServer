#include"../headers/gamemaster.h"
#include"../headers/splitter.h"
#include<errno.h>
#include<limits.h>
#include<stdlib.h>
#include<string.h>
#include<thread>
#include<fstream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<signal.h>
#include<stdbool.h>
#include<iostream>
#include<vector>
#include<string>

// volatile bool STOP = false;
// void sigint_handler(int sig);


int main(int argc,char** argv)
{
    if( argc != 2)
    {
        printf("Invalid number of arguments: %s [port]\n",argv[0]);
        exit(-1);
    }

    // check port input
    char *p;
    errno = 0;
    long port = strtol(argv[1], &p, 10);
    if(errno != 0 || *p != '\0' || port > INT_MAX || port < INT_MIN)
    {
        printf("Unable to accept port argument! errno: %s\n",strerror(errno));
        exit(-1);
    }


    std::ifstream fileNames;
    fileNames.open("../chatter/00fileNames.txt", std::ios::in);
    if(!fileNames.is_open())
    {
        std::cout << "Could not find 00fileNames.txt" << std::endl;
    }
    std::string line;
    std::vector<std::string> parsed;

    while(getline(fileNames,line))
    {
        std::vector<std::string> pr = splitter(line,"\n");
        parsed.insert(parsed.end(),pr.begin(),pr.end());
    }
    fileNames.close();
    std::vector<std::ifstream*> fds;
    for(std::vector<std::string>::iterator t = parsed.begin(); t != (parsed.end()); t++)
    {
        std::cout << "Filename: " << *t << std::endl;
        std::ifstream *fd = new std::ifstream;
        std::string buildStr = "../chatter/" + *t;
        fd->open(buildStr, std::fstream::in);
        fds.push_back(fd);
    }
    int i = 0;
    Gamemaster GM;
    
    for(auto &t : fds)
    {
        parsed.clear();
        std::cout << "Building object number: "<< (i+1) << std::endl;
        while(getline(*t,line))
        {
            std::vector<std::string> pr = splitter(line,"\n");
            parsed.insert(parsed.end(),pr.begin(),pr.end());
        }
        t->close();
        for(std::vector<std::string>::iterator m = parsed.begin(); m != (parsed.end()); m++)
        {
            bool checksOut = GM.buildChatter(i,m);
            if(!checksOut)
                std::cout << "Failure in buildChatter()!" << std::endl;
        }
        i++;
    }
    for(auto t : fds){delete t;}
    fds.clear();
    // build baddies
    // int roomCount = 1000;
    GM.craftRoomNames();
    GM.estabSizes();
    GM.populateSpawner();
    
    // build rooms [portal + roomCount] 
    GM.buildRooms();
    GM.populateRooms();

    //establish connection
    // signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

    struct sockaddr_in sai;
    sai.sin_family = AF_INET;
    sai.sin_addr.s_addr = INADDR_ANY;
    sai.sin_port = htons(port);

    int dubSkt = socket(AF_INET,SOCK_STREAM, 0);
    bind(dubSkt, (struct sockaddr *)(&sai), sizeof(struct sockaddr_in));
    listen(dubSkt, 32);

    int player_fd;
    struct sockaddr_in client_addr;
    socklen_t address_size = sizeof(struct sockaddr_in);

    while(1)
    {
        std::cout << "Listening..." << std::endl;
        player_fd = accept(dubSkt, (struct sockaddr*)(&client_addr), &address_size);
        printf("DEBUG: %s has successfully connected.\n",inet_ntoa(client_addr.sin_addr));

        std::thread t1(&Gamemaster::GMController,&GM,player_fd);
        t1.detach();
    }
    // say goodbye.
    return 0;
}

// void sigint_handler(int sig){STOP = true;}
