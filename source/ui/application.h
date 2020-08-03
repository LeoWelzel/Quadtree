#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <SFML/Graphics.hpp>

#include "../defs.h"
#include "../quadtree.h"

namespace
{
    struct Agent
    {
        Agent(const QuadtreeCollider& collider, int xMotion, int yMotion);

        QuadtreeCollider collider;
        int xMotion, yMotion;
    };

    bool collidersIntersect(const QuadtreeCollider* collider1, const QuadtreeCollider* collider2);

    void handleCollision(Agent& agent1, Agent& agent2);
}

class Application
{
public:
    Application();

    void initialise(), run();

private:
    size_t width, height, frameRate;

    bool initialised;

    std::string title;

    void loopAction(), handleEvents();
};

#endif