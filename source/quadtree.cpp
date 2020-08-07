#include "quadtree.hpp"

QuadNode::QuadNode(int firstChild, int numElements)
    : firstChild(firstChild), numElements(numElements)
{
}

ElementNode::ElementNode(int next, int colliderIndex)
    : next(next), colliderIndex(colliderIndex)
{
}

QuadNodeData::QuadNodeData()
    : quadNodeIndex(0), depth(0), top(0), bottom(0), left(0), right(0)
{
}

QuadNodeData::QuadNodeData(int quadNodeIndex, int depth, int top, int bottom, int left, int right)
    : quadNodeIndex(quadNodeIndex), depth(depth), top(top), bottom(bottom), left(left), right(right)
{
}

QuadTree::QuadTree(int top, int bottom, int left, int right, int maxDivisions, int maxEltsPerNode)
    : topBound(top), bottomBound(bottom), leftBound(left), rightBound(right),
      maxDivisions(maxDivisions), maxEltsPerNode(maxEltsPerNode), queryTable(new bool[FREELIST_FIXED_SIZE] { 0 }),
      queryTableSize(FREELIST_FIXED_SIZE)
{
    this->rootNodeIndex = this->quadNodes.insert();
    
    #ifdef ASSERTIONS
        assert(this->rootNodeIndex == 0);
    #endif

    QuadNode& rootNode = this->quadNodes.at(this->rootNodeIndex);

    rootNode.firstChild = ElementNode::NONE;
    rootNode.numElements = 0;
}

QuadTree::~QuadTree()
{
    this->elementNodes.~FreeList();
    this->quadNodes.~FreeList();
    this->colliderPtrs.~FreeList();
 
    delete[] this->queryTable;
}

int QuadTree::insert(QuadTreeCollider* collider)
{
    FreeList<QuadNodeData> leavesForInsertion;

    this->getLeaves(&leavesForInsertion, collider->top, collider->bottom, collider->left, collider->right, this->rootNodeIndex, 0,
        this->topBound, this->bottomBound, this->leftBound, this->rightBound);

    const int numLeaves = leavesForInsertion.size();

    const int colliderIndex = this->colliderPtrs.insert();
    this->colliderPtrs.at(colliderIndex) = collider;

    for (int i = 0; i < numLeaves; i++)
        this->nodeInsert(colliderIndex, leavesForInsertion.at(i));

    return colliderIndex;
}

void QuadTree::remove(QuadTreeCollider* collider, int colliderIndex)
{
    FreeList<QuadNodeData> leavesForRemoval;

    this->getLeaves(&leavesForRemoval, collider->top, collider->bottom, collider->left, collider->right, this->rootNodeIndex, 0,
        this->topBound, this->bottomBound, this->leftBound, this->rightBound);

    const int numLeaves = leavesForRemoval.size();

    /* Remove from all leaves that the collider occupies. */
    for (int i = 0; i < numLeaves; i++)
    {
        const QuadNodeData currentLeaf = leavesForRemoval.at(i);

        int currentElement = this->quadNodes.at(currentLeaf.quadNodeIndex).firstChild, previous = ElementNode::NONE;

        while (currentElement != ElementNode::NONE && this->elementNodes.at(currentElement).colliderIndex != colliderIndex)
        {
            previous = currentElement;
            currentElement = this->elementNodes.at(currentElement).next;
        }

        if (currentElement != ElementNode::NONE)
        {
            const int nextIndex = this->elementNodes.at(currentElement).next;
            
            /* In this case, we found the element immediately. */
            if (previous == ElementNode::NONE)
                this->quadNodes.at(currentLeaf.quadNodeIndex).firstChild = nextIndex;
            /* Otherwise we simply skip over the element in the linked list. */
            else
                this->elementNodes.at(previous).next = nextIndex;
            
            this->elementNodes.erase(currentElement);
            /* Decrement element node count. */
            this->quadNodes.at(currentLeaf.quadNodeIndex).numElements--;
        }
    }

    /* Finally we remove the collider from the collider pointer freelist. */
    this->colliderPtrs.erase(colliderIndex);
}

