#include "application.h"

namespace
{
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
