#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, unsigned int depth)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_worldPtr(ptr) {}

bool Actor::isAlive() const
{
    return !isOffScreen();
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
// bool CollidableActor::isOverlappingGR() const
// {
//     return isOverlapping(getWorld()->getGR());
// }

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
bool HPActor::isAlive() const
{
    return hp > 0 && Actor::isAlive();
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
    if (!isAlive())
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
            if (getDirection() < LEFT_ANGLE_LIMIT)
            {
                setDirection(getDirection() + TURN_ANGLE_INCREMENT);
            }
            break;

        case KEY_PRESS_RIGHT:
            if (getDirection() < RIGHT_ANGLE_LIMIT)
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
    double deltaX = cos(getDirection()) * MAX_SHIFT_PER_TICK;
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