void QuadTree::query(FreeList<QuadTreeCollider*>* output, int top, int bottom, int left, int right)
{
    FreeList<QuadNodeData> includedLeaves;
    FreeList<int> usedIndices;

    this->getLeaves(&includedLeaves, top, bottom, left, right, this->rootNodeIndex, 0, this->topBound, this->bottomBound, this->leftBound, this->rightBound);

    /* Re-size the buffer if needed. */
    if (this->queryTableSize != this->colliderPtrs.size())
    {
        delete[] this->queryTable;

        this->queryTableSize = this->colliderPtrs.size();
        this->queryTable = new bool[this->queryTableSize] { 0 };
    }

    /* Iterate over the leaves. */
    const int numLeaves = includedLeaves.size();
    int elementIndex, colliderIndex;

    QuadTreeCollider* currentColliderPtr{ nullptr };

    for (int i = 0; i < numLeaves; i++)
    {
        elementIndex = this->quadNodes.at(includedLeaves.at(i).quadNodeIndex).firstChild;

        while (elementIndex != ElementNode::NONE)
        {
            colliderIndex = this->elementNodes.at(elementIndex).colliderIndex;
            currentColliderPtr = this->colliderPtrs.at(colliderIndex);

            /* Append to the list if it intersects the given boundaries and hasn't yet been added. */
            if (!this->queryTable[colliderIndex] && 
                currentColliderPtr->left <= right &&
                currentColliderPtr->right >= left &&
                currentColliderPtr->top >= bottom &&
                currentColliderPtr->bottom <= top)
            {
                this->queryTable[colliderIndex] = 1;
                usedIndices.at(usedIndices.pushBack()) = colliderIndex;
                output->at(output->pushBack()) = currentColliderPtr;
            }

            elementIndex = this->elementNodes.at(elementIndex).next;
        }
    }

    /* Unmark the elements that were inserted. */
    const int numAddedElements = usedIndices.size();
    for (int i = 0; i < numAddedElements; i++)
        this->queryTable[usedIndices.at(i)] = 0;
}

void QuadTree::clearElements()
{
    this->elementNodes.clear();
    this->colliderPtrs.clear();

    FreeList<int> leafIndices;
    this->getAllLeaves(&leafIndices);

    const int numLeaves = leafIndices.size();

    for (int i = 0; i < numLeaves; i++)
    {
        this->quadNodes.at(leafIndices.at(i)).firstChild = ElementNode::NONE;
        this->quadNodes.at(leafIndices.at(i)).numElements = 0;
    }
}

void QuadTree::getAllLeaves(FreeList<int>* nodeIndices)
{
    FreeList<int> toProcess;

    toProcess.at(toProcess.pushBack()) = this->rootNodeIndex;

    while (toProcess.size())
    {
        const int quadNodeIndex = toProcess.at(toProcess.size() - 1),
            numElements = this->quadNodes.at(quadNodeIndex).numElements,
            first = this->quadNodes.at(quadNodeIndex).firstChild;

        toProcess.popBack();

        int newIndex;

        /* In this case, we have a leaf node. */
        if (numElements != QuadNode::BRANCH_NODE)
            nodeIndices->at(nodeIndices->pushBack()) = quadNodeIndex;
        /* Otherwise, we push back the indices of all the quadnode children. */
        else for (int i = 0; i < 4; i++)
            toProcess.at(toProcess.pushBack()) = first + i;
    }
}

void QuadTree::getAllLeafDatas(FreeList<QuadNodeData>* quadNodeDatas)
{
    this->getLeaves(quadNodeDatas, this->topBound, this->bottomBound,
        this->leftBound, this->rightBound, 0, 0, this->topBound,
        this->bottomBound, this->leftBound, this->rightBound);
}

void QuadTree::cleanup()
{
    /* If the root node is a leaf, exit immediately. */
    if (this->quadNodes.at(this->rootNodeIndex).numElements != QuadNode::BRANCH_NODE)
        return;
    
    FreeList<int> toProcess;

    toProcess.at(toProcess.pushBack()) = this->rootNodeIndex;

    while (toProcess.size())
    {
        const int nodeIndex = toProcess.at(toProcess.size() - 1),
            firstChild = this->quadNodes.at(nodeIndex).firstChild;

        int emptyChildren = 0;

        toProcess.popBack();

        for (int i = 0; i < 4; i++)
        {
            const int childIndex = firstChild + i,
                numElements = this->quadNodes.at(childIndex).numElements;

            if (numElements == 0)
                emptyChildren++;
            else if (numElements == QuadNode::BRANCH_NODE)
                toProcess.at(toProcess.pushBack()) = childIndex;
        }
    
        if (emptyChildren == 4)
        {
            /* Remove all four children in reverse order so the memory vacancies can be reclaimed
             * in subsequent iterations in proper order. */
            this->quadNodes.erase(firstChild + 3);
            this->quadNodes.erase(firstChild + 2);
            this->quadNodes.erase(firstChild + 1);
            this->quadNodes.erase(firstChild);

            /* Indicate that the node is now a leaf. */
            this->quadNodes.at(nodeIndex).numElements = 0;
            this->quadNodes.at(nodeIndex).firstChild = ElementNode::NONE;
        }
    }
}

