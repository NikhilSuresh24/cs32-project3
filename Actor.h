#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    static constexpr double X_SCALE = 0.25;
    static constexpr double Y_SCALE = 0.6;

    Actor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, bool isCAW, double startXSpeed, double startYSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~Actor();

    bool canCollideGR() const;
    bool canCollideWater() const;
    StudentWorld *getWorld() const;
    double getHorizSpeed() const;
    double getVertSpeed() const;
    bool isAlive() const;
    bool isCAW() const;
    bool isOffScreen() const;
    bool isOverlapping(const Actor *other) const;
    bool isOverlappingGR() const;

    void setHorizSpeed(double speed);
    void setVertSpeed(double speed);
    void setIsAlive(bool isAlive);

    virtual void onCollideGR() = 0;
    virtual void onCollideWater() = 0;
    virtual void doSomething() = 0;
    virtual void move();

private:
    bool m_canCollideGR;
    bool m_canColllideWater;
    StudentWorld *m_worldPtr;
    double m_horizSpeed;
    double m_vertSpeed;
    bool m_isAlive;
    bool m_hasHp;
    int m_initHp;
    int m_hp;
    bool m_CAW;
};

class Agent : public Actor
{
public:
    static const bool IS_CAW = true;
    static const unsigned int DEPTH = 0;
    static constexpr double START_X_SPEED = 0;

    static const int INIT_MOVEMENT_PLAN = 0;
    static const int Y_SPEED_UPPER_BOUND = 3;
    static const int Y_SPEED_LOWER_BOUND = 1;
    static const int MOVEMENT_PLAN_LOWER_BOUND = 4;
    static const int MOVEMENT_PLAN_UPPER_BOUND = 32;
    static const int LEFT_DIR = 180;
    static const int RIGHT_DIR = 0;

    Agent(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, double startYSpeed, int imageID, double startX, double startY, int dir, double size, double startHP);
    virtual ~Agent();

    int getHP() const;
    void healHP(int heal);
    void takeDamage(int damage);
    virtual void newMovementPlan();
    int getMovementPlan() const;
    void setMovementPlan(int movementPlan);

private:
    int m_initHp;
    int m_hp;
    int m_movementPlan;
};

class GhostRacer : public Agent
{
public:
    static const bool CAN_COLLIDE_GR = false;
    static const bool CAN_COLLIDE_WATER = false;

    static const int INIT_HP = 100;
    static const int START_DIR = up; // degrees
    static const int REBOUND_RIGHT_DIR = 82;
    static const int REBOUND_LEFT_DIR = 98;
    static const int INIT_WATER_COUNT = 10;

    static constexpr double SIZE = 4;
    static constexpr double START_X = 128;
    static constexpr double START_Y = 32;
    static constexpr double BORDER_DMG = 10;
    static constexpr double START_Y_SPEED = 0;
    static constexpr double TURN_ANGLE_INCREMENT = 8;
    static constexpr double LEFT_ANGLE_TURN_LIMIT = 114;
    static constexpr double RIGHT_ANGLE_TURN_LIMIT = 66;
    static constexpr double LOWER_ANGLE_LIMIT = 60;
    static constexpr double UPPER_ANGLE_LIMIT = 120;
    static constexpr double MAX_VERT_SPEED = 5;
    static constexpr double MIN_VERT_SPEED = -1;
    static constexpr double SPEED_INCREMENT = 1;
    static constexpr double MAX_SHIFT_PER_TICK = 4.0;
    static constexpr double DEG_2_RAD = M_PI / 180;

    GhostRacer(StudentWorld *ptr);
    virtual ~GhostRacer();

    int getSprayCount() const;
    void addSprays(int numSprays);
    void onOil();
    void applyUserInput();

    virtual void onCollideGR();
    virtual void onCollideWater();
    virtual void doSomething();
    virtual void move();

private:
    int m_sprayCount;
    void makeSpray();
    void decrementSprayCount();
};

class StaticActor : public Actor
{
public:
    static const bool IS_CAW = false;
    static constexpr double START_X_SPEED = 0;
    static constexpr double START_Y_SPEED = -4;
    static const unsigned int DEPTH = 2;

    StaticActor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, int imageID, double startX, double startY, int dir, double size);
    virtual ~StaticActor();

    virtual void doSomething();
};

class BorderLine : public StaticActor
{
public:
    static const int START_DIR = 0;
    static const bool CAN_COLLIDE_GR = false;
    static const bool CAN_COLLIDE_WATER = false;
    static constexpr double SIZE = 2;

    BorderLine(StudentWorld *ptr, int imageID, double startX, double startY);
    virtual ~BorderLine();

    virtual void onCollideGR();
    virtual void onCollideWater();
};

class OilSlick : public StaticActor
{
public:
    static const int START_DIR = 0;
    static const bool CAN_COLLIDE_GR = true;
    static const bool CAN_COLLIDE_WATER = false;
    static const int SIZE_LOWER_BOUND = 2;
    static const int SIZE_UPPER_BOUND = 5;

    OilSlick(StudentWorld *ptr, double startX, double startY);
    virtual ~OilSlick();

    virtual void onCollideGR();
    virtual void onCollideWater();
};

class Goodie : public StaticActor
{
public:
    static const bool CAN_COLLIDE_GR = true;

    Goodie(StudentWorld *ptr, bool canCollideWater, int imageID, double startX, double startY, int dir, double size, int scoreIncrement, int onCollectSound);
    virtual ~Goodie();

    virtual void onCollideGR();
    virtual void incrementStat() = 0;

private:
    int m_scoreIncrement;
    int m_collectSound;
};

