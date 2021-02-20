#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

using namespace std;

/* 
 * Initalize Actor
 * @param ptr: a ptr to the StudentWorld the actor is in
 * @param canCollideGR: can actor collide with GhostRacer
 * @param isCAW: is Collision-avoidance worthy actor
 * @param startXSpeed: initial horizontal speed
 * @param startYSpeed: initial vertical speed
 * @param imageID: imageID of actor
 * @param startX: starting X coordinate
 * @param startY: starting Y coordinate
 * @param dir: starting direction
 * @param size: size of actor
 * @param depth: determines which actor gets screen priority
 * @param startHP: initial HP of actor (-1 if doesn't have HP)
 */
Actor::Actor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, bool isCAW, double startXSpeed, double startYSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth, double startHP = NO_HP)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_worldPtr(ptr), m_canCollideGR(canCollideGR), m_canColllideWater(canCollideWater), m_CAW(isCAW), m_horizSpeed(startXSpeed), m_isAlive(true), m_vertSpeed(startYSpeed), m_initHp(startHP), m_hp(startHP)
{
    m_hasHp = (m_initHp > 0) ? true : false; // actor only uses HP if its initial HP is above 0
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

/* Returns true if other overlaps with this */
bool Actor::isOverlapping(const Actor &other)
{
    // compare position difference with sizes of objects to determine overlap
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
    // deal damage if actor uses HP
    if (m_hasHp)
    {
        if (damage > 0)
        {
            m_hp -= damage;
        }

        // if out of HP, set to dead
        if (m_hp <= 0)
        {
            setIsAlive(false);
        }
    }
}

void Actor::healHP(int heal)
{
    // set HP to min of initial health and current health + heal
    if (m_hasHp && heal > 0)
    {
        int newHp = m_hp + heal;
        if (newHp > m_initHp)
        {
            m_hp = m_initHp;
        }
        else
        {
            m_hp = newHp;
        }
    }
}

GhostRacer::GhostRacer(StudentWorld *ptr)
    : Actor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, IS_CAW, START_X_SPEED, START_Y_SPEED, IID_GHOST_RACER, START_X, START_Y, START_DIR, SIZE, DEPTH, INIT_HP), m_sprayCount(INIT_WATER_COUNT) {}

GhostRacer::~GhostRacer() {}

void GhostRacer::incrementSprayCount()
{
    ++m_sprayCount;
}
void GhostRacer::decrementSprayCount()
{
    --m_sprayCount;
}

int GhostRacer::getSprayCount() const
{
    return m_sprayCount;
}

/* Ghost Racer's behavior when interacting with oil slick */
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
void GhostRacer::onDeath() const
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

// Ghost racer does not interact with itself or holy water
void GhostRacer::onCollideGR() {}
void GhostRacer::onCollideWater() {}

/* Ghost Racer's action each tick */
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

/* Apply user input to Ghost Racer movement */
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

/* Move Ghost Racer based on direction */
void GhostRacer::move()
{
    double deltaX = cos(DEG_2_RAD * getDirection()) * MAX_SHIFT_PER_TICK;
    moveTo(getX() + deltaX, getY());
}

StaticActor::StaticActor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, int imageID, double startX, double startY, int dir, double size, double startHP = NO_HP)
    : Actor(ptr, canCollideWater, canCollideWater, IS_CAW, START_X_SPEED, START_Y_SPEED, imageID, startX, startY, dir, size, DEPTH, startHP) {}

StaticActor::~StaticActor() {}

/* Static Actor's action each tick */
void StaticActor::doSomething()
{
    move();

    // set dead if offscreen
    if (isOffScreen())
    {
        setIsAlive(false);
        return;
    }

    // deal with GR collision
    if (getCanCollideGR() && isOverlapping(*(getWorld()->getGR())))
    {
        onCollideGR();
    }
}

/* Move Static Actor based on GR speed */ 
void StaticActor::move()
{
    // static actor's vert speed depends on ghost racer's vert speed
    double newVertSpeed = getVertSpeed() - getWorld()->getGR()->getVertSpeed();
    double newY = getY() + newVertSpeed;
    double newX = getX() + getHorizSpeed();
    moveTo(newX, newY);
}

BorderLine::BorderLine(StudentWorld *ptr, int imageID, double startX, double startY)
    : StaticActor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, imageID, startX, startY, START_DIR, SIZE, START_HP) {}
BorderLine::~BorderLine() {}

// Borderline does not collide with GR or water, has no death properties
void BorderLine::onCollideGR() {}
void BorderLine::onCollideWater() {}
void BorderLine::onDeath() const {}