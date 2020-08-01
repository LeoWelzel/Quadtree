#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

/* We will need to make certain assertions regardless of whether we are debugging. */
#include <cassert>

#include "defs.h"

#ifdef ASSERTIONS
    #include <cmath>
#endif

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
    ~Quadtree();

    /* Inserts the collider into the quadtree, returning its index in the collider freelist. */
    int insert(QuadtreeCollider* colliderPtr);

    /* Removes the collider from the quadtree. No shortcuts are taken if the collider is */
    /* already absent from the tree. */
    void remove(QuadtreeCollider* colliderPtr);

    #ifdef TO_STRING
    std::string toString() const;
    #endif

    // TODO: assess the safety of using a freelist with pointers.
    FreeList<QuadtreeCollider*> colliderPtrs;
    FreeList<QuadNode> quadNodes;
    FreeList<ElementNode> elementNodes;

private:
    /* Populates the stack with the quadnodedata objects for the leaf nodes containing some part */
    /* of the given collider boundaries. */
    void getLeaves(FreeStack<QuadNodeData>* output, QuadNodeData searchSpace,
        int colliderTop, int colliderBottom, int colliderLeft, int colliderRight) const;

    /* Helper that pushes back a QuadNodeData object with the given attributes. */
    static inline void pushBackNode(FreeStack<QuadNodeData>* output, int quadNodeIndex, int depth,
        int top, int bottom, int left, int right);

    /* Inserts the specified collider into the specified quadnode. */
    void nodeInsert(int colliderIndex, QuadNodeData data);

    /* Subdivides the specified quadnode, populating the new children with the needed elements. */
    void subdivideNode(const QuadNodeData& data);

    int treeTop, treeBottom, treeLeft, treeRight, maxDivisions, maxEltsPerNode;

    /* Stores the index of the tree root in the quadtree. This should be 0, 100% of the time. */
    int rootNodeIndex;

    /* Used for ensuring that no colliders are appended to an output list more than once when */
    /* the query space is not a single quadnode. */
    bool* queryTable;
    int queryTableSize;
};

inline void Quadtree::pushBackNode(FreeStack<QuadNodeData>* output, int quadNodeIndex, int depth,
    int top, int bottom, int left, int right)
{
    QuadNodeData data = QuadNodeData(
        quadNodeIndex, depth, top, bottom, left, right
    );

    output->pushBack(data);
}

#endif