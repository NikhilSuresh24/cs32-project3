#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <iostream>
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
Actor::Actor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, bool isCAW, double startXSpeed, double startYSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_worldPtr(ptr), m_canCollideGR(canCollideGR), m_canColllideWater(canCollideWater), m_CAW(isCAW), m_horizSpeed(startXSpeed), m_isAlive(true), m_vertSpeed(startYSpeed)
{
}
Actor::~Actor() {}

bool Actor::canCollideGR() const
{
    return m_canCollideGR;
}
bool Actor::canCollideWater() const
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

bool Actor::isOverlappingGR()
{
    return isOverlapping(*(getWorld()->getGR()));
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

void Actor::move()
{
    // actor's vert speed depends on ghost racer's vert speed
    double newVertSpeed = getVertSpeed() - getWorld()->getGR()->getVertSpeed();
    double newY = getY() + newVertSpeed;
    double newX = getX() + getHorizSpeed();
    moveTo(newX, newY);
}

Agent::Agent(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, double startYSpeed, int imageID, double startX, double startY, int dir, double size, double startHP)
    : Actor(ptr, canCollideGR, canCollideWater, IS_CAW, START_X_SPEED, startYSpeed, imageID, startX, startY, dir, size, DEPTH), m_hp(startHP), m_initHp(startHP) {}
Agent::~Agent() {}

int Agent::getHP() const
{
    return m_hp;
}
void Agent::healHP(int heal)
{
    // only heal if amt > 0
    if (heal > 0)
    {
        int newHp = m_hp + heal;
        // heal up to max of init hp
        m_hp = (newHp > m_initHp) ? m_initHp : newHp;
    }
}
void Agent::takeDamage(int damage)
{
    // deal damage
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

GhostRacer::GhostRacer(StudentWorld *ptr)
    : Agent(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, START_Y_SPEED, IID_GHOST_RACER, START_X, START_Y, START_DIR, SIZE, INIT_HP), m_sprayCount(INIT_WATER_COUNT) {}

GhostRacer::~GhostRacer() {}

void GhostRacer::addSprays(int numSprays)
{
    m_sprayCount += numSprays;
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

StaticActor::StaticActor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, int imageID, double startX, double startY, int dir, double size)
    : Actor(ptr, canCollideGR, canCollideWater, IS_CAW, START_X_SPEED, START_Y_SPEED, imageID, startX, startY, dir, size, DEPTH) {}

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
    if (canCollideGR() && isOverlappingGR())
    {
        onCollideGR();
    }
}

BorderLine::BorderLine(StudentWorld *ptr, int imageID, double startX, double startY)
    : StaticActor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, imageID, startX, startY, START_DIR, SIZE) {}
BorderLine::~BorderLine() {}

// Borderline does not collide with GR or water, has no death properties
void BorderLine::onCollideGR() {}
void BorderLine::onCollideWater() {}

OilSlick::OilSlick(StudentWorld *ptr, double startX, double startY)
    : StaticActor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, IID_OIL_SLICK, startX, startY, START_DIR, randInt(SIZE_LOWER_BOUND, SIZE_UPPER_BOUND)) {}
OilSlick::~OilSlick() {}

/* Oil Slick action on collision with Ghost Racer*/
void OilSlick::onCollideGR()
{
    getWorld()->playSound(SOUND_OIL_SLICK);
    getWorld()->getGR()->onOil();
}

// Oil slick does nothing on collision with water or on death
void OilSlick::onCollideWater() {}

Goodie::Goodie(StudentWorld *ptr, bool canCollideWater, int imageID, double startX, double startY, int dir, double size, int scoreIncrement, int onCollectSound)
    : StaticActor(ptr, CAN_COLLIDE_GR, canCollideWater, imageID, startX, startY, dir, size), m_scoreIncrement(scoreIncrement), m_collectSound(onCollectSound) {}
Goodie::~Goodie() {}

void Goodie::onCollideGR()
{
    incrementStat();
    setIsAlive(false);
    getWorld()->playSound(m_collectSound);
    getWorld()->increaseScore(m_scoreIncrement);
}

Soul::Soul(StudentWorld *ptr, double startX, double startY)
    : Goodie(ptr, CAN_COLLIDE_WATER, IID_SOUL_GOODIE, startX, startY, START_DIR, SIZE, SCORE_INCREMENT, SOUND_GOT_SOUL) {}
Soul::~Soul() {}

void Soul::incrementStat()
{
    getWorld()->soulSaved();
}

void Soul::onCollideWater() {}
void Soul::move()
{
    Actor::move();
    setDirection(getDirection() - ANG_SPEED); // rotate soul
}

DamageableGoodie::DamageableGoodie(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, int scoreIncrement)
    : Goodie(ptr, CAN_COLLIDE_WATER, imageID, startX, startY, dir, size, scoreIncrement, ON_COLLECT_SOUND) {}
DamageableGoodie::~DamageableGoodie() {}

void DamageableGoodie::onCollideWater()
{
    setIsAlive(false);
}

