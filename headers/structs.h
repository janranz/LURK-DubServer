#ifndef STRUCTS_H
#define STRUCTS_H
#include<string>
#include<vector>

struct chatter_messages
{
    std::vector<std::string> awaken,baddies,danger,death,error,fightHit,food,health,
                             loot,pvp,roomNames,safeEv,trans,weapons,baddie_desc,room_desc,
                             adj,noun,roomTainer;
    // char awakenKEY,baddiesKEY,dangerKEY,deathKEY,errorKEY,fightHitKEY,foodKEY,healthKEY,
    //                          lootKEY,pvpKEY,roomNamesKEY,safeEvKEY,transKEY,weaponsKEY;
};

#endif //STRUCTS_H
