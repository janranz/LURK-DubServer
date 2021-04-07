#ifndef GM_H
#define GM_H
#include"../headers/structs.h"
#include"../headers/baddie.h"
#include"../headers/player.h"
#include"../headers/room.h"

#include<cstdint>
#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<thread>
#include<mutex>
#include<memory>
#include"../headers/fmt/format.h"



class Gamemaster
{
    private:
        static unsigned int g_seed;
        static uint16_t MAX_STAT;
        static int16_t BASE_HEALTH;
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
        // use dataDump with memset() to attempt to clear client's skt buffer in case of err.
        char dataDump[BIG_BUFFER];
        chatter_messages c_m;
        std::vector<Baddie> BDSpawner;
        
        std::vector<Room> MasterRoomList;
        std::vector<Player*> MasterPlayerList;
        std::shared_ptr<std::mutex> masterLock;
        

    public:
        std::mutex GMlock;
        Gamemaster();
        ~Gamemaster();
        int fast_rand(void);
        //server innit functions
        bool buildChatter(int,std::vector<std::string>::iterator);
        void craftRoomNames(int);
        void estabSizes();
        void populateSpawner();
        void buildRooms(int);
        void populateRooms();

        // network functions
        void GMController(int);
        bool checkStats(Player*);
        // bool checkStats(std::unique_ptr<Player>);

        void GMPM(Player*,std::string&); // personal custom messenger for GM
        void mailroom(Player*,int,int32_t);
        void postman(Player*,LURK_MSG,char*);
        void gatekeeper(char,Player*,uint8_t,uint8_t); // fast access to accept/deny
        void census(int);
        void movePlayer(Player*,char);
        void ragequit(Player*);
};

#endif //GM_H
