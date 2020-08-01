#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include "defs.h"
#include "freelist.h"
#include "freestack.h"
#include "quadtreecollider.h"

/* Stores all needed data about a single "element node" - a single collider element within a */
/* specified quadnode. */
struct ElementNode
{
    const static int NONE = -1;

    ElementNode(int next = NONE, int colliderIndex = NONE);

    int next, colliderIndex;
};

/* Stores all needed data about a single quadnode. */
struct QuadNode
{
    const static int BRANCH_NODE = -1;

    QuadNode(int firstChild = 0, int numElements = BRANCH_NODE);

    /* The element nodes are stored as a singly linked list. */
    int firstChild, numElements;
};

/* Stores a quadnode, as well as its boundaries and depth. */
struct QuadNodeData
{
    QuadNodeData();
    QuadNodeData(int quadNodeIndex, int depth, int top, int bottom, int left, int right);

    int quadNodeIndex, depth, top, bottom, left, right;
};

class Quadtree
{
public:
    Quadtree(int top, int bottom, int left, int right, int maxDivisions, int maxEltsPerNode);

    /* Inserts the collider into the quadtree, returning its index in the collider freelist. */
    int insert(QuadtreeCollider* colliderPtr);

    /* Removes the collider from the quadtree. No shortcuts are taken if the collider is */
    /* already absent from the tree. */
    void remove(QuadtreeCollider* colliderPtr);

    // TODO: assess the safety of using a freelist with pointers.
    FreeList<QuadtreeCollider*> colliderPtrs;
    FreeList<QuadNode> quadNodes;
    FreeList<ElementNode> elementNodes;

private:
    int top, bottom, left, right, maxDivisions, maxEltsPerNode;

    /* Stores the index of the tree root in the quadtree. This should be 0, 100% of the time. */
    int rootNodeIndex;

    /* Used for ensuring that no colliders are appended to an output list more than once when */
    /* the query space is not a single quadnode. */
    bool* queryTable;
    int queryTableSize;
};

#endif