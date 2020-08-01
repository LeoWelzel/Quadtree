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
    
}
