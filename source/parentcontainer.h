#ifndef PARENT_CONTAINER_H_INCLUDED
#define PARENT_CONTAINER_H_INCLUDED

#include <cstddef>

#include "defs.h"

template<typename TypeName, const size_t FixedSize = 128>
class ParentContainer
{
public:
    ParentContainer();
    ~ParentContainer();

    /* Returns the number of elements stored in the container. */
    int size() const;

    #ifdef DEBUGGING
        int getCapacity() const
        {
            return this->capacity;
        }

        #ifdef TO_STRING
        std::string toString() const
        {
            std::string output = "{ ";

            for (size_t i = 0; i < this->numElements - 1; i++)
                output.append(std::to_string(this->dataPtr[i]) + ", ");
            
            output.append(std::to_string(this->dataPtr[this->numElements - 1]) + " }");
            
            return output;
        }
        #endif
    #endif

protected:
    /* Reallocates the memory. */
    void reallocate();

    /* Allocate memory on the stack to avoid heap allocation until necessary. */
    TypeName fixed[FixedSize];

    /* Points to the data being used in the freelist. */
    TypeName* dataPtr,
        *tempDebugging;

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

template<typename TypeName, const size_t FixedSize>
void ParentContainer<TypeName, FixedSize>::reallocate()
{
    /* Arbitrarily chose to double in size. */
    this->capacity *= 2;

    TypeName* temp = new TypeName[this->capacity];
    tempDebugging = temp;

    /* Populate with existing contents. */
    std::copy(this->dataPtr, this->dataPtr + this->capacity / 2, temp);

    if (this->dataPtr != this->fixed)
        delete[] this->dataPtr;

    this->dataPtr = temp;
}

#endif