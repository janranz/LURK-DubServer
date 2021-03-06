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
        ,p->cTainer.CHARACTER_NAME,std::to_string(firepower));
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

    std::string m = fmt::format("{0} has left the room.\n",p->cTainer.CHARACTER_NAME);
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
        (*t)->write_character(p->cTainer,p->desc);
    }
    // }
}

bool Room::initiate_loot_sequence(std::shared_ptr<Player> p, unsigned char* target)
{
    std::unique_lock<std::shared_mutex>QLock(rLock,std::defer_lock);
    std::shared_lock<std::shared_mutex>SLock(rLock,std::defer_lock);
    SLock.lock();
    if(fight_in_progress)
    {
        return false;
    }
    SLock.unlock();

    bool found;
    QLock.lock();
    fight_in_progress = true;
    found = loot_controller(p,target);
    fight_in_progress = false;
    QLock.unlock();
    return found;
}

bool Room::loot_controller(std::shared_ptr<Player> t, unsigned char* target)
{// locked from the outside.
    bool success = false;
    bool player = false;
    std::shared_ptr<Player> pTmp;
    std::shared_ptr<Baddie> bTmp;
    
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        if(compare_to_lowers(M_ToCP((*p)->cTainer.CHARACTER_NAME),M_ToCP(target)))
        {
            pTmp = (*p);
            if(pTmp->getFD() != t->getFD())
            {
                success = true;
                player = true;
                break;
            }
        }
    }
    if(!success)
    {
        for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
        {
            bTmp = (*b);
            success = true;
            player = false;
            break;
        }
    }
    if(!success)
    {
        return success;
    }
    std::string m;
    uint16_t loot;
    int smack;
    int16_t damage = (fast_rand() % (10000 - 500) + 500);
    int fiddy = fast_rand() & 1;
    if(fiddy && player)
    {// succeeds player
        loot = pTmp->loot_me();
        m = fmt::format("\n\nAmongst the chaos, {0} glances over at {1}, and sees {2} coin pouch...\n",t->cTainer.CHARACTER_NAME,pTmp->cTainer.CHARACTER_NAME,pTmp->genderPos);
        if(loot > 0)
        {
            m += fmt::format("Failing to resist the temptation, {0} grabs the coin pouch, spilling {1} coins on the floor!\nSweeping it up, {2} attempts to make good his escape!\n",
            t->cTainer.CHARACTER_NAME,loot,t->genderHeShe);
            t->give_gold(loot);
        }else{
            m += fmt::format("After close examination, {0} quickly realizes {1} has like... {2} coins...\n Wasn't worth the beef anyways.\n",
            t->cTainer.CHARACTER_NAME,pTmp->cTainer.CHARACTER_NAME,loot);
        }
        smack = fast_rand() & 1;
        if(smack && pTmp->isPlayerAlive())
        {// smack succeeds
            m += fmt::format("But {0} doesn't appreciate {1} sizing {2} up like that!\n Going off like Coke and Mentos, {0} reaches for {3} trusty bat and delivers the most savage and ruthless blow to\nthe back of {4}'s goofy dome!\n",
            pTmp->cTainer.CHARACTER_NAME,t->cTainer.CHARACTER_NAME,pTmp->gender,pTmp->genderPos,t->cTainer.CHARACTER_NAME);
            if(!(t->hurt_player(damage)))
            {// player died.
                m += fmt::format("With {0} points of blind-sided damage, {1} folds like a laundromat! RIP!\n{2} claims {3} refund of {4} gold!\n",
                damage,t->cTainer.CHARACTER_NAME,pTmp->cTainer.CHARACTER_NAME,pTmp->genderPos,loot);
                t->take_gold(loot);
                pTmp->give_gold(loot);
                inform_death(t);
                collect_donations(t->loot_me());
            }else{
                m += fmt::format("With {0} points of blind-sided damage, {1} manages to hobble away with {2} coins from {3}'s pouch...\nWhat a rat!\n",
                damage,t->cTainer.CHARACTER_NAME,loot,pTmp->cTainer.CHARACTER_NAME);
            }
        }else{
            m += fmt::format("As blind as a mole, {0} feels nothing as {1} makes good with {2} loot!\n",
            pTmp->cTainer.CHARACTER_NAME,t->cTainer.CHARACTER_NAME,pTmp->genderPos);
        }
    }else if(fiddy && !player)
    {//succeeds baddie
        loot = bTmp->loot_me();
        m = fmt::format("\n\nLike an absolute psycho, {0} locks eyes onto a coin pouch stuck to the side of {1}! {2}'s not about to do what I think--\n",
        t->cTainer.CHARACTER_NAME,bTmp->bTainer.CHARACTER_NAME,t->genderHeShe);
        smack = fast_rand() & 1;
        if(loot > 0)
        {
            m += fmt::format("Failing to resist the temptation, {0} makes a dash for the coin pouch, managing to spill {1} coins on the floor!\nSweeping it up, {0} attempts to dip out!\n",
            t->cTainer.CHARACTER_NAME,loot);
            t->give_gold(loot);
        }else{
            m += fmt::format("After closer inspection, {0} consumes some water and re-thinks that decision.\n Would have gotten bopped for like {1} coins!\n",
            t->cTainer.CHARACTER_NAME,loot);
        }
        if(smack && bTmp->is_alive())
        {
            m += fmt::format("But {0} manages to put two-and-two together... In fact, it looks rather hungry!\nWith a haste, {0} delivers a painful blow of {1} damage to the back of {2}'s noggin!\n",
            bTmp->bTainer.CHARACTER_NAME,damage,t->cTainer.CHARACTER_NAME);
            if(!(t->hurt_player(damage)))
            {
                m += fmt::format("With {0} points of blind-sided damage, {1} folds like one of those fancy ironing boards (that are like... built into the walls).\nSince Baddies are into Crypto and not fiat, {2} puts the coins in the room's coffer.\n",
                damage,t->cTainer.CHARACTER_NAME,bTmp->bTainer.CHARACTER_NAME);
                collect_donations(loot);
                t->take_gold(loot);
                inform_death(t);
            }
        }else{
            m += fmt::format("I don't actually believe it! {0} manages to escape with {1}'s gold unscathed! Bet {2} won't try that again. (For violence-sake, let's hope {2} does!)\n",
            t->cTainer.CHARACTER_NAME,bTmp->bTainer.CHARACTER_NAME,t->genderHeShe);
        }

    }else if(!fiddy && player)
    {//fails player
        loot = t->loot_me();
        m += fmt::format("\n\nAbout as silent as a strategic bomber, {0} reaches for {1}'s coin pouch...\nonly to get elbow-dropped from the Stratosphere as {2} delivers a brutal {3} damage!\n",
        t->cTainer.CHARACTER_NAME,pTmp->cTainer.CHARACTER_NAME, pTmp->genderHeShe, damage);
        if(!pTmp->isPlayerAlive())
        {
            m += fmt::format("Which literally makes ZERO sense... {0} was just lying there a moment ago, I thought {1} was dead??\n",
            pTmp->cTainer.CHARACTER_NAME,pTmp->genderHeShe);
        }
        if(loot <= 0)
        {// My bad!
            m += fmt::format("\n\nSTOP WAIT!! Uhh, okay... Time freezes.. {0}'s life flashes before {1} eyes.. {2} sees that the server author failed to check if {2} had gold on line {3} of {4}!\n",
            t->cTainer.CHARACTER_NAME,t->genderPos,t->genderHeShe,__LINE__,__FILE__);
            loot = serverStats::CRUTCH_BUMP;
            m += fmt::format("{0} decides to do what any loyal portal enthusiast would do, and takes out a loan of {1} coins before climbing back into {2}'s arms so {3} can finish the beatdown!\nWow.\n\n",
            t->cTainer.CHARACTER_NAME,loot,pTmp->cTainer.CHARACTER_NAME, pTmp->genderHeShe);
        }
        if(!(t->hurt_player(damage)))
        {
            m += fmt::format("Unfortunately for {0}, the damage was too much for {1} to endure, smacking {1} back to the portal.\n Turning the tables like a hidden Uno card, {2} claims their bounty of {3} coins from {0}'s coin pouch\nThat's how mafia works!",
            t->cTainer.CHARACTER_NAME,t->gender,pTmp->cTainer.CHARACTER_NAME,loot);
            collect_donations(t->loot_me());
            pTmp->give_gold(loot);
            inform_death(t);
            
        }else{
            m += fmt::format("Feeling embarrassed, {0} winds up taking {1} damage in exchange for {2} of {3} coins...\nWhich goes straight into {4}'s pouch!\nBetter luck next time. THIEF.\n",
            t->cTainer.CHARACTER_NAME,damage,loot,t->genderPos,pTmp->cTainer.CHARACTER_NAME);
        }
        pTmp->give_gold(loot);

    }else if(!fiddy && !player)
    {//fails baddie
        m += fmt::format("\n\nJust as {0} reaches for {1}'s coin pouch, \n {2} trips over a broom and makes {1} very... VERY angry...\n",
        t->cTainer.CHARACTER_NAME,bTmp->bTainer.CHARACTER_NAME,t->genderHeShe);
        if(!bTmp->is_alive())
        {
            m += fmt::format("Of course, any logical person would be questioning how {0} mysteriously respawned and got angry...\nWell that's just the way it works in these savage streets!\n",
            bTmp->bTainer.CHARACTER_NAME);
        }
        loot = t->loot_me();
        if(!(t->hurt_player(damage)))
        {
            m += fmt::format("Just as one would expect to happen (messing with a big ol' ugly baddie), {0} succumbs to {1} injuries after {2} delivers a whopping {3}!\n",
            t->cTainer.CHARACTER_NAME,t->genderPos,bTmp->bTainer.CHARACTER_NAME,damage);
            if(loot)
            {
                m += fmt::format("{0} scoops up {1} coins from {2}'s banged up body and casually distributes it in the coffer.\n",
                bTmp->bTainer.CHARACTER_NAME,loot,t->cTainer.CHARACTER_NAME);
                collect_donations(loot);
            }else{
                m += fmt::format("{0} walks over to {1}'s banged up body to collect gold for the coffer, but is left with nothing.\n",
                bTmp->bTainer.CHARACTER_NAME,t->cTainer.CHARACTER_NAME);
            }
            inform_death(t);
        }else{
            if(loot)
            {
                m += fmt::format("After getting roughed up some, {0} manages to break free from {1} deadly grasp, dropping {2} coins in the process.\n",
                t->cTainer.CHARACTER_NAME,bTmp->bTainer.CHARACTER_NAME,loot);
                collect_donations(loot);
            }else{
                m += fmt::format("After getting roughed up some, {0} manages to break free from {1} deadly grasp! At least {2} has no coins to drop!\n",
                t->cTainer.CHARACTER_NAME,bTmp->bTainer.CHARACTER_NAME,t->genderHeShe);
            }

        }
    }else{
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG LOOT (hit unexpected else block): Line {0} - {1}\n",__LINE__,__FILE__);}
    }
    room_write(m);
    big_bundle_update();
    return success;
}

