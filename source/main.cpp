#include"../headers/fmt/format.h"
#include"../headers/splitter.h"
#include"../headers/gamemaster.h"
#include"../headers/structs.h"
#include"../headers/fmt/format.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<thread>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout << fmt::format("Invalid number of arguments: {0} [port]\n",argv[0]);
        exit(-1);
    }
    //check port input consider if I should set memory
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
    GM.craft_room_names();
    GM.build_rooms();
    GM.populate_rooms();

    //establish connection
    signal(SIGPIPE, SIG_IGN);
    
    struct sockaddr_in sai;
    sai.sin_family = AF_INET;
    sai.sin_addr.s_addr = INADDR_ANY;
    sai.sin_port = htons(port);

    int dubSkt = socket(AF_INET,SOCK_STREAM, 0);
    bind(dubSkt, (struct sockaddr *)(&sai),sizeof(struct sockaddr_in));
    listen(dubSkt,32);

    int player_fd;
    struct sockaddr_in client_addr;
    socklen_t address_size = sizeof(struct sockaddr_in);

    //listener loop.
    while(1)
    {
        std::cout << "Listening..." << std::endl;
        player_fd = accept(dubSkt, (struct sockaddr*)(&client_addr), &address_size);
        std::cout << fmt::format("Connection established from {0}\n",inet_ntoa(client_addr.sin_addr));

        std::thread t1(&Gamemaster::GMController,&GM,player_fd);

        t1.detach();
    }
    return 0;
}
