#ifndef PLAYER_H
#define PLAYER_H

#include"../headers/structs.h"
#include"../headers/splitter.h"
#include"../headers/fmt/format.h"
#include<shared_mutex>
#include<iostream>
#include<unistd.h>
#include<bsd/string.h>
class Player
{
    private:
        int socketFD;
        bool sktAlive;
        bool started;
        bool validToon;
        bool freshSpawn;
        bool playerAlive;
        uint16_t highPVEScore;
        uint16_t currPVEScore;
        uint16_t pvpKills;


        // fight logic
        uint32_t critDamage;
        uint32_t totalDeaths;
    
    public:
        LURK_CHARACTER charTainer;
        std::string desc;
        std::string gender;
        std::string genderPos;
        std::string genderHeShe;
        std::shared_mutex pLock;

        Player(int);

        //state check
        bool isSktAlive();
        bool isStarted();
        bool isValidToon();
        bool isFreshSpawn();
        bool isPlayerAlive();

        //state set
        void startPlayer(bool);
        void quitPlayer();
        void setValid();
        void giveRoom(uint16_t);
        void respawn();
        void despawn();
        bool hurt_player(int32_t);
        void heal_player(int32_t);
        void tally_PVE_kill();
        void give_gold(uint32_t);
        void take_gold(uint16_t); // refund
        
        void tally_pvp();
        void full_restore_health();
        

        //getter
        int getFD();
        uint16_t getCrit();
        uint16_t getRoomNumber();
        uint16_t getHighScore();
        uint16_t getCurrScore();
        uint16_t get_gold();
        uint16_t loot_me();
        uint16_t getPVPKills();
        uint16_t drop_gold();
        uint16_t get_deaths();
        //writer
        void write_msg(LURK_MSG, std::string);
        void write_error(LURK_ERROR, std::string);
        void write_accept(uint8_t);
        void write_room(LURK_ROOM,std::string);
        void write_character(LURK_CHARACTER, std::string);
        void write_game(LURK_GAME, std::string);
        void write_connection(LURK_ROOM,std::string);
        void write_version(LURK_VERSION);
        void write_reflect();
};

#endif // PLAYER_H
