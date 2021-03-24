#include"../headers/baddie.h"
#include"../headers/player.h"
#include"../headers/room.h"
#include"../headers/splitter.h"
#include"../headers/structs.h"
#include"../headers/gamemaster.h"
#include<errno.h>
#include<limits.h>
#include<stdlib.h>
#include<string.h>
#include<thread>
#include<fstream>

int main(int argc,char** argv)
{
    if( argc != 2)
    {
        printf("Invalid number of arguments: %s [port]\n",argv[0]);
        exit(-1);
    }

    // sanitize user input
    char *p;
    int num;
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
    // chatter_messages c_m;
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
    int roomCount = 100;
    GM.craftRoomNames(roomCount);
    GM.estabSizes();
    GM.populateSpawner();
    
    // build rooms [portal + roomCount] 
    GM.buildRooms(roomCount);
    GM.populateRooms();

    //establish connection
    
    return 0;
}
