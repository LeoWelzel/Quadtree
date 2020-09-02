#ifndef FREELIST_INL_INCLUDED
#define FREELIST_INL_INCLUDED

template<typename TypeName, const int FixedSize>
FreeList<TypeName, FixedSize>::FreeList()
    : data(fixed), capacity(FixedSize), numElements(0), listSize(0), freeElement(NONE_REMOVED)
{
    #ifdef ASSERTIONS
        assert(sizeof(TypeName) >= sizeof(int));
    #endif
}

template<typename TypeName, const int FixedSize>
FreeList<TypeName, FixedSize>::FreeList(const FreeList& other)
    : capacity(other.capacity), listSize(other.listSize), numElements(other.numElements),
        freeElement(other.freeElement)
{
    if (other.data == other.fixed)
        this->data = this->fixed;
    else
        this->data = new TypeName[this->capacity];
    std::copy(other.data, other.data + other.capacity, this->data);
}

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::operator=(const FreeList& other)
{
    this->capacity = other.capacity;
    this->listSize = other.listSize;
    this->numElements = other.numElements;
    this->freeElement = other.freeElement;

    if (this->data != this->fixed) delete[] this->data;

    if (other.data == other.fixed)
        this->data = this->fixed;
    else
        this->data = new TypeName[this->capacity];
    std::copy(other.data, other.data + other.capacity, this->data);
}

template<typename TypeName, const int FixedSize>
FreeList<TypeName, FixedSize>::~FreeList()
{
    if (this->data != this->fixed)
        delete[] this->data;
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::size() const
{
    return this->listSize;
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::getNumElements() const
{
    return this->numElements;
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::getCapacity() const
{
    return this->capacity;
}

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::clear()
{
    this->listSize = 0;
    this->freeElement = NONE_REMOVED;
    this->numElements = 0;
}

template<typename TypeName, const int FixedSize>
TypeName& FreeList<TypeName, FixedSize>::at(const int index) const
{
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
    const int newPosition = (this->listSize + 1);

    /* Reallocate if the list is full. */
    if (newPosition >= this->capacity)
    {
        const int newCapacity = this->capacity * 2;
        TypeName* temp = new TypeName[newCapacity];
        std::copy(this->data, this->data + this->listSize, temp);

        if (this->data != this->fixed)
            delete[] this->data;
        
        this->capacity = newCapacity;
        this->data = temp;
    }

    this->numElements++;
    return this->listSize++;
}

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::popBack()
{
    this->listSize--;
    this->numElements--;
}

template<typename TypeName, const int FixedSize>
int FreeList<TypeName, FixedSize>::insert()
{
    /* This means that elements have been removed before. */
    if (this->freeElement != NONE_REMOVED)
    {
        const int index = this->freeElement;
        this->freeElement = *(int*)(this->data + index);
        this->numElements++;

        return index;
    }
    else return this->pushBack();
}

template<typename TypeName, const int FixedSize>
void FreeList<TypeName, FixedSize>::erase(int index)
{
    *(int*)(this->data + index) = this->freeElement;
    this->freeElement = index;

    this->numElements--;
}

#endif