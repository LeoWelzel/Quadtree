#ifndef QUAD_COLLIDER_HPP_INCLUDED
#define QUAD_COLLIDER_HPP_INCLUDED

struct QuadTreeCollider
{
    QuadTreeCollider();
    QuadTreeCollider(int top, int bottom, int left, int right, int xMotion, int yMotion);

    const bool operator ==(const QuadTreeCollider& other) const;
    const bool operator !=(const QuadTreeCollider& other) const;

    int top, bottom, left, right, xMotion, yMotion;
};

#endif