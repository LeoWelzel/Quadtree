#ifndef FREELIST_HPP_INCLUDED
#define FREELIST_HPP_INCLUDED

#include <cassert>
#include <algorithm>

/* Simple memory allocator that can be used as a stack or freelist. */
template<typename TypeName, const int FixedSize = 64>
class FreeList
{
public:
    FreeList();
    FreeList(const FreeList& other);
    void operator=(const FreeList& other);
    ~FreeList();

    /* Returns the size of the freelist. */
    int size() const;

    /* Returns the number of elements in the freelist. Do not use to iterate over list. */
    int getNumElements() const;

    /* Returns the capactiy of the freelist. */
    int getCapacity() const;

    /* Clears the freelist. */
    void clear();

    /* Returns a reference to the element at the given index. */
    /* Performs bounds checking. */
    inline TypeName& at(const int index) const;

    /* Returns a reference to the element at the given index. */
    /* Does not perform bounds checking. */
    inline TypeName& unsafeRef(const int index) const;

    /* Returns a pointer to the element at the given index. */
    /* Performs bounds checking. */
    inline TypeName* safePtr(const int index);

    /* Returns a pointer to the element at the given index. */
    /* Does not perform bounds checking. */
    inline TypeName* unsafePtr(const int index);

    /* ------------------------------------------------------------------------------- */
    /* Stack interface. This enables the FreeList to be used as a stack.               */
    /* ------------------------------------------------------------------------------- */
    /* Inserts an element at the back of the list and returns its index. */
    int pushBack();
    
    /* Removes the element from the back of the list. */
    void popBack();


    /* -------------------------------------------------------------------------------- */
    /* Freelist interface. Enables the list to be used as a memory allocator.           */
    /* -------------------------------------------------------------------------------- */
    /* Inserts an element into a vacant position in the freelist and returns its index. */
    int insert();

    /* Erases the element from the specified index in the intlist. */
    void erase(int index);

private:
    /* Fixed size array to be accessed on the stack. */
    TypeName fixed[FixedSize];

    /* Points to the data. */
    TypeName* data;

    /* The capacity of the freelist. */
    int capacity;

    /* The furthest back any element has been pushed in the freelist. */
    int listSize;

    /* The number of elements in the freelist. */
    int numElements;

    /* Stores the earliest free element in the freelist. */
    int freeElement;
    
    static const int NONE_REMOVED = -1;
};

#include "freelist.inl"

#endif