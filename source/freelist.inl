#ifndef FREELIST_INL_INCLUDED
#define FREELIST_INL_INCLUDED

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
    #ifdef ASSERTIONS
        assert(index >= 0 && index < this->capacity);
    #endif
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
        this->freeElement = *(int*)(this->data + index);

        return index;
    }
    else return this->pushBack();
}

template<typename TypeName>
void FreeList<TypeName>::erase(int index)
{
    assert(index >= 0 && index < this->capacity);

    int* tempPtr = (int*)(this->data + index);
    *tempPtr = this->freeElement;
    this->freeElement = index;
}

#endif