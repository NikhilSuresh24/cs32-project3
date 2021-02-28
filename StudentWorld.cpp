#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <cmath>
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

/* Cleanup StudentWorld */
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

            // check for hitting pedestrian
            if (m_isHumanHit)
            {
                decLives();
                resetHumanHit();
                return GWSTATUS_PLAYER_DIED;
            }

            // quit if GR died
            if (!m_gr->isAlive())
            {
                decLives();
                playSound(SOUND_PLAYER_DIE);
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

/* Add all sorts of actors every tick */
void StudentWorld::addActors()
{
    addBorders();
    addOilSlick();
    addSoul();
    addWaterGoodie();
    addHuman();
    addZombiePed();
    addZombieCab();
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
    BorderLine *leftBorder = new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_LEFT_EDGE, height);
    BorderLine *rightBorder = new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_RIGHT_EDGE, height);
    m_objects.push_back(leftBorder);
    m_objects.push_back(rightBorder);
}

/* Add a pair of whites borders at lane dividers at @param height */
void StudentWorld::addWhiteBorders(double height)
{
    BorderLine *midLeftBorder = new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_DIVIDER_X, height);
    BorderLine *midRightBorder = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_DIVIDER_X, height);
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

/* Add oil slick to top of screen based on level */
void StudentWorld::addOilSlick()
{
    // add oil slick based on chance at random X pos on road
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    if (shouldCreateActor(chanceOilSlick))
    {
        OilSlick *oil = new OilSlick(this, getRandomRoadX(), VIEW_HEIGHT);
        m_objects.push_back(oil);
    }
}
/* increment souls saved count*/
void StudentWorld::soulSaved()
{
    m_soulsSaved++;
}

/* Add soul to world */
void StudentWorld::addSoul()
{
    if (shouldCreateActor(100))
    {
        Soul *soul = new Soul(this, getRandomRoadX(), VIEW_HEIGHT);
        m_objects.push_back(soul);
    }
}

/* Get random X coord on road */
double StudentWorld::getRandomRoadX() const
{
    return randInt(ROAD_LEFT_EDGE, ROAD_RIGHT_EDGE);
}

/* Determine if a actor should be made based on a certain chance */
bool StudentWorld::shouldCreateActor(int chance) const
{
    int upperBound = chance - 1; // randInt is inclusive, so -1 from chance
    return randInt(0, upperBound) == 0;
}

void StudentWorld::addWaterGoodie()
{
    int chanceHolyWater = 100 + 10 * getLevel();
    if (shouldCreateActor(chanceHolyWater))
    {
        WaterGoodie *waterGoodie = new WaterGoodie(this, getRandomRoadX(), VIEW_HEIGHT);
        m_objects.push_back(waterGoodie);
    }
}

/* Let StudentWorld know human has been hit */
void StudentWorld::humanHit()
{
    m_isHumanHit = true;
}

void StudentWorld::resetHumanHit()
{
    m_isHumanHit = false;
}

void StudentWorld::addHuman()
{
    int chanceHuman = max(200 - getLevel() * 10, 30);
    if (shouldCreateActor(chanceHuman))
    {
        HumanPedestrian *human = new HumanPedestrian(this, getRandomScreenX(), VIEW_HEIGHT);
        m_objects.push_back(human);
    }
}

/* Get random X coord on screen */
double StudentWorld::getRandomScreenX() const
{
    return randInt(0, VIEW_WIDTH);
}

/* Add actor to world */
void StudentWorld::addActor(Actor *actor)
{
    m_objects.push_back(actor);
}

void StudentWorld::addZombiePed()
{
    int chanceZombie = max(100 - getLevel() * 10, 20);
    if (shouldCreateActor(chanceZombie))
    {
        ZombiePedestrian *zombie = new ZombiePedestrian(this, getRandomScreenX(), VIEW_HEIGHT);
        m_objects.push_back(zombie);
    }
}

/* Check all actors for holy water collisions*/
bool StudentWorld::checkProjectileHit(HolyWater *projectile)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        // only collide with holy water if actor can and is overlapping w/ holy water
        if ((*it)->canCollideWater() && (*it)->isOverlapping(projectile))
        {
            (*it)->onCollideWater();
            // let holy water know it hit something
            return true;
        }
    }
    // let holy water know it didn't hit something
    return false;
}

