#include"../headers/baddie.h"
#include"../headers/player.h"
#include"../headers/room.h"
#include"../headers/splitter.h"
#include"../headers/structs.h"
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
    chatter_messages c_m;
    
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
            // std::cout << "Line: " << *m << std::endl;
            switch(i)
            {
                case 0:{c_m.awaken.push_back(*m);c_m.awakenKEY = i;break;}
                case 1:{c_m.baddies.push_back(*m);c_m.baddiesKEY = i;break;}
                case 2:{c_m.danger.push_back(*m);c_m.dangerKEY = i;break;}
                case 3:{c_m.death.push_back(*m);c_m.deathKEY = i;break;}
                case 4:{c_m.error.push_back(*m);c_m.errorKEY = i;break;}
                case 5:{c_m.fightHit.push_back(*m);c_m.fightHitKEY = i;break;}
                case 6:{c_m.food.push_back(*m);c_m.foodKEY = i;break;}
                case 7:{c_m.health.push_back(*m);c_m.healthKEY = i;break;}
                case 8:{c_m.loot.push_back(*m);c_m.lootKEY = i;break;}
                case 9:{c_m.pvp.push_back(*m);c_m.pvpKEY = i;break;}
                case 10:{c_m.roomNames.push_back(*m);c_m.roomNamesKEY = i;break;}
                case 11:{c_m.safeEv.push_back(*m);c_m.safeEvKEY = i;break;}
                case 12:{c_m.trans.push_back(*m);c_m.transKEY = i;break;}
                case 13:{c_m.weapons.push_back(*m);c_m.weaponsKEY = i;break;}
            }
        }
        i++;
    }
    for(auto t : fds)
    {
        delete t;
    }
    fds.clear();
    
    return 0;
}