bool Room::initiate_fight_player(std::shared_ptr<Player> p, unsigned char* target)
{
    //if(compare_to_lowers(M_ToCP(t),M_ToCP((*p)->cTainer.CHARACTER_NAME)))
    std::unique_lock<std::shared_mutex>QLock(rLock,std::defer_lock);
    std::shared_lock<std::shared_mutex>SLock(rLock,std::defer_lock);


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
        if(compare_to_lowers(M_ToCP((*p)->cTainer.CHARACTER_NAME),M_ToCP(target)))
        {
            tmp = (*p);
            if(tmp->getFD() != t->getFD())
            {
                success = true;
                break;
            }
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
    after_action += fmt::format("\nInstigator: {0}\n",t->cTainer.CHARACTER_NAME);
    if(!(tmp->isPlayerAlive()))
    {
        after_action += fmt::format("{0} shoots a piercing-dagger look at {1} as {2} reaches for {3} bat."
        " But {0} has no interest in beating up a corpse\n",
        t->cTainer.CHARACTER_NAME,tmp->cTainer.CHARACTER_NAME,t->genderHeShe,t->genderPos);
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
        t->cTainer.CHARACTER_NAME,tmp->cTainer.CHARACTER_NAME,t->genderHeShe,t->genderPos);
    // room_write(m);
    after_action += fmt::format("\n-ATTACKER DATA: [Attacker: {0} | Health: {1}] vs [Target: {2} | Health: {3}]\n\n",
    t->cTainer.CHARACTER_NAME, static_cast<int16_t>(t->cTainer.HEALTH),tmp->cTainer.CHARACTER_NAME,static_cast<int16_t>(tmp->cTainer.HEALTH));
    crit = t->getCrit();
    base = t->cTainer.ATTACK;
    roll = ((fast_rand())%((crit + 1) - base) + base);
    if(roll >= (base * 2))
    {
        after_action += fmt::format("ATTACKER DAMAGE(CRITICAL): {0} generates {1} points of damage! Look out!\n",t->cTainer.CHARACTER_NAME,roll);
    }else if(roll < base){
        after_action += fmt::format("ATTACKER DAMAGE(WEAK): {0} only musters a pathetic {1} damage!\n",t->cTainer.CHARACTER_NAME,roll);
    }else{
        after_action += fmt::format("ATTACKER DAMAGE(NORMAL): {0} winds up an attack set to deliver {1} damage!\n",t->cTainer.CHARACTER_NAME,roll);
    }
    // room_write(m);
    def = tmp->cTainer.DEFENSE * 2;
    negate = (fast_rand() % (def));
    if(negate >= roll)
    {
        roll = 0;
        int16_t netNeg = abs(negate-roll);
        after_action += fmt::format("TARGET NEGATES(FULL): {0} manages to negate all {1} points, taking absolutely {0} damage!\n",tmp->cTainer.CHARACTER_NAME,netNeg,roll);
    }else if((roll - negate) <= 100){
        roll -= negate;
        after_action += fmt::format("TARGET NEGATES(PARTIAL): {0} takes some of the blows to {1} body,\nblocking {2} points and taking {3} damage!\n",
            tmp->cTainer.CHARACTER_NAME,tmp->genderPos,negate,roll);
    }else if(negate < 50){
        roll -= negate;
        after_action += fmt::format("TARGET NEGATES(MINIMAL): {0} just stands there dumbfounded as {1} wails on {2} like a rented mule!\nDamage Taken:{3}\tDamage Negated:{4}\n",
            tmp->cTainer.CHARACTER_NAME,t->cTainer.CHARACTER_NAME,tmp->gender,roll,negate);
    }else{
        after_action += fmt::format("TARGET NEGATES: {0} takes a boppin', soaking up {1} points of {2}'s {3} damage. Wow.\n",tmp->cTainer.CHARACTER_NAME,negate,t->cTainer.CHARACTER_NAME,roll);
    }
    // room_write(m);
    if(!(tmp->hurt_player(roll)))
    {
        after_action += fmt::format("TARGET WOUNDED(FATAL): {0} proved to be no match for {1}.. but will {0} come back and settle this beef?\n",
        tmp->cTainer.CHARACTER_NAME,t->cTainer.CHARACTER_NAME);
        t->tally_pvp();
        totalDeaths++;
        collect_donations(tmp->loot_me());
        dead = true;
        inform_death(t);
    }else{
        after_action += fmt::format("TARGET WOUNDED: {0} was able to survive the daunting blow, leaving {1} at {2} HP! MEDIC!\n",tmp->cTainer.CHARACTER_NAME,tmp->genderPos,static_cast<int16_t>(tmp->cTainer.HEALTH));
    }
    big_bundle_update();
    // room_write(m);
    
    if(dead)
    {
        // room_write(d);
        gold = tmp->drop_gold();
        after_action += fmt::format("{0} has fully restored any prior lost health. and loots {1} gold from {2}\n",t->cTainer.CHARACTER_NAME,gold,tmp->cTainer.CHARACTER_NAME);
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
        tmp->cTainer.CHARACTER_NAME, tmp->gender,t->cTainer.CHARACTER_NAME);

        after_action += fmt::format("\n-ATTACKER DATA: [Attacker: {0} | Health: {1}] vs [Target: {2} | Health: {3}]\n\n",
        tmp->cTainer.CHARACTER_NAME, static_cast<int16_t>(tmp->cTainer.HEALTH),t->cTainer.CHARACTER_NAME,static_cast<int16_t>(t->cTainer.HEALTH));

        ///HERE
        crit = tmp->getCrit();
        base = tmp->cTainer.ATTACK;
        roll = ((fast_rand())%((crit + 1) - base) + base);
        if(roll >= (base * 2))
        {
            after_action += fmt::format("ATTACKER DAMAGE(CRITICAL): {0} generates {1} points of damage! Look out!\n",tmp->cTainer.CHARACTER_NAME,roll);
        }else if(roll < base){
            after_action += fmt::format("ATTACKER DAMAGE(WEAK): {0} only musters a pathetic {1} damage!\n",tmp->cTainer.CHARACTER_NAME,roll);
        }else{
            after_action += fmt::format("ATTACKER DAMAGE(NORMAL): {0} winds up an attack set to deliver {1} damage!\n",tmp->cTainer.CHARACTER_NAME,roll);
        }
        // room_write(m);
        def = t->cTainer.DEFENSE * 2;
        negate = (fast_rand() % (def));
        if(negate >= roll)
        {
            roll = 0;
            int16_t netNeg = abs(negate-roll);
            after_action += fmt::format("TARGET NEGATES(FULL): {0} manages to negate all {1} points, taking absolutely {2} damage!\n",t->cTainer.CHARACTER_NAME,netNeg,roll);
        }else if((roll - negate) <= 100){
            roll -= negate;
            after_action += fmt::format("TARGET NEGATES(PARTIAL): {0} takes some of the blows to {1} body,\nblocking {2} points and taking {3} damage!\n",
                t->cTainer.CHARACTER_NAME,t->genderPos,negate,roll);
        }else if(negate < 50){
            roll -= negate;
            after_action += fmt::format("TARGET NEGATES(MINIMAL): {0} just stands there dumbfounded as {1} wails on {2} like a rented mule!\nDamage Taken:{3}\tDamage Negated:{4}\n",
                t->cTainer.CHARACTER_NAME,tmp->cTainer.CHARACTER_NAME,t->gender,roll,negate);
        }else{
            after_action += fmt::format("TARGET NEGATES: {0} takes a boppin', soaking up {1} points of {2}'s {3} damage. Wow.\n",t->cTainer.CHARACTER_NAME,negate,tmp->cTainer.CHARACTER_NAME,roll);
        }
        // room_write(m);
        if(!(t->hurt_player(roll)))
        {
            after_action += fmt::format("TARGET WOUNDED(FATAL): {0} proved to be no match for {1}.. but will {0} come back and settle this beef?\n",
            t->cTainer.CHARACTER_NAME,tmp->cTainer.CHARACTER_NAME);
            tmp->tally_pvp();
            dead = true;
            collect_donations(t->loot_me());
            inform_death(t);
        }else{
            after_action += fmt::format("TARGET WOUNDED: {0} was able to survive the daunting blow, leaving {1} at {2} HP! MEDIC!\n",t->cTainer.CHARACTER_NAME,t->gender,static_cast<int16_t>(t->cTainer.HEALTH));
        }
        big_bundle_update();
        // room_write(m);
        
        if(dead)
        {
            // room_write(d);
            gold = tmp->drop_gold();
            after_action += fmt::format("{0} has fully restored any prior lost health. and loots {1} gold from {2}\n",tmp->cTainer.CHARACTER_NAME,gold,t->cTainer.CHARACTER_NAME);
            tmp->give_gold(gold);
            tmp->full_restore_health();
            big_bundle_update();
            room_write(after_action);
            totalDeaths++;
            collect_donations(t->loot_me());
            return true;
        }
        after_action += "\n-PLAYER REGEN DATA:\n";
        // int16_t genMulti = (fast_rand() % (((serverStats::PLAYER_BASE_HEALTH + 1) - (t->cTainer.REGEN) / 2)) + (t->cTainer.REGEN / 2));
        int16_t genMulti = (fast_rand() % ((((t->cTainer.REGEN) + 1) - (t->cTainer.REGEN) / 2)) + (t->cTainer.REGEN / 2));
        int16_t pastH = t->cTainer.HEALTH;
        t->heal_player(genMulti);
        after_action += fmt::format("{0} dresses {1} wounds, recovering {2} points to {1} past {3} health, restoring {4} to {5} HP!\n",
        t->cTainer.CHARACTER_NAME,t->genderPos,genMulti,pastH,t->gender,static_cast<int16_t>(t->cTainer.HEALTH));
        
        // genMulti = (fast_rand() % (((serverStats::PLAYER_BASE_HEALTH + 1) - (tmp->cTainer.REGEN) / 2)) + (tmp->cTainer.REGEN / 2));
        genMulti = (fast_rand() % ((((t->cTainer.REGEN) + 1) - (tmp->cTainer.REGEN) / 2)) + (tmp->cTainer.REGEN / 2));
        pastH = tmp->cTainer.HEALTH;
        tmp->heal_player(genMulti);
        after_action += fmt::format("{0} squashes the beef, and recovers {1} points to {2} past {3} health, restoring {4}self to {5} HP!\n",
        tmp->cTainer.CHARACTER_NAME,genMulti,tmp->genderHeShe,pastH,tmp->gender,static_cast<int16_t>(tmp->cTainer.HEALTH));
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
    std::lock_guard<std::shared_mutex>lock(rLock);
    // std::unique_lock<std::shared_mutex>QLock(rLock,std::defer_lock);
    // std::shared_lock<std::shared_mutex>SLock(rLock,std::defer_lock);
    std::string m;
    // SLock.lock();
    // if(!(isValidBaddie()) || fight_in_progress) // Double check logic here
    if(fight_in_progress) // Double check logic here
    {
        return false;
    }
    // SLock.unlock();

    // QLock.lock();
    fight_in_progress = true;
    if(isValidBaddie())
    {
        fight_controller(p);
        mass_kill_report();
        //assume a fight has been fought and no baddies remain... distribute wealth:
        if(!(isValidBaddie()))
        {
            spread_wealth();
        }
    }else{
        fight_in_progress = false;
        return fight_in_progress;
    }
    fight_in_progress = false;
    // QLock.unlock();
    return !fight_in_progress;
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
        uint16_t split = 0;
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
                if((*b)->isPlayerAlive())
                {
                    uint32_t cut = (coffer / split);
                    (*b)->give_gold(cut);
                    n = fmt::format("{0} obtains {1} gold\n",(*b)->cTainer.CHARACTER_NAME,cut);
                    m += n;
                }else{
                    m += fmt::format("{0} gets no share of the cut, because {1}'s dead! (send an action to respawn)",(*b)->cTainer.CHARACTER_NAME,(*b)->genderHeShe);
                }
            }
        coffer = 0;
        }else{
            n = fmt::format("Nobody survived to distribute.\n");
            m += n;
        }
        room_write(m);
        big_bundle_update();
    }
}

