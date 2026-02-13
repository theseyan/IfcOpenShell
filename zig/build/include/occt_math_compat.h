#ifndef IFCOPENSHELL_OCCT_MATH_COMPAT_H
#define IFCOPENSHELL_OCCT_MATH_COMPAT_H

// OCCT uses M_PI and M_PI_2 in many headers. These macros are not guaranteed
// to be provided by every libc/cmath combination in cross builds.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#endif
