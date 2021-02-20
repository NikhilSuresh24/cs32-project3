#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld *createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

/* Initialize stats and assets from @param assetPath */
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

/* returns diff in souls required for level and souls already saved */
int StudentWorld::soulsRequired() const
{
    return (2 * getLevel() + 5) - m_soulsSaved;
}

/* Initialize actors */
int StudentWorld::init()
{
    // create ghostracer
    if (m_gr == nullptr)
    {
        m_gr = new GhostRacer(this);
    }

    // create borderlines
    initBorders();

    return GWSTATUS_CONTINUE_GAME;
}

/* Update world for a tick */
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
                m_gr->onDeath();
                return GWSTATUS_PLAYER_DIED;
            }

            // move to next lvl if enough souls collected
            if (soulsRequired() == 0)
            {
                increaseScore(m_bonusPts);
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    // let the ghost racer move
    m_gr->doSomething();

    // remove dead actors
    for (auto it = m_objects.begin(); it != m_objects.end();)
    {
        if (!(*it)->isAlive())
        {
            (*it)->onDeath();
            delete *it;
            it = m_objects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // update pos of last white border
    updateLastBorderY();

    // add new actors
    addActors();

    // update status text
    setStats();

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
    for (auto it = m_objects.begin(); it != m_objects.end();)
    {
        delete *it;
        it = m_objects.erase(it);
    }

    // delete GR
    if (m_gr != nullptr)
    {
        delete m_gr;
        m_gr = nullptr;
    }

    resetVars();
}

/* Create land and road borders for road */
void StudentWorld::initBorders()
{
    // create N yellow borders on each side
    for (int i = 0; i < N_YELLOW_LINES; ++i)
    {
        double height = i * SPRITE_HEIGHT;
        addYellowBorders(height);
    }

    // create M white borders for each inner divider
    for (int i = 0; i < M_WHITE_LINES; ++i)
    {
        double height = i * (4 * SPRITE_HEIGHT);
        addWhiteBorders(height);
    }
}

/* Update Y position of last made white border */
void StudentWorld::updateLastBorderY()
{
    // simulate last border's movement 
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

/* Add set of borders if enough space */
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

/* Add a pair of yellow borders on edge of road at @param height */
void StudentWorld::addYellowBorders(double height)
{
    BorderLine *leftBorder = new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, height);
    BorderLine *rightBorder = new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, height);
    m_objects.push_back(leftBorder);
    m_objects.push_back(rightBorder);
}

/* Add a pair of whites borders at lane dividers at @param height */
void StudentWorld::addWhiteBorders(double height)
{
    BorderLine *midLeftBorder = new BorderLine(this, IID_WHITE_BORDER_LINE, MID_LEFT_X, height);
    BorderLine *midRightBorder = new BorderLine(this, IID_WHITE_BORDER_LINE, MID_RIGHT_X, height);
    m_objects.push_back(midLeftBorder);
    m_objects.push_back(midRightBorder);
    m_lastBorderY = height;
}

/* Set stat line */
void StudentWorld::setStats()
{
    ostringstream stats;
    stats << "Score: " << getScore();
    stats << setw(7) << "Lvl: " << getLevel();
    stats << setw(14) << "Souls2Save: " << soulsRequired();
    stats << setw(9) << "Lives: " << getLives();
    stats << setw(10) << "Health: " << m_gr->getHP();
    stats << setw(10) << "Sprays: " << m_gr->getSprayCount();
    stats << setw(9) << "Bonus: " << m_bonusPts;

    setGameStatText(stats.str());
}

/* Reset stat variables */
void StudentWorld::resetVars()
{
    m_bonusPts = START_BONUS_PTS;
    m_soulsSaved = START_SOULS_SAVED;
    m_lastBorderY = START_LAST_BORDER_Y;
}