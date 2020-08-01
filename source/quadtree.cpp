#include "quadtree.h"

ElementNode::ElementNode(int next, int colliderIndex)
    : next(next), colliderIndex(colliderIndex)
{
}

QuadNode::QuadNode(int firstChild, int numElements)
    : firstChild(firstChild), numElements(numElements)
{
}

QuadNodeData::QuadNodeData()
    : quadNodeIndex(0), depth(0), top(0), bottom(0), left(0), right(0)
{
}

QuadNodeData::QuadNodeData(int quadNodeIndex, int depth, int top, int bottom, int left, int right)
    : quadNodeIndex(quadNodeIndex), depth(depth), top(top), bottom(bottom), left(left),
      right(right)
{
}

Quadtree::Quadtree(int top, int bottom, int left, int right, int maxDivisions, int maxEltsPerNode)
    : treeTop(top), treeBottom(bottom), treeLeft(left), treeRight(right), maxDivisions(maxDivisions),
      maxEltsPerNode(maxEltsPerNode), rootNodeIndex(0),

      queryTable(nullptr), queryTableSize(0)
{
    this->rootNodeIndex = this->quadNodes.insert();
    assert(this->rootNodeIndex == 0);

    /* Initialise root node. */
    this->quadNodes.at(this->rootNodeIndex).firstChild = ElementNode::NONE;
    this->quadNodes.at(this->rootNodeIndex).numElements = 0;
}

Quadtree::~Quadtree()
{
    if (this->queryTableSize)
        delete[] this->queryTable;
}

int Quadtree::insert(QuadtreeCollider* colliderPtr)
{
    FreeStack<QuadNodeData> leavesForInsertion;

    // AWAITING: getLeaves() function.
}

void Quadtree::getLeaves(FreeStack<QuadNodeData>* output, QuadNodeData searchSpace,
    int colliderTop, int colliderBottom, int colliderLeft, int colliderRight) const
{
    /* Return early if collider not contained within boundaries. */
    if (searchSpace.top <= colliderBottom || searchSpace.bottom > colliderTop ||
        searchSpace.right <= colliderLeft || searchSpace.left > colliderBottom)
        return;

    /* Avoid recursion by using a stack instead. */
    FreeStack<QuadNodeData> processingStack;

    Quadtree::pushBackNode(&processingStack, this->rootNodeIndex, 0, this->treeTop,
        this->treeBottom, this->treeLeft, this->treeRight);

    while (processingStack.size() > 0)
    {
        const QuadNodeData topData = processingStack.top();
        processingStack.popBack();

        #ifdef ASSERTIONS
            const int divisor = pow(2, topData.depth);
            assert((topData.top - topData.bottom) == (this->treeTop - this->treeBottom) / divisor);
            assert((topData.right - topData.left) == (this->treeRight - this->treeLeft) / divisor);
        #endif

        /* Push back all discovered leaf nodes. */
        if (this->quadNodes.at(topData.quadNodeIndex).numElements != QuadNode::BRANCH_NODE)
            output->pushBack(topData);
        else
        {
            const int firstChild = this->quadNodes.at(topData.quadNodeIndex).firstChild,
                halfX = topData.left + ((topData.right - topData.left) / 2),
                halfY = topData.bottom + ((topData.top - topData.bottom) / 2);

            if (colliderLeft < halfX)
            {
                /* Top left. */
                if (colliderTop >= halfY)
                    Quadtree::pushBackNode(&processingStack, firstChild, topData.depth + 1,
                        topData.top, halfY, topData.left, halfX);
                /* Bottom left. */
                if (colliderBottom < halfY)
                    Quadtree::pushBackNode(&processingStack, firstChild + 2, topData.depth + 1,
                        halfY, topData.bottom, topData.left, halfX);
            }
            if (colliderRight >= halfX)
            {
                /* Top right. */
                if (colliderTop >= halfY)
                    Quadtree::pushBackNode(&processingStack, firstChild + 1, topData.depth + 1,
                        topData.top, halfY, halfX, topData.right);
                /* Bottom right. */
                if (colliderBottom < halfY)
                    Quadtree::pushBackNode(&processingStack, firstChild + 3, topData.depth + 1,
                        halfY, topData.bottom, halfX, topData.right);
            }
        }
        
    }
}
