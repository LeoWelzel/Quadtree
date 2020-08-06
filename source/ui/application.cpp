#include "application.hpp"

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

    bool collidersIntersect(const QuadtreeCollider* collider1, const QuadtreeCollider* collider2)
    {
        #ifdef ASSERTIONS
            assert(collider1);
            assert(collider2);
        #endif

        bool debugging1 = collider2->left   <= collider1->right,
             debugging2 = collider2->right  >= collider1->left,
             debugging3 = collider2->top    >= collider1->bottom,
             debugging4 = collider2->bottom <= collider1->top;

        return collider2->left   <= collider1->right  && 
               collider2->right  >= collider1->left   &&
               collider2->top    >= collider1->bottom &&
               collider2->bottom <= collider1->top;
    }

    void handleCollision(QuadtreeCollider* collider1, QuadtreeCollider* collider2)
    {
        int xDiff = (collider2->right + collider2->left -
             collider1->right - collider1->left) / 2;

        int yDiff = (collider2->top + collider2->bottom -
            collider1->top - collider1->bottom) / 2;

        double collider1Mag = sqrt(collider1->xMotion * collider1->xMotion + collider1->yMotion * collider1->yMotion),
            collider2Mag = sqrt(collider2->xMotion * collider2->xMotion + collider2->yMotion * collider2->yMotion),
            newVectorMag = sqrt(xDiff * xDiff + yDiff * yDiff);

        if (newVectorMag)
        {
            collider2->xMotion = round(xDiff * collider1Mag / newVectorMag);
            collider2->yMotion = round(yDiff * collider1Mag / newVectorMag);

            collider1->xMotion = -collider2->xMotion;
            collider1->yMotion = -collider2->yMotion;
        }
    }
}

PRNG Application::randomGenerator = PRNG(23543298);

Application::Application(size_t width, size_t height, size_t frameRate, std::string title,
    int agentWidth, int agentRows, int agentColumns, sf::Color backgroundColour,
    int treeWidth, int treeHeight)
    : width(width), height(height), frameRate(frameRate), title(title),
      backgroundColour(backgroundColour), agentWidth(agentWidth), agentRows(agentRows),
      agentColumns(agentColumns), quadtree(treeHeight, 0, 0, treeWidth, 8, 8), initialised(false),
      windowPtr(nullptr), xMultiplier(width / float(treeWidth)),
      yMultiplier(height / float(treeHeight)), treeWidth(treeWidth), treeHeight(treeHeight),
      running(false)
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
                y * totalBlockSize + agentWidth, y * totalBlockSize, x * totalBlockSize,
                x * totalBlockSize + agentWidth
            );
            collider.xMotion = randomGenerator.rand<int>() % 40;
            collider.yMotion = randomGenerator.rand<int>() % 40;
            this->colliders.pushBack(collider);
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

void Application::loopAction()
{
    this->windowPtr->clear(this->backgroundColour);

    this->lineVertexArray.clear();
    this->quadVertexArray.clear();

    if (this->running)
    {
        this->moveColliders();

        std::cout << "Moved colliders.\n";

        this->applyCollisions();
    }

    this->drawColliders();

    this->windowPtr->display();
}

void Application::moveColliders()
{
    const int numColliders = this->colliders.size();
    for (int i = 0; i < this->colliders.size(); i++)
    {
        QuadtreeCollider& collider = this->colliders.at(i);

        if (collider.left < 0 && collider.xMotion < 0)
            collider.xMotion = -collider.xMotion;
        if (collider.right > this->treeWidth && collider.xMotion > 0)
            collider.xMotion = -collider.xMotion;

        if (collider.bottom < 0 && collider.yMotion < 0)
            collider.yMotion = -collider.yMotion;
        if (collider.top > this->treeHeight && collider.yMotion > 0)
            collider.yMotion = -collider.yMotion;

        collider.left += collider.xMotion;
        collider.right += collider.xMotion;
        collider.top += collider.yMotion;
        collider.bottom += collider.yMotion;

        this->quadtree.insert(&collider);
    }
    std::cout << "Exited loop.\n";
}

void Application::applyCollisions()
{
    FreeStack<int> treeLeafIndices;
    this->quadtree.getAllLeaves(&treeLeafIndices);

    const int numLeaves = treeLeafIndices.size();
    int element1 = 0, element2 = 0, currentNodeIndex;

    QuadtreeCollider* collider1{ nullptr }, *collider2{ nullptr };

    for (int i = 0; i < numLeaves; i++)
    {
        currentNodeIndex = treeLeafIndices.at(i);
        element1 = this->quadtree.quadNodes.at(currentNodeIndex).firstChild;

        while (element1 != ElementNode::NONE)
        {
            element2 = this->quadtree.elementNodes.at(element1).next;

            while (element2 != ElementNode::NONE)
            {
                collider1 = this->quadtree.colliderPtrs.at(this->quadtree.elementNodes.at(element1).colliderIndex);
                collider2 = this->quadtree.colliderPtrs.at(this->quadtree.elementNodes.at(element2).colliderIndex);

                if (collidersIntersect(collider1, collider2))
                    handleCollision(collider1, collider2);

                element2 = this->quadtree.elementNodes.at(element2).next;
            }

            element1 = this->quadtree.elementNodes.at(element1).next;
        }
    }
    
    this->quadtree.cleanup();
    this->quadtree.clearElements();
}

void Application::drawColliders()
{
    #define APPEND_VERTEX(vertexArray, x, y, colour)        \
    vertexArray.append(sf::Vertex(sf::Vector2f(x, y), colour));

    for (int i = 0; i < this->colliders.size(); i++)
    {
        QuadtreeCollider collider = this->colliders.at(i);

        float left = collider.left * this->xMultiplier,
            right = collider.right * this->xMultiplier,
            top = collider.top * this->yMultiplier,
            bottom = collider.bottom * this->yMultiplier;

        APPEND_VERTEX(this->quadVertexArray, left, top, sf::Color(0x7fabdb));
        APPEND_VERTEX(this->quadVertexArray, left, bottom, sf::Color(0x7fabdb));
        APPEND_VERTEX(this->quadVertexArray, right, bottom, sf::Color(0x7fabdb));
        APPEND_VERTEX(this->quadVertexArray, right, top, sf::Color(0x7fabdb));
    }

    #undef APPEND_VERTEX

    this->windowPtr->draw(this->quadVertexArray);
}

void Application::handleEvents()
{
    sf::Event event;

    while (this->windowPtr->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            this->windowPtr->close();
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::K)
                this->running = !this->running;
        }
    }
}