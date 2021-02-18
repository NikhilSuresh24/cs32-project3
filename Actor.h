#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
    Actor(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~Actor();
    virtual bool takesProjectile() const = 0;
    virtual void doSomething() = 0;
    virtual void move() = 0;
    virtual bool isCAW() const;
    bool getIsAlive() const;
    void setIsAlive(bool isAlive);
    virtual bool checkIsAlive();
    StudentWorld *getWorld() const;
    bool isOffScreen() const;
    void setHorizSpeed(double speed);
    void setVertSpeed(double speed);
    double getHorizSpeed();
    double getVertSpeed();

private:
    StudentWorld *m_worldPtr;
    double m_horizSpeed;
    double m_vertSpeed;
    bool m_isAlive;
};

class CollidableActor : public Actor
{
public:
    static constexpr double X_SCALE = 0.25;
    static constexpr double Y_SCALE = 0.6;

    CollidableActor(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~CollidableActor();
    bool isOverlapping(const Actor &other) const;
    bool isOverlappingGR() const;
    virtual void onOverlap() = 0;
};

class HPActor : public CollidableActor
{
public:
    HPActor(StudentWorld *ptr, int initHP, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~HPActor();
    void takeDamage(int damage);
    void gainHP(int health);

    virtual bool checkIsAlive();
    virtual bool takesProjectile() const;

private:
    int hp;
    int initHP;
};

class GhostRacer : public HPActor
{
public:
    static const int INIT_HP = 100;
    static const int IMAGE_ID = IID_GHOST_RACER;
    static constexpr double START_X = 128;
    static constexpr double START_Y = 32;
    static const int START_DIR = up; // degrees
    static constexpr double SIZE = 4;
    static const unsigned int DEPTH = 0;
    static constexpr double LEFT_BOUNDARY_X = ROAD_CENTER - ROAD_WIDTH / 2;
    static constexpr double RIGHT_BOUNDARY_X = ROAD_CENTER + ROAD_WIDTH / 2;
    static constexpr double BORDER_DMG = 10;
    static const int REBOUND_RIGHT_DIR = 82;
    static constexpr double TURN_ANGLE_INCREMENT = 8;
    static constexpr double LEFT_ANGLE_TURN_LIMIT = 114;
    static constexpr double RIGHT_ANGLE_TURN_LIMIT = 66;
    static constexpr double LOWER_ANGLE_LIMIT = 60;
    static constexpr double UPPER_ANGLE_LIMIT = 120;
    static constexpr double MAX_SPEED = 5; // vertical speeds
    static constexpr double MIN_SPEED = -1;
    static constexpr double START_SPEED = 0;
    static constexpr double SPEED_INCREMENT = 1;
    static constexpr double MAX_SHIFT_PER_TICK = 4.0;
    static constexpr double DEG_2_RAD = 180 / M_PI;
    static const int INIT_WATER_COUNT = 10;

    GhostRacer(StudentWorld *ptr);
    virtual ~GhostRacer();
    virtual bool isCAW() const;
    virtual bool takesProjectile() const;
    virtual void doSomething();
    virtual void onOverlap();
    virtual void move();
    void applyUserInput();
    void onDeath();
    void addWater(int addWater);
    void decreaseWater(int lowerWater);
    void onOil();

private:
    int waterCount;
};

class BorderLine : public Actor
{
public:
    static const int START_DIR = 0;
    static const int DEPTH = 2;
    static const unsigned int SIZE = 2;
    static constexpr double START_VERT_SPEED = -4; // vertical speed
    static constexpr double START_HORIZ_SPEED = 0; // vertical speed

    BorderLine(StudentWorld *ptr, int imageID, double startX, double startY);
    virtual ~BorderLine();
    virtual bool takesProjectile() const;
    virtual void doSomething();
    virtual void move();
};
#endif // ACTOR_H_
