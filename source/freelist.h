#ifndef FREELIST_H_INCLUDED
#define FREELIST_H_INCLUDED

#include <cstddef>

/* Simple memory allocator. Also supports usage as a stack, though this must not be mixed with
 * freelist usage. */
template<typename TypeName, const size_t FixedSize = 128>
class FreeList
{
public:
    FreeList();
    ~FreeList();

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
};

#endif