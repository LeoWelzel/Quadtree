#ifndef COLLIDER_HPP_INCLUDED
#define COLLIDER_HPP_INCLUDED

#include "defs.hpp"

/* Rectangle used to indicate the boundaries of a collider. Such a structure can also be used */
/* to house any other type of polygon. */
struct QuadtreeCollider
{
    /* Default constructor needed for initialisation of freelists. */
    QuadtreeCollider();

    QuadtreeCollider(int top, int bottom, int left, int right);

    int top, bottom, left, right;

    #ifdef APPLICATION
    int xMotion, yMotion;
    #endif

    #ifdef TO_STRING
        std::string toString() const;
    #endif
};

#endif