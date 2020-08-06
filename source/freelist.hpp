#ifndef FREELIST_HPP_INCLUDED
#define FREELIST_HPP_INCLUDED

#include <cassert>
#include <algorithm>

const int FREELIST_FIXED_SIZE = 64;

/* Simple memory allocator that can be used as a stack or freelist. */
template<typename TypeName>
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
    TypeName fixed[FREELIST_FIXED_SIZE];

    /* Points to the data. */
    TypeName* data;

    /* The capacity of the freelist. */
    int capacity;

    /* The number of elements in the freelist. */
    int numElements;

    /* Stores the earliest free element in the freelist. */
    int freeElement;

    static const int NONE_REMOVED = -1;
    
};

#include "freelist.hpp"

template<typename TypeName>
FreeList<TypeName>::FreeList()
    : data(fixed), capacity(FREELIST_FIXED_SIZE), numElements(0), freeElement(NONE_REMOVED)
{
    #ifdef ASSERTIONS
        assert(sizeof(TypeName) >= sizeof(int));
    #endif
}

template<typename TypeName>
FreeList<TypeName>::~FreeList()
{
    if (this->data != this->fixed)
    {
        #ifdef ASSERTIONS
            assert(this->capacity != FREELIST_FIXED_SIZE);
        #endif

        delete[] this->data;
    }
}

template<typename TypeName>
int FreeList<TypeName>::size() const
{
    return this->numElements;
}

template<typename TypeName>
void FreeList<TypeName>::clear()
{
    this->numElements = 0;
    this->freeElement = NONE_REMOVED;
}

template<typename TypeName>
TypeName& FreeList<TypeName>::at(const int index) const
{
    // assert(index >= 0 && index < this->capacity);
    return this->data[index];
}

template<typename TypeName>
TypeName& FreeList<TypeName>::unsafeRef(const int index) const
{
    return this->data[index];
}

template<typename TypeName>
inline TypeName* FreeList<TypeName>::safePtr(const int index)
{
    assert(index >= 0 && index < this->capacity);
    return this->data + index;
}

template<typename TypeName>
inline TypeName* FreeList<TypeName>::unsafePtr(const int index)
{
    return this->data + index;
}



template<typename TypeName>
int FreeList<TypeName>::pushBack()
{
    const int newPosition = (this->numElements + 1);

    /* Reallocate if the list is full. */
    if (newPosition >= this->capacity)
    {
        this->capacity <<= 1;
        TypeName* temp = new TypeName[this->capacity];

        // memcpy(temp, this->data, sizeof(TypeName) * this->numElements);
        std::copy(this->data, this->data + this->numElements, temp);

        if (this->data != this->fixed)
            delete[] this->data;
        
        this->data = temp;
    }

    return this->numElements++;
}

template<typename TypeName>
void FreeList<TypeName>::popBack()
{
    #ifdef ASSERTIONS
    assert(this->numElements > 0);
    #endif

    this->numElements--;
}

template<typename TypeName>
int FreeList<TypeName>::insert()
{
    /* This means that elements have been inserted before. */
    if (this->freeElement != NONE_REMOVED)
    {
        const int index = this->freeElement;
        /* Pure evil. */
        this->freeElement = *(int*)(this->data + index);

        return index;
    }
    else return this->pushBack();
}

template<typename TypeName>
void FreeList<TypeName>::erase(int index)
{
    assert(index >= 0 && index < this->capacity);

    /* Evil and ugly. */
    int* tempPtr = (int*)(this->data + index);
    *tempPtr = this->freeElement;
    this->freeElement = index;
}

#endif