/* Add zombie cab depending if there's space on screen */
void StudentWorld::addZombieCab()
{
    int chanceCab = max(100 - getLevel() * 10, 20);
    if (shouldCreateActor(chanceCab))
    {
        set<int> checkedLanes;
        bool foundLane = false;
        // get random lane to check
        int curLane = randInt(1, NUM_LANES);
        double startX;
        double startY;
        double ySpeed;

        // look for new lane to insert if we haven't found a lane and haven't checked all lanes
        while (!foundLane && checkedLanes.size() != NUM_LANES)
        {
            checkedLanes.insert(curLane);
            switch (curLane)
            {
            case 1:
                // check lane 1 top of screen for space
                if (distanceClosestCAWActor(ROAD_LEFT_EDGE, LEFT_DIVIDER_X, 0) > VIEW_HEIGHT / 3)
                {
                    foundLane = true;
                    startX = LEFT_LANE_CENTER;
                    startY = SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() + getCabSpeedModifier();
                    break;
                }
                // check lane 1 bottom of screen for space
                if (distanceClosestCAWActor(ROAD_LEFT_EDGE, LEFT_DIVIDER_X, VIEW_HEIGHT) > VIEW_HEIGHT / 3)
                {
                    foundLane = true;
                    startX = LEFT_LANE_CENTER;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() - getCabSpeedModifier();
                    break;
                }
            case 2:
                // check lane 2 top of screen for space
                if (distanceClosestCAWActor(LEFT_DIVIDER_X, RIGHT_DIVIDER_X, 0) > VIEW_HEIGHT / 3)
                {
                    foundLane = true;
                    startX = ROAD_CENTER;
                    startY = SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() + getCabSpeedModifier();
                    break;
                }
                // check lane 2 bottom of screen for space
                if (distanceClosestCAWActor(LEFT_DIVIDER_X, RIGHT_DIVIDER_X, VIEW_HEIGHT) > VIEW_HEIGHT / 3)
                {
                    foundLane = true;
                    startX = ROAD_CENTER;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() - getCabSpeedModifier();
                    break;
                }
            case 3:
                // check lane 3 top of screen for space
                if (distanceClosestCAWActor(RIGHT_DIVIDER_X, ROAD_RIGHT_EDGE, 0) > VIEW_HEIGHT / 3)
                {
                    foundLane = true;
                    startX = RIGHT_LANE_CENTER;
                    startY = SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() + getCabSpeedModifier();
                    break;
                }
                // check lane 3 bottom of screen for space
                if (distanceClosestCAWActor(RIGHT_DIVIDER_X, ROAD_RIGHT_EDGE, VIEW_HEIGHT) > VIEW_HEIGHT / 3)
                {
                    foundLane = true;
                    startX = RIGHT_LANE_CENTER;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() - getCabSpeedModifier();
                    break;
                }
            }

            // find next lane to check, since current lane not suitable
            // if all lanes, checked, curLane not changed, but while loop above ends
            for (int i = 1; i <= NUM_LANES; ++i)
            {
                if (checkedLanes.find(i) == checkedLanes.end())
                {
                    curLane = i;
                    break;
                }
            }
        }

        // add cab if good lane found
        if (foundLane)
        {
            ZombieCab *cab = new ZombieCab(this, ySpeed, startX, startY);
            m_objects.push_back(cab);
        }
    }
}

/* Finds shortest absolute distance */
double StudentWorld::distanceClosestCAWActor(double xMin, double xMax, double y) const
{
    // minDist initialized to max height in case no actors found in lane
    double minDist = VIEW_HEIGHT;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        // if CAW actor and in lane, find absolute distance from y pos
        if ((*it)->isCAW() && (*it)->getX() >= xMin && (*it)->getX() < xMax)
        {
            double dist = abs((*it)->getY() - y);
            if (dist < minDist)
            {
                minDist = dist;
            }
        }
    }

    // check ghost racer distance (its not in the actors vec)
    if (getGR()->getX() >= xMin && getGR()->getX() < xMax)
    {
        double dist = abs(getGR()->getY() - y);
        if (dist < minDist)
        {
            minDist = dist;
        }
    }
    // return the smallest distance from Y pos
    return minDist;
}

/* Determine closest CAW actor in front or behind cab */
double StudentWorld::directionalDistanceClosetCAWActor(const ZombieCab *cab, bool inFront) const
{
    double xMin;
    double xMax;
    int lane = cab->getLane();

    // get X bounds based on lane
    switch (lane)
    {
    case 1:
        xMin = ROAD_LEFT_EDGE;
        xMax = LEFT_DIVIDER_X;
        break;
    case 2:
        xMin = LEFT_DIVIDER_X;
        xMax = RIGHT_DIVIDER_X;
        break;
    case 3:
        xMin = RIGHT_DIVIDER_X;
        xMax = ROAD_RIGHT_EDGE;
        break;
    }

    double minDist = VIEW_HEIGHT; // set as such to return in case of no actor found
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        // don't consider cab itself
        if ((*it) == cab)
        {
            continue;
        }
        // only check CAW actors within x bounds
        if ((*it)->isCAW() && (*it)->getX() >= xMin && (*it)->getX() < xMax)
        {
            double dist = (*it)->getY() - cab->getY();
            // only consider actors in proper direction from cab
            if ((inFront && dist < 0) || (!inFront && dist > 0))
            {
                continue;
            }
            dist = abs(dist);
            if (dist < minDist)
            {
                minDist = dist;
            }
        }
    }

    // check GR for distance to cab
    if (getGR()->getX() >= xMin && getGR()->getX() < xMax)
    {
        double dist = getGR()->getY() - cab->getY();
        // if GR not in right dir, just return
        if ((inFront && dist < 0) || (!inFront && dist > 0))
        {
            return minDist;
        }
        dist = abs(dist);
        if (dist < minDist)
        {
            minDist = dist;
        }
    }

    // return shortest distance to CAW actor either in front or behind actor (inFront boolean)
    return minDist;
}

/* Add variation to cab speed on initialization */
double StudentWorld::getCabSpeedModifier() const
{
    return randInt(2, 4);
}
