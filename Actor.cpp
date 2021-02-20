#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <random>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, bool isCAW, double startXSpeed, double startYSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth, double startHP = -1)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_worldPtr(ptr), m_canCollideGR(canCollideGR), m_canColllideWater(canCollideWater), m_CAW(isCAW), m_horizSpeed(startXSpeed), m_isAlive(true), m_vertSpeed(startYSpeed), m_initHp(startHP), m_hp(startHP)
{
}
Actor::~Actor() {}

bool Actor::getCanCollideGR() const
{
    return m_canCollideGR;
}
bool Actor::getCanCollideWater() const
{
    return m_canColllideWater;
}
StudentWorld *Actor::getWorld() const
{
    return m_worldPtr;
}
double Actor::getHorizSpeed() const
{
    return m_horizSpeed;
}
double Actor::getVertSpeed() const
{
    return m_vertSpeed;
}
bool Actor::isAlive() const
{
    return m_isAlive;
}
bool Actor::getCAW() const
{
    return m_CAW;
}
bool Actor::isOffScreen() const
{
    return (getX() < 0 || getX() > VIEW_WIDTH) || (getY() < 0 || getY() > VIEW_HEIGHT);
}

bool Actor::isOverlapping(const Actor &other)
{
    double deltaX = (other.getX() - getX() > 0) ? other.getX() - getX() : getX() - other.getX();
    double deltaY = (other.getY() - getY() > 0) ? other.getY() - getY() : getY() - other.getY();
    double radiusSum = other.getRadius() + getRadius();

    return deltaX < radiusSum * X_SCALE && deltaY < radiusSum * Y_SCALE;
}

void Actor::setHorizSpeed(double speed)
{
    m_horizSpeed = speed;
}
void Actor::setVertSpeed(double speed)
{
    m_vertSpeed = speed;
}
void Actor::setIsAlive(bool isAlive)
{
    m_isAlive = isAlive;
}

int Actor::getHP() const
{
    return m_hp;
}
void Actor::takeDamage(int damage)
{
    if (damage > 0)
    {
        m_hp -= damage;
    }

    //TODO: this should only happen if the actor uses HP
    if (m_hp <= 0)
    {
        setIsAlive(false);
    }
}

void Actor::healHP(int heal)
{
    if (heal > 0)
    {
        m_hp += heal;
    }
}

GhostRacer::GhostRacer(StudentWorld *ptr)
    : Actor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, IS_CAW, START_X_SPEED, START_Y_SPEED, IMAGE_ID, START_X, START_Y, START_DIR, SIZE, DEPTH, INIT_HP), m_waterCount(INIT_WATER_COUNT) {}

GhostRacer::~GhostRacer() {}

void GhostRacer::incrementWaterCount()
{
    ++m_waterCount;
}
void GhostRacer::decrementWaterCount()
{
    --m_waterCount;
}

int GhostRacer::getWaterCount() const
{
    return m_waterCount;
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
void GhostRacer::onDeath()
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
    setIsAlive(false);
}

void GhostRacer::onCollideGR() {}
void GhostRacer::onCollideWater() {}
void GhostRacer::doSomething()
{
    // exit if not alive
    if (!isAlive())
    {
        return;
    }
    // if hitting left boundary, deal damage and turn back
    if (getX() <= StudentWorld::LEFT_EDGE && getDirection() > up)
    {
        takeDamage(BORDER_DMG);
        setDirection(REBOUND_RIGHT_DIR);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    // if hitting right boundary, deal damage and turn back
    else if (getX() >= StudentWorld::RIGHT_EDGE && getDirection() < up)
    {
        takeDamage(BORDER_DMG);
        setDirection(REBOUND_LEFT_DIR);
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
            if (getDirection() > RIGHT_ANGLE_TURN_LIMIT)
            {
                setDirection(getDirection() - TURN_ANGLE_INCREMENT);
            }
            break;
        case KEY_PRESS_UP:
            if (getVertSpeed() < MAX_VERT_SPEED)
            {
                setVertSpeed(getVertSpeed() + SPEED_INCREMENT);
            }
            break;

        case KEY_PRESS_DOWN:
            if (getVertSpeed() > MIN_VERT_SPEED)
            {
                setVertSpeed(getVertSpeed() - SPEED_INCREMENT);
            }
            break;
        }
    }
}
void GhostRacer::move()
{
    double deltaX = cos(DEG_2_RAD * getDirection()) * MAX_SHIFT_PER_TICK;
    moveTo(getX() + deltaX, getY());
}

StaticActor::StaticActor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, int imageID, double startX, double startY, int dir, double size, double startHP)
    : Actor(ptr, canCollideWater, canCollideWater, IS_CAW, START_X_SPEED, START_Y_SPEED, imageID, startX, startY, dir, size, DEPTH, startHP) {}

StaticActor::~StaticActor() {}

void StaticActor::doSomething()
{
    move();
    if (isOffScreen())
    {
        setIsAlive(false);
        return;
    }
    if (getCanCollideGR())
    {
        onCollideGR();
    }
}

void StaticActor::move()
{
    double newVertSpeed = getVertSpeed() - getWorld()->getGR()->getVertSpeed();
    double newY = getY() + newVertSpeed;
    double newX = getX() + getHorizSpeed();
    moveTo(newX, newY);
}

BorderLine::BorderLine(StudentWorld *ptr, int imageID, double startX, double startY)
    : StaticActor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, imageID, startX, startY, START_DIR, SIZE, START_HP) {}
BorderLine::~BorderLine() {}

void BorderLine::onCollideGR() {}
void BorderLine::onCollideWater() {}
void BorderLine::onDeath() {}