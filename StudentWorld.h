#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    // static vars
    static const int START_BONUS_PTS = 5000;
    static const int START_SOULS_SAVED = 0;
    static const int START_LAST_BORDER_Y = 0;
    static const int N_YELLOW_LINES = VIEW_HEIGHT / SPRITE_HEIGHT;
    static const int M_WHITE_LINES = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    static constexpr double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    static constexpr double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    static constexpr double MID_LEFT_X = LEFT_EDGE + ROAD_WIDTH / 3;
    static constexpr double MID_RIGHT_X = RIGHT_EDGE - ROAD_WIDTH / 3;

    // constructor, destructor, essential methods
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    GhostRacer *getGR() const;

private:
    GhostRacer *m_gr;
    std::vector<Actor *> m_objects;
    int m_soulsSaved;
    int m_bonusPts;
    double m_lastBorderY;

    // helper methods
    void addYellowBorders(double height);
    void addWhiteBorders(double height);
    void initBorders();
    void addBorders();
    void addActors();
    void updateLastBorderY();
    void setStats();
    void resetVars();
    int soulsRequired() const;

};

#endif // STUDENTWORLD_H_
