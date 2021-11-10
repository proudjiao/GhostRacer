#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool levelFinished() const;
    void addActor(Actor* a){m_actors.push_back(a);}
    GhostRacer* ghostRacer(){return m_ghostRacer;};
    bool touchesAHolyWaterSpray(Actor* myself, Actor*& closestWaterSpray);
    bool detectsActorsInLane(Actor* myCab, Actor*& closest, bool detectFront);
    int inLane(Actor* myself);
    bool hasActorsInLane(int lane, Actor*& closet);
private:
    vector<Actor*> m_actors;
    double m_lastWhiteBorderLineY;
    
    
//    BorderLine* m_yellowBorderLines[VIEW_HEIGHT / SPRITE_HEIGHT * 2];
//    int m_nYellowBorderLines;
//    BorderLine* m_whiteBorderLines[VIEW_HEIGHT / (4 * SPRITE_HEIGHT) * 2];
//    int m_nWhiteBorderLines;
    //Actor* m_actors[100];
    //int m_nActors;
    GhostRacer* m_ghostRacer;
    int m_souls2Save;
    int m_bonus; 
};

#endif // STUDENTWORLD_H_
