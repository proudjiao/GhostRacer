#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"



class StudentWorld;
//Base class for all actors: all actors have (x,y) coordinates,
class Actor: public GraphObject{
public:
    Actor(StudentWorld* world,
          int imageID,
          int startX,
          int startY,
          int startDirection,
          double size,
          int verticalSpeed,
          int horizontalSpeed,
          int hitPoint,
          int depth = 0,
          bool CollisionAvoidanceWorthy = true,
          bool isABorderLine = false,
          bool isHolyWaterSpray = false)
    :GraphObject(imageID, startX, startY, startDirection, size, depth),
    m_world(world),
    m_vSpeed(verticalSpeed),
    m_hp(hitPoint),
    m_collisionAvoidenceWorthy(CollisionAvoidanceWorthy),
    m_alive(true),
    m_movementPlan(0),
    m_isHolyWaterSpray(isHolyWaterSpray),
    m_activated(false)
    {}
    //member public function
    bool isAlive(){return m_alive;}
    StudentWorld* getWorld(){return m_world;} //every actor needs a reference to the world they are in
    int getHP(){return m_hp;}
    void setHP(int hp){m_hp = hp;}
    double getVSpeed(){return m_vSpeed;}
    double getHSpeed(){return m_hSpeed;}
    double getMovementPlan(){return m_movementPlan;}
    void pickNewMovementPlan();
    void setVSpeed(int s){m_vSpeed = s;}
    void setHSpeed(int s){m_hSpeed = s;}
    void setMovementPlan(double x){m_movementPlan = x;}
    void decrementMovementPlan(){m_movementPlan--;}
    bool isCollisionAvoidanceWorthy(){return m_collisionAvoidenceWorthy;}
    virtual void doSomething() = 0;
    virtual bool overlapsWith(Actor* other);
    StudentWorld* myWorld(){return m_world;}
    void setAliveTo(bool b){m_alive = b;}
    void damage(int hp){m_hp -= hp;}
    bool movedOffScreen();
    void move(bool adjustHSpeedWithPlayer);
    bool isHolyWaterSpray(){return m_isHolyWaterSpray;}
    void activate(){m_activated = true;}
    bool activated(){return m_activated;}
    //virtual void damaged() = 0;
    //virtual void die() = 0;
    //virtual void activate() = 0;
private:
    bool m_alive;
    //double m_speed;
    double m_vSpeed;
    double m_hSpeed;
    int m_hp;
    double m_movementPlan;
    bool m_collisionAvoidenceWorthy;
    //bool isABorderLine;//so that projectile does not affect it
    bool m_isHolyWaterSpray;
    bool m_activated;
    StudentWorld* m_world;
};


class GhostRacer: public Actor{
public:
    GhostRacer(StudentWorld* w)
    :Actor(w, IID_GHOST_RACER, 128, 32, up, 4.0, 0, 0, 100){
        m_holyWaterSpray = 10;
        m_nLostSoulsSaved = 0;
    }
    //bool levelComplete(){return false;} //needs modificaiton
    void doSomething();
    //void loseALife();
    void heal(int hp);
    void addHolyWater(int amount);
    int getNSavedSouls(){return m_nLostSoulsSaved;}
    void increaseNSavedSouls(){m_nLostSoulsSaved++;}
    int getNSprays(){return m_holyWaterSpray;}
    void dies(){damage(10000);}
    void spin();
private:
    int m_holyWaterSpray;
    int m_nLostSoulsSaved;
   // GameWorld* m_world;
    
};

class BorderLine: public Actor{
public:
    BorderLine(StudentWorld* w, int color, int xCoord, int yCoord)
    :Actor(w, color, xCoord, yCoord, right, 2.0, -4, 0, 10000, 2, false, true){ //can I just set my borderline hp to 10000?
    }
    //void move();
    void doSomething();
    
private:
};

class Pedestrian: public Actor{
public:
    Pedestrian(StudentWorld* w, int xCoord, int yCoord)
    :Actor(w, IID_HUMAN_PED, xCoord, yCoord, right, 2.0, -4, 0, 2)
    {}
    void doSomething();
};

class Zombie: public Actor{
public:
    Zombie(StudentWorld* w, int xCoord, int yCoord)
    :Actor(w, IID_ZOMBIE_PED, xCoord, yCoord, right, 3.0, -4, 0, 2),
    m_ticks(0)
    {}
    void doSomething();
private:
    int m_ticks;
};

class HolyWaterProjectile: public Actor{
public:
    HolyWaterProjectile(StudentWorld* w, int xCoord, int yCoord, int direction)
    :Actor(w, IID_HOLY_WATER_PROJECTILE, xCoord, yCoord, direction, 1.0, 0, 0, 10000, 1, false, false, true),
    m_distanceMoved(0)
    {}
    void doSomething();
private:
    double m_maximumTravellingDistance = 160;
    double m_distanceMoved;
};

class ZombieCab: public Actor{
public:
    ZombieCab(StudentWorld* w, int xCoord, int yCoord, int initCabSpeed)
    :Actor(w, IID_ZOMBIE_CAB, xCoord, yCoord, up, 4.0, initCabSpeed, 0, 3),
    m_planLength(0),
    m_hasDamagedPlayer(false)
    {}
    void doSomething();
private:
    double m_planLength;
    bool m_hasDamagedPlayer;
};

class Goodie: public Actor{
public:
    Goodie(StudentWorld* w, int image, int xCoord, int yCoord, int direction, double size)
    :Actor(w, image, xCoord, yCoord, direction, size, -4, 0, 10000, 2, false)
    {}
    virtual void doSomething();
};

class HealingGoodie: public Goodie{
public:
    HealingGoodie(StudentWorld* w, int xCoord, int yCoord)
    :Goodie(w, IID_HEAL_GOODIE,xCoord, yCoord, right, 1.0)
    {}
    virtual void doSomething();
};

class HolyWaterGoodie: public Goodie{
public:
    HolyWaterGoodie(StudentWorld* w, int xCoord, int yCoord)
    :Goodie(w, IID_HOLY_WATER_GOODIE, xCoord, yCoord, up, 2.0)
    {}
    virtual void doSomething();
};

class SoulGoodie: public Goodie{
public:
    SoulGoodie(StudentWorld* w, int xCoord, int yCoord)
    :Goodie(w, IID_SOUL_GOODIE, xCoord, yCoord, right, 4.0)
    {}
    virtual void doSomething();
};

class OilSlick: public Goodie{
public:
    OilSlick(StudentWorld* w, int xCoord, int yCoord)
    :Goodie(w, IID_OIL_SLICK, xCoord, yCoord, right, randInt(2,5))
    {}
    virtual void doSomething();
};





#endif // ACTOR_H_
