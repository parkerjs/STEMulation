#ifndef TRIGONOMETRY_H
#define TRIGONOMETRY_H

#include "angle_unit_type.h"
#include "math_constants.h"
#include <cmath>

namespace math
{

namespace trigonometric
{

/**
 * Function wrapper for the std::acos() function - restricts arguments to the interval [-1, 1]
 */
template<typename T> inline T acos(T x)
{
    if (x > (T)1)
        x = (T)1;
    else if (x < (T)-1)
        x = (T)-1;

    return std::acos(x);
}

/**
 * Function wrapper for the std::asin() function - restricts arguments to the interval [-1, 1]
 */
template<typename T> inline T asin(T x)
{
    if (x > (T)1)
        x = (T)1;
    else if (x < (T)-1)
        x = (T)-1;

    return std::asin(x);
}

/**
 * Function wrapper for the std::cos() function that allows the user to specify angle units in degrees/radians.
 */
template<typename T> inline T cos(T x, const AngleUnitType &angleUnits = AngleUnitType::Radians)
{
    if (angleUnits != AngleUnitType::Radians)
        x *= DEGREES_TO_RADIANS;

    return std::cos(x);
}

/**
 * Function wrapper for the std::sin() function that allows the user to specify angle units in degrees/radians.
 */
template<typename T> inline T sin(T x, const AngleUnitType &angleUnits = AngleUnitType::Radians)
{
    if (angleUnits != AngleUnitType::Radians)
        x *= DEGREES_TO_RADIANS;

    return std::sin(x);
}

}

}

#endif
