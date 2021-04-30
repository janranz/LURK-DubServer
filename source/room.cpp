#include"../headers/room.h"

Room::Room(std::string name,std::string desc,uint16_t num)
{
    strncpy(M_ToCP(roomTainer.ROOM_NAME),name.c_str(),sizeof(roomTainer.ROOM_NAME));
    roomTainer.ROOM_NAME[31] = 0;
    roomDesc = desc;
    roomTainer.ROOM_NUMBER = num;
    roomTainer.DESC_LENGTH = desc.length();
    std::string m = "The Mysterious Butler";
    strncpy(M_ToCP(rmpm.SENDER_NAME),m.c_str(),sizeof(rmpm.SENDER_NAME));
    difficulty = 1;
    firepower = 1;
    fight_in_progress = false;
    coffer = 0;
    totalKills = 0;
    totalDeaths = 0;

}

void Room::calculateDiff()
{// lock from outside.
    firepower = player_list.size();
}

void Room::emplace_connection(std::shared_ptr<Room> r)
{
    std::lock_guard<std::shared_mutex> lock(rLock);
    room_connections.emplace_back(r);
}

void Room::emplace_player(std::shared_ptr<Player>p)
{
    p->write_reflect();
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        p->giveRoom(roomTainer.ROOM_NUMBER);
        player_list.emplace_back(p);
        calculateDiff();
    }
    {
        std::shared_lock<std::shared_mutex>lock(rLock);
        bundle_update(p);
        single_kill_report(p);
    }
    std::string m = fmt::format("{0} has joined the room to fight by your side!\nCurrent room strength: {1}\n"
        ,p->charTainer.CHARACTER_NAME,std::to_string(firepower));
    int fd = p->getFD();
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            if((*t)->getFD() != fd)
            {
                (*t)->write_msg(rmpm,m);
            }
        }
    }
}

void Room::remove_player(std::shared_ptr<Player>p)
{// assume this to be called after player(p) has been given a new room.
    int pfd = p->getFD();
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            int tfd = (*t)->getFD();
            if(tfd == pfd)
            {
                
                t = player_list.erase(t);
                break; // --t;
            }
        }
        calculateDiff();
        if(player_list.size() == 0)
        {
            respawn_baddies();
        }
    }

    std::string m = fmt::format("{0} has left the room.\n",p->charTainer.CHARACTER_NAME);
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = player_list.begin(); t != player_list.end(); ++t)
        {
            (*t)->write_msg(rmpm, m);
        }
    }
    
}

void Room::bundle_update(std::shared_ptr<Player> p)
{// lock from the outside.
        p->write_reflect();
        p->write_room(roomTainer,roomDesc);
        inform_player_friendly(p);
        inform_baddies(p);
        inform_connections(p); // may be out of order with baddies. double check
        inform_others_player(p); // call from GM. ?
}

void Room::big_bundle_update()
{
    for(auto b = player_list.begin(); b != player_list.end(); ++b)
    {
        bundle_update((*b));
    }
}

bool Room::isValidConnection(uint16_t r)
{
    {
        std::shared_lock<std::shared_mutex> lock(rLock);
        for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
        {
            if((*t)->roomTainer.ROOM_NUMBER == r)
            {
                return true;
            }
        }
    }
    return false;
}
void Room::emplace_baddie(std::shared_ptr<Baddie> b)
{
    {
        std::lock_guard<std::shared_mutex> lock(rLock);
        baddie_list.emplace_back(b);
    }
    if(baddie_list.empty())
    {
        difficulty = 1;
    }else{
        difficulty = baddie_list.size();
    }
}

void Room::inform_connections(std::shared_ptr<Player> p)
{// lock from outside.
    
        // std::shared_lock<std::shared_mutex> lock(rLock); // TEMPORARY
        //inform connections
    for(auto t = room_connections.begin(); t != room_connections.end(); ++t)
    {
        p->write_connection((*t)->roomTainer,(*t)->roomDesc);
    }
    
}
void Room::inform_others_player(std::shared_ptr<Player> p)
{// inform other players, but assume player has joined the room // lock from outside.
    if(!p->isSktAlive())
    {
        p->giveRoom(room_connections.at(0)->roomTainer.ROOM_NUMBER);
    }
    // {
        // std::shared_lock<std::shared_mutex> lock(rLock);
    for(auto t = player_list.begin(); t != player_list.end(); ++t)
    {
        (*t)->write_character(p->charTainer,p->desc);
    }
    // }
}


