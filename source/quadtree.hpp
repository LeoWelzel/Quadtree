#ifndef QUADTREE_HPP_INCLUDED
#define QUADTREE_HPP_INCLUDED

#ifdef ASSERTIONS
    #include <cmath>
#endif

#include "../../util/freelist.hpp"
#include "quadtreecollider.hpp"

/* Stores data about a single node in the quadtree. */
struct QuadNode
{
    const static int BRANCH_NODE = -1;

    QuadNode(int firstChild = 0, int numElements = BRANCH_NODE);

    int firstChild;
    int numElements;
};

/* Stores data about a single element in a quadtree leaf. */
struct ElementNode
{
    const static int NONE = -1;

    ElementNode(int next = NONE, int colliderIndex = NONE);

    int next, colliderIndex;
};

/* Stores additional data about a single QuadNode. */
struct QuadNodeData
{
    QuadNodeData();
    QuadNodeData(int quadNodeIndex, int depth, int top, int bottom, int left, int right);

    int quadNodeIndex, depth, top, bottom, left, right;
};

class QuadTree
{
public:
    QuadTree(int top, int bottom, int left, int right, int maxDivisions, int maxEltsPerNode);

    ~QuadTree();

    /* Inserts the collider into the quadtree. */
    int insert(const QuadTreeCollider& collider);

    /* Removes the collider from the quadtree. */
    void remove(const QuadTreeCollider& collider, int colliderIndex);

    /* Clears the quadtree of all inserted elements. */
    void clearElements();

    /* Populates the list with all the quadtree leaf indices. */
    void getAllLeaves(FreeList<int>* nodeIndices);

    /* Populates the list with all the quadnode leaf data. */
    void getAllLeafDatas(FreeList<QuadNodeData>* quadNodeDatas);

    /* Cleans up the quadtree. */
    void cleanup();

    /* Populates the freelist with the pointers to the colliders inside the boundaries. */
    void query(FreeList<QuadTreeCollider*>* output, int top, int bottom, int left, int right);

    FreeList<QuadTreeCollider> colliders;
    FreeList<QuadNode> quadNodes;
    FreeList<ElementNode> elementNodes;

private:
    int topBound, bottomBound, leftBound, rightBound, maxDivisions, maxEltsPerNode;

    int rootNodeIndex;
    
    bool* queryTable;
    int queryTableSize;

    /* Populates the passed freelist with the quadNodeData objects corresponding to the quadnodes
     * that contain some part of the passed boundaries. */
    void getLeaves(FreeList<QuadNodeData>* output, int colliderTop, int colliderBottom, int colliderLeft, int colliderRight,
        int quadNodeIndex, int depth, int top, int bottom, int left, int right);

    /* Inserts the given collider pointer into the given quadnode. */
    void nodeInsert(int colliderIndex, const QuadNodeData& data);

    /* Subdivides the given node. */
    void subdivideNode(int quadNodeIndex, int depth, int top, int bottom, int left, int right);
};

inline void pushBackNode(FreeList<QuadNodeData>* output, int quadNodeIndex, int depth, int top, int bottom, int left, int right)
{
    QuadNodeData& data = output->at(output->pushBack());

    data.quadNodeIndex = quadNodeIndex;
    data.depth = depth;
    data.top = top;
    data.bottom = bottom;
    data.left = left;
    data.right = right;
}

#endif