#include "quadtreecollider.hpp"

QuadtreeCollider::QuadtreeCollider()
    : top(0), bottom(0), left(0), right(0)
{
}

QuadtreeCollider::QuadtreeCollider(int top, int bottom, int left, int right)
    : top(top), bottom(bottom), left(left), right(right)
{
}

#ifdef TO_STRING
std::string QuadtreeCollider::toString() const
{
    return "[" + std::to_string(this->top) + ", " + std::to_string(this->bottom)
        + ", " + std::to_string(this->left) + ", " + std::to_string(this->right) + "]";
}
#endif
