#ifndef PARENT_CONTAINER_H_INCLUDED
#define PARENT_CONTAINER_H_INCLUDED

#include <cstddef>

#include "../defs.h"

template<typename TypeName, const size_t FixedSize = 128>
class ParentContainer
{
public:
    ParentContainer();
    ~ParentContainer();

    /* Returns the number of elements stored in the container. */
    int size() const;

protected:
    /* Allocate memory on the stack to avoid heap allocation until necessary. */
    TypeName fixed[FixedSize];

    /* Points to the data being used in the freelist. */
    TypeName* dataPtr;

    /* Indicates the freelist capacity. */
    int capacity;

    /* The number of elements stored. */
    int numElements;
};

template<typename TypeName, const size_t FixedSize>
ParentContainer<TypeName, FixedSize>::ParentContainer()
    : dataPtr(fixed), capacity(FixedSize), numElements(0)
{
}

template<typename TypeName, const size_t FixedSize>
ParentContainer<TypeName, FixedSize>::~ParentContainer()
{
    /* Free dynamically allocated memory. */
    if (this->dataPtr != this->fixed)
        delete[] this->dataPtr;
}

template<typename TypeName, const size_t FixedSize>
int ParentContainer<TypeName, FixedSize>::size() const
{
    return this->numElements;
}

#endif