#ifndef GM_H
#define GM_H
#include"../headers/structs.h"
#include"../headers/baddie.h"
#include"../headers/player.h"
#include"../headers/room.h"

#include<cstdint>
#include<time.h>
#include<sys/socket.h>

class Gamemaster
{
    private:
        static unsigned int g_seed;
        static uint16_t MAX_STAT;
        static uint16_t MAX_BADDIES;
        static uint16_t AWAKEN_VECTOR_SIZE;
        static uint16_t BADDIES_VECTOR_SIZE;
        static uint16_t DANGER_VECTOR_SIZE;
        static uint16_t DEATH_VECTOR_SIZE;
        static uint16_t ERROR_VECTOR_SIZE;
        static uint16_t FIGHTHIT_VECTOR_SIZE;
        static uint16_t FOOD_VECTOR_SIZE;
        static uint16_t HEALTH_VECTOR_SIZE;
        static uint16_t LOOT_VECTOR_SIZE;
        static uint16_t PVP_VECTOR_SIZE;
        static uint16_t ROOMNAMES_VECTOR_SIZE;
        static uint16_t SAFEEV_VECTOR_SIZE;
        static uint16_t TRANS_VECTOR_SIZE;
        static uint16_t WEAPONS_VECTOR_SIZE;
        static uint16_t BADDIE_DESC_SIZE;
        static uint16_t ROOM_DESC_SIZE;

        chatter_messages c_m;
        std::vector<Baddie*> BDSpawner;
        std::vector<Room*> MasterRoomList;
        std::vector<Player> MasterPlayerList;
        

    public:
        std::mutex GMlock;
        Gamemaster();
        ~Gamemaster();
        // inline void fast_srand(int seed);
        int fast_rand(void);
        bool buildChatter(int,std::vector<std::string>::iterator);
        void craftRoomNames(int);
        void estabSizes();
        void populateSpawner();
        void buildRooms(int);
        void populateRooms();

        void constructPlayer(int);
        
        // void pushPlayerList(int);
        // void popPlayerList(int);

        // network functions
        void GMController();
        void ragequit();
};

#endif //GM_H
