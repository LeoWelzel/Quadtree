#ifndef FREELIST_H_INCLUDED
#define FREELIST_H_INCLUDED

#include <cstddef>

#include "defs.h"

/* Simple memory allocator. Also supports usage as a stack, though this must not be mixed with
 * freelist usage. */
template<typename TypeName, const size_t FixedSize = 128>
class FreeList
{
public:
    FreeList();
    ~FreeList();

    /* Returns the number of elements stored in the freelist. */
    int size() const;

    /* Clears the freelist. */
    void clear();

    /* Returns a reference to the element at the given index. */
    inline TypeName& at(const int index) const;

private:
    /* Allocate memory on the stack to avoid heap allocation until necessary. */
    TypeName fixed[FixedSize];

    /* Points to the data being used in the freelist. */
    TypeName* dataPtr;

    /* Indicates the freelist capacity. */
    int capacity;

    /* The number of elements stored. */
    int numElements;

    /* The index of the earliest free element in the freelist. */
    int freeElement;

    /* The default value of freeElement, if no element has been removed from the "middle". */
    static const int NONE_REMOVED = -1;
};

template<typename TypeName, const size_t FixedSize>
FreeList<TypeName, FixedSize>::FreeList()
    : dataPtr(fixed), capacity(FixedSize), numElements(0), freeElement(FreeList::NONE_REMOVED)
{
}

template<typename TypeName, const size_t FixedSize>
FreeList<TypeName, FixedSize>::~FreeList()
{
    /* Free dynamically allocated memory. */
    if (this->dataPtr != this->fixed)
        delete[] this->dataPtr;
}

template<typename TypeName, const size_t FixedSize>
int FreeList<TypeName, FixedSize>::size() const
{
    return this->numElements;
}

template<typename TypeName, const size_t FixedSize>
void FreeList<TypeName, FixedSize>::clear()
{
    this->numElements = 0;
    this->freeElement = FreeList::NONE_REMOVED;
}

template<typename TypeName, const size_t FixedSize>
inline TypeName& FreeList<TypeName, FixedSize>::at(const int index) const
{
    /* Leave this in until it has been ascertained that this never goes wrong. */
    #ifdef ASSERTIONS
        assert(index >= 0 && index < this->capacity);
    #endif

    return this->dataPtr[index];
}

#endif