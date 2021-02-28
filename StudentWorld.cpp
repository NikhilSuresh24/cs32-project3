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

void StudentWorld::soulSaved()
{
    m_soulsSaved++;
}

void StudentWorld::addSoul()
{
    if (shouldCreateActor(100))
    {
        Soul *soul = new Soul(this, getRandomRoadX(), VIEW_HEIGHT);
        m_objects.push_back(soul);
    }
}

double StudentWorld::getRandomRoadX() const
{
    return randInt(ROAD_LEFT_EDGE, ROAD_RIGHT_EDGE);
}

bool StudentWorld::shouldCreateActor(int chance) const
{
    int upperBound = chance - 1;
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

double StudentWorld::getRandomScreenX() const
{
    return randInt(0, VIEW_WIDTH);
}

void StudentWorld::addActor(Actor *actor) //TODO: can this be a const actor?
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

bool StudentWorld::checkProjectileHit(HolyWater *projectile)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        if ((*it)->canCollideWater() && (*it)->isOverlapping(projectile))
        {
            (*it)->onCollideWater();
            return true;
        }
    }

    return false;
}

void StudentWorld::addZombieCab()
{
    int chanceCab = max(100 - getLevel() * 10, 20);
    if (shouldCreateActor(chanceCab))
    {
        set<int> checkedLanes;
        bool foundLane = false;
        int curLane = randInt(1, NUM_LANES);
        double startX;
        double startY;
        double ySpeed;

        // look for new lane to insert if we haven't found a lane and haven't checked all lanes
        while (!foundLane && checkedLanes.size() != NUM_LANES)
        {
            // cout << "LANE: " << curLane << endl;
            // cout << "CHECKED LANES: " << checkedLanes.size() << endl;
            checkedLanes.insert(curLane);
            switch (curLane)
            {
            case 1:
                // check lane 1 top and bottom
                // cout << "CASE 1 " << endl;

                if (distanceClosestCAWActor(ROAD_LEFT_EDGE, LEFT_DIVIDER_X, 0) > VIEW_HEIGHT / 3)
                {
                    // cout << "CASE 1 BOT CHECK" << endl;

                    foundLane = true;
                    startX = LEFT_LANE_CENTER;
                    startY = SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() + getCabSpeedModifier();
                    break;
                }

                if (distanceClosestCAWActor(ROAD_LEFT_EDGE, LEFT_DIVIDER_X, VIEW_HEIGHT) > VIEW_HEIGHT / 3)
                {
                    // cout << "CASE 1 TOP CHECK" << endl;

                    foundLane = true;
                    startX = LEFT_LANE_CENTER;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() - getCabSpeedModifier();
                    break;
                }
            case 2:
                // cout << "CASE 2 " << endl;

                // check lane 2 top and bottom
                if (distanceClosestCAWActor(LEFT_DIVIDER_X, RIGHT_DIVIDER_X, 0) > VIEW_HEIGHT / 3)
                {
                    // cout << "CASE 2 BOT CHECK" << endl;

                    foundLane = true;
                    startX = ROAD_CENTER;
                    startY = SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() + getCabSpeedModifier();
                    break;
                }

                if (distanceClosestCAWActor(LEFT_DIVIDER_X, RIGHT_DIVIDER_X, VIEW_HEIGHT) > VIEW_HEIGHT / 3)
                {
                    // cout << "CASE 2 TOP CHECK" << endl;
                    foundLane = true;
                    startX = ROAD_CENTER;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() - getCabSpeedModifier();
                    break;
                }
            case 3:
                // check lane 3 top and bottom
                // cout << "CASE 3 " << endl;

                if (distanceClosestCAWActor(RIGHT_DIVIDER_X, ROAD_RIGHT_EDGE, 0) > VIEW_HEIGHT / 3)
                {
                    // cout << "CASE 3 BOT CHECK" << endl;
                    foundLane = true;
                    startX = RIGHT_LANE_CENTER;
                    startY = SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() + getCabSpeedModifier();
                    break;
                }

                if (distanceClosestCAWActor(RIGHT_DIVIDER_X, ROAD_RIGHT_EDGE, VIEW_HEIGHT) > VIEW_HEIGHT / 3)
                {
                    // cout << "CASE 3 TOP CHECK" << endl;

                    foundLane = true;
                    startX = RIGHT_LANE_CENTER;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    ySpeed = getGR()->getVertSpeed() - getCabSpeedModifier();
                    break;
                }
            }

            // find next lane to check
            // of all lanes, checked, curLane not changed, but while loop above ends
            for (int i = 1; i <= NUM_LANES; ++i)
            {
                if (checkedLanes.find(i) == checkedLanes.end())
                {
                    curLane = i;
                    // cout << "NEW LANE: " << i << endl;
                    break;
                }
            }
        }

        // add cab
        if (foundLane)
        {
            ZombieCab *cab = new ZombieCab(this, ySpeed, startX, startY);
            m_objects.push_back(cab);
        }
    }
}

double StudentWorld::distanceClosestCAWActor(double xMin, double xMax, double y) const
{
    // minDist initialized to max height in case no actors found in lane
    double minDist = VIEW_HEIGHT;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        // if CAW actor and in lane
        if ((*it)->isCAW() && (*it)->getX() >= xMin && (*it)->getX() < xMax)
        {
            double dist = abs((*it)->getY() - y);
            if (dist < minDist)
            {
                minDist = dist;
            }
        }
    }

    // check ghost racer
    if (getGR()->getX() >= xMin && getGR()->getX() < xMax)
    {
        double dist = abs(getGR()->getY() - y);
        if (dist < minDist)
        {
            minDist = dist;
        }
    }
    return minDist;
}

double StudentWorld::directionalDistanceClosetCAWActor(const ZombieCab *cab, bool inFront) const
{
    double xMin;
    double xMax;
    int lane = cab->getLane();

    // get X bounds
    switch (lane)
    {
    case 1:
        // cout << "CASE 1" << endl;
        xMin = ROAD_LEFT_EDGE;
        xMax = LEFT_DIVIDER_X;
        break;
    case 2:
        // cout << "CASE 2" << endl;
        xMin = LEFT_DIVIDER_X;
        xMax = RIGHT_DIVIDER_X;
        break;
    case 3:
        // cout << "CASE 3" << endl;
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

        if ((*it)->isCAW() && (*it)->getX() >= xMin && (*it)->getX() < xMax)
        {
            double dist = (*it)->getY() - cab->getY();
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
    return minDist;
}

double StudentWorld::getCabSpeedModifier() const
{
    return randInt(2, 4);
}
