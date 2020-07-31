#ifndef FREELIST_H_INCLUDED
#define FREELIST_H_INCLUDED

#include <cstddef>

#include "defs.h"
#include "parentcontainer.h"

template<typename TypeName, const size_t FixedSize = 128>
class FreeList : public ParentContainer
{
public:
    FreeList();
    ~FreeList();

    /* Inserts an element into the freelist, returning its index. */
    int insert();

    /* Inserts the given value into the freelist. */
    void insert(const TypeName& value);

    /* Erases the element from the specified index in the freelist. */
    void erase(const int index);

private:
    /* The index of the earliest freed element in the freelist. */
    int freeElement;

    static const int NONE_REMOVED = -1;
};

template<typename TypeName, const size_t FixedSize>
FreeList<TypeName, FixedSize>::FreeList()
    : ParentContainer<TypeName, FixedSize>()
{
}

template<typename TypeName, const size_t FixedSize>
FreeList<TypeName, FixedSize>::~FreeList()
{
    ParentContainer<TypeName, FixedSize>::~ParentContainer();
}

template<typename TypeName, const size_t FixedSize>
int FreeList<TypeName, FixedSize>::insert()
{

}

#endif