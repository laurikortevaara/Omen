//
// Created by Lauri Kortevaara on 27/12/15.
//

#ifndef OMEN_VECTOR_H
#define OMEN_VECTOR_H

#include "typedef.h"

namespace omen::math {

    template <typename T>
    class vec2 
    {
    // data
    public:
        T x, y;
    // api
    public:

        explicit vec2(T x, T t) : x(x), y(y) {};

        vec2 operator =(const vec2& v) { this->x = v.x; this->y = v.y; return *this; }
    };
} // namespace omen::math


#endif //OMEN_VECTOR_H
