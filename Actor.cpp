#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <random>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, unsigned int depth)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_worldPtr(ptr) {}

bool Actor::getIsAlive() const
{
    return m_isAlive;
    // return !isOffScreen();
}

void Actor::setIsAlive(bool isAlive)
{
    m_isAlive = isAlive;
}

bool Actor::checkIsAlive()
{
    if (isOffScreen())
    {
        setIsAlive(false);
    }

    return getIsAlive();
}
bool Actor::isCAW() const
{
    return false;
}

StudentWorld *Actor::getWorld() const
{
    return m_worldPtr;
}

bool Actor::isOffScreen() const
{
    return (getX() < 0 || getX() > VIEW_WIDTH) || (getY() < 0 || getY() > VIEW_HEIGHT);
}

void Actor::setHorizSpeed(double speed)
{
    m_horizSpeed = speed;
}
void Actor::setVertSpeed(double speed)
{
    m_vertSpeed = speed;
}
double Actor::getHorizSpeed()
{
    return m_horizSpeed;
}
double Actor::getVertSpeed()
{
    return m_vertSpeed;
}

Actor::~Actor() {}

CollidableActor::CollidableActor(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, unsigned int depth)
    : Actor(ptr, imageID, startX, startY, dir, size, depth) {}

bool CollidableActor::isOverlapping(const Actor &other) const
{
    double deltaX = (other.getX() - getX() > 0) ? other.getX() - getX() : getX() - other.getX();
    double deltaY = (other.getY() - getY() > 0) ? other.getY() - getY() : getY() - other.getY();
    double radiusSum = other.getRadius() + getRadius();

    return deltaX < radiusSum * X_SCALE && deltaY < radiusSum * Y_SCALE;
}
bool CollidableActor::isOverlappingGR() const
{
    return isOverlapping(*(getWorld()->getGR()));
}

CollidableActor::~CollidableActor() {}

HPActor::HPActor(StudentWorld *ptr, int initHP, int imageID, double startX, double startY, int dir, double size, unsigned int depth)
    : CollidableActor(ptr, imageID, startX, startY, dir, size, depth), hp(initHP), initHP(initHP) {}

HPActor::~HPActor() {}

void HPActor::takeDamage(int damage)
{
    if (damage > 0)
    {
        hp -= damage;
    }
}

void HPActor::gainHP(int health)
{
    if (health < 0)
    {
        return;
    }
    int newHealth = hp + health;
    if (newHealth > initHP)
    {
        hp = initHP;
    }
    else
    {
        hp = newHealth;
    }
}
bool HPActor::checkIsAlive()
{
    if (hp < 0 || isOffScreen())
    {
        setIsAlive(false);
    }
    return getIsAlive();
}

bool HPActor::takesProjectile() const
{
    return true;
}

GhostRacer::GhostRacer(StudentWorld *ptr)
    : HPActor(ptr, INIT_HP, IMAGE_ID, START_X, START_Y, START_DIR, SIZE, DEPTH)
{
    setVertSpeed(START_SPEED);
}

GhostRacer::~GhostRacer() {}

bool GhostRacer::isCAW() const
{
    return true;
}

void GhostRacer::doSomething()
{
    // exit if not alive
    if (!getIsAlive())
    {
        return;
    }
    // if hitting left boundary, deal damage and turn back
    if (getX() <= LEFT_BOUNDARY_X && getDirection() > up)
    {
        takeDamage(BORDER_DMG);
        setDirection(REBOUND_RIGHT_DIR);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    // if hitting right boundary, deal damage and turn back
    else if (getX() >= RIGHT_BOUNDARY_X && getDirection() < up)
    {
        takeDamage(BORDER_DMG);
        setDirection(REBOUND_RIGHT_DIR);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    // get user input on direction
    else
    {
        applyUserInput();
    }
    move();
}

void GhostRacer::applyUserInput()
{
    int ch;
    // if user presses key, change car action
    //TODO: holy water spraying
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
        case KEY_PRESS_LEFT:
            if (getDirection() < LEFT_ANGLE_TURN_LIMIT)
            {
                setDirection(getDirection() + TURN_ANGLE_INCREMENT);
            }
            break;

        case KEY_PRESS_RIGHT:
            if (getDirection() < RIGHT_ANGLE_TURN_LIMIT)
            {
                setDirection(getDirection() - TURN_ANGLE_INCREMENT);
            }
            break;
        case KEY_PRESS_UP:
            if (getVertSpeed() < MAX_SPEED)
            {
                setVertSpeed(getVertSpeed() + SPEED_INCREMENT);
            }
            break;

        case KEY_PRESS_DOWN:
            if (getVertSpeed() > MIN_SPEED)
            {
                setVertSpeed(getVertSpeed() - SPEED_INCREMENT);
            }
            break;
        }
    }
}

// move GR based on angle
void GhostRacer::move()
{
    double deltaX = cos(DEG_2_RAD * getDirection()) * MAX_SHIFT_PER_TICK;
    moveTo(getX() + deltaX, getY());
}

bool GhostRacer::takesProjectile() const
{
    return false;
}

// on death play death sound and set status to dead TODO: should status be a bool var
void GhostRacer::onDeath()
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

void GhostRacer::onOverlap() {}

void GhostRacer::addWater(int addWater)
{
    if (addWater > 0)
    {
        waterCount += addWater;
    }
}
void GhostRacer::decreaseWater(int lowerWater)
{
    if (lowerWater > 0)
    {
        waterCount -= lowerWater;
    }
}

void GhostRacer::onOil()
{
    // set bounds and determine if dir change is pos or neg
    int bound1 = 5;
    int bound2 = 20;
    bool isPosRange = (randInt(0, 1) == 0) ? true : false;

    // get dirChange
    int dirChange;
    dirChange = (isPosRange) ? randInt(bound1, bound2) : randInt(-bound2, -bound1);
    int newDir = getDirection() + dirChange;

    // set new direction
    if (newDir < LOWER_ANGLE_LIMIT)
    {
        setDirection(LOWER_ANGLE_LIMIT);
    }
    else if (newDir > UPPER_ANGLE_LIMIT)
    {
        setDirection(UPPER_ANGLE_LIMIT);
    }
    else
    {
        setDirection(newDir);
    }
}

BorderLine::BorderLine(StudentWorld *ptr, int imageID, double startX, double startY)
    : Actor(ptr, imageID, startX, startY, START_DIR, SIZE, DEPTH)
{
    setVertSpeed(START_VERT_SPEED);
    setHorizSpeed(START_HORIZ_SPEED);
}

BorderLine::~BorderLine() {}

bool BorderLine::takesProjectile() const
{
    return false;
}
void BorderLine::doSomething()
{
    move();
    // if checkIsAlive();
}
void BorderLine::move()
{
    double newVertSpeed = getVertSpeed() - getWorld()->getGR()->getVertSpeed();
    double newY = getY() + newVertSpeed;
    double newX = getX() + getHorizSpeed();
    moveTo(newX, newY);
}