//
// Created by Lauri Kortevaara(personal) on 29/12/15.
//

#ifndef OMEN_UTILS_H_H
#define OMEN_UTILS_H_H

#endif //OMEN_UTILS_H_H

#include <cstdlib>

namespace Omen {
    inline int random (int low, int high) {
        if (low > high) return high;
        return low + (std::rand() % (high - low + 1));
    }
}