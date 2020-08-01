#ifndef FREELIST_H_INCLUDED
#define FREELIST_H_INCLUDED

#include <cstddef>
#include <cassert>

#include "defs.h"
#include "parentcontainer.h"

template<typename TypeName, const size_t FixedSize = 128>
class FreeList : public ParentContainer<TypeName, FixedSize>
{
public:
    FreeList();

    /* Inserts an element into the freelist, returning its index. */
    int insert();

    /* Inserts the given value into the freelist. */
    void insert(const TypeName& value);

    /* Erases the element from the specified index in the freelist. */
    void erase(const int index);

    /* Returns a reference to the element at the specified index. */
    /* Peforms bounds checking, but is unable to check if this index is for a removed element. */
    inline TypeName& at(const int index) const;

    /* Returns a reference to the element at the specified index, without bounds checking. */
    inline TypeName& unsafeReference(const int index) const;

private:
    /* The index of the earliest freed element in the freelist. */
    int freeElement;

    static const int NONE_REMOVED = -1;
};

template<typename TypeName, const size_t FixedSize>
FreeList<TypeName, FixedSize>::FreeList()
    : ParentContainer<TypeName, FixedSize>(), freeElement(FreeList::NONE_REMOVED)
{
}

template<typename TypeName, const size_t FixedSize>
int FreeList<TypeName, FixedSize>::insert()
{
    /* Elements have been removed. */
    if (this->freeElement != FreeList::NONE_REMOVED)
    {
        const int index = this->freeElement;
        this->freeElement = *(int*)(this->dataPtr + index);

        return index;
    }
    else return this->pushBack();
}

template<typename TypeName, const size_t FixedSize>
void FreeList<TypeName, FixedSize>::insert(const TypeName& value)
{
    const int index = this->insert();

    #ifdef ASSERTIONS
        assert(index >= 0);
        assert(index < this->capacity);
    #endif

    this->dataPtr[index] = value;
}

template<typename TypeName, const size_t FixedSize>
void FreeList<TypeName, FixedSize>::erase(int index)
{
    #ifdef ASSERTIONS
        assert(index >= 0);
        assert(index < this->capacity);
    #endif

    int* tempPtr = (int*)(this->dataPtr + index);

    /* Reconnect the linked list. */
    *tempPtr = this->freeElement;
    this->freeElement = index;
}

template<typename TypeName, const size_t FixedSize>
inline TypeName& FreeList<TypeName, FixedSize>::at(const int index) const
{
    assert(index >= 0);
    assert(index < this->capacity);
    return this->dataPtr[index];
}

template<typename TypeName, const size_t FixedSize>
inline TypeName& FreeList<TypeName, FixedSize>::unsafeReference(const int index) const
{
    return this->dataPtr[index];
}

#endif