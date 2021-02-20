#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld *createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

// TODO: constructor, move, cleanup
StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), m_gr(nullptr), m_bonusPts(START_BONUS_PTS), m_soulsSaved(0), m_lastBorderY(0)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

GhostRacer *StudentWorld::getGR() const
{
    return m_gr;
}

// returns diff in souls required for level and souls already saved
int StudentWorld::soulsRequired()
{
    return (2 * getLevel() + 5) - m_soulsSaved;
}

int StudentWorld::init()
{
    if (m_gr == nullptr)
    {
        m_gr = new GhostRacer(this);
    }

    initBorders();

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // let actors doSomething
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        // only doSomething if still alive
        if ((*it)->isAlive())
        {
            (*it)->doSomething();

            // quit if GR died
            if (!m_gr->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }

            if (soulsRequired() == 0)
            {
                increaseScore(m_bonusPts);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    // remove dead actors
    auto it = m_objects.begin();
    while (it != m_objects.end())
    {
        if (!(*it)->isAlive())
        {
            delete *it;
            it = m_objects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // update pos of last white border
    updateLastBordY();
    // TODO: add new actors
    addActors();
    //TODO: update status text

    // decrease bonus points each tick
    if (m_bonusPts > 0)
    {
        m_bonusPts--;
    }

    // player hasn't beaten lvl or died, so continue
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete all actors
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        delete *it;
    }

    // delete GR
    if (m_gr != nullptr)
    {
        delete m_gr;
    }
}

void StudentWorld::initBorders()
{
    for (int i = 0; i < N_YELLOW_LINES; ++i)
    {
        double height = i * SPRITE_HEIGHT;
        addYellowBorders(height);
    }

    for (int i = 0; i < M_WHITE_LINES; ++i)
    {
        double height = i * (4 * SPRITE_HEIGHT);
        addWhiteBorders(height);
    }
}

void StudentWorld::updateLastBordY()
{
    if (m_lastBorderY > 0)
    {
        double vertSpeed = StaticActor::START_Y_SPEED - m_gr->getVertSpeed();
        m_lastBorderY += vertSpeed;
    }
}

void StudentWorld::addActors()
{
    addBorders();
}

void StudentWorld::addBorders()
{
    double newBorderY = VIEW_HEIGHT - SPRITE_HEIGHT;
    double deltaY = newBorderY - m_lastBorderY;

    if (deltaY >= SPRITE_HEIGHT)
    {
        addYellowBorders(newBorderY);
    }

    if (deltaY >= (4 * SPRITE_HEIGHT))
    {
        addWhiteBorders(newBorderY);
    }
}

void StudentWorld::addYellowBorders(double height)
{
    BorderLine *leftBorder = new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, height);
    BorderLine *rightBorder = new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, height);
    m_objects.push_back(leftBorder);
    m_objects.push_back(rightBorder);
}

void StudentWorld::addWhiteBorders(double height)
{
    BorderLine *midLeftBorder = new BorderLine(this, IID_WHITE_BORDER_LINE, MID_LEFT_X, height);
    BorderLine *midRightBorder = new BorderLine(this, IID_WHITE_BORDER_LINE, MID_RIGHT_X, height);
    m_objects.push_back(midLeftBorder);
    m_objects.push_back(midRightBorder);
    m_lastBorderY = height;
}