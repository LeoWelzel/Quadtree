#include "quadtreecollider.hpp"

QuadTreeCollider::QuadTreeCollider()
    : top(0), bottom(0), left(0), right(0), xMotion(0), yMotion(0)
{
}

QuadTreeCollider::QuadTreeCollider(int top, int bottom, int left, int right, int xMotion, int yMotion)
    : top(top), bottom(bottom), left(left), right(right), xMotion(xMotion), yMotion(yMotion)
{
}

const bool QuadTreeCollider::operator ==(const QuadTreeCollider& other) const
{
    return this->top == other.top &&
           this->bottom == other.bottom &&
           this->left == other.left &&
           this->right == other.right &&
           this->xMotion == other.xMotion &&
           this->yMotion == other.yMotion;
}

const bool QuadTreeCollider::operator !=(const QuadTreeCollider& other) const
{
    return !(*this == other);
}


#ifdef TO_STRING
std::string QuadTreeCollider::toString() const
{
    return "(" + std::to_string(this->top) + ", " + std::to_string(this->bottom) + ", " +
        std::to_string(this->left) + ", " + std::to_string(this->right) + ")";
}
#endif
