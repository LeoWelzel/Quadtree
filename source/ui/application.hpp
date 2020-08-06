#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <ctime>
#include <cstdlib>

#include <chrono>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "../defs.hpp"
#include "../quadtree.hpp"

namespace
{
    typedef unsigned long long int U64;
    typedef std::chrono::high_resolution_clock::time_point timePoint;

    class PRNG
    {
        U64 seed;
        U64 rand64();

    public:
        PRNG(U64 _seed);
        template<typename T> T rand();
        template<typename T> T sparse_rand();
    };

    inline timePoint getCurrentTime()
    {
        return std::chrono::high_resolution_clock::now();
    }

    inline double getTimeDiff(timePoint earlier, timePoint later)
    {
        constexpr double divisor = 1000 * 1000 * 1000;
        return std::chrono::duration_cast<std::chrono::nanoseconds>(later - earlier).count() / divisor;
    }

    bool collidersIntersect(const QuadtreeCollider* collider1, const QuadtreeCollider* collider2);

    void handleCollision(QuadtreeCollider* collider1, QuadtreeCollider* collider2);
}

class Application
{
public:
    Application(size_t width, size_t height, size_t frameRate, std::string title,
        int agentWidth, int agentRows, int agentColumns, sf::Color backgroundColour =
            sf::Color(0, 11, 45), int treeWidth = 65536, int treeHeight = 65536);
    
    ~Application();

    void initialise(), run();

private:
    bool initialised, running;
    float xMultiplier, yMultiplier;
    int agentWidth, agentRows, agentColumns, treeWidth, treeHeight;

    size_t width, height, frameRate;
    std::string title;

    sf::RenderWindow* windowPtr;
    sf::Color backgroundColour;
    sf::VertexArray quadVertexArray, lineVertexArray;
    
    FreeStack<QuadtreeCollider> colliders;
    Quadtree quadtree;

    static PRNG randomGenerator;

    void moveColliders(), applyCollisions(), drawColliders();
    
    void loopAction(), handleEvents();
};

#endif