#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld *createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), m_gr(nullptr)
{
}

int StudentWorld::init()
{
    if (m_gr == nullptr) {
        m_gr = new GhostRacer(this);
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    // decLives();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
}

GhostRacer *StudentWorld::getGR() const
{
    return m_gr;
}
