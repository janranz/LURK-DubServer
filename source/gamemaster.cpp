#include"../headers/room.h"


void Room::emplace_connection(int r)
{
    connection_dex.emplace_back(r);
}
void Room::emplace_player(Player* p)
{
    Player* p2 = p;
    player_list.emplace_back(p2);
}
