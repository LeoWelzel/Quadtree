#ifndef FREESTACK_H_INCLUDED
#define FREESTACK_H_INCLUDED

#include "defs.h"
#include "parentcontainer.h"

template<typename TypeName, const size_t FixedSize = 128>
class FreeStack : public ParentContainer<TypeName, FixedSize>
{
public:
    FreeStack();
    ~FreeStack();

    inline TypeName& top() const;

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

template<typename TypeName, const size_t FixedSize>
inline TypeName& FreeStack<TypeName, FixedSize>::top() const
{
    #ifdef ASSERTIONS
        assert(this->numElements > 0);
    #endif

    return this->dataPtr[this->numElements - 1];
}

template<typename TypeName, const size_t FixedSize>
void FreeStack<TypeName, FixedSize>::pushBack(const TypeName& value)
{
    const int index = ParentContainer<TypeName, FixedSize>::pushBack();

    #ifdef ASSERTIONS
        /* Over-caution with pointers. */
        assert(this->numElements <= this->capacity);
        assert(index >= 0);
        assert(index < this->capacity);
    #endif

    this->dataPtr[index] = value;
}

template<typename TypeName, const size_t FixedSize>
void FreeStack<TypeName, FixedSize>::popBack()
{
    #ifdef ASSERTIONS
        /* Ensure never unnecessarily called. */
        assert(this->numElements > 0);
    #endif

    this->numElements--;
}

#endif