class Soul : public Goodie
{
public:
    static const bool CAN_COLLIDE_WATER = false;
    static constexpr double SIZE = 4.0;
    static const int START_DIR = 0;
    static const int SCORE_INCREMENT = 100;
    static const int ANG_SPEED = 10;

    Soul(StudentWorld *ptr, double startX, double startY);
    virtual ~Soul();

    virtual void incrementStat();
    virtual void onCollideWater();
    virtual void move(); // must redefine move to rotate soul
};

class DamageableGoodie : public Goodie
{
public:
    static const bool CAN_COLLIDE_WATER = true;
    static const int ON_COLLECT_SOUND = SOUND_GOT_GOODIE;

    DamageableGoodie(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, int scoreIncrement);
    virtual ~DamageableGoodie();

    virtual void onCollideWater();
};

class HealGoodie : public DamageableGoodie
{
public:
    static const int START_DIR = 0;
    static constexpr double SIZE = 1.0;
    static const int SCORE_INCREMENT = 250;
    static const int HEALTH_INCREMENT = 10;

    HealGoodie(StudentWorld *ptr, double startX, double startY);
    virtual ~HealGoodie();

    virtual void incrementStat();
};

class WaterGoodie : public DamageableGoodie
{
public:
    static const int START_DIR = 90;
    static constexpr double SIZE = 2.0;
    static const int SCORE_INCREMENT = 50;
    static const int SPRAY_INCREMENT = 10;

    WaterGoodie(StudentWorld *ptr, double startX, double startY);
    virtual ~WaterGoodie();

    virtual void incrementStat();
};

class Pedestrian : public Agent
{
public:
    static const int INIT_HP = 2;
    static constexpr double START_Y_SPEED = -4;
    static const int START_DIR = 0;
    static const bool CAN_COLLIDE_GR = true;
    static const bool CAN_COLLIDE_WATER = true;

    Pedestrian(StudentWorld *ptr, int imageID, double startX, double startY, double size);
    virtual ~Pedestrian();

    virtual void doSomething();
    virtual void aggroGR() = 0; // only used by Zombie Ped
};

class HumanPedestrian : public Pedestrian
{
public:
    static constexpr double SIZE = 2.0;

    HumanPedestrian(StudentWorld *ptr, double startX, double startY);
    virtual ~HumanPedestrian();

    virtual void aggroGR(); // human doesn't aggro GR, so will be empty
    virtual void onCollideGR();
    virtual void onCollideWater();
};

class ZombiePedestrian : public Pedestrian
{
public:
    static constexpr double SIZE = 3.0;
    static const int INIT_GRUNT_TICKS = 0;
    static const int RESET_GRUNT_TICKS = 20;
    static const int SCORE_INCREMENT = 150;
    static const int DMG_TO_GR = 5;
    static const int DMG_FROM_GR = 2;
    static constexpr double GR_DELTA_X = 30.0;
    static const int ATTACK_GR_DIR = 270;
    static constexpr double ATTACK_GR_X_SPEED = 1.0;

    ZombiePedestrian(StudentWorld *ptr, double startX, double startY);
    virtual ~ZombiePedestrian();

    virtual void aggroGR();
    virtual void onCollideGR();
    virtual void onCollideWater();

private:
    int m_gruntTicks;
    void decrementGruntTicks();
    void resetGruntTicks();
};

class HolyWater : public Actor
{
public:
    static const bool CAN_COLLIDE_GR = false;
    static const bool CAN_COLLIDE_WATER = false;
    static const bool IS_CAW = false;
    static constexpr double START_X_SPEED = 0.0; // unused by class
    static constexpr double START_Y_SPEED = 0.0; // unused by class
    static const unsigned int DEPTH = 1;
    static const int DAMAGE = 1;
    static constexpr double MAX_TRAVEL_DIST = 160.0;
    static constexpr double SIZE = 1.0;

    HolyWater(StudentWorld *ptr, int imageID, double startX, double startY, int dir);
    virtual ~HolyWater();

    virtual void onCollideGR();
    virtual void onCollideWater();
    virtual void doSomething();
    virtual void move();

private:
    double m_travel;

    void updateTravel();
};

class ZombieCab : public Agent
{
public:
    static const bool CAN_COLLIDE_GR = true;
    static const bool CAN_COLLIDE_WATER = true;
    static const int START_DIR = 90;
    static const bool DAMAGED_GR = false;
    static const int INIT_HP = 3;
    static constexpr double SIZE = 4.0;
    static const int SCORE_INCREMENT = 200;
    static const int DMG_TO_GR = 20;
    static constexpr double POST_COLLISION_X_SPEED = 5.0;
    static const int POST_COLLISION_RIGHT_DIR = 60;
    static const int POST_COLLISION_LEFT_DIR = 120;
    static const int RAND_DIRECTION_RANGE = 20;
    static constexpr double Y_SPEED_INCREMENT = 0.5;
    static constexpr double SAFE_DIST = 96.0; // # pixels diff between nearest CAW
    static constexpr int MOVEMENT_PLAN_SPEED_MODIFER = 2;

    ZombieCab(StudentWorld *ptr, double startYSpeed, double startX, double startY);
    ~ZombieCab();

    virtual void onCollideGR();
    virtual void onCollideWater();
    virtual void doSomething();
    virtual void newMovementPlan();
    int getLane() const;

private:
    bool m_hasDamagedGR;

    double getRandomDirectionShift() const;
    bool vertSpeedAdjustment();
    double getRandomSpeedModifier() const;
};
#endif // ACTOR_H_