bool Room::initiate_fight_player(std::shared_ptr<Player> p, unsigned char* target)
{
    //if(compare_to_lowers(M_ToCP(t),M_ToCP((*p)->charTainer.CHARACTER_NAME)))
    std::unique_lock<std::shared_mutex>QLock(rLock,std::defer_lock);
    std::shared_lock<std::shared_mutex>SLock(rLock,std::defer_lock);
    std::string m;

    SLock.lock();
    if(fight_in_progress)
    {
        return false;
    }
    SLock.unlock();
    bool found = false;

    QLock.lock();
    fight_in_progress = true;
    // pvp controller
    found = pvp_controller(p,target);
    // after action report
    mass_kill_report();
    fight_in_progress = false;
    QLock.unlock();
    return true;

}

bool Room::pvp_controller(std::shared_ptr<Player> t, unsigned char* target)
{// locked from outside.
    bool success = false;
    std::shared_ptr<Player> tmp;
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if(compare_to_lowers(M_ToCP((*p)->charTainer.CHARACTER_NAME),M_ToCP(target)))
        {
            tmp = (*p);
            success = true;
            break;
        }
    }
    if(!success)
    {
        return false;
    }
    std::string m;
    std::string d;
    if(!(tmp->isPlayerAlive()))
    {
        m = fmt::format("{0} shoots a piercing-dagger look at {1} as {2} reaches for {3} bat."
        "\nBut {0} has no interest in beating up a corpse\n",
        t->charTainer.CHARACTER_NAME,tmp->charTainer.CHARACTER_NAME,t->genderHeShe,t->genderPos);
        room_write(m);
        return false;
    }
    uint32_t crit;
    uint32_t base;
    uint32_t roll;
    uint32_t negate;
    uint32_t def;
    uint32_t regen;
    uint16_t gold;
    bool dead = false;
    m = fmt::format("{0} shoots a piercing-dagger look at {1} as {2} reaches for {3} bat. Look out!\n",
        t->charTainer.CHARACTER_NAME,tmp->charTainer.CHARACTER_NAME,t->genderHeShe,t->genderPos);
    room_write(m);

    crit = t->getCrit();
    base = t->charTainer.ATTACK;
    roll = ((fast_rand())%((crit + 1) - base) + base);
    if(roll >= (base * 2))
    {
        m = fmt::format("PVP CRITICAL DAMAGE INCOMING: {0} CRANKS OUT MASSIVE DAMAGE: {1} damage!\n",t->charTainer.CHARACTER_NAME,roll);
    }else if(roll < base){
        m = fmt::format("PVP Weak attack incoming: {0} only musters a pathetic {1} damage!\n",t->charTainer.CHARACTER_NAME,roll);
    }else{
        m = fmt::format("PVP Attack Incoming: {0} winds up an attack set to deliver {1} damage!\n",t->charTainer.CHARACTER_NAME,roll);
    }
    room_write(m);
    def = tmp->charTainer.DEFENSE * 2;
    negate = (fast_rand() % (def));
    if(negate >= roll)
    {
        roll = 0;
        m = fmt::format("PVP DAMAGE NEGATED: {0} manages to negate {1} points, taking {0} damage!\n",tmp->charTainer.CHARACTER_NAME,negate,roll);
    }else if((roll - negate) <= 100){
        roll -= negate;
        m = fmt::format("PVP Some Damage negated!: {0} takes some of the blows to {1} body,\nblocking {2} points and taking {3} damage!\n",
            tmp->charTainer.CHARACTER_NAME,tmp->genderPos,negate,roll);
    }else if(negate < 50){
        roll -= negate;
        m = fmt::format("PVP Pathetic damage negated!: {0} just stands there dumbfounded as {1} wails on {2} like a rented mule!\nDamage Taken:{3}\tDamage Negated{4}",
            tmp->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME,tmp->gender,roll,negate);
    }
    room_write(m);
    if(!(tmp->hurt_player(roll)))
    {
        d = fmt::format("{0} proved to be no match for {1}.. but will {0} come back and settle this beef?\n",
        tmp->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME);
        t->tally_pvp();
        dead = true;
    }
    big_bundle_update();
    room_write(m);
    
    if(dead)
    {
        room_write(d);
        gold = tmp->drop_gold();
        m = fmt::format("{0} has fully restored any prior lost health. and loots {1} gold from {2}\n",t->charTainer.CHARACTER_NAME,gold,tmp->charTainer.CHARACTER_NAME);
        t->give_gold(gold);
        t->full_restore_health();
        big_bundle_update();
        room_write(m);
        return true;
    }
    // Player Two's attack now.
    
    return true;
    
    // for(auto p = player_list.begin(); p != player_list.end(); ++p)
    // {

    // }
}
bool Room::initiate_fight_baddie(std::shared_ptr<Player> p)
{// need a routine for those who get in here but are late to the battle. (passes check in GM, but queued to aquire lock)

    std::unique_lock<std::shared_mutex>QLock(rLock,std::defer_lock);
    std::shared_lock<std::shared_mutex>SLock(rLock,std::defer_lock);
    std::string m;

    SLock.lock();
    if(!(isValidBaddie()) || fight_in_progress) // Double check logic here
    {
        // {std::lock_guard<std::mutex>lock(printLock);fmt::print("No baddies!\n");}
        return false;
    }
    SLock.unlock();

    QLock.lock();
    fight_in_progress = true;
    fight_controller(p);
    if(!(isValidBaddie()))
    {
        spread_wealth();
    }
    mass_kill_report();
    fight_in_progress = false;
    QLock.unlock();


    // after report

    
    return true;
    // think about communicating a cleanup routine after death. How will we move them to spawn?
}

