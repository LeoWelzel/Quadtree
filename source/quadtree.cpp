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

    this->rootData = QuadNodeData(this->rootNodeIndex, 0, this->treeTop, this->treeBottom,
        this->treeLeft, this->treeRight);
}

Quadtree::~Quadtree()
{
    if (this->queryTableSize)
        delete[] this->queryTable;
}

int Quadtree::insert(QuadtreeCollider* colliderPtr)
{
    FreeStack<QuadNodeData> leavesForInsertion;

    /* Retrieve all leaf nodes into which this collider needs to be inserted. */
    this->getLeaves(
        &leavesForInsertion, this->rootData, colliderPtr->top, colliderPtr->bottom,
        colliderPtr->left, colliderPtr->right
    );

    const int numLeaves = leavesForInsertion.size(),
        colliderIndex = this->colliderPtrs.insert();

    this->colliderPtrs.at(colliderIndex) = colliderPtr;

    for (int i = 0; i < numLeaves; i++)
        this->nodeInsert(colliderIndex, leavesForInsertion.at(i));

    return colliderIndex;
}

void Quadtree::remove(QuadtreeCollider* colliderPtr, int colliderIndex)
{
    FreeStack<QuadNodeData> leavesForRemoval;

    this->getLeaves(
        &leavesForRemoval, this->rootData, colliderPtr->top, colliderPtr->bottom,
        colliderPtr->left, colliderPtr->right
    );

    const int numLeaves = leavesForRemoval.size();

    for (int i = 0; i < numLeaves; i++)
    {
        const QuadNodeData currentLeaf = leavesForRemoval.at(i);

        int currentElementIndex = this->quadNodes.at(currentLeaf.quadNodeIndex).firstChild,
            previousElementIndex = ElementNode::NONE;

        /* Iterate forward until required to stop. */
        while (currentElementIndex != ElementNode::NONE &&
            this->elementNodes.at(currentElementIndex).colliderIndex != colliderIndex)
        {
            previousElementIndex = currentElementIndex;
            currentElementIndex = this->elementNodes.at(currentElementIndex).next;
        }

        if (currentElementIndex != ElementNode::NONE)
        {
            const int nextIndex = this->elementNodes.at(currentElementIndex).next;

            /* In this case, we found the element immediately. */
            if (previousElementIndex == ElementNode::NONE)
                this->quadNodes.at(currentElementIndex).firstChild = nextIndex;
            /* Otherwise we skip over this element in the linked list. */
            else
                this->elementNodes.at(previousElementIndex).next = nextIndex;

            this->elementNodes.erase(currentElementIndex);
            /* Decrement element node count. */
            this->quadNodes.at(currentLeaf.quadNodeIndex).numElements--;
        }
    }

    /* Finally we remove the collider from the collider pointer freelist. */
    this->colliderPtrs.erase(colliderIndex);
}

void Quadtree::getAllLeaves(FreeStack<int>* nodeIndices) const
{
    FreeStack<int> toProcess;

    toProcess.pushBack(this->rootNodeIndex);

    while (toProcess.size())
    {
        const int quadNodeIndex = toProcess.top(),
            numElements = this->quadNodes.at(quadNodeIndex).numElements,
            first = this->quadNodes.at(quadNodeIndex).firstChild;

        toProcess.popBack();

        int newIndex;

        /* In this case, there is a leaf node. */
        if (numElements != QuadNode::BRANCH_NODE)
            nodeIndices->pushBack(quadNodeIndex);
        /* Otherwise, we push back the indices of all the quadnode children. */
        else for (int i = 0; i < 4; i++)
            toProcess.pushBack(first + i);
    }
}

void Quadtree::getAllLeafNodeDatas(FreeStack<QuadNodeData>* nodeDatas) const
{
    return this->getLeaves(nodeDatas, this->rootData, this->treeTop, this->treeBottom,
           this->treeLeft, this->treeRight);
}

void Quadtree::query(FreeStack<QuadtreeCollider*> output, int top, int bottom, int left, int right)
{
    FreeStack<QuadNodeData> leaves;
    FreeStack<int> usedColliderIndices;

    this->getLeaves(&leaves, this->rootData, top, bottom, left, right);

    /* Re-size the buffer if needed. */
    if (this->queryTableSize != this->colliderPtrs.size())
    {
        if (this->queryTable)
            delete[] this->queryTable;

        this->queryTableSize = this->colliderPtrs.size();
        this->queryTable = new bool[this->queryTableSize] { 0 };
    }

    /* Iterate over the leaves. */
    const int numLeaves = leaves.size();
    int elementIndex, colliderIndex;

    QuadtreeCollider* currentColliderPtr{ nullptr };

    for (int i = 0; i < numLeaves; i++)
    {
        elementIndex = this->quadNodes.at(leaves.at(i).quadNodeIndex).firstChild;

        while (elementIndex != ElementNode::NONE)
        {
            colliderIndex = this->elementNodes.at(elementIndex).colliderIndex;
            currentColliderPtr = this->colliderPtrs.at(colliderIndex);

            /* Append to the list if it intersects the boundaries and hasn't been added yet. */
            if (!this->queryTable[colliderIndex]  &&
                currentColliderPtr->left <= right &&
                currentColliderPtr->right >= left &&
                currentColliderPtr->top >= bottom &&
                currentColliderPtr->bottom <= top)
            {
                /* Update table. */
                this->queryTable[colliderIndex] = 1;
                usedColliderIndices.pushBack(colliderIndex);
                output.pushBack(currentColliderPtr);
            }
            elementIndex = this->elementNodes.at(elementIndex).next;
        }
    }

    /* Unmark the table entries for the inserted elements. */
    const int numAddedElements = usedColliderIndices.size();

    for (int i = 0; i < numAddedElements; i++)
        this->queryTable[usedColliderIndices.at(i)] = 0;
}

