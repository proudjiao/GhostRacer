#include "Actor.h"
#include "StudentWorld.h"
#include "SoundFX.h"
#include "math.h"

bool Actor::movedOffScreen(){
    if (getY() < 0 || getX() < 0 || getY() > VIEW_HEIGHT || getX() > VIEW_WIDTH){
        setAliveTo(false);
        return true;
    }
    else
        return false;
}

bool Actor::overlapsWith(Actor* other){
    double deltaX = abs(getX() - other->getX());
    double deltaY = abs(getY() - other->getY());
    double sumOfRadius = getRadius() + other->getRadius();
    if (deltaX < sumOfRadius * 0.25 && deltaY < sumOfRadius * 0.6)
        return true;
    else
        return false;
}

void Actor::move(bool adjustHSpeedWithPlayer){
    GhostRacer* player = myWorld()->ghostRacer();
    double vert_speed = getVSpeed() - player->getVSpeed();
    double horiz_speed = getHSpeed();
    if (adjustHSpeedWithPlayer)
       horiz_speed -= player->getHSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (movedOffScreen())
        return;
}

void Actor::pickNewMovementPlan(){
    do {
        setHSpeed(randInt(-3, 3));
    } while (getHSpeed() == 0);
    setMovementPlan(randInt(4, 32));
    if (getHSpeed() < 0)
        setDirection(left);
    else
        setDirection(right);
}


void GhostRacer::doSomething(){
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    
    //If the Ghost Racer is not currently alive (i.e., its hit points are zero or less), its doSomething() method must return immediately – none of the following steps should be performed.
    if (getHP() <= 0)
        return;
    
    //Check if Ghost Racer is facing on the left boundary
    if (getX() <= LEFT_EDGE){
        //if it's still facing the boundary, damage 10 hp per tick
        if (getDirection() > 90){
            damage(10);
        }
        setDirection(82);
        myWorld()->playSound(SOUND_VEHICLE_CRASH);
    } else
    
    //Check if Ghost Racer is facing on the right boundary
    if (getX() >= RIGHT_EDGE){
        //if it's still facing the boundary, damage 10 hp per tick
        if (getDirection() < 90){
            damage(10);
        }
        setDirection(98);
        myWorld()->playSound(SOUND_VEHICLE_CRASH);
    } else
        
        //Ghost Racer is not on the boundary, then check to see if player pressed a key
    {
        int ch;
        if (getWorld()->getKey(ch))
        {
            // user hit a key during this tick!
            switch (ch)
            {
                case KEY_PRESS_LEFT:
                    if (getDirection() < 114)
                        setDirection(getDirection() + 8);
                    break;
                    
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66)
                        setDirection(getDirection() - 8);
                    break;
                case KEY_PRESS_UP:
                    if (getVSpeed() < 5)
                        setVSpeed(getVSpeed() + 1);
                    break;
                case KEY_PRESS_DOWN:
                    if (getVSpeed() > -1)
                        setVSpeed(getVSpeed() - 1);
                    break;
                case KEY_PRESS_SPACE: //TODO: yet to implement water spray
                    if (m_holyWaterSpray > 0){
                        myWorld()->addActor(new HolyWaterProjectile(myWorld(), getX(), getY(), getDirection())); //TODO: Why we need this hint?
//                        The cos() and
//                        sin() functions can be used to determine the proper delta_x and
//                        delta_y in front of Ghost Racer where to place the new holy water
//                        projectile
                        myWorld()->playSound(SOUND_PLAYER_SPRAY);
                        m_holyWaterSpray--;
                    }
                    break;
                    // etc…
            }
        }
    }
    
    //move Ghost Racer Accordingly
    double max_shift_per_tick = 4.0;
    double direction = getDirection();
    double delta_x = cos(direction * M_PI/180) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::spin(){
    bool movePositive = (randInt(0, 1) == 0);
    if(movePositive)
        setDirection(getDirection() + (randInt(5, 20)));
    else
        setDirection(getDirection() - (randInt(5, 20)));
   //angle check
    if (getDirection() < 60)
        setDirection(60);
    if (getDirection() > 120)
        setDirection(120);
}

void GhostRacer::heal(int hp){
    setHP(getHP() + hp);
    if (getHP() > 100)
        setHP(100);
}

void GhostRacer::addHolyWater(int amount){
    m_holyWaterSpray += amount;
}

void BorderLine::doSomething(){
    int vert_speed = getVSpeed() - myWorld()->ghostRacer()->getVSpeed();
    //int horiz_speed = getHSpeed();
    double new_y = getY() + vert_speed;
    //double new_x = getX() + horiz_speed;
    moveTo(getX(), new_y);
    //if move off screen, mark it dead
    movedOffScreen();
    return;
}

void Pedestrian::doSomething(){
    if (!isAlive())
        return;
    
    GhostRacer* player = myWorld()->ghostRacer();
    if (overlapsWith(player)){
        player->dies();
        myWorld()->playSound(SOUND_PLAYER_DIE);
        return;
    }
    
    //Pedestrian move algorithm Pg34
    bool horizontalSpeedAffectedByPlayer = true;
    move(horizontalSpeedAffectedByPlayer);
    
    //Pedestrian movement plan algorithm Pg35
    decrementMovementPlan();
    if (getMovementPlan() > 0)
        return;
    else{ //else pick a new movement plan
        pickNewMovementPlan();
    }
    
    //nothing happens if human ped touches holy water
    Actor* temp = this;
    if (myWorld()->touchesAHolyWaterSpray(this, temp)){}
    
    return;
}