void Room::spread_wealth()
{
    std::string m = fmt::format("\n\t-----Coffer Distribution of {0} gold-----\n",coffer);
    std::string n;
    if(!(player_list.empty()))
    {
        int split = player_list.size();
        while(coffer % split != 0)
        {
            coffer++;
        }
        
        for(auto b = player_list.begin(); b != player_list.end(); ++b)
        {
            uint16_t cut = (coffer / split);
            (*b)->give_gold(cut);
            n = fmt::format("{0} obtains {1} gold\n",(*b)->charTainer.CHARACTER_NAME,cut);
            m += n;
        }
        big_bundle_update();
        room_write(m);
        coffer = 0;
    }
}

void Room::fight_controller(std::shared_ptr<Player> inst)
{// locked by unique room lock
    
    std::string m;
    bool next = false;
    uint32_t crit;
    uint16_t base;
    uint32_t roll;
    uint32_t negate;
    uint32_t defMulti;
    uint16_t kills = 0;
    

    int bDex;
    
    bDex = LiveBaddieDex();
    if(bDex != -1)
    {
        m = fmt::format("{0} grows tired of {1} looking at them with googly eyes and decides to start a fight!\n",
        inst->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME); // potential UB.
        room_write(m);
    }else{
        std::lock_guard<std::mutex>lock(printLock);
        fmt::print("bDex crapped out?: {0}",bDex);
    }
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if(!(*p)->isPlayerAlive())
        {
            m = fmt::format("{0}'s pulse grows weaker...\n",(*p)->charTainer.CHARACTER_NAME);
            room_write(m);
            continue;
        }
        if(!isValidBaddie())
        {
            m = fmt::format("{0} and their homies have successfully slain all baddies in the room!\n",(*p)->charTainer.CHARACTER_NAME);
            room_write(m);
            break;
        }else if(next)
        {
            next = false;
            bDex = LiveBaddieDex();
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("bDex sanity check: {0}\n",bDex);}
            m = fmt::format("{0} sees their homie deliver the final blow on the last baddie, and locks eyes with {1}!\n"
                ,(*p)->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
            room_write(m);
        }
        uint32_t multi = 100 * firepower;
        crit = ((*p)->getCrit()) + multi;
        base = (*p)->charTainer.ATTACK;

        if(baddie_list.at(bDex)->is_alive())
        {
            roll = (fast_rand() % ((crit + 1) - base) + base);
            if(roll >= (base * 2))
            {
                m = fmt::format("FRIENDLY CRITICAL DAMAGE INCOMING: {0} CRANKS OUT MASSIVE {1} DAMAGE!\n",(*p)->charTainer.CHARACTER_NAME, roll); // to string?
                room_write(m);
            }
            negate = (fast_rand() % (baddie_list.at(bDex)->bTainer.DEFENSE + 1));
            if(negate >= roll)
            {
                roll = 0;
            }else{
                roll -= negate;
            }
            
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("{0} rolls: {1} damage ({2} negated)!\n",(*p)->charTainer.CHARACTER_NAME,roll,negate);}
            if(!(baddie_list.at(bDex)->hurt_baddie(roll)))
            {// final blow
                m = fmt::format("ENEMY FATALITY: {0} delivers the most savage, and ruthless blow of {1} damage\nto {2}'s goofy looking head, casting him into the lake of fire with the quickness!\n",
                (*p)->charTainer.CHARACTER_NAME,std::to_string(roll),baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
                coffer += baddie_list.at(bDex)->loot_me();
                kills++;
                big_bundle_update();
                room_write(m);
                next = true;
            }else{
                m = fmt::format("{0} delivers a striking blow, dealing {1} damage to {2}. {3} points of damage was negated!\n",
                (*p)->charTainer.CHARACTER_NAME,std::to_string(roll),baddie_list.at(bDex)->bTainer.CHARACTER_NAME, std::to_string(negate));
                big_bundle_update();
                room_write(m);
            }
        }else{
            {std::lock_guard<std::mutex>lock(printLock);fmt::print("Baddie dead?: {0}\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME);}
        }
    }
    uint16_t bonus = difficulty - firepower;
    if((bonus > 0) && isValidBaddie())
    {
        bonus = (difficulty * (fast_rand() % (500) + 1));
        int bDex = LiveBaddieDex();
        if(!(baddie_list.at(bDex)->hurt_baddie(bonus)))
        {
            m = fmt::format("ENEMY FATALITY: {0} takes unnecessary bonus damage equal to {1}! Good Googly Moogly!!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,bonus);
            kills++;
        }else{
            m = fmt::format("BONUS: {0} takes unnecessary bonus damage equal to {1}! holy cow!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,bonus);
        }

        big_bundle_update();
        room_write(m);
        //tally any kills
    }
    if(kills != 0)
    {
        tally_kills(kills);
        totalKills += kills;
    }
        
    // baddies attack
    if(isValidBaddie())
    {
        bDex = LiveBaddieDex();

        for(auto p = player_list.begin(); p != player_list.end(); ++p)
        {
            if((*p)->isPlayerAlive())
            {
                crit = baddie_list.at(bDex)->getCrit();
                base = baddie_list.at(bDex)->bTainer.ATTACK;
                defMulti = (*p)->charTainer.DEFENSE + (100 * firepower);
                roll = (fast_rand() % ((crit + 1) - base) + base);
                if(roll >= (base * 2))
                {
                    m = fmt::format("ENEMY CRITICAL DAMAGE INCOMING: {0} CRANKS OUT MASSIVE {1} DAMAGE!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME, std::to_string(roll));
                room_write(m);
                }
                // negate = (fast_rand() % ((*p)->charTainer.DEFENSE + 1));
                negate = (fast_rand() % (defMulti + 1));
                if(negate >= roll)
                {
                    roll = 0;
                }else{
                    roll -= negate;
                }
                {std::lock_guard<std::mutex>lock(printLock);fmt::print("{0} rolls: {1} damage ({2} negated)!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,roll,negate);}
                if(!((*p)->hurt_player(roll)))
                {
                    m = fmt::format("{0} delivers a fatal blow to {1}...\nslap, chop, smacking them back to the Portal Room! Yikes!\n({2} damage, {3} negated)\n"
                        ,baddie_list.at(bDex)->bTainer.CHARACTER_NAME, (*p)->charTainer.CHARACTER_NAME,roll,negate);
                    totalDeaths++;
                }else if(roll == 0){
                    m = fmt::format("{0} face-tanks the damage, negating {1} points! Savage!\n",(*p)->charTainer.CHARACTER_NAME,negate);
                }else{
                    m = fmt::format("{0} soaks up a painful {1} in damage, after negating {2} points... delivered by {3}!\n",(*p)->charTainer.CHARACTER_NAME,roll,negate,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
                }
            }else{
                m = fmt::format("{0} takes a whiff of {1}'s unconscious body but is allergic to the weakness! Achoo!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,(*p)->charTainer.CHARACTER_NAME);
            }
                big_bundle_update();
                room_write(m);
        }
    }else{
        m = fmt::format("All baddies have been cleared, and you live to see another day.\n");
        room_write(m);
    }
    // uint16_t bonus = difficulty - firepower;
    // if((bonus > 0) && isValidBaddie())
    // {
    //     bonus = (difficulty * (fast_rand() % (500) + 1));
    //     int bDex = LiveBaddieDex();
    //     if(!(baddie_list.at(bDex)->hurt_baddie(bonus)))
    //     {
    //         m = fmt::format("ENEMY FATALITY: {0} takes unnecessary bonus damage equal to {1}! Good Googly Moogly!!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,bonus);
    //         currentKills++;
    //     }else{
    //         m = fmt::format("BONUS: {0} takes unnecessary bonus damage equal to {1}! holy cow!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,bonus);
    //     }

    //     big_bundle_update();
    //     room_write(m);
    // }
    //regenerate
    int genMulti;
    //player
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if((*p)->isPlayerAlive())
        {
            if(difficulty > firepower)
            {
                genMulti = 1000;
            }else{
                genMulti = 500;
            }
            genMulti += (fast_rand() % ((*p)->charTainer.REGEN + 1) + 1);
            (*p)->heal_player(genMulti);
            big_bundle_update();
            
            m = fmt::format("{0} recovers {1} health!\n",(*p)->charTainer.CHARACTER_NAME,genMulti);
        }else{
            m = fmt::format("{0} fails to heal because... well.. they dead.\n",(*p)->charTainer.CHARACTER_NAME);
        }
        room_write(m);
    }

    int i = liveBaddieCount();
    if(i)
    {
        m = fmt::format("{0} baddies remaining!\n",std::to_string(i));
    }else{
        m = fmt::format("No more baddies remain!\n",std::to_string(i));
    }
    room_write(m);

    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->is_alive())
        {
            if(firepower > difficulty)
            {
                genMulti = 500;
            }else{
                genMulti = 0;
            }
            genMulti += (fast_rand() % ((*b)->bTainer.REGEN + 1) + 1);
            (*b)->heal_baddie(genMulti);
            big_bundle_update();
            
            m = fmt::format("{0} has recovered {1} health! and is angry as heck\n YIKES!",(*b)->bTainer.CHARACTER_NAME,genMulti);
        }else{
            m = fmt::format("{0} lies there, looking ugly as sin.\n Whew...",(*b)->bTainer.CHARACTER_NAME);
        }
        room_write(m);

    }
    m = fmt::format("Current Room Coffer: {0} gold!\n",coffer);
    room_write(m);
}

void Room::tally_kills(uint16_t k)
{// locked outside
        for(auto p = player_list.begin(); p != player_list.end(); ++p)
        {
            if((*p)->isPlayerAlive())
            {
                (*p)->tally_curr(k);
            }
        }
}

void Room::mass_kill_report()
{// not thread safe.
    std::string m = fmt::format("\n\t----- Kill Count -----\n");
    std::string n;
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        n = fmt::format("----- {0} Current Kills: {1}\tHigh Score: {2} -----\n",(*p)->charTainer.CHARACTER_NAME,(*p)->getCurrScore(),(*p)->getHighScore());
        m += n;
    }
    n = fmt::format("\n----- CURRENT ROOM STATS -----\nTotal Kills: {0}\nCoffer:{1}\n",totalKills,coffer);
    m += n;
    room_write(m);

}

void Room::single_kill_report(std::shared_ptr<Player> b)
{// not thread safe.
    std::string m = fmt::format("\n\t----- Kill Count -----\n");
    std::string n;
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        n = fmt::format("----- {0} Current Kills: {1}\tHigh Score: {2} -----\n",(*p)->charTainer.CHARACTER_NAME,(*p)->getCurrScore(),(*p)->getHighScore());
        m += n;
    }
    n = fmt::format("\n----- CURRENT ROOM STATS -----\nTotal Kills: {0}\nCoffer:{1}\n",totalKills,coffer);
    m += n;
    b->write_msg(rmpm,m);

}

void Room::respawn_baddies()
{
    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if(!(*b)->is_alive())
        {
            (*b)->respawn();
        }
    }
}

void Room::room_write(std::string m)
{
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        (*p)->write_msg(rmpm,m);
    }
}

bool Room::isFightInProgress()
{
    {
        std::shared_lock<std::shared_mutex>lock(rLock);
        return fight_in_progress;
    }
}

bool Room::isValidBaddie()
{// locked by initiate_fight

    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
            return true;
    }
    return false;
}