void QuadTree::getLeaves(FreeList<QuadNodeData>* output, int colliderTop, int colliderBottom, int colliderLeft, int colliderRight,
        int quadNodeIndex, int depth, int top, int bottom, int left, int right)
{
    /* Return early if the collider is not contained within the boundaries. */
    if (top <= colliderBottom ||
        bottom > colliderTop ||
        right <= colliderLeft ||
        left > colliderRight)
        return;

    FreeList<QuadNodeData> processingStack;

    pushBackNode(&processingStack, quadNodeIndex, depth, top, bottom, left, right);

    while (processingStack.size() > 0)
    {
        const QuadNodeData topData = processingStack.at(processingStack.size() - 1);
        processingStack.popBack();
        
        #ifdef ASSERTIONS
            const int divisor = pow(2, topData.depth);
            assert((topData.top - topData.bottom) == (this->topBound - this->bottomBound) / divisor);
            assert((topData.right - topData.left) == (this->rightBound - this->leftBound) / divisor);
        #endif

        /* In this case, we've found a leaf node. */
        if (this->quadNodes.at(topData.quadNodeIndex).numElements != QuadNode::BRANCH_NODE)
            output->at(output->pushBack()) = topData;
        else
        {
            const int firstChild = this->quadNodes.at(topData.quadNodeIndex).firstChild,
                halfX = topData.left + ((topData.right - topData.left) / 2),
                halfY = topData.bottom + ((topData.top - topData.bottom) / 2);

            if (colliderLeft < halfX)
            {
                /* Top left. */
                if (colliderTop >= halfY)
                    pushBackNode(&processingStack, firstChild, topData.depth + 1, topData.top, halfY, topData.left, halfX);
                /* Bottom left. */
                if (colliderBottom < halfY)
                    pushBackNode(&processingStack, firstChild + 2, topData.depth + 1, halfY, topData.bottom, topData.left, halfX);
            }
            if (colliderRight >= halfX)
            {
                /* Top right. */
                if (colliderTop >= halfY)
                    pushBackNode(&processingStack, firstChild + 1, topData.depth + 1, topData.top, halfY, halfX, topData.right);
                /* Bottom right. */
                if (colliderBottom < halfY)
                    pushBackNode(&processingStack, firstChild + 3, topData.depth + 1, halfY, topData.bottom, halfX, topData.right);
            }
        }
    }
}

void QuadTree::nodeInsert(int colliderIndex, QuadNodeData data)
{
    QuadNode& quadNode = this->quadNodes.at(data.quadNodeIndex);

    /* Create an element node and push it back in the quadnode list. */
    const int newElementIndex = this->elementNodes.insert();

    ElementNode& newElement = this->elementNodes.at(newElementIndex);

    newElement.colliderIndex = colliderIndex;
    newElement.next = quadNode.firstChild;

    quadNode.firstChild = newElementIndex;
    
    /* Subdivide the node if needed and allowed. */
    if (++quadNode.numElements > this->maxEltsPerNode && data.depth < this->maxDivisions)
        this->subdivideNode(data.quadNodeIndex, data.depth, data.top, data.bottom, data.left, data.right);
}

void QuadTree::subdivideNode(int quadNodeIndex, int depth, int top, int bottom, int left, int right)
{
    #ifdef ASSERTIONS
    assert(this->quadNodes.at(quadNodeIndex).numElements > this->maxEltsPerNode);
    #endif

    /* First, we need to retrieve all the collider indices. */
    FreeList<int> colliderIndexStack;
    int currentEltIndex = this->quadNodes.at(quadNodeIndex).firstChild, previous;

    while (currentEltIndex != ElementNode::NONE)
    {        
        previous = currentEltIndex;

        colliderIndexStack.at(colliderIndexStack.pushBack()) = this->elementNodes.at(currentEltIndex).colliderIndex;
        currentEltIndex = this->elementNodes.at(currentEltIndex).next;

        this->elementNodes.erase(previous);
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

    /* Now assign the quadnode's new values. */
    this->quadNodes.at(quadNodeIndex).numElements = QuadNode::BRANCH_NODE;
    this->quadNodes.at(quadNodeIndex).firstChild = newChild;

    FreeList<QuadNodeData> leavesForInsertion;

    const int numColliders = colliderIndexStack.size();

    for (int i = 0; i < numColliders; i++)
    {
        const QuadTreeCollider* colliderPtr = this->colliderPtrs.at(colliderIndexStack.at(i));

        this->getLeaves(&leavesForInsertion, colliderPtr->top, colliderPtr->bottom, colliderPtr->left,
            colliderPtr->right, quadNodeIndex, depth, top, bottom, left, right);
        const int numLeaves = leavesForInsertion.size();

        /* Insert the collider pointer into the leaf at the given leaf. */
        for (int j = 0; j < numLeaves; j++)
            this->nodeInsert(colliderIndexStack.at(i), leavesForInsertion.at(j));

        leavesForInsertion.clear();
    }
}
