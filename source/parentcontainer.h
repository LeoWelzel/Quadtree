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

    /* Returns a reference to the element at the specified index. */
    /* Peforms bounds checking, but is unable to check if this index is for a removed element. */
    inline TypeName& at(const int index) const;

    /* Returns a reference to the element at the specified index, without bounds checking. */
    inline TypeName& unsafeReference(const int index) const;

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
    /* Makes space for an element at the back of the array, returning its index. */
    int pushBack();

    /* Allocate memory on the stack to avoid heap allocation until necessary. */
    TypeName fixed[FixedSize];

    /* Points to the data being used in the freelist. */
    TypeName* dataPtr;

    /* Indicates the freelist capacity. */
    int capacity;

    /* The number of elements stored. */
    int numElements;

// private:
    /* Reallocates the memory. */
    void reallocate();
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
    // std::cout << "Base destructor entered.\n";
    if (this->dataPtr != this->fixed)
        delete[] this->dataPtr;

    // std::cout << "Base destructor exited.\n";
}

template<typename TypeName, const size_t FixedSize>
int ParentContainer<TypeName, FixedSize>::size() const
{
    return this->numElements;
}

template<typename TypeName, const size_t FixedSize>
int ParentContainer<TypeName, FixedSize>::pushBack()
{
    const int newPosition = this->numElements++;

    if (newPosition >= this->capacity)
        this->reallocate();
    
    return newPosition;
}

template<typename TypeName, const size_t FixedSize>
void ParentContainer<TypeName, FixedSize>::reallocate()
{
    /* Arbitrarily chose to double in size. */
    const int formerCapacity = this->capacity;
    this->capacity *= 2;

    TypeName* temp = new TypeName[this->capacity];

    /* Populate with existing contents. */
    std::copy(this->dataPtr, this->dataPtr + formerCapacity, temp);

    if (this->dataPtr != this->fixed)
        delete[] this->dataPtr;

    this->dataPtr = temp;
}

template<typename TypeName, const size_t FixedSize>
inline TypeName& ParentContainer<TypeName, FixedSize>::at(const int index) const
{
    assert(index >= 0);
    assert(index < this->capacity);
    return this->dataPtr[index];
}

template<typename TypeName, const size_t FixedSize>
inline TypeName& ParentContainer<TypeName, FixedSize>::unsafeReference(const int index) const
{
    return this->dataPtr[index];
}

#endif