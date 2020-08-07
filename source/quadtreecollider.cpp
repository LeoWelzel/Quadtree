#include "quadtreecollider.hpp"

QuadTreeCollider::QuadTreeCollider()
    : top(0), bottom(0), left(0), right(0)
{
}

QuadTreeCollider::QuadTreeCollider(int top, int bottom, int left, int right)
    : top(top), bottom(bottom), left(left), right(right)
{
}

const bool QuadTreeCollider::operator ==(const QuadTreeCollider& other) const
{
    return this->top == other.top &&
           this->bottom == other.bottom &&
           this->left == other.left &&
           this->right == other.right;
}

const bool QuadTreeCollider::operator !=(const QuadTreeCollider& other) const
{
    return !(*this == other);
}
