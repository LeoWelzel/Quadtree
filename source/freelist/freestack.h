#ifndef FREESTACK_H_INCLUDED
#define FREESTACK_H_INCLUDED

#include "../defs.h"
#include "parentcontainer.h"

template<typename TypeName, const size_t FixedSize = 128>
class FreeStack : public ParentContainer<TypeName, FixedSize>
{
public:
    FreeStack();
    ~FreeStack();

    inline TypeName& top() const;

    /* Pushes an element to the top of the stack. */
    void pushBack();

    /* Inserts the given element to the top of the stack. */
    void pushBack(const TypeName& value);

    /* Removes the element from the top of the stack. */
    void popBack();
};

template<typename TypeName, const size_t FixedSize>
FreeStack<TypeName, FixedSize>::FreeStack()
    : ParentContainer<TypeName, FixedSize>()
{
}

template<typename TypeName, const size_t FixedSize>
FreeStack<TypeName, FixedSize>::~FreeStack()
{
    ParentContainer<TypeName, FixedSize>::~ParentContainer();
}



#endif