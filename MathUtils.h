//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#ifndef OMEN_MATHUTILS_H
#define OMEN_MATHUTILS_H

inline float threshold(float comp, float threshold) { return comp < threshold ? 0 : comp; }
inline float threshold_abs(float comp, float threshold) { return fabs(comp) < threshold ? 0 : comp; }


#endif //OMEN_MATHUTILS_H
