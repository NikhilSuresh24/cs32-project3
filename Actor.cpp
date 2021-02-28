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

// self explanatory methods not commented
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
bool Actor::isCAW() const
{
    return m_CAW;
}

/*Returns true if Actor is offscreen*/
bool Actor::isOffScreen() const
{
    return (getX() < 0 || getX() > VIEW_WIDTH) || (getY() < 0 || getY() > VIEW_HEIGHT);
}

/* Returns true if other overlaps with this */
bool Actor::isOverlapping(const Actor *other) const
{
    // compare position difference with sizes of objects to determine overlap
    double deltaX = abs(other->getX() - getX());
    double deltaY = abs(other->getY() - getY());
    double radiusSum = other->getRadius() + getRadius();

    return deltaX < radiusSum * X_SCALE && deltaY < radiusSum * Y_SCALE;
}

/* Returns true if actor overlaps with GR */
bool Actor::isOverlappingGR() const
{
    return isOverlapping(getWorld()->getGR());
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

/* Base movement algorithm that updates position based on speeds */
void Actor::move()
{
    // actor's vert speed depends on ghost racer's vert speed
    double newVertSpeed = getVertSpeed() - getWorld()->getGR()->getVertSpeed();
    double newY = getY() + newVertSpeed;
    double newX = getX() + getHorizSpeed();
    moveTo(newX, newY);
}

Agent::Agent(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, double startYSpeed, int imageID, double startX, double startY, int dir, double size, double startHP)
    : Actor(ptr, canCollideGR, canCollideWater, IS_CAW, START_X_SPEED, startYSpeed, imageID, startX, startY, dir, size, DEPTH), m_hp(startHP), m_initHp(startHP), m_movementPlan(INIT_MOVEMENT_PLAN) {}
Agent::~Agent() {}

int Agent::getHP() const
{
    return m_hp;
}

/* Heal up to init hp by @param heal */
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

/* Take damage and if out of HP, set agent to dead */
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

/* Base movement plan used by Pedestrians */
void Agent::newMovementPlan()
{
    --m_movementPlan;
    if (m_movementPlan > 0)
    {
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

int Agent::getMovementPlan() const
{
    return m_movementPlan;
}
void Agent::setMovementPlan(int movementPlan)
{
    m_movementPlan = movementPlan;
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
    if (getX() <= StudentWorld::ROAD_LEFT_EDGE && getDirection() > up)
    {
        takeDamage(BORDER_DMG);
        setDirection(REBOUND_RIGHT_DIR);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    // if hitting right boundary, deal damage and turn back
    else if (getX() >= StudentWorld::ROAD_RIGHT_EDGE && getDirection() < up)
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

/* Make Holy Water spray in front of Ghost Racer */
void GhostRacer::makeSpray()
{

    if (m_sprayCount >= 1)
    {
        // determine starting pos of spray
        double sprayX = getX() + SPRITE_HEIGHT * cos(DEG_2_RAD * getDirection());
        double sprayY = getY() + SPRITE_HEIGHT * sin(DEG_2_RAD * getDirection());
        HolyWater *projectile = new HolyWater(getWorld(), IID_HOLY_WATER_PROJECTILE, sprayX, sprayY, getDirection());
        getWorld()->addActor(projectile);
        getWorld()->playSound(SOUND_PLAYER_SPRAY);
        decrementSprayCount();
    }
}

/* Apply user input to Ghost Racer movement */
void GhostRacer::applyUserInput()
{
    int ch;
    // if user presses key, change car action
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
        case KEY_PRESS_SPACE:
            makeSpray();
            break;
        }
    }
}

/* Move Ghost Racer based on direction */
void GhostRacer::move()
{
    // determine change in X pos
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
    // play sound and make GR spin out
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
    // kill goodie, increment stats and scores
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
    : Agent(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, START_Y_SPEED, imageID, startX, startY, START_DIR, size, INIT_HP) {}
Pedestrian::~Pedestrian() {}

void Pedestrian::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    // deal with GR collision
    if (isOverlappingGR())
    {
        onCollideGR();
        return;
    }

    // deal with movement and aggroing GR
    aggroGR();
    move();

    // kill ped if offscreen
    if (isOffScreen())
    {
        setIsAlive(false);
        return;
    }

    newMovementPlan();
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
    // reverse pedestrian speed and dir when hit by water
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

    // aggro GR if in 30 pixel X pos range
    if (abs(deltaX) < GR_DELTA_X && getY() > gr->getY())
    {
        setDirection(ATTACK_GR_DIR);

        // determine speed based on sign of deltaX
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

        // determine whether zombie will grunt
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
    // take damage, give GR damage, increase score
    getWorld()->getGR()->takeDamage(DMG_TO_GR);
    takeDamage(DMG_FROM_GR); // take damage deals with setting alive status of zombie
    getWorld()->playSound(SOUND_PED_DIE);
    getWorld()->increaseScore(SCORE_INCREMENT);
}
void ZombiePedestrian::onCollideWater()
{
    takeDamage(HolyWater::DAMAGE);
    if (getHP() <= 0)
    {
        getWorld()->playSound(SOUND_PED_DIE);

        // if didn't die to GR, 1/5 chance spawn healgoodie
        if (!isOverlappingGR() && randInt(1, 5) == 1)
        {
            HealGoodie *healGoodie = new HealGoodie(getWorld(), getX(), getY());
            getWorld()->addActor(healGoodie);
        }
        getWorld()->increaseScore(SCORE_INCREMENT);
    }
    else
    {
        // not dead, so play hurt sounds
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

HolyWater::HolyWater(StudentWorld *ptr, int imageID, double startX, double startY, int dir)
    : Actor(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, IS_CAW, START_X_SPEED, START_Y_SPEED, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, SIZE, DEPTH), m_travel(0) {}
HolyWater::~HolyWater() {}

void HolyWater::onCollideGR() {}
void HolyWater::onCollideWater() {}

void HolyWater::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    // if projectile hits hittable actor, kill the projectile
    // studentworld takes care of dealing damage to other actor
    if (getWorld()->checkProjectileHit(this))
    {
        setIsAlive(false);
        return;
    }

    move();
    if (isOffScreen())
    {
        setIsAlive(false);
        return;
    }

    // kill projectile if traveled too much
    if (m_travel >= MAX_TRAVEL_DIST)
    {
        setIsAlive(false);
    }
}
void HolyWater::move()
{
    moveForward(SPRITE_HEIGHT);
    updateTravel();
}
void HolyWater::updateTravel()
{
    m_travel += SPRITE_HEIGHT;
}

ZombieCab::ZombieCab(StudentWorld *ptr, double startYSpeed, double startX, double startY)
    : Agent(ptr, CAN_COLLIDE_GR, CAN_COLLIDE_WATER, startYSpeed, IID_ZOMBIE_CAB, startX, startY, START_DIR, SIZE, INIT_HP), m_hasDamagedGR(DAMAGED_GR) {}
ZombieCab::~ZombieCab() {}

void ZombieCab::onCollideGR()
{
    // only collide if hasnt collided previously
    if (m_hasDamagedGR)
    {
        return;
    }
    // deal dmg to GR
    getWorld()->playSound(SOUND_VEHICLE_CRASH);
    getWorld()->getGR()->takeDamage(DMG_TO_GR);

    // calculate new speed and direction
    double deltaX = getX() - getWorld()->getGR()->getX();

    // if cab to right of GR, send cab to far right
    if (deltaX > 0)
    {
        setHorizSpeed(POST_COLLISION_X_SPEED);
        setDirection(POST_COLLISION_RIGHT_DIR - getRandomDirectionShift());
    }
    // cab to left or same X of GR, send cab to far left
    else
    {
        setHorizSpeed(-POST_COLLISION_X_SPEED);
        setDirection(POST_COLLISION_LEFT_DIR + getRandomDirectionShift());
    }

    // save this interaction so cab cannot interact with GR again
    m_hasDamagedGR = true;
}
void ZombieCab::onCollideWater()
{
    takeDamage(HolyWater::DAMAGE);

    if (getHP() < 0)
    {
        // set alive to false taken care of by takeDamage
        getWorld()->playSound(SOUND_VEHICLE_DIE);
        // 1/5 chance of adding oil slick
        if (randInt(1, 5) == 1)
        {
            OilSlick *oil = new OilSlick(getWorld(), getX(), getY());
            getWorld()->addActor(oil);
        }
        // increment score and return
        getWorld()->increaseScore(SCORE_INCREMENT);
        return;
    }
    else
    {
        // if not dead, play hurt sound
        getWorld()->playSound(SOUND_VEHICLE_HURT);
    }
}
void ZombieCab::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    // handle GR interaction
    if (isOverlappingGR())
    {
        onCollideGR();
    }

    move();
    if (isOffScreen())
    {
        setIsAlive(false);
        return;
    }

    // make vert speed change and return if change made
    if (vertSpeedAdjustment())
    {
        return;
    };

    // keep track of movement plan
    newMovementPlan();
}

double ZombieCab::getRandomDirectionShift() const
{
    return randInt(0, RAND_DIRECTION_RANGE - 1);
}

/* Adjust speed based on nearby CAW actors. Return true if speed change made, false otherwise */
bool ZombieCab::vertSpeedAdjustment()
{
    double deltaYSpeed = getVertSpeed() - getWorld()->getGR()->getVertSpeed();
    // don't do anything if cab not in a lane
    if (getLane() == -1)
    {
        return false;
    }
    // slow down if going faster than GR and closest CAW actor too close
    if (deltaYSpeed > 0 && getWorld()->directionalDistanceClosetCAWActor(this, true) < SAFE_DIST)
    {
        setVertSpeed(getVertSpeed() - Y_SPEED_INCREMENT);
        return true;
    }
    // speed up if going slower or same speed as GR and closest CAW actor too close
    else if (deltaYSpeed <= 0 && getWorld()->directionalDistanceClosetCAWActor(this, false) < SAFE_DIST)
    {
        setVertSpeed(getVertSpeed() + Y_SPEED_INCREMENT);
        return true;
    }

    // return false since no speed change made
    return false;
}

/* Determine which lane zombiecab is in */
int ZombieCab::getLane() const
{
    // left Lane
    if (getX() >= StudentWorld::ROAD_LEFT_EDGE && getX() < StudentWorld::LEFT_DIVIDER_X)
    {
        return 1;
    }
    // mid lane
    else if (getX() >= StudentWorld::LEFT_DIVIDER_X && getX() < StudentWorld::RIGHT_DIVIDER_X)
    {
        return 2;
    }

    // right lane
    else if (getX() >= StudentWorld::RIGHT_DIVIDER_X && getX() < StudentWorld::ROAD_RIGHT_EDGE)
    {
        return 3;
    }
    // not on the road
    else
    {
        return -1;
    }
}
/* Update cab's movement plan and create new one if necessary*/
void ZombieCab::newMovementPlan()
{
    setMovementPlan(getMovementPlan() - 1);
    // only create new movement plan if existing movement plan expired
    if (getMovementPlan() > 0)
    {
        return;
    }

    setMovementPlan(randInt(MOVEMENT_PLAN_LOWER_BOUND, MOVEMENT_PLAN_UPPER_BOUND));
    setVertSpeed(getVertSpeed() + getRandomSpeedModifier());
}

/* Random speed modifier on movement plan */
double ZombieCab::getRandomSpeedModifier() const
{
    return randInt(-MOVEMENT_PLAN_SPEED_MODIFER, MOVEMENT_PLAN_SPEED_MODIFER);
}
