#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

int LEFT_LANE = 0;
int MIDDLE_LANE = 1;
int RIGHT_LANE = 2;
int NOT_IN_LANE = -1;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    
    m_souls2Save = 2 * getLevel() + 5;
    m_bonus = 5000;
    //add GhostRacer
    m_ghostRacer = new GhostRacer(this);
    
    //add N yellow Border Lines
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
//    for (int i = 0; i < N; i++) //This was my first attempt before I was recommended using vectors
//        m_yellowBorderLines[i] = new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT);
//    for (int i = 0; i < N; i++)
//        m_yellowBorderLines[N + i] = new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT);
    for (int i = 0; i < N; i++)
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
    for (int i = 0; i < N; i++)
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT));
   // m_nYellowBorderLines = 2 * N;
    
    //add M white Boderlines
    int M = VIEW_HEIGHT / (4*SPRITE_HEIGHT);
    for (int i = 0; i < M; i++)
      //  m_whiteBorderLines[i] = new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, i * 4 * SPRITE_HEIGHT);
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, i * 4 * SPRITE_HEIGHT));
    for (int i = 0; i < M; i++)
     //  m_whiteBorderLines[M + i] = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, i * 4 * SPRITE_HEIGHT);
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, i * 4* SPRITE_HEIGHT));
    //m_nWhiteBorderLines = 2 * M;
    //store a pointer to the last white borderline
    m_lastWhiteBorderLineY = m_actors.at(2*N + 2*M - 1)->getY();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{//move pseudocode
//
    if (m_bonus > 0)
        m_bonus--;
//     // The term "actors" refers to all actors, Ghost Racer, pedestrians,
//     // vehicles, goodies, oil slicks, holy water, spray, lost souls, etc.
//     // Give each actor a chance to do something, including Ghost Racer
    for (int i = 0; i < m_actors.size(); i++){
//    for each of the actors in the game world
//    {
        if (m_actors[i] != nullptr && m_actors[i]->isAlive()){
            
    //     if (the actor is still active/alive)
    //     {
            m_actors[i]->doSomething();
    //     // tell that actor to do something (e.g. move)
    //     the actor->doSomething();
            if (m_ghostRacer->getHP() <= 0){
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
        //    if (Ghost Racer was destroyed during this tick)
        //    return GWSTATUS_PLAYER_DIED;
            if (levelFinished()){
        //    if (Ghost Racer completed the currentLevel)
        //    {
                increaseScore(10000);//TODO: to be corrected
                return GWSTATUS_FINISHED_LEVEL;
            }
            
        }
    }
    
    //remove dead actors
    for (int i = 0; i < m_actors.size(); i++){
        if (!m_actors[i]->isAlive()){
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
            i--;
        }
    }
    
    //ghostRacer should also do something
    m_ghostRacer->doSomething();
    
    
    //add new borderlines
    m_lastWhiteBorderLineY -= 4 + m_ghostRacer->getVSpeed();
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - m_lastWhiteBorderLineY;
    if (delta_y >= SPRITE_HEIGHT){
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));
    }
    if (delta_y >= 4*SPRITE_HEIGHT){
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, new_border_y));
        m_lastWhiteBorderLineY = m_actors.back()->getY();
    }
    
    //add new human peds
    int xPos = randInt(0, VIEW_WIDTH);
    int yPos = VIEW_HEIGHT;
    
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    if (randInt(0, ChanceHumanPed) == 0)
        m_actors.push_back(new Pedestrian(this, xPos, yPos));
    
    //add new Zombies
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    if (randInt(0, ChanceZombiePed) == 0)
        m_actors.push_back(new Zombie(this, xPos, yPos));
    
    
    //add new zombie cabs
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int cur_lane = randInt(LEFT_LANE, RIGHT_LANE);
    int initCabSpeed = 4;
    bool laneSafe = false;
    double cabYPos = SPRITE_HEIGHT / 2;
    double cabXPos;
    for (int i = 0; i < 3; i++){
        Actor* bottom = nullptr;
        bool laneHasActors = hasActorsInLane(cur_lane, bottom);
        if (bottom != nullptr && (!laneHasActors || bottom->getY() > VIEW_HEIGHT / 3)){
            cabYPos = SPRITE_HEIGHT / 2;
            initCabSpeed = ghostRacer()->getVSpeed() + randInt(2, 4);
            laneSafe = true;
            break;
        }
        //TODO: implement
        /*Determine the closest “collision avoidance-worthy actor” to the TOP of
        the screen in the candidate lane
        d. If there is no such actor in the candidate lane, or there is such an actor and
        it has a Y coordinate that is less than (VIEW_HEIGHT * 2 / 3) then:
        i. cur_lane is the chosen lane for the new vehicle
        ii. The start Y coordinate for this new vehicle will be
        VIEW_HEIGHT - SPRITE_HEIGHT / 2
        iii. The initial vertical speed of the new vehicle will be the Ghost
        Racer’s vertical speed MINUS a random integer between 2 and 4
        inclusive.
        iv. Break out of the loop and proceed to step 3*/
        cur_lane++;
        if (cur_lane == 3)
            cur_lane -= 3;
    }
    if (cur_lane == LEFT_LANE)
        cabXPos = ROAD_CENTER - ROAD_WIDTH/3;
    else if (cur_lane == RIGHT_LANE)
        cabXPos = ROAD_CENTER + ROAD_WIDTH/3;
    else
        cabXPos = ROAD_CENTER;
            
    if (randInt(0, ChanceVehicle) == 0 && laneSafe)
        m_actors.push_back(new ZombieCab(this, cabXPos, cabYPos, initCabSpeed));
    
    //add new oil slicks
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, ChanceOilSlick) == 0)
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    
    //add new Holy Water Refill Goodies
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    if (randInt(0, ChanceOfHolyWater) == 0)
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    
    //add new Lost Soul Goodies
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul) == 0)
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    
    //update display text with stringstream
    ostringstream ossScore, ossLevel, ossSouls2Save, ossLives, ossHealth, ossSprays, ossBonus;
    ossScore << getScore();
    string score = ossScore.str();
    ossLevel << getLevel();
    string level = ossLevel.str();
    ossSouls2Save << (m_souls2Save - ghostRacer()->getNSavedSouls());
    string souls2Save = ossSouls2Save.str();
    ossLives << getLives();
    string lives = ossLives.str();
    ossHealth << m_ghostRacer->getHP();
    string health = ossHealth.str();
    ossSprays << m_ghostRacer->getNSprays();
    string sprays = ossSprays.str();
    ossBonus << m_bonus;
    string bonus = ossBonus.str();
    
    // Update the Game Status Line
    setGameStatText("Score: " + score +
                    "  Lvl: " + level +
                    "  Souls2Save: " + souls2Save +
                    "  Lives: " + lives +
                    "  Health: " + health +
                    "  Sprays: " + sprays +
                    "  Bonus: " + bonus
                    );

