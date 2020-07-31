#ifndef FREELIST_H_INCLUDED
#define FREELIST_H_INCLUDED

#include <cstddef>

/* Simple memory allocator. Also supports usage as a stack, though this must not be mixed with
 * freelist usage. */
template<typename TypeName, const size_t FixedSize = 128>
class FreeList
{
public:
    FreeList();

private:
    TypeName fixed[FixedSize];
};

#endif