#include "coordinateSystem.h"
#include "eulers.h"
#include "math_constants.h"
#include "matrix3x3.h"
#include "position_axis_type.h"
#include "spherical_acceleration_axis_type.h"
#include "spherical_position_axis_type.h"
#include "spherical_velocity_axis_type.h"
#include "trigonometry.h"
#include "vector3d.h"

// using namespace declarations
using namespace math;
using namespace math::geometric::orientation;
using namespace math::linear_algebra::vector;
using namespace math::trigonometric;

namespace physics
{

namespace kinematics
{

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates (angles assumed to be in
 * degrees)
 * @param position   the Cartesian input vector
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d cartesianToSpherical(const Vector3d &position,
                              tAngleConversionFunction pConverter)
{
    return cartesianToSpherical(position, AngleUnitType::Degrees, pConverter);
}

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates
 * @param position   the Cartesian input vector
 * @param angleUnits the desired angle output units, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d cartesianToSpherical(const Vector3d &position,
                              const AngleUnitType &angleUnits,
                              tAngleConversionFunction pConverter)
{
    auto x = position[PositionAxisType::X];
    auto y = position[PositionAxisType::Y];
    auto z = position[PositionAxisType::Z];

    return cartesianToSpherical(x, y, z, angleUnits, pConverter);
}

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates (angles assumed to be in
 * degrees)
 * @param x, y, z    define the input Cartesian coordinates
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d cartesianToSpherical(double x,
                              double y,
                              double z,
                              tAngleConversionFunction pConverter)
{
    return cartesianToSpherical(x, y, z, AngleUnitType::Degrees, pConverter);
}

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates
 * @param x, y, z    define the input Cartesian coordinates
 * @param angleUnits the desired angle output units, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d cartesianToSpherical(double x,
                              double y,
                              double z,
                              const AngleUnitType &angleUnits,
                              tAngleConversionFunction pConverter)
{
    auto radial = std::sqrt(x * x + y * y  + z * z);
    auto horizontal = std::atan2(y, x);
    auto vertical = trigonometric::acos(z / radial);

    if (angleUnits == AngleUnitType::Degrees)
    {
        horizontal /= DEGREES_TO_RADIANS;
        vertical /= DEGREES_TO_RADIANS;
    }

    Vector3d position(horizontal, vertical, radial);
    if (pConverter != nullptr)
        (*pConverter)(position, angleUnits);

    return position;
}

/**
 * Convert spherical elevation acceleration (measured from x-y plane to +z) to vertical angle acceleration with
 * respect to zenith (measured from +z)
 */
double elevationAccelerationToZenithAcceleration(double elevationAcceleration)
{
    return -elevationAcceleration;
}

/**
 * Convert spherical acceleration described with respect to elevation (measured from x-y plane to +z) to
 * vertical angle acceleration with respect to zenith (measured from +z)
 */
void elevationAccelerationToZenithAcceleration(Vector3d &acceleration)
{
    auto elevationAcceleration = acceleration.get(SphericalAccelerationAxisType::Vertical);

    auto zenithAcceleration = elevationAccelerationToZenithAcceleration(elevationAcceleration);

    acceleration[SphericalAccelerationAxisType::Vertical] = zenithAcceleration;
}

/**
 * Convert spherical elevation rate (measured from x-y plane to +z) to vertical angle rate with respect to
 * zenith (measured from +z)
 */
double elevationRateToZenithRate(double elevationRate)
{
    return -elevationRate;
}

/**
 * Convert spherical velocity described with respect to elevation (measured from x-y plane to +z) to vertical
 * angle velocity with respect to zenith (measured from +z)
 */
void elevationRateToZenithRate(Vector3d &velocity)
{
    auto elevationRate = velocity.get(SphericalVelocityAxisType::Vertical);

    auto zenithRate = elevationRateToZenithRate(elevationRate);

    velocity[SphericalVelocityAxisType::Vertical] = zenithRate;
}

/**
 * Convert spherical elevation (measured from x-y plane to +z) to vertical angle with respect to zenith
 * (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
double elevationToZenith(double elevation,
                         const AngleUnitType &angleUnits)
{
    auto angleShift = PI_OVER_TWO;
    if (angleUnits == AngleUnitType::Degrees)
        angleShift *= RADIANS_TO_DEGREES;

    return angleShift - elevation;
}

/**
 * Convert spherical position described with respect to elevation (measured from x-y plane to +z) to vertical
 * angle with respect to zenith (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
void elevationToZenith(Vector3d &position,
                       const AngleUnitType &angleUnits)
{
    auto elevation = position.get(SphericalPositionAxisType::Vertical);

    auto zenith = elevationToZenith(elevation, angleUnits);

    position[SphericalPositionAxisType::Vertical] = zenith;
}

/**
 * Function to convert Euler angles (yaw, pitch) to Cartesian direction vector
 * @param eulers the vector of Euler angles
 */
Vector3d eulersToCartesian(const Eulers &eulers)
{
    Vector3d direction(1.0, 0.0, 0.0);

    auto &&rotation = eulers.calcRotationMatrix(RotationType::Active);

    return rotation * direction;
}

/**
 * Function to convert yaw and pitch Euler angles to Cartesian direction vector
 * @param yaw, pitch define two of the Euler angle degrees of freedom
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
Vector3d eulersToCartesian(double yaw,
                           double pitch,
                           const AngleUnitType &angleUnits)
{
    Vector3d direction(1.0, 0.0, 0.0), yAxis(0.0, 1.0, 0.0), zAxis(0.0, 0.0, 1.0);
    direction.rotate(yaw, zAxis, angleUnits);
    direction.rotate(pitch, yAxis, angleUnits);

    return direction;
}

/**
 * Function to convert yaw and pitch Euler angles to spherical direction vector (horizontal, vertical, radial).
 * Angles are assumed to be in degrees.
 * @param yaw, pitch define two of the Euler angle degrees of freedom
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d eulersToSpherical(double yaw,
                           double pitch,
                           tAngleConversionFunction pConverter)
{
    return eulersToSpherical(yaw, pitch, AngleUnitType::Degrees, pConverter);
}

/**
 * Function to convert yaw and pitch Euler angles to spherical direction vector (horizontal, vertical, radial).
 * @param yaw, pitch define two of the Euler angle degrees of freedom
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d eulersToSpherical(double yaw,
                           double pitch,
                           const AngleUnitType &angleUnits,
                           tAngleConversionFunction pConverter)
{
    auto &&direction = eulersToCartesian(yaw, pitch, angleUnits);

    return cartesianToSpherical(direction, angleUnits, pConverter);
}

/**
 * Function to convert Euler angles (yaw, pitch) to spherical direction vector (horizontal, vertical, radial).
 * The roll degree of freedom is ignored.
 * @param eulers     the vector of Euler angles
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d eulersToSpherical(const Eulers &eulers,
                           tAngleConversionFunction pConverter)
{
    auto yaw = eulers.getYaw();
    auto pitch = eulers.getPitch();

    return eulersToSpherical(yaw, pitch, eulers.getAngleUnits(), pConverter);
}

/**
 * Convert spherical negative elevation acceleration (measured from x-y plane to +z) to vertical angle
 * acceleration with respect to zenith (measured from +z)
 */
double negativeElevationAccelerationToZenithAcceleration(double negativeElevationAcceleration)
{
    return -negativeElevationAcceleration;
}

/**
 * Convert spherical acceleration described with respect to negative elevation (measured from x-y plane to +z)
 * to vertical angle acceleration with respect to zenith (measured from +z)
 */
void negativeElevationAccelerationToZenithAcceleration(Vector3d &acceleration)
{
    auto negativeElevationAcceleration = acceleration.get(SphericalAccelerationAxisType::Vertical);

    auto zenithAcceleration = elevationAccelerationToZenithAcceleration(negativeElevationAcceleration);

    acceleration[SphericalAccelerationAxisType::Vertical] = zenithAcceleration;
}

/**
 * Convert spherical negative elevation rate (measured from x-y plane to +z) to vertical angle rate with
 * respect to zenith (measured from +z)
 */
double negativeElevationRateToZenithRate(double negativeElevationRate)
{
    return -negativeElevationRate;
}

/**
 * Convert spherical velocity described with respect to negative elevation (measured from x-y plane to +z) to
 * vertical angle acceleration with respect to zenith (measured from +z)
 */
void negativeElevationRateToZenithRate(Vector3d &velocity)
{
    auto negativeElevationRate = velocity.get(SphericalVelocityAxisType::Vertical);

    auto zenithRate = negativeElevationRateToZenithRate(negativeElevationRate);

    velocity[SphericalVelocityAxisType::Vertical] = zenithRate;
}

/**
 * Convert spherical negative elevation (measured from x-y plane to -z) to vertical angle described with
 * respect to zenith (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
double negativeElevationToZenith(double negativeElevation,
                                 const AngleUnitType &angleUnits)
{
    auto angleShift = PI_OVER_TWO;
    if (angleUnits == AngleUnitType::Degrees)
        angleShift *= RADIANS_TO_DEGREES;

    return angleShift + negativeElevation;
}

/**
 * Convert spherical position described with respect to negative elevation (measured from x-y plane to -z) to
 * vertical angle described with respect to zenith (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
void negativeElevationToZenith(Vector3d &position,
                               const AngleUnitType &angleUnits)
{
    auto negativeElevation = position.get(SphericalPositionAxisType::Vertical);

    auto zenith = negativeElevationToZenith(negativeElevation, angleUnits);

    position[SphericalPositionAxisType::Vertical] = zenith;
}

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates (angles assumed to be in
 * degrees)
 * @param position   the spherical input vector
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d sphericalToCartesian(const Vector3d &position,
                              tAngleConversionFunction pConverter)
{
    return sphericalToCartesian(position, AngleUnitType::Degrees, pConverter);
}

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates
 * @param position   the spherical input vector
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Vector3d sphericalToCartesian(const Vector3d &position,
                              const AngleUnitType &angleUnits,
                              tAngleConversionFunction pConverter)
{
    auto horizontal = position[SphericalPositionAxisType::Horizontal];
    auto vertical = position[SphericalPositionAxisType::Vertical];
    auto radial = position[SphericalPositionAxisType::Radial];

    return sphericalToCartesian(horizontal, vertical, radial, angleUnits, pConverter);
}

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates (angles assumed to be in
 * degrees)
 * @param horizontal, vertical, radial the input spherical coordinates
 * @param pConverter                   a pointer to a function that defines how to convert the resultant
 *                                     azimuth (measured counter-clockwise from +x), zenith (measured from +z
 *                                     to -z) to user-defined horizontal, vertical angles
 */
Vector3d sphericalToCartesian(double horizontal,
                              double vertical,
                              double radial,
                              tAngleConversionFunction pConverter)
{
    return sphericalToCartesian(horizontal, vertical, radial, AngleUnitType::Degrees, pConverter);
}

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates
 * @param horizontal, vertical, radial the input spherical coordinates
 * @param angleUnits                   the units of the input rotational quantities, degrees or Radians
 * @param pConverter                   a pointer to a function that defines how to convert the resultant
 *                                     azimuth (measured counter-clockwise from +x), zenith (measured from +z
 *                                     to -z) to user-defined horizontal, vertical angles
 */
Vector3d sphericalToCartesian(double horizontal,
                              double vertical,
                              double radial,
                              const AngleUnitType &angleUnits,
                              tAngleConversionFunction pConverter)
{
    Vector3d position;
    if (pConverter != nullptr)
    {
        position.set(horizontal, vertical, radial);
        (*pConverter)(position, angleUnits);
        position.get(horizontal, vertical, radial);
    }

    auto cosAz = trigonometric::cos(horizontal, angleUnits);
    auto sinAz = trigonometric::sin(horizontal, angleUnits);
    auto cosZe = trigonometric::cos(vertical, angleUnits);
    auto sinZe = trigonometric::sin(vertical, angleUnits);

    auto rSinZe = radial * sinZe;
    position[PositionAxisType::X] = rSinZe * cosAz;
    position[PositionAxisType::Y] = rSinZe * sinAz;
    position[PositionAxisType::Z] = radial * cosZe;

    return position;
}

/**
 * Function to convert spherical direction vector (horizontal, vertical, radial) to Euler angles (yaw, pitch).
 * The roll degree of freedom is zeroed on output. Angles are assumed to be in degrees.
 * @param position   the spherical input vector
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Eulers sphericalToEulers(const Vector3d &position,
                         tAngleConversionFunction pConverter)
{
    return sphericalToEulers(position, AngleUnitType::Degrees, pConverter);
}

/**
 * Function to convert spherical direction vector (horizontal, vertical, radial) to Euler angles (yaw, pitch).
 * The roll degree of freedom is zeroed on output.
 * @param position   the spherical input vector
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
Eulers sphericalToEulers(const Vector3d &position,
                         const AngleUnitType &angleUnits,
                         tAngleConversionFunction pConverter)
{
    auto horizontal = position[SphericalPositionAxisType::Horizontal];
    auto vertical = position[SphericalPositionAxisType::Vertical];

    return sphericalToEulers(horizontal, vertical, angleUnits, pConverter);
}

/**
 * Function to convert horizontal, vertical direction angles to Euler angles (yaw, pitch). The roll degree of
 * freedom is zeroed on output. Angles are assumed to be in degrees.
 * @param horizontal, vertical the input spherical coordinates
 * @param pConverter           a pointer to a function that defines how to convert the resultant azimuth
 *                             (measured counter-clockwise from +x), zenith (measured from +z to -z) to
 *                             user-defined horizontal, vertical angles
 */
Eulers sphericalToEulers(double horizontal,
                         double vertical,
                         tAngleConversionFunction pConverter)
{
    return sphericalToEulers(horizontal, vertical, AngleUnitType::Degrees, pConverter);
}

/**
 * Function to convert horizontal, vertical direction angles to Euler angles (yaw, pitch). The roll degree of
 * freedom is zeroed on output.
 * @param horizontal, vertical the input spherical coordinates
 * @param angleUnits           the units of the input rotational quantities, degrees or Radians
 * @param pConverter           a pointer to a function that defines how to convert the resultant azimuth
 *                             (measured counter-clockwise from +x), zenith (measured from +z to -z) to
 *                             user-defined horizontal, vertical angles
 */
Eulers sphericalToEulers(double horizontal,
                         double vertical,
                         const AngleUnitType &angleUnits,
                         tAngleConversionFunction pConverter)
{
    Vector3d direction = sphericalToCartesian(horizontal, vertical, 1.0, pConverter), zAxis(0.0, 0.0, 1.0);
    Eulers eulers(AngleUnitType::Radians);
    eulers.setYaw(std::atan2(direction[PositionAxisType::Y], direction[PositionAxisType::X]));

    direction.rotate(-eulers.getYaw(), zAxis, AngleUnitType::Radians);
    eulers.setPitch(std::atan2(direction[PositionAxisType::Z], direction[PositionAxisType::X]));
    eulers.convertAngleUnits(angleUnits);

    return eulers;
}

/**
 * Convert spherical zenith acceleration (measured from +z) to vertical angle acceleration described with
 * respect to elevation (measured from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
double zenithAccelerationToElevationAcceleration(double zenithAcceleration)
{
    return -zenithAcceleration;
}

/**
 * Convert spherical acceleration described with respect to zenith (measured from +z) to vertical angle
 * acceleration with respect to elevation (measured from x-y plane toward +z)
 */
void zenithAccelerationToElevationAcceleration(Vector3d &acceleration)
{
    auto zenithAcceleration = acceleration.get(SphericalAccelerationAxisType::Vertical);

    auto elevationAcceleration = zenithAccelerationToElevationAcceleration(zenithAcceleration);

    acceleration[SphericalAccelerationAxisType::Vertical] = elevationAcceleration;
}

/**
 * Convert spherical zenith acceleration (measured from +z) to vertical angle acceleration described with
 * respect to negative elevation (measured from x-y plane toward -z)
 */
double zenithAccelerationToNegativeElevationAcceleration(double zenithAcceleration)
{
    return -zenithAcceleration;
}

/**
 * Convert spherical acceleration described with respect to zenith (measured from +z) to vertical angle
 * acceleration with respect to negative elevation (measured from x-y plane toward +z)
 */
void zenithAccelerationToNegativeElevationAcceleration(Vector3d &acceleration)
{
    auto zenithAcceleration = acceleration.get(SphericalAccelerationAxisType::Vertical);

    auto negativeElevationAcceleration = zenithAccelerationToNegativeElevationAcceleration(zenithAcceleration);

    acceleration[SphericalAccelerationAxisType::Vertical] = negativeElevationAcceleration;
}

/**
 * Convert spherical zenith rate (measured from +z) to vertical angle rate described with respect to elevation
 * (measured from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
double zenithRateToElevationRate(double zenithRate)
{
    return -zenithRate;
}

/**
 * Convert spherical velocity described with respect to zenith (measured from +z) to vertical angle velocity
 * with respect to negative elevation (measured from x-y plane toward +z)
 */
void zenithRateToElevationRate(Vector3d &velocity)
{
    auto zenithRate = velocity.get(SphericalVelocityAxisType::Vertical);

    auto elevationAcceleration = zenithRateToElevationRate(zenithRate);

    velocity[SphericalVelocityAxisType::Vertical] = elevationAcceleration;
}

/**
 * Convert spherical zenith rate (measured from +z) to vertical angle rate described with respect to negative
 * elevation (measured from x-y plane toward -z)
 */
double zenithRateToNegativeElevationRate(double zenithRate)
{
    return -zenithRate;
}

/**
 * Convert spherical velocity described with respect to zenith (measured from +z) to vertical angle velocity
 * with respect to elevation (measured from x-y plane toward +z)
 */
void zenithRateToNegativeElevationRate(Vector3d &velocity)
{
    auto zenithRate = velocity.get(SphericalVelocityAxisType::Vertical);

    auto negativeElevationAcceleration = zenithRateToNegativeElevationRate(zenithRate);

    velocity[SphericalVelocityAxisType::Vertical] = negativeElevationAcceleration;
}

/**
 * Convert spherical zenith (measured from +z) to vertical angle described with respect to elevation (measured
 * from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
double zenithToElevation(double zenith,
                         const AngleUnitType &angleUnits)
{
    auto angleShift = PI_OVER_TWO;
    if (angleUnits == AngleUnitType::Degrees)
        angleShift *= RADIANS_TO_DEGREES;

    return angleShift - zenith;
}

/**
 * Convert spherical position described with respect to zenith (measured from +z) to vertical angle described
 * with respect to elevation (measured from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
void zenithToElevation(Vector3d &position,
                       const AngleUnitType &angleUnits)
{
    auto zenith = position.get(SphericalPositionAxisType::Vertical);

    auto elevation = zenithToElevation(zenith, angleUnits);

    position[SphericalPositionAxisType::Vertical] = elevation;
}

/**
 * Convert spherical zenith (measured from +z) to vertical angle described with respect to negative elevation
 * (measured from x-y plane toward -z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
double zenithToNegativeElevation(double zenith,
                                 const AngleUnitType &angleUnits)
{
    auto angleShift = PI_OVER_TWO;
    if (angleUnits == AngleUnitType::Degrees)
        angleShift *= RADIANS_TO_DEGREES;

    return zenith - angleShift;
}

/**
 * Convert spherical position described with respect to zenith (measured from +z) to vertical angle described
 * with respect to negative elevation (measured from x-y plane toward -z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
void zenithToNegativeElevation(Vector3d &position,
                               const AngleUnitType &angleUnits)
{
    auto zenith = position.get(SphericalPositionAxisType::Vertical);

    auto negativeElevation = zenithToNegativeElevation(zenith, angleUnits);

    position[SphericalPositionAxisType::Vertical] = negativeElevation;
}

}

}
