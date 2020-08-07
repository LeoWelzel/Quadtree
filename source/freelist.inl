#ifndef FREELIST_INL_INCLUDED
#define FREELIST_INL_INCLUDED

template<typename TypeName, const int FixedSize>
FreeList<TypeName, FixedSize>::FreeList()
    : data(fixed), capacity(FixedSize), numElements(0), freeElement(NONE_REMOVED)
{
    #ifdef ASSERTIONS
        assert(sizeof(TypeName) >= sizeof(int));
    #endif
}

template<typename TypeName, const int FixedSize>
FreeList<TypeName, FixedSize>::~FreeList()
{
    if (this->data != this->fixed)
    {
        #ifdef ASSERTIONS
            assert(this->capacity != FixedSize);
        #endif

        delete[] this->data;
    }
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::size() const
{
    return this->numElements;
}

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::clear()
{
    this->numElements = 0;
    this->freeElement = NONE_REMOVED;
}

template<typename TypeName, const int FixedSize>
TypeName& FreeList<TypeName, FixedSize>::at(const int index) const
{
    #ifdef ASSERTIONS
        assert(index >= 0 && index < this->capacity);
    #endif
    return this->data[index];
}

template<typename TypeName, const int FixedSize>
TypeName& FreeList<TypeName, FixedSize>::unsafeRef(const int index) const
{
    return this->data[index];
}

template<typename TypeName, const int FixedSize>
inline TypeName* FreeList<TypeName, FixedSize>::safePtr(const int index)
{
    assert(index >= 0 && index < this->capacity);
    return this->data + index;
}

template<typename TypeName, const int FixedSize>
inline TypeName* FreeList<TypeName, FixedSize>::unsafePtr(const int index)
{
    return this->data + index;
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::pushBack()
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

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::popBack()
{
    #ifdef ASSERTIONS
        assert(this->numElements > 0);
    #endif

    this->numElements--;
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::insert()
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

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::erase(int index)
{
    assert(index >= 0 && index < this->capacity);

    int* tempPtr = (int*)(this->data + index);
    *tempPtr = this->freeElement;
    this->freeElement = index;
}

#endif