//
// Created by Lauri Kortevaara on 27/12/15.
//

#ifndef OMEN_MATHUTILS_H
#define OMEN_MATHUTILS_H

/* Constants rounded for 21 decimals. */
/*
#define M_E 2.71828182845904523536 // The base of natural logarithms
#define M_LOG2E 1.44269504088896340736  // The logarithm to base 2 of M_E
#define M_LOG10E 0.434294481903251827651 // The logarithm to base 10 of M_E.
#define M_LN2 0.693147180559945309417 // The natural logarithm of 2
#define M_LN10 2.30258509299404568402 // The natural logarithm of 10
#define M_PI 3.14159265358979323846 // Pi, the ratio of a circle's circumference to its diameter
#define M_PI_2 1.57079632679489661923 // Pi divided by two
#define M_PI_4 0.785398163397448309616 // Pi divided by four
#define M_1_PI 0.318309886183790671538 // The reciprocal of pi (1/pi) 
#define M_2_PI 0.636619772367581343076 // Two times the reciprocal of pi 2*(1/pi)
#define M_1_SQRTPI 0.564189583547756286948 // The reciprocal of the square root of pi (1/sqrt(pi))
#define M_2_SQRTPI 1.12837916709551257390 // Two times the reciprocal of the square root of pi (2*(1/sqrt(pi)))
#define M_SQRT2 1.41421356237309504880 // The square root of two (sqrt(2))
#define M_SQRT_2 0.707106781186547524401 // The reciprocal of the square root of two (also the square root of 1/2) ( sqrt(2)/2 or sqrt(1/2) )
#define M_SQRT_2PI 2.506628274631000502415 // (sqrt(2*pi))
*/
#include <math.h>

	#define M_E        2.71828182845904523536   // e
	#define M_LOG2E    1.44269504088896340736   // log2(e)
	#define M_LOG10E   0.434294481903251827651  // log10(e)
	#define M_LN2      0.693147180559945309417  // ln(2)
	#define M_LN10     2.30258509299404568402   // ln(10)
	#define M_PI       3.14159265358979323846   // pi
	#define M_PI_2     1.57079632679489661923   // pi/2
	#define M_PI_4     0.785398163397448309616  // pi/4
	#define M_1_PI     0.318309886183790671538  // 1/pi
	#define M_2_PI     0.636619772367581343076  // 2/pi
	#define M_1_SQRTPI 0.564189583547756286948  // 1/sqrt(pi)
	#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
	#define M_SQRT2    1.41421356237309504880   // sqrt(2)
	#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
	#define M_SQRT_2PI 2.506628274631000502415  // sqrt(2*pi)

inline float threshold(float comp, float threshold) { return comp < threshold ? 0 : comp; }
inline float threshold_abs(float comp, float threshold) { return fabs(comp) < threshold ? 0 : comp; }

// Define float precision
namespace omen {
#ifndef floatprec
    typedef float floatprec;
#endif
}


#endif //OMEN_MATHUTILS_H
