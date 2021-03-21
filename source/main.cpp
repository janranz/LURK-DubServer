#include"baddie.h"
#include"player.h"
#include"room.h"
#include"splitter.h"
#include<fstream>

int main(int argc,char** argv)
{
    if( argc != 3)
    {
        std::cout << "Please include a HOST and PORT" << std::endl;
        exit(-1);
    }
    std::ifstream fileNames;
    fileNames.open("../chatter/00fileNames.txt", std::fstream::in);
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

    for(std::vector<std::string>::iterator t = parsed.begin(); t != (parsed.end()); t++)
    {
        std::cout << "Filename: " << *t << std::endl;
    }
    return 0;
}