void Zombie::doSomething(){
    if (!isAlive())
        return;
    
    GhostRacer* player = myWorld()->ghostRacer();
    if (overlapsWith(player)){
        player->damage(5);
        setAliveTo(false);
        myWorld()->increaseScore(150);
        myWorld()->playSound(SOUND_PED_DIE);
        return;
    }
    
    Actor* temp = this;
    if (myWorld()->touchesAHolyWaterSpray(this, temp)){
        damage(1);
        if (getHP() <= 0){
            setAliveTo(false);
            myWorld()->increaseScore(150);
            myWorld()->playSound(SOUND_PED_DIE);
        }
        else
            myWorld()->playSound(SOUND_PED_HURT);
    }
    
    
    //chase ghostRacer
    bool closeToPlayer = abs(getX() - player->getX()) <= 30;
    bool inFrontOfPalyer = getY() >= player->getY();
    if (closeToPlayer && inFrontOfPalyer){
        setDirection(down);
        bool leftOfPlayer = getX() < player->getX();
        bool rightOfPlayer = getX() > player->getX();
        if (leftOfPlayer)
            setHSpeed(1);
        else if (rightOfPlayer)
            setHSpeed(-1);
        else
            setHSpeed(0);
        m_ticks--;
        if (m_ticks <= 0){
            myWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticks = 20;
        }
    }
    
    //move algorithm
    bool hSpeedAffectedByPlayer = false;
    move(hSpeedAffectedByPlayer);
    
    
    if (getMovementPlan() > 0){
        decrementMovementPlan();
        return;
    }
    else {
        pickNewMovementPlan();
    }

    
}

void HolyWaterProjectile::doSomething(){
    if (!isAlive())
        return;
    
    if (activated()){
        setAliveTo(false);
        return;
    }
    
    moveForward(SPRITE_HEIGHT);
    m_distanceMoved += SPRITE_HEIGHT;
    
    if (movedOffScreen())
        return;
    
    if (m_distanceMoved >= m_maximumTravellingDistance)
        setAliveTo(false);
    return;
}

void ZombieCab::doSomething(){
    if (!isAlive())
        return;
    
    GhostRacer* player = myWorld()->ghostRacer();
    
    if (overlapsWith(player) && !m_hasDamagedPlayer){
        myWorld()->playSound(SOUND_VEHICLE_CRASH);
        player->damage(20);
        bool leftOfPlayer = getX() <= player->getX();
        if (leftOfPlayer){
            setHSpeed(-5);
            setDirection(120 + randInt(0, 20));
        } else {
            setHSpeed(5);
            setDirection(60 - randInt(0, 20));
        }
        m_hasDamagedPlayer = true;
    }
    
    Actor* waterSpray = this;
    if (myWorld()->touchesAHolyWaterSpray(this, waterSpray)){
        damage(1);
        if (getHP() <= 0){
            setAliveTo(false);
            myWorld()->playSound(SOUND_VEHICLE_DIE);
            //TODO: There is a 1 in 5 chance that the zombie cab will add a new oil slick at its current position.
            myWorld()->increaseScore(200);
            return;
        } else
            myWorld()->playSound(SOUND_VEHICLE_HURT);
    }
    
    //move accordingly
    bool hSpeedAffectedByPlayer = false;
    move(hSpeedAffectedByPlayer);
    
    //vertical speed reset algorithm
    bool detectFront = true;
    Actor* other = this;
    if (getVSpeed() > player->getVSpeed() && myWorld()->detectsActorsInLane(this, other, detectFront))
        if(other->getY() > 96 + getY()){
            setVSpeed(getVSpeed() - 0.5);
            return;
        }
    
    if (getVSpeed() <= player->getVSpeed() && myWorld()->detectsActorsInLane(this, other, !detectFront))
        if(other != player && other->getY() < getY() - 96){
            setVSpeed(getVSpeed() + 0.5);
            return;
    }
    
    decrementMovementPlan();
    if (getMovementPlan() > 0)
        return;
    else{
        setMovementPlan(randInt(4, 32));
        setVSpeed(getVSpeed() + randInt(-2, 2));
    }
    return;
    
}




void Goodie::doSomething(){
    move(false);
}

void OilSlick::doSomething(){
    Goodie::doSomething();
    if (overlapsWith(myWorld()->ghostRacer())){
        myWorld()->playSound(SOUND_OIL_SLICK);
        myWorld()->ghostRacer()->spin();
    }
}

void HealingGoodie::doSomething(){
    Goodie::doSomething();
    if (overlapsWith(myWorld()->ghostRacer())){
        myWorld()->playSound(SOUND_GOT_GOODIE);
        myWorld()->ghostRacer()->heal(10);
        setAliveTo(false);
        myWorld()->increaseScore(250);
    }
}

void HolyWaterGoodie::doSomething(){
    Goodie::doSomething();
    if (overlapsWith(myWorld()->ghostRacer())){
        myWorld()->playSound(SOUND_GOT_GOODIE);
        myWorld()->ghostRacer()->addHolyWater(10);
        setAliveTo(false);
        myWorld()->increaseScore(50);
    }
}

void SoulGoodie::doSomething(){
    Goodie::doSomething();
    if (overlapsWith(myWorld()->ghostRacer())){
        myWorld()->playSound(SOUND_GOT_SOUL);
        myWorld()->ghostRacer()->increaseNSavedSouls();
        setAliveTo(false);
        myWorld()->increaseScore(100);
    }
    setDirection(getDirection() - 10);
}