//    }
//     // Remove newly-dead actors after each tick
//    Remove and delete dead game objects
//     // Potentially add new actors to the game
//     // (e.g., oil slicks or goodies or border lines)
//    Add new actors
    
    
//     // the player hasn’t completed the current level and hasn’t died, so
//     // continue playing the current level
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    while (!m_actors.empty()){
        delete m_actors[m_actors.size() - 1];
        m_actors.pop_back();
    }

    if (m_ghostRacer != nullptr)
        delete m_ghostRacer;
}

bool StudentWorld::levelFinished() const{
    if (m_ghostRacer->getNSavedSouls() >= 2 * getLevel() + 5)
        return true;
    return false;
}


bool StudentWorld::touchesAHolyWaterSpray(Actor* myself, Actor*& closestWaterSpray){
    for (int i = 0; i < m_actors.size(); i++){
        if (m_actors.at(i)->isHolyWaterSpray() && m_actors.at(i)->overlapsWith(myself)){
            closestWaterSpray = m_actors.at(i);
            m_actors.at(i)->activate();
            return true;
        }
    }
    return false;
}
//    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
//    {
//        if (*it != myself && myself->overlapsWith(*it)){
//            closestWaterSpray = *it;
//            return true;
//        }
//
//    }


//This method is for Zombie car only
bool StudentWorld::detectsActorsInLane(Actor* myCab, Actor*& closest, bool detectFront){
    int myLane = inLane(myCab);
    for (int i = 0; i < m_actors.size(); i++){
        if (m_actors.at(i) != myCab && m_actors.at(i)->isCollisionAvoidanceWorthy() && myLane == inLane(m_actors.at(i)) && m_actors.at(i)->getY() > myCab->getY()){
            if (detectFront) {
                    if (m_actors.at(i)->getY() > myCab->getY()){
                        closest = m_actors.at(i);
                        return true;
                    }

            } else {
                    if (m_actors.at(i)->getY() > myCab->getY()){
                        closest = m_actors.at(i);
                        return true;
                    }
        
            }
        }
    }
    return false;
}

//return 0 if in left, 1 if in middle, 2 if in right, -1 if in none
int StudentWorld::inLane(Actor* myself){
    if (myself->getX() >= LEFT_EDGE && myself->getX() < LEFT_EDGE + ROAD_WIDTH/3)
        return LEFT_LANE;
    if (myself->getX() >= LEFT_EDGE + ROAD_WIDTH/3 && myself->getX() < RIGHT_EDGE - ROAD_WIDTH/3)
        return MIDDLE_LANE;
    if (myself->getX() >= RIGHT_EDGE - ROAD_WIDTH/3 && myself->getX() < RIGHT_EDGE)
        return RIGHT_LANE;
    return NOT_IN_LANE;
}

//this method Determine the closest “collision avoidance-worthy actor” to the BOTTOM of the screen in candidate lane


bool StudentWorld::hasActorsInLane(int lane, Actor*& closest){
    bool actorExists = false;
    for (int i = 0; i < m_actors.size(); i++){
        if (m_actors[i]->isCollisionAvoidanceWorthy() && inLane(m_actors[i]) == lane){
            if (!actorExists){
                closest = m_actors[i];
                actorExists = true;
            }
            if (closest != nullptr && m_actors[i]->getY() < closest->getY())
                closest = m_actors[i];
        }
    }
    return actorExists;
}
