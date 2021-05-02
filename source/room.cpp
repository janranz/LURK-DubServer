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

bool Room::initiate_loot_sequence(std::shared_ptr<Player> p, unsigned char* target)
{
    
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
    
    bool found;
    QLock.lock();
    fight_in_progress = true;
    // pvp controller
    found = pvp_controller(p,target);
    // after action report
    mass_kill_report();
    fight_in_progress = false;
    QLock.unlock();
    return found;

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
    std::string after_action = "\n========== PVP FIGHT SUMMARY ==========\n";
    bool dead = false;
    after_action += fmt::format("\nInstigator: {0}\n",t->charTainer.CHARACTER_NAME);
    if(!(tmp->isPlayerAlive()))
    {
        after_action += fmt::format("{0} shoots a piercing-dagger look at {1} as {2} reaches for {3} bat."
        " But {0} has no interest in beating up a corpse\n",
        t->charTainer.CHARACTER_NAME,tmp->charTainer.CHARACTER_NAME,t->genderHeShe,t->genderPos);
        room_write(after_action);
        return false;
    }
    uint32_t crit;
    uint32_t base;
    uint32_t roll;
    uint32_t negate;
    uint32_t def;
    // uint32_t regen;
    uint32_t gold;
    after_action += fmt::format("{0} shoots a piercing-dagger look at {1} as {2} reaches for {3} bat. Look out!\n",
        t->charTainer.CHARACTER_NAME,tmp->charTainer.CHARACTER_NAME,t->genderHeShe,t->genderPos);
    // room_write(m);
    after_action += fmt::format("\n-ATTACKER DATA: [Attacker: {0} | Health: {1}] vs [Target: {2} | Health: {3}]\n\n",
    t->charTainer.CHARACTER_NAME, static_cast<int16_t>(t->charTainer.HEALTH),tmp->charTainer.CHARACTER_NAME,static_cast<int16_t>(tmp->charTainer.HEALTH));
    crit = t->getCrit();
    base = t->charTainer.ATTACK;
    roll = ((fast_rand())%((crit + 1) - base) + base);
    if(roll >= (base * 2))
    {
        after_action += fmt::format("ATTACKER DAMAGE(CRITICAL): {0} generates {1} points of damage! Look out!\n",t->charTainer.CHARACTER_NAME,roll);
    }else if(roll < base){
        after_action += fmt::format("ATTACKER DAMAGE(WEAK): {0} only musters a pathetic {1} damage!\n",t->charTainer.CHARACTER_NAME,roll);
    }else{
        after_action += fmt::format("ATTACKER DAMAGE(NORMAL): {0} winds up an attack set to deliver {1} damage!\n",t->charTainer.CHARACTER_NAME,roll);
    }
    // room_write(m);
    def = tmp->charTainer.DEFENSE * 2;
    negate = (fast_rand() % (def));
    if(negate >= roll)
    {
        roll = 0;
        int16_t netNeg = abs(negate-roll);
        after_action += fmt::format("TARGET NEGATES(FULL): {0} manages to negate all {1} points, taking absolutely {0} damage!\n",tmp->charTainer.CHARACTER_NAME,netNeg,roll);
    }else if((roll - negate) <= 100){
        roll -= negate;
        after_action += fmt::format("TARGET NEGATES(PARTIAL): {0} takes some of the blows to {1} body,\nblocking {2} points and taking {3} damage!\n",
            tmp->charTainer.CHARACTER_NAME,tmp->genderPos,negate,roll);
    }else if(negate < 50){
        roll -= negate;
        after_action += fmt::format("TARGET NEGATES(MINIMAL): {0} just stands there dumbfounded as {1} wails on {2} like a rented mule!\nDamage Taken:{3}\tDamage Negated:{4}\n",
            tmp->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME,tmp->gender,roll,negate);
    }else{
        after_action += fmt::format("TARGET NEGATES: {0} takes a boppin', soaking up {1} points of {2}'s {3} damage. Wow.\n",tmp->charTainer.CHARACTER_NAME,negate,t->charTainer.CHARACTER_NAME,roll);
    }
    // room_write(m);
    if(!(tmp->hurt_player(roll)))
    {
        after_action += fmt::format("TARGET WOUNDED(FATAL): {0} proved to be no match for {1}.. but will {0} come back and settle this beef?\n",
        tmp->charTainer.CHARACTER_NAME,t->charTainer.CHARACTER_NAME);
        t->tally_pvp();
        totalDeaths++;
        dead = true;
    }else{
        after_action += fmt::format("TARGET WOUNDED: {0} was able to survive the daunting blow, leaving {1} at {2} HP! MEDIC!\n",tmp->charTainer.CHARACTER_NAME,tmp->genderPos,static_cast<int16_t>(tmp->charTainer.HEALTH));
    }
    big_bundle_update();
    // room_write(m);
    
    if(dead)
    {
        // room_write(d);
        gold = tmp->drop_gold();
        after_action += fmt::format("{0} has fully restored any prior lost health. and loots {1} gold from {2}\n",t->charTainer.CHARACTER_NAME,gold,tmp->charTainer.CHARACTER_NAME);
        t->give_gold(gold);
        t->full_restore_health();
        big_bundle_update();
        room_write(after_action);

        return true;
    }
    // Player Two's attack now.
    dead = false;
    if((t->isPlayerAlive()))
    {
        after_action += fmt::format("{0} manages to pick {1}self back up onto {1} feet and is now determined to return {2} the kind gesture. Oh no!\n",
        tmp->charTainer.CHARACTER_NAME, tmp->gender,t->charTainer.CHARACTER_NAME);

        after_action += fmt::format("\n-ATTACKER DATA: [Attacker: {0} | Health: {1}] vs [Target: {2} | Health: {3}]\n\n",
        tmp->charTainer.CHARACTER_NAME, static_cast<int16_t>(tmp->charTainer.HEALTH),t->charTainer.CHARACTER_NAME,static_cast<int16_t>(t->charTainer.HEALTH));

        ///HERE
        crit = tmp->getCrit();
        base = tmp->charTainer.ATTACK;
        roll = ((fast_rand())%((crit + 1) - base) + base);
        if(roll >= (base * 2))
        {
            after_action += fmt::format("ATTACKER DAMAGE(CRITICAL): {0} generates {1} points of damage! Look out!\n",tmp->charTainer.CHARACTER_NAME,roll);
        }else if(roll < base){
            after_action += fmt::format("ATTACKER DAMAGE(WEAK): {0} only musters a pathetic {1} damage!\n",tmp->charTainer.CHARACTER_NAME,roll);
        }else{
            after_action += fmt::format("ATTACKER DAMAGE(NORMAL): {0} winds up an attack set to deliver {1} damage!\n",tmp->charTainer.CHARACTER_NAME,roll);
        }
        // room_write(m);
        def = t->charTainer.DEFENSE * 2;
        negate = (fast_rand() % (def));
        if(negate >= roll)
        {
            roll = 0;
            int16_t netNeg = abs(negate-roll);
            after_action += fmt::format("TARGET NEGATES(FULL): {0} manages to negate all {1} points, taking absolutely {2} damage!\n",t->charTainer.CHARACTER_NAME,netNeg,roll);
        }else if((roll - negate) <= 100){
            roll -= negate;
            after_action += fmt::format("TARGET NEGATES(PARTIAL): {0} takes some of the blows to {1} body,\nblocking {2} points and taking {3} damage!\n",
                t->charTainer.CHARACTER_NAME,t->genderPos,negate,roll);
        }else if(negate < 50){
            roll -= negate;
            after_action += fmt::format("TARGET NEGATES(MINIMAL): {0} just stands there dumbfounded as {1} wails on {2} like a rented mule!\nDamage Taken:{3}\tDamage Negated:{4}\n",
                t->charTainer.CHARACTER_NAME,tmp->charTainer.CHARACTER_NAME,t->gender,roll,negate);
        }else{
            after_action += fmt::format("TARGET NEGATES: {0} takes a boppin', soaking up {1} points of {2}'s {3} damage. Wow.\n",t->charTainer.CHARACTER_NAME,negate,tmp->charTainer.CHARACTER_NAME,roll);
        }
        // room_write(m);
        if(!(t->hurt_player(roll)))
        {
            after_action += fmt::format("TARGET WOUNDED(FATAL): {0} proved to be no match for {1}.. but will {0} come back and settle this beef?\n",
            t->charTainer.CHARACTER_NAME,tmp->charTainer.CHARACTER_NAME);
            tmp->tally_pvp();
            dead = true;
        }else{
            after_action += fmt::format("TARGET WOUNDED: {0} was able to survive the daunting blow, leaving {1} at {2} HP! MEDIC!\n",t->charTainer.CHARACTER_NAME,t->genderPos,static_cast<int16_t>(t->charTainer.HEALTH));
        }
        big_bundle_update();
        // room_write(m);
        
        if(dead)
        {
            // room_write(d);
            gold = tmp->drop_gold();
            after_action += fmt::format("{0} has fully restored any prior lost health. and loots {1} gold from {2}\n",tmp->charTainer.CHARACTER_NAME,gold,t->charTainer.CHARACTER_NAME);
            tmp->give_gold(gold);
            tmp->full_restore_health();
            big_bundle_update();
            room_write(after_action);
            totalDeaths++;
            return true;
        }
        after_action += "\n-PLAYER REGEN DATA:\n";
        // int16_t genMulti = (fast_rand() % (((serverStats::PLAYER_BASE_HEALTH + 1) - (t->charTainer.REGEN) / 2)) + (t->charTainer.REGEN / 2));
        int16_t genMulti = (fast_rand() % ((((t->charTainer.REGEN) + 1) - (t->charTainer.REGEN) / 2)) + (t->charTainer.REGEN / 2));
        int16_t pastH = t->charTainer.HEALTH;
        t->heal_player(genMulti);
        after_action += fmt::format("{0} dresses {1} wounds, recovering {2} points to {1} past {3} health, restoring {4} to {5} HP!\n",
        t->charTainer.CHARACTER_NAME,t->genderPos,genMulti,pastH,t->gender,static_cast<int16_t>(t->charTainer.HEALTH));
        
        // genMulti = (fast_rand() % (((serverStats::PLAYER_BASE_HEALTH + 1) - (tmp->charTainer.REGEN) / 2)) + (tmp->charTainer.REGEN / 2));
        genMulti = (fast_rand() % ((((t->charTainer.REGEN) + 1) - (tmp->charTainer.REGEN) / 2)) + (tmp->charTainer.REGEN / 2));
        pastH = tmp->charTainer.HEALTH;
        tmp->heal_player(genMulti);
        after_action += fmt::format("{0} squashes the beef, and recovers {1} points to {2} past {3} health, restoring {4}self to {5} HP!\n",
        tmp->charTainer.CHARACTER_NAME,genMulti,tmp->genderHeShe,pastH,tmp->gender,static_cast<int16_t>(tmp->charTainer.HEALTH));
        big_bundle_update();
        room_write(after_action);
        /// HERE STOP
    }
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
    mass_kill_report();
    if(!(isValidBaddie()))
    {
        spread_wealth();
    }
    fight_in_progress = false;
    QLock.unlock();


    // after report

    
    return true;
    // think about communicating a cleanup routine after death. How will we move them to spawn?
}

