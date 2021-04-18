#include"../headers/fmt/format.h"
#include"../headers/splitter.h"
#include"../headers/gamemaster.h"
#include"../headers/structs.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<string>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout << fmt::format("Invalid number of arguments: {0} [port]\n",argv[0]);
        exit(-1);
    }
    //check port input
    char *p;
    errno = 0;
    long port = strtol(argv[1], &p, 10);
    if(errno != 0 || *p != '\0' || port > INT_MAX || port < INT_MIN)
    {
        std::cout << fmt::format("Unable to accept port argument. {0}\n",strerror(errno));
    }

    //read in game dialogue / messages.
    std::ifstream fileNames;
    fileNames.open("../chatter/00fileNames.txt", std::ios::in);
    if(!fileNames.is_open())
    {
        std::cout << fmt::format("Could not find 00fileNames.txt");
    }
    std::string line;
    std::vector<std::string> parsed;
    
    while(getline(fileNames,line))
    {
        std::vector<std::string> pr = splitter(line,"\n");
        parsed.insert(parsed.end(), pr.begin(), pr.end());
    }
    fileNames.close();

    //build vector of file descriptors
    std::vector<std::ifstream*> fds;
    for(std::vector<std::string>::iterator t = parsed.begin(); t != parsed.end(); ++t)
    {
        std::ifstream* fd = new std::ifstream;
        std::string buildStr = "../chatter/" + *t;
        fd->open(buildStr, std::fstream::in);
        fds.emplace_back(fd);
    }
    int i = 0;
    Gamemaster GM;

    for(auto &t : fds)
    {
        parsed.clear();
        while(getline(*t,line))
        {
            std::vector<std::string> pr = splitter(line, "\n");
            parsed.insert(parsed.end(),pr.begin(),pr.end());
        }
        t->close();
        for(std::vector<std::string>::iterator m = parsed.begin(); m != parsed.end(); ++m)
        {
            GM.build_chatter(i,m);
        }
        i++;
    }
    for(auto t : fds){delete t;}
    fds.clear();
    GM.size_vectors();


    return 0;
}