int Room::LiveBaddieDex()
{
    liveDex.clear();
    int i = 0;
    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
        {
            liveDex.emplace_back(i); // value of i DOES matter here.
        }
        i++;
    }
    {std::lock_guard<std::mutex>lock(printLock);fmt::print("Size of live baddies: {0}\n",liveDex.size());}
    if(liveDex.size() > 0)
    {
        return liveDex.at((fast_rand() % liveDex.size()));
    }else{
        return -1; // should never hit.
    }
}

int Room::liveBaddieCount()
{
    int i = 0;
    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
        {
            i++;
        }
    }
    return i;
}

void Room::inform_player_friendly(std::shared_ptr<Player> p)
{// lock from outside.
    int fd = p->getFD();
    
        // std::shared_lock<std::shared_mutex> lock(rLock);
    for(auto t = player_list.begin(); t != player_list.end(); ++t)
    {
        if(fd != (*t)->getFD())
        {
            p->write_character((*t)->charTainer,(*t)->desc);
        }
    }
}
void Room::inform_baddies(std::shared_ptr<Player> p)
{// static no rLock but baddie lock(bLock)
    
    for(auto t = baddie_list.begin(); t != baddie_list.end(); ++t)
    {
        // std::lock_guard<std::mutex> lock((*t)->bLock);
        p->write_character((*t)->bTainer,(*t)->desc);
    }
}
