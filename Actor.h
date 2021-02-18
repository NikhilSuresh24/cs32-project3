#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

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
    virtual bool isAlive() const;
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

};

class CollidableActor : public Actor
{
public:
    CollidableActor(StudentWorld *ptr, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~CollidableActor();
    bool isOverlapping(const Actor &other) const;
    bool isOverlappingGR() const;
    virtual void onOverlap() = 0;

private:
    const double X_SCALE = 0.25;
    const double Y_SCALE = 0.6;
};

class HPActor : public CollidableActor
{
public:
    HPActor(StudentWorld *ptr, int initHP, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual ~HPActor();
    void takeDamage(int damage);
    void gainHP(int health);

    virtual bool isAlive() const;
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
    static constexpr double LEFT_BOUNDARY_X = ROAD_CENTER - ROAD_WIDTH/2;
    static constexpr double RIGHT_BOUNDARY_X = ROAD_CENTER + ROAD_WIDTH/2;
    static constexpr double BORDER_DMG = 10;
    static const int REBOUND_RIGHT_DIR = 82;
    static constexpr double TURN_ANGLE_INCREMENT = 8;
    static constexpr double LEFT_ANGLE_LIMIT = 114;
    static constexpr double RIGHT_ANGLE_LIMIT = 66;
    static constexpr double MAX_SPEED = 5; // vertical speeds
    static constexpr double MIN_SPEED = -1;
    static constexpr double START_SPEED = 0;
    static constexpr double SPEED_INCREMENT = 1;
    static constexpr double MAX_SHIFT_PER_TICK = 4.0;



    GhostRacer(StudentWorld *ptr);
    virtual ~GhostRacer();
    virtual bool isCAW() const;
    virtual bool takesProjectile() const;
    virtual void doSomething();
    virtual void onOverlap();
    virtual void move(); 
    void applyUserInput();
    void onDeath();
};

#endif // ACTOR_H_