HealGoodie::HealGoodie(StudentWorld *ptr, double startX, double startY)
    : DamageableGoodie(ptr, IID_HEAL_GOODIE, startX, startY, START_DIR, SIZE, SCORE_INCREMENT) {}
HealGoodie::~HealGoodie() {}

void HealGoodie::incrementStat()
{
    getWorld()->getGR()->healHP(HEALTH_INCREMENT);
}

WaterGoodie::WaterGoodie(StudentWorld *ptr, double startX, double startY)
    : DamageableGoodie(ptr, IID_HOLY_WATER_GOODIE, startX, startY, START_DIR, SIZE, SCORE_INCREMENT) {}
WaterGoodie::~WaterGoodie() {}

void WaterGoodie::incrementStat()
{
    getWorld()->getGR()->addSprays(SPRAY_INCREMENT);
}

Pedestrian::Pedestrian(StudentWorld *ptr, int imageID, double startX, double startY, double size)
    : Agent(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, START_Y_SPEED, imageID, startX, startY, START_DIR, size, INIT_HP), m_movementPlan(INIT_MOVEMENT_PLAN) {}
Pedestrian::~Pedestrian() {}

void Pedestrian::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    if (isOverlappingGR())
    {
        onCollideGR();
        return;
    }

    aggroGR();
    move();
    if (isOffScreen())
    {
        return;
    }
    setMovementPlan();
}

void Pedestrian::setMovementPlan()
{
    if (m_movementPlan > 0)
    {
        --m_movementPlan;
        return;
    }

    double newHorizSpeed = (randInt(0, 1) == 0) ? randInt(-Y_SPEED_UPPER_BOUND, -Y_SPEED_LOWER_BOUND) : randInt(Y_SPEED_LOWER_BOUND, Y_SPEED_UPPER_BOUND);
    setHorizSpeed(newHorizSpeed);
    m_movementPlan = randInt(MOVEMENT_PLAN_LOWER_BOUND, MOVEMENT_PLAN_UPPER_BOUND);

    if (getHorizSpeed() < 0)
    {
        setDirection(LEFT_DIR);
    }
    else
    {
        setDirection(RIGHT_DIR);
    }
}

HumanPedestrian::HumanPedestrian(StudentWorld *ptr, double startX, double startY)
    : Pedestrian(ptr, IID_HUMAN_PED, startX, startY, SIZE) {}
HumanPedestrian::~HumanPedestrian() {}

void HumanPedestrian::aggroGR() {} // human doesn't aggro GR, so will be empty

// lets studentworld know human was hit
void HumanPedestrian::onCollideGR()
{
    getWorld()->humanHit();
}
void HumanPedestrian::onCollideWater()
{
    setHorizSpeed(-1 * getHorizSpeed()); // reverse direction
    int newDirection = (getDirection() == LEFT_DIR) ? RIGHT_DIR : LEFT_DIR;
    setDirection(newDirection);
    getWorld()->playSound(SOUND_PED_HURT);
}

ZombiePedestrian::ZombiePedestrian(StudentWorld *ptr, double startX, double startY)
    : Pedestrian(ptr, IID_ZOMBIE_PED, startX, startY, SIZE), m_gruntTicks(INIT_GRUNT_TICKS) {}
ZombiePedestrian::~ZombiePedestrian() {}

void ZombiePedestrian::aggroGR()
{
    GhostRacer *gr = getWorld()->getGR();
    double deltaX = getX() - gr->getX();
    if (abs(deltaX) < GR_DELTA_X && getY() > gr->getY())
    {
        setDirection(ATTACK_GR_DIR);
        if (deltaX < 0)
        {
            setHorizSpeed(ATTACK_GR_X_SPEED);
        }
        else if (deltaX > 0)
        {
            setHorizSpeed(-ATTACK_GR_X_SPEED);
        }
        else
        {
            setHorizSpeed(0);
        }
        decrementGruntTicks();
        if (m_gruntTicks <= 0)
        {
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            resetGruntTicks();
        }
    }
}
void ZombiePedestrian::onCollideGR()
{
    getWorld()->getGR()->takeDamage(DMG_TO_GR);
    takeDamage(DMG_FROM_GR);
    getWorld()->playSound(SOUND_PED_DIE);
    getWorld()->increaseScore(SCORE_INCREMENT);
}
void ZombiePedestrian::onCollideWater()
{
    takeDamage(1); //TODO: this will be a var in the water proj class
    if (getHP() <= 0)
    {
        // status set to dead by take damage method
        getWorld()->playSound(SOUND_PED_DIE);
        if (!isOverlappingGR() && randInt(1, 5) == 1)
        {
            HealGoodie *healGoodie = new HealGoodie(getWorld(), getX(), getY());
            getWorld()->addActor(healGoodie); //TODO: not sure if this works or not
            getWorld()->increaseScore(SCORE_INCREMENT);
        }
    }
    else
    {
        getWorld()->playSound(SOUND_PED_HURT);
    }
}

void ZombiePedestrian::decrementGruntTicks()
{
    --m_gruntTicks;
}
void ZombiePedestrian::resetGruntTicks()
{
    m_gruntTicks = RESET_GRUNT_TICKS;
}