void Room::spread_wealth()
{
    std::string m = fmt::format("\n-----Coffer Distribution of {0} gold-----\n",coffer);
    std::string n;
    if(!(player_list.empty()))
    {
        int split = 0;
        for(auto p = player_list.begin(); p != player_list.end(); ++p)
        {
            if((*p)->isPlayerAlive())
                split++;
        }
        if(split > 0)
        {
            while(coffer % split != 0)
            {
                coffer++;
            }
            
            for(auto b = player_list.begin(); b != player_list.end(); ++b)
            {
                uint32_t cut = (coffer / split);
                (*b)->give_gold(cut);
                n = fmt::format("{0} obtains {1} gold\n",(*b)->charTainer.CHARACTER_NAME,cut);
                m += n;
            }
        coffer = 0;
        }else{
            n = fmt::format("Nobody survived to distribute.\n");
            m += n;
        }
        big_bundle_update();
        room_write(m);
    }
}

void Room::fight_controller(std::shared_ptr<Player> inst)
{// locked by unique room lock
    
    std::string m;
    bool next = false;
    uint32_t crit;
    uint32_t base;
    uint32_t roll;
    uint32_t negate;
    uint32_t defMulti;
    uint16_t kills = 0;
    uint16_t h = 0;
    std::string after_action = "\n========== FIGHT SUMMARY ==========\n";
    // std::string aa_ap;
    int bDex = -1;
    // after_action += "-PLAYER FIGHT DATA: ";
    if(isValidBaddie())
    {
        bDex = LiveBaddieDex();
    }else{
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("BAD DEX: Line {0} - {1}\n",__LINE__,__FILE__);}
    }
        
    if(bDex != -1)
    {
        // m = fmt::format("{0} grows tired of {1} looking at them with googly eyes and decides to start a fight!\n",
        // inst->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME); // potential UB.
        // room_write(m);
        after_action += fmt::format("Instigator: {0}\n",inst->charTainer.CHARACTER_NAME);
        after_action += fmt::format("{0} grows tired of {1} looking at them with googly eyes and decides to start a fight!\n\n",
        inst->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
    }else{
        std::lock_guard<std::mutex>lock(printLock);
        fmt::print("bDex crapped out?: {0}",bDex);
    }

    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if(isValidBaddie())
        {
            h = (*p)->charTainer.HEALTH;
            
            if(!(*p)->isPlayerAlive())
            {
                // m = fmt::format("{0}'s pulse grows weaker...\n",(*p)->charTainer.CHARACTER_NAME);
                after_action += fmt::format("Status (Deceased): {0}'s grows cold...\n",(*p)->charTainer.CHARACTER_NAME);
                continue;
            }
            if(next)
            {// I think it is safe to assume a baddie is alive. double check though. consider parent condition (line 393)
                next = false;
                bDex = LiveBaddieDex();
                {std::lock_guard<std::mutex>lock(printLock);fmt::print("bDex sanity check: {0}\n",bDex);}
                after_action += fmt::format("{0} sees their homie deliver the final blow on the last baddie, and locks eyes with {1}!\n\n"
                    ,(*p)->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
            }
            
            uint32_t multi = 100 * firepower;
            // crit = ((*p)->getCrit()) + multi;
            base = (*p)->charTainer.ATTACK;
            
            if(baddie_list.at(bDex)->is_alive())
            {// balance issues
                uint32_t halfDef = (baddie_list.at(bDex)->bTainer.DEFENSE / 2);
                crit = ((*p)->getCrit()) + (multi + halfDef);
                after_action += "-PLAYER FIGHT DATA: ";
                after_action += fmt::format("[ FIGHTER: {0} | HEALTH: {1} ] vs ",(*p)->charTainer.CHARACTER_NAME,h);
                h = baddie_list.at(bDex)->bTainer.HEALTH;
                after_action += fmt::format("[ TARGET: {0} | HEALTH: {1} ]\n\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,h);
                roll = (fast_rand() % ((crit + 1) - base) + base);
                if(roll >= (base * 2))
                {
                    // m = fmt::format("FRIENDLY CRITICAL DAMAGE INCOMING: {0} CRANKING MASSIVE {1} DAMAGE!\n",(*p)->charTainer.CHARACTER_NAME, roll); // to string?
                    after_action += fmt::format("FRIENDLY CRITICAL: {0} rolls {1} critical damage against {2}\n",(*p)->charTainer.CHARACTER_NAME,roll,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
                    
                }else{
                    after_action += fmt::format("FRIENDLY DAMAGE: {0} rolls {1} critical damage against {2}\n",(*p)->charTainer.CHARACTER_NAME,roll,baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
                }
                negate = (fast_rand() % (baddie_list.at(bDex)->bTainer.DEFENSE + 1));
                if(negate >= roll)
                {
                    roll = 0;
                    int16_t netNeg = abs(negate-roll);
                    after_action += fmt::format("ENEMY NEGATES(FULL): {0} manages to cheat death, negating all {1} points of that damage (net {2} damage).\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,netNeg,roll);
                }else{
                    roll -= negate;
                    after_action += fmt::format("ENEMY NEGATES(PARTIAL): {0} manages to negate {1} points of that damage (net {2} damage).\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,negate,roll);
                }

                if(!(baddie_list.at(bDex)->hurt_baddie(roll)))
                {// final blow
                    uint32_t loot = baddie_list.at(bDex)->loot_me();
                    collect_donations(loot);
                    kills++;
                    big_bundle_update();
                    // room_write(m);
                    next = true;
                    after_action += fmt::format("ENEMY WOUNDED(FATAL): {0} fatally wounds {1} with a final damage amount of {2}. {3} added to the coffer.\n",(*p)->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME,roll,loot);

                }else{
                    after_action += fmt::format("ENEMY WOUNDED: {0} delivers a striking blow, dealing {1} damage to {2}. {3} points of damage was negated!\n",
                    (*p)->charTainer.CHARACTER_NAME,roll,baddie_list.at(bDex)->bTainer.CHARACTER_NAME, negate);
                    big_bundle_update();
                    // room_write(m);
                    // after_action += fmt::format("{0} only wounds {1} with a final damage amount of {2}.\n",(*p)->charTainer.CHARACTER_NAME,baddie_list.at(bDex)->bTainer.CHARACTER_NAME,roll);
                }
            }else{
                {std::lock_guard<std::mutex>lock(printLock);fmt::print("Baddie dead?: {0}\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME);}
            }
        }
    }
            uint16_t bonus = difficulty - firepower;
            if((bonus > 0) && isValidBaddie())
            {
                bonus = (difficulty * (fast_rand() % (500) + 1));
                int bDex = LiveBaddieDex();
                if(!(baddie_list.at(bDex)->hurt_baddie(bonus)))
                {
                    collect_donations(1500);
                    after_action += fmt::format("ENEMY FATALITY(BONUS): {0} takes unnecessary bonus damage equal to {1}! Good Googly Moogly!!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,bonus);
                    kills++;
                }else{
                    after_action += fmt::format("ENEMY WOUNDED(BONUS): {0} takes unnecessary bonus damage equal to {1}! holy cow!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,bonus);
                }

                big_bundle_update();
                // room_write(m);
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
                // h = baddie_list.at(bDex)->bTainer.HEALTH;
                // after_action += fmt::format("[ FIGHTER: {0} | HEALTH: {1} ]\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,h);
                for(auto p = player_list.begin(); p != player_list.end(); ++p)
                {
                    if((*p)->isPlayerAlive())
                    {
                        after_action += "\n-BADDIE FIGHT DATA: ";
                        h = baddie_list.at(bDex)->bTainer.HEALTH;
                        after_action += fmt::format("[ FIGHTER: {0} | HEALTH: {1} ] vs ",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,h);
                        h = (*p)->charTainer.HEALTH;
                        after_action += fmt::format("[ TARGET: {0} | HEALTH: {1} ]\n\n",(*p)->charTainer.CHARACTER_NAME,h);
                        crit = baddie_list.at(bDex)->getCrit();
                        base = baddie_list.at(bDex)->bTainer.ATTACK;
                        defMulti = (*p)->charTainer.DEFENSE + (1000 * firepower);
                        roll = (fast_rand() % ((crit + 1) - base) + base);
                        if(roll >= (base * 2))
                        {
                            // m = fmt::format("ENEMY CRITICAL DAMAGE INCOMING: {0} CRANKING MASSIVE {1} DAMAGE!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME, std::to_string(roll));
                            after_action += fmt::format("ENEMY CRITICAL: {0} rolls {1} critical damage against {2}. {3} better pucker up!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,roll,(*p)->charTainer.CHARACTER_NAME,(*p)->genderHeShe);
                        // room_write(m);
                        }
                        // negate = (fast_rand() % ((*p)->charTainer.DEFENSE + 1));
                        negate = (fast_rand() % (defMulti + 1));
                        // after_action += fmt::format("and {0} manages to negate {1} points of that damage.\n",(*p)->charTainer.CHARACTER_NAME,negate);
                        if(negate >= roll)
                        {
                            roll = 0;
                            int16_t netNeg = abs(negate-roll);
                            after_action += fmt::format("FRIENDLY NEGATES(FULL): {0} manages to cheat death, negating all {1} points of that damage (net {2} damage).\n",(*p)->charTainer.CHARACTER_NAME,netNeg,roll);
                        }else{
                            roll -= negate;
                            after_action += fmt::format("FRIENDLY NEGATES(PARTIAL): {0} manages to negate {1} points of that damage (net {2} damage).\n",(*p)->charTainer.CHARACTER_NAME,negate,roll);
                        }
                        // {std::lock_guard<std::mutex>lock(printLock);fmt::print("{0} rolls: {1} damage ({2} negated)!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,roll,negate);}
                        if(!((*p)->hurt_player(roll)))
                        {
                            // m = fmt::format("ENEMY FATAL DAMAGE INCOMING:{0} delivers a fatal blow to {1}...\nslap, chop, smacking them back to the Portal Room! Yikes!\n({2} damage, {3} negated)\n"
                            //     ,baddie_list.at(bDex)->bTainer.CHARACTER_NAME, (*p)->charTainer.CHARACTER_NAME,roll,negate);
                            totalDeaths++;
                            after_action += fmt::format("FRIENDLY WOUNDED(FATAL): {0} fatally wounds {1} with a final damage amount of {2} - Slap, chop, smacking {3} back to the Portal Room with the quickness! Yikes\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,(*p)->charTainer.CHARACTER_NAME,roll,(*p)->gender);
                        }else{
                            after_action += fmt::format("FRIENDLY WOUNDED: {0} delivers a painful {1} points of damage to {2}.\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,roll,(*p)->charTainer.CHARACTER_NAME);
                            
                        }
                    }else{
                        after_action += fmt::format("FRIENDLY SKIPPED: {0} takes a whiff of {1}'s unconscious body but is allergic to the weakness! Achoo!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,(*p)->charTainer.CHARACTER_NAME);
                    }
                        big_bundle_update();
                        // room_write(m);
                }
            }else{
                after_action += fmt::format("========== All baddies have been cleared, and you live to see another day. ==========\n");
                // after_action += "\n... all baddies have been killed ...\n";
                // room_write(m);
            }
    //regenerate
    after_action += "\n-PLAYER REGEN DATA:\n";
    int16_t genMulti;
    //player
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if((*p)->isPlayerAlive())
        {

            genMulti = (fast_rand() % (((serverStats::PLAYER_BASE_HEALTH + 1) - ((*p)->charTainer.REGEN) / 2)) + ((*p)->charTainer.REGEN / 2));
            (*p)->heal_player(genMulti);
            big_bundle_update();
            
            // m = fmt::format("PLAYER HEALS:{0} recovers {1} health!\n",(*p)->charTainer.CHARACTER_NAME,genMulti);
            after_action += fmt::format("PLAYER HEALS: {0} recovers {1} health.\n",(*p)->charTainer.CHARACTER_NAME,genMulti);
        }else{
            after_action += fmt::format("PLAYER HEALS: {0} fails to heal because... well.. {1}'s dead.\n",(*p)->charTainer.CHARACTER_NAME,(*p)->genderHeShe);
        }
        // room_write(m);
    }
    after_action += "\n-BADDIE REGEN DATA:\n";
    if(baddie_list.at(bDex)->is_alive())
    {
        genMulti = (fast_rand() % (baddie_list.at(bDex)->bTainer.REGEN + 1) + 1);
        baddie_list.at(bDex)->heal_baddie(genMulti);
        after_action += fmt::format("{0} has recovered {1} health! and is angry as heck. YIKES!\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME,genMulti);
        big_bundle_update();
    }else{
        //does this ever hit?
        after_action += fmt::format("{0} lies there, looking ugly as sin. Whew... at least it didn't heal\n",baddie_list.at(bDex)->bTainer.CHARACTER_NAME);
    }
    
    // for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    // {
    //     if((*b)->is_alive())
    //     {
    //         if(firepower > difficulty)
    //         {
    //             genMulti = 500;
    //         }else{
    //             genMulti = 0;
    //         }
    //         // genMulti += (fast_rand() % ((*b)->bTainer.REGEN + 1) + 1);
    //         genMulti = (fast_rand() % (serverStats::BADDIE_MAX_HEALTH + 1) + 1);
    //         (*b)->heal_baddie(genMulti);
    //         big_bundle_update();
            
    //         after_action += fmt::format("{0} has recovered {1} health! and is angry as heck. YIKES!\n",(*b)->bTainer.CHARACTER_NAME,genMulti);
    //         // after_action += fmt::format("{0} recovers {1} health.\n",(*b)->bTainer.CHARACTER_NAME,genMulti);
    //     }else{
    //         after_action += fmt::format("{0} lies there, looking ugly as sin. Whew... at least it didn't heal\n",(*b)->bTainer.CHARACTER_NAME);            
    //     }
    //     // room_write(m);
    // }
    after_action += "==========\n";
    room_write(after_action);

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

void Room::collect_donations(uint16_t p)
{// lock outside
    if((coffer + p) <= serverStats::PLAYER_MAX_STAT)
    {
        coffer += p;
    }else{
        coffer = serverStats::PLAYER_MAX_STAT;
    }
}

void Room::mass_kill_report()
{// not thread safe.
    std::string m = fmt::format("\n\n========== KILL COUNT ==========\n");
    std::string n;
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        n = fmt::format("[ {0}: Current PVE Kills: {1} | High Score(PVE): {2} | PVP Kills: {3} ]\n",
            (*p)->charTainer.CHARACTER_NAME,(*p)->getCurrScore(),(*p)->getHighScore(),(*p)->getPVPKills());
        m += n;
    }
    int i = liveBaddieCount();
    std::string o;
    if(i)
    {
        o = fmt::format("{0} baddies remain.\n",i);
    }else{
        o = fmt::format("No baddies remain... but they will be back.\n");
    }
    n = fmt::format("========== CURRENT ROOM STATS ==========\nTotal Baddies Killed: {0}\nTotal Player Deaths: {1}\nCoffer:{2} gold\n{3}",totalKills,totalDeaths,coffer,o);
    m += n;
    room_write(m);
}

void Room::single_kill_report(std::shared_ptr<Player> b)
{// not thread safe.
    std::string m = fmt::format("\n\n========== KILL COUNT ==========\n");
    std::string n;
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        n = fmt::format("[ {0}: Current PVE Kills: {1} | High Score(PVE): {2} | PVP Kills: {3} ]\n",
            (*p)->charTainer.CHARACTER_NAME,(*p)->getCurrScore(),(*p)->getHighScore(),(*p)->getPVPKills());
        m += n;
    }
    int i = liveBaddieCount();
    std::string o;
    if(i)
    {
        o = fmt::format("{0} baddies remain.\n",i);
    }else{
        o = fmt::format("No baddies remain... but they will be back.\n");
    }
    n = fmt::format("========== CURRENT ROOM STATS ==========\nTotal Baddies Killed: {0}\nTotal Player Deaths: {1}\nCoffer:{2} gold\n{3}",
        totalKills,totalDeaths,coffer,o);
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
    if(liveDex.empty())
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("PROBLEM WITH LIVEDEX: Line {0} - {1}\n",__LINE__,__FILE__);}
        return -1;
    }else{
        int dex = (fast_rand() % liveDex.size());
        return liveDex.at(dex);
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