void Quadtree::clearElements()
{
    this->elementNodes.clear();
    this->colliderPtrs.clear();

    FreeStack<int> leafIndices;
    this->getAllLeaves(&leafIndices);

    const int numLeaves = leafIndices.size();

    for (int i = 0; i < numLeaves; i++)
    {
        this->quadNodes.at(leafIndices.at(i)).firstChild = ElementNode::NONE;
        this->quadNodes.at(leafIndices.at(i)).numElements = QuadNode::BRANCH_NODE;
    }
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

void Quadtree::nodeInsert(int colliderIndex, QuadNodeData data)
{
    QuadNode& quadNode = this->quadNodes.at(data.quadNodeIndex);

    /* Create element node and push it back in the quadnode list. */
    const int newElementNodeIndex = this->elementNodes.insert();

    this->elementNodes.at(newElementNodeIndex).colliderIndex = colliderIndex;

    /* Join the linked list together such that the new element node is at the front. */
    this->elementNodes.at(newElementNodeIndex).next = quadNode.firstChild;
    quadNode.firstChild = newElementNodeIndex;

    /* Subdivide the node if needed. */
    if (++quadNode.numElements > this->maxEltsPerNode && data.depth < this->maxDivisions)
        this->subdivideNode(data);
}

void Quadtree::subdivideNode(const QuadNodeData& data)
{
    #ifdef ASSERTIONS
        assert(data.depth < this->maxDivisions);
        assert(this->quadNodes.at(data.quadNodeIndex).numElements > this->maxEltsPerNode);
    #endif

    /* Retrieve all currently present colliders. */
    FreeStack<int> colliderIndices;

    int currentElementIndex = this->quadNodes.at(data.quadNodeIndex).firstChild,
        previousElementIndex;

    /* Iterate over singly linked list, pushing all collider indices to the stack. */
    while (currentElementIndex != ElementNode::NONE)
    {
        colliderIndices.pushBack(this->elementNodes.at(currentElementIndex).colliderIndex);

        previousElementIndex = currentElementIndex;
        currentElementIndex = this->elementNodes.at(currentElementIndex).next;

        this->elementNodes.erase(previousElementIndex);
    }

    /* Allocate child nodes. */
    const int newChild = this->quadNodes.insert();

    #ifdef ASSERTIONS
        assert(this->quadNodes.insert() == newChild + 1);
        assert(this->quadNodes.insert() == newChild + 2);
        assert(this->quadNodes.insert() == newChild + 3);
    #else
        this->quadNodes.insert();
        this->quadNodes.insert();
        this->quadNodes.insert();
    #endif

    /* Set all child nodes as empty. */
    for (size_t i = 0; i < 4; i++)
    {
        this->quadNodes.at(newChild + i).firstChild = ElementNode::NONE;
        this->quadNodes.at(newChild + i).numElements = 0;
    }

    /* Update original quadnode's attributes. */
    this->quadNodes.at(data.quadNodeIndex).numElements = QuadNode::BRANCH_NODE;
    this->quadNodes.at(data.quadNodeIndex).firstChild = newChild;

    FreeStack<QuadNodeData> leavesForInsertion;

    const int numColliders = colliderIndices.size();

    /* Iterate over the colliders, inserting them into the leaf nodes as needed. */
    for (int i = 0; i < numColliders; i++)
    {
        const int colliderIndex = colliderIndices.at(i);
        const QuadtreeCollider* colliderPtr = this->colliderPtrs.at(colliderIndex);

        this->getLeaves(
            &leavesForInsertion, data, colliderPtr->top, colliderPtr->bottom, colliderPtr->left,
            colliderPtr->right
        );

        const int numLeaves = leavesForInsertion.size();

        for (int j = 0; j < numLeaves; j++)
            Quadtree::nodeInsert(colliderIndex, leavesForInsertion.at(j));

        /* Empty the stack for re-use next iteration. */
        leavesForInsertion.clear();
    }
}
