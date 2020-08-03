#include "application.h"

namespace
{
    PRNG::PRNG(U64 _seed) : seed(_seed)
    {
    }

    U64 PRNG::rand64()
    {
        seed ^= seed >> 12;
        seed ^= seed << 25;
        seed ^= seed >> 27;
        return seed * 2685821657736338717LL;
    }

    
    template<typename T> T PRNG::rand()
    {
        return T(rand64());
    }

    Agent::Agent(const QuadtreeCollider& collider, int xMotion, int yMotion)
        : collider(collider), xMotion(xMotion), yMotion(yMotion)
    {
    }

    bool collidersIntersect(const QuadtreeCollider* collider1, const QuadtreeCollider* collider2)
    {
        #ifdef ASSERTIONS
            assert(collider1);
            assert(collider2);
        #endif

        return collider2->left   <= collider1->right  && 
            collider2->right  >= collider1->left   &&
            collider2->top    >= collider1->bottom &&
            collider2->bottom <= collider1->top;
    }

    void handleCollision(Agent& agent1, Agent& agent2)
    {
        int xDiff = (agent2.collider.right + agent2.collider.left -
             agent1.collider.right - agent1.collider.left) / 2;

        int yDiff = (agent2.collider.top + agent2.collider.bottom -
            agent1.collider.top - agent1.collider.bottom) / 2;

        double agent1Mag = sqrt(agent1.xMotion * agent1.xMotion + agent1.yMotion * agent1.yMotion),
            agent2Mag = sqrt(agent2.xMotion * agent2.xMotion + agent2.yMotion * agent2.yMotion),
            newVectorMag = sqrt(xDiff * xDiff + yDiff * yDiff);

        if (newVectorMag)
        {
            agent2.xMotion = round(xDiff * agent1Mag / newVectorMag);
            agent2.yMotion = round(yDiff * agent1Mag / newVectorMag);

            agent1.xMotion = -agent2.xMotion;
            agent1.yMotion = -agent2.yMotion;
        }
    }
}

PRNG Application::randomGenerator = PRNG(984356546);

Application::Application(size_t width, size_t height, size_t frameRate, std::string title,
    int agentWidth, int agentRows, int agentColumns, sf::Color backgroundColour,
    int treeWidth, int treeHeight)
    : width(width), height(height), frameRate(frameRate), title(title),
      backgroundColour(backgroundColour), agentWidth(agentWidth), agentRows(agentRows),
      agentColumns(agentColumns), quadtree(treeHeight, 0, 0, treeWidth, 8, 8), initialised(false),
      windowPtr(nullptr), xMultiplier(this->width / float(treeWidth)),
      yMultiplier(this->height / float(treeHeight))
{
}

Application::~Application()
{
    delete this->windowPtr;
}

void Application::initialise()
{
    std::srand(std::time(nullptr));

    this->windowPtr = new sf::RenderWindow(sf::VideoMode(this->width, this->height), this->title);
    this->windowPtr->setFramerateLimit(this->frameRate);

    this->quadVertexArray.setPrimitiveType(sf::Quads);
    this->lineVertexArray.setPrimitiveType(sf::Lines);

    const int halfDimensions = this->agentWidth >> 1,
        totalBlockSize = this->agentWidth + halfDimensions;

    /* Add all colliders to the freestack. */
    for (int x = 0; x < this->agentColumns; x++)
    {
        for (int y = 0; y < this->agentRows; y++)
        {
            QuadtreeCollider collider = QuadtreeCollider(
                y * totalBlockSize, y * totalBlockSize + agentWidth, x * totalBlockSize,
                x * totalBlockSize + agentWidth
            );
            this->agents.pushBack(Agent(collider, randomGenerator.rand<int>() % 40,
                randomGenerator.rand<int>() % 40));
        }
    }

    initialised = true;
}

void Application::run()
{
    if (!initialised) this->initialise();

    size_t numFrames = 0;

    timePoint startTime, endTime;
    startTime = getCurrentTime();

    while (this->windowPtr->isOpen())
    {
        this->handleEvents();
        this->loopAction();
        numFrames++;
    }

    endTime = getCurrentTime();
    double seconds = getTimeDiff(startTime, endTime);

    std::cout << numFrames << " produced in " << seconds << " seconds. Average FPS: " << numFrames / seconds << ".\n";
}

void Application::handleEvents()
{
    sf::Event event;

    while (this->windowPtr->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            this->windowPtr->close();
    }
}