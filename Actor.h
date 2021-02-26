#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>

class StudentWorld;

class Actor : public GraphObject
{
public:
    static constexpr double X_SCALE = 0.25;
    static constexpr double Y_SCALE = 0.6;
    static const int NO_HP = -1;

    Actor(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, bool isCAW, double startXSpeed, double startYSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~Actor();

    bool canCollideGR() const;
    bool canCollideWater() const;
    StudentWorld *getWorld() const;
    double getHorizSpeed() const;
    double getVertSpeed() const;
    bool isAlive() const;
    bool getCAW() const;
    bool isOffScreen() const;
    bool isOverlapping(const Actor &other);

    void setHorizSpeed(double speed);
    void setVertSpeed(double speed);
    void setIsAlive(bool isAlive);

    virtual void onCollideGR() = 0;
    virtual void onCollideWater() = 0;
    virtual void doSomething() = 0;
    virtual void move() = 0;
    virtual void onDeath() const = 0;

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
    static constexpr double START_Y_SPEED = 0;

    Agent(StudentWorld *ptr, bool canCollideGR, bool canCollideWater, double startXSpeed, int imageID, double startX, double startY, int dir, double size, double startHP);
    ~Agent();

    int getHP() const;
    void healHP(int heal);
    void takeDamage(int damage);

private:
    int m_initHp;
    int m_hp;
};

class GhostRacer : public Agent
{
public:
    static const bool CAN_COLLIDE_GR = false;
    static const bool CAN_COLLIDE_WATER = false;
    // static const bool IS_CAW = true;

    static const int INIT_HP = 100;
    static const int START_DIR = up; // degrees
    static const int REBOUND_RIGHT_DIR = 82;
    static const int REBOUND_LEFT_DIR = 98;
    static const int INIT_WATER_COUNT = 10;
    // static const unsigned int DEPTH = 0;

    static constexpr double SIZE = 4;
    static constexpr double START_X = 128;
    static constexpr double START_Y = 32;
    static constexpr double BORDER_DMG = 10;
    static constexpr double START_X_SPEED = 0;
    // static constexpr double START_Y_SPEED = 0;
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
    void incrementSprayCount();
    void decrementSprayCount();
    void onOil();
    void applyUserInput();

    virtual void onCollideGR();
    virtual void onCollideWater();
    virtual void doSomething();
    virtual void move();
    virtual void onDeath() const;

private:
    int m_sprayCount;
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
    virtual void move();
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
    virtual void onDeath() const;
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
    virtual void onDeath() const;
};
#endif // ACTOR_H_