void Room::fight_controller(std::shared_ptr<Player> inst)
{// locked by unique room lock. We assert through calling this function that at least one baddie is alive.
    
    std::string m; // = "\n========== FIGHT SUMMARY ==========\n";
    bool initial = true;
    bool next = false;
    bool roomCleared = false;
    bool playerDead = false;
    uint32_t attackerCrit;
    uint32_t FPmulti;
    uint32_t baseAttack;
    uint32_t halfDef;
    uint32_t roll;
    // uint32_t negate;
    // uint32_t defMulti;
    uint16_t defenderDef;
    // uint16_t kills = 0;
    int16_t h = 0;
    

    std::shared_ptr<Baddie> BADDIE(nullptr);
    BADDIE = retrieve_a_baddie();
    if(BADDIE == nullptr)
    {
        {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG fight_controller (Initial live baddie not found!): Line {0} - {1}\n",__LINE__,__FILE__);}
        return;
    }
    m += fmt::format("{0} grows tired of {1} looking at {2} with googly eyes and decides to start a fight!\n\n",
    inst->cTainer.CHARACTER_NAME,BADDIE->bTainer.CHARACTER_NAME, inst->gender);
    room_write(m);
    std::string after_action;
    for(auto p = player_list.begin(); p != player_list.end(); ++p)
    {
        after_action = fmt::format("Instigator: {0}\n",inst->cTainer.CHARACTER_NAME);;
        playerDead = false;
        if(next)
        {
            BADDIE = retrieve_a_baddie();
            if(BADDIE == nullptr)
            {// room cleared!
                roomCleared = true;
                after_action += fmt::format("{0} sees their homie deliver the final blow on the last baddie... The room looks clear!\n\n",(*p)->cTainer.CHARACTER_NAME);
                // break;
            }
        }

        if(!roomCleared)
        {
            h = (*p)->cTainer.HEALTH;
            if(h == 0)
            {// player dead
                after_action += fmt::format("Status (Deceased): {0} grows cold...\n",(*p)->cTainer.CHARACTER_NAME);
                playerDead = true;
            }else if(next){
                // {std::lock_guard<std::mutex>lock(printLock);fmt::print("Baddie sanity check: {0}\n",BADDIE->bTainer.CHARACTER_NAME);}
                after_action += fmt::format("{0} sees their homie deliver the final blow on the last baddie, and locks eyes with {1}!\n\n"
                    ,(*p)->cTainer.CHARACTER_NAME,BADDIE->bTainer.CHARACTER_NAME);
                    playerDead = false;
            }
            // else{
            //     {std::lock_guard<std::mutex>lock(printLock);fmt::print("DEBUG fight_controller (hit else block!): Line {0} - {1}\n",__LINE__,__FILE__);}
            // }
        }

        if(!roomCleared && !playerDead)
        {// fight sequence one player strikes one baddie
            FPmulti = 100 * firepower;
            halfDef = (BADDIE->bTainer.DEFENSE / 2);
            attackerCrit = ((*p)->getCrit() + (FPmulti + halfDef));
            baseAttack = (*p)->cTainer.ATTACK;

            after_action += fmt::format("-PLAYER FIGHT DATA: [PLAYER: {0} | HEALTH: {1}] vs [BADDIE: {2} | HEALTH: {3}]\n\n",
            (*p)->cTainer.CHARACTER_NAME,static_cast<int16_t>((*p)->cTainer.HEALTH),BADDIE->bTainer.CHARACTER_NAME,static_cast<int16_t>(BADDIE->bTainer.HEALTH));

            defenderDef = BADDIE->bTainer.DEFENSE;

            if((*p)->cTainer.ATTACK < BADDIE->bTainer.DEFENSE)
            {
                defenderDef = (BADDIE->bTainer.DEFENSE / difficulty);
            }
            roll = fight_roll(attackerCrit,baseAttack,defenderDef);

            if(roll > (*p)->cTainer.ATTACK)
            {
                after_action += fmt::format("{0} delivers CRITICAL damage of {1} straight to the dome of {2}! That's a lot of damage!\n",
                (*p)->cTainer.CHARACTER_NAME,roll,BADDIE->bTainer.CHARACTER_NAME);
            }else{
                after_action += fmt::format("{0} dishes out some gnarly {1} damage upon {2}! Look out, it might explode like a Goo-filled Pinata!\n",
                (*p)->cTainer.CHARACTER_NAME,roll,BADDIE->bTainer.CHARACTER_NAME);
            }

            if(!BADDIE->hurt_baddie(roll))
            {// baddie died
                if(isValidBaddie())
                {
                    after_action += fmt::format("{0} becomes staggered by {1}'s ruthless blow, and collapses into a pile of goo!\n.. but {0}'s baddie buddies don't look amused in the slightest...\n",
                    BADDIE->bTainer.CHARACTER_NAME,(*p)->cTainer.CHARACTER_NAME);
                }else{
                    after_action += fmt::format("{0} stumbles back and trips on the pile of other dead baddies... the room seems to look clear!\n",
                    BADDIE->bTainer.CHARACTER_NAME);
                    // roomCleared = true; // check and set at top of loop.
                }
                next = true;
                collect_donations(BADDIE->loot_me());
                // tally_PVE_kill(); // maybe unique kill
                (*p)->tally_PVE_kill();
                (*p)->full_restore_health();
                after_action += fmt::format("{0} gets a huuuuge morale boost after smacking that baddie into the lake of fire, and {1} manages to restore full health!\n",
                (*p)->cTainer.CHARACTER_NAME,(*p)->genderHeShe);
            }else{
                after_action += fmt::format("{0} staggers after getting hit, but manages to persevere! Looks like {1} and {2} friends should give that baddie another Hurt Hug!\n",
                BADDIE->bTainer.CHARACTER_NAME,(*p)->cTainer.CHARACTER_NAME,(*p)->genderPos);
            }

            // baddie heal attempt
            if(BADDIE->is_alive())
            {
                int heelflip = (fast_rand() & 1) || (fast_rand() & 1);
                if(heelflip)
                {// baddie heals
                    uint16_t roll = heal_roll(BADDIE->bTainer.REGEN,BADDIE->bTainer.HEALTH);
                    // roll = (roll > serverStats::BADDIE_MAX_HEALTH) ? serverStats::BADDIE_MAX_HEALTH : roll;
                    BADDIE->heal_baddie(roll);
                    after_action += fmt::format("{0} appears to mutate! Restoring {1} health! YIKES KILL IT!!\n",
                    BADDIE->bTainer.CHARACTER_NAME,roll);
                }else{
                    after_action += fmt::format("{0} Grunts loudly and reaches for a Band-Aid, but it's big ugly baddie fingers fails to open the wrapper! Whew.\n",
                    BADDIE->bTainer.CHARACTER_NAME);
                }
            }else{
                after_action += fmt::format("{0} lets out a dying whimper..\n",BADDIE->bTainer.CHARACTER_NAME);
            }
            

            // baddie attacks
            if(!next)
            {// baddie is still alive
                after_action += fmt::format("-BADDIE FIGHT DATA: [BADDIE: {0} | HEALTH: {1}] vs [PLAYER: {2} | HEALTH: {3}]\n\n",
                    BADDIE->bTainer.CHARACTER_NAME,static_cast<int16_t>(BADDIE->bTainer.HEALTH),(*p)->cTainer.CHARACTER_NAME,static_cast<int16_t>((*p)->cTainer.HEALTH));
                
                attackerCrit = BADDIE->getCrit();
                baseAttack = BADDIE->bTainer.ATTACK;
                defenderDef = (*p)->cTainer.DEFENSE;
                
                if(baseAttack > defenderDef)
                {
                    defenderDef += serverStats::CRUTCH_BUMP;
                }
                roll = fight_roll(attackerCrit,baseAttack,defenderDef);
                if(roll > BADDIE->bTainer.ATTACK)
                {
                    after_action += fmt::format("{0} goes absolutely NUCLEAR! Delivering CRITICAL damage equal to {1} right to the body of {2}. {3} stands there, wobbling like penguin!\n",
                    BADDIE->bTainer.CHARACTER_NAME,roll,(*p)->cTainer.CHARACTER_NAME,(*p)->genderHeShe);
                }else{
                    after_action += fmt::format("{0} does a normal attack that cracks into {1}'s body with {2} points of damage! Yahowee!\n",
                    BADDIE->bTainer.CHARACTER_NAME,(*p)->cTainer.CHARACTER_NAME,roll);
                }
                //hurt the player
                if(!((*p)->hurt_player(roll)))
                {// player dies
                    after_action += fmt::format("{0} fails to survive {1} injuries, and folds like a cheap suit at a high school prom!\n",
                    (*p)->cTainer.CHARACTER_NAME,(*p)->genderPos);
                    collect_donations((*p)->loot_me());
                    totalDeaths++;
                    playerDead = true;
                }else{
                    after_action += fmt::format("{0} ends up rocking {1} right out of {2} dang boots! But looks like {3} managed to face-tank the hit!\n",
                    BADDIE->bTainer.CHARACTER_NAME,(*p)->cTainer.CHARACTER_NAME,(*p)->genderPos,(*p)->genderHeShe);
                }
                // heal player
                if(!playerDead)
                {
                    roll = heal_roll((*p)->cTainer.REGEN,(*p)->cTainer.HEALTH);
                    (*p)->heal_player(roll);
                }
            }else{
             // baddie died.
                if(!playerDead)
                {
                    roll = heal_roll((*p)->cTainer.REGEN,(*p)->cTainer.HEALTH);
                    (*p)->heal_player(roll);
                    after_action += fmt::format("{0} takes a moment to get {1} Bering Strait and restores {2} points of health.\n",
                    (*p)->cTainer.CHARACTER_NAME,(*p)->genderPos,roll);
                }
            }
        }else if(!roomCleared && playerDead){
            after_action += fmt::format("{0} lies there like a rug! RIP!\n", (*p)->cTainer.CHARACTER_NAME);
        }
        room_write(after_action);
        big_bundle_update();
    }
}

int16_t Room::heal_roll(uint16_t regen, int16_t health)
{
    if((regen + health) > serverStats::PLAYER_BASE_HEALTH)
    {
        regen = (regen / 2);
    }
    int16_t roll;
    if(regen != 0)
         roll = (fast_rand() % regen);
    return roll;
}
uint32_t Room::fight_roll(uint32_t crit, uint32_t base,uint32_t def)
{
    if(crit - base < 1)
    {
        crit = 1;
        base = 1;
        // return 0;
    }
    if(!def)
    {
        def = 1;
    }
    uint32_t dmg = (fast_rand() % ((crit + 1) - base) + base );
    uint32_t negate = (fast_rand() % def);
    if(negate > dmg)
    {
        dmg = 0;
    }
    return dmg;
}

void Room::tally_PVE_kill()
{// locked outside
        for(auto p = player_list.begin(); p != player_list.end(); ++p)
        {
            if((*p)->isPlayerAlive())
            {
                (*p)->tally_PVE_kill();
            }
        }
    totalKills++;
}

void Room::inform_death(std::shared_ptr<Player> p)
{
    std::string m = fmt::format("\nHello. This is your friendly butler here. Looks like you got SMACKED up!\nFeel Free to just lie here while everyone else is doing their thing, or you can send any action to respawn!");
    p->write_msg(rmpm,m);
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
            (*p)->cTainer.CHARACTER_NAME,(*p)->getCurrScore(),(*p)->getHighScore(),(*p)->getPVPKills());
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
            (*p)->cTainer.CHARACTER_NAME,(*p)->getCurrScore(),(*p)->getHighScore(),(*p)->getPVPKills());
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

std::shared_ptr<Baddie> Room::retrieve_a_baddie()
{// always lock from the outside.
    std::shared_ptr<Baddie> bTmp(nullptr);
    int i = 0;
    for(auto b = baddie_list.begin(); b != baddie_list.end(); ++b)
    {
        if((*b)->bTainer.FLAGS == serverStats::BADDIE_AFLAGS)
        {
            bTmp = (*b);
            break;
        }
        i++;
    }
    // if(bTmp != nullptr)
        // {std::lock_guard<std::mutex>lock(printLock);fmt::print("bTmp sanity check: {0}\n",bTmp->bTainer.CHARACTER_NAME);}
    
    return bTmp;
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
            p->write_character((*t)->cTainer,(*t)->desc);
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
