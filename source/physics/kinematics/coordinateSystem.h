#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include "angle_unit_type.h"
#include "export_library.h"

// forward declarations
namespace math { namespace geometric { namespace orientation { class Eulers; } } }
namespace math { namespace linear_algebra { namespace vector { class Vector3d; } } };

namespace physics
{

namespace kinematics
{

/**
 * Typedef declarations
 */
typedef void (* tAngleConversionFunction)(math::linear_algebra::vector::Vector3d &,
                                          const math::trigonometric::AngleUnitType &);

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates (angles assumed to be in
 * degrees)
 * @param position   the Cartesian input vector
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
cartesianToSpherical(const math::linear_algebra::vector::Vector3d &position,
                     tAngleConversionFunction pConverter);

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates
 * @param position   the Cartesian input vector
 * @param angleUnits the desired angle output units, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
cartesianToSpherical(const math::linear_algebra::vector::Vector3d &position,
                     const math::trigonometric::AngleUnitType &angleUnits,
                     tAngleConversionFunction pConverter);

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates (angles assumed to be in
 * degrees)
 * @param x, y, z    define the input Cartesian coordinates
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
cartesianToSpherical(double x,
                     double y,
                     double z,
                     tAngleConversionFunction pConverter);

/**
 * Calculate spherical coordinates for the corresponding Cartesian coordinates
 * @param x, y, z    define the input Cartesian coordinates
 * @param angleUnits the desired angle output units, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
cartesianToSpherical(double x,
                     double y,
                     double z,
                     const math::trigonometric::AngleUnitType &angleUnits,
                     tAngleConversionFunction pConverter);

/**
 * Convert spherical elevation acceleration (measured from x-y plane to +z) to vertical angle acceleration with
 * respect to zenith (measured from +z)
 */
EXPORT_STEM double elevationAccelerationToZenithAcceleration(double elevationAcceleration);

/**
 * Convert spherical acceleration described with respect to elevation (measured from x-y plane to +z) to
 * vertical angle acceleration with respect to zenith (measured from +z)
 */
EXPORT_STEM void
elevationAccelerationToZenithAcceleration(math::linear_algebra::vector::Vector3d &acceleration);

/**
 * Convert spherical elevation rate (measured from x-y plane to +z) to vertical angle rate with respect to
 * zenith (measured from +z)
 */
EXPORT_STEM double elevationRateToZenithRate(double elevationRate);

/**
 * Convert spherical velocity described with respect to elevation (measured from x-y plane to +z) to vertical
 * angle velocity with respect to zenith (measured from +z)
 */
EXPORT_STEM void elevationRateToZenithRate(math::linear_algebra::vector::Vector3d &velocity);

/**
 * Convert spherical elevation (measured from x-y plane to +z) to vertical angle with respect to zenith
 * (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM double elevationToZenith(double elevation,
                                     const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Convert spherical position described with respect to elevation (measured from x-y plane to +z) to vertical
 * angle with respect to zenith (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM void elevationToZenith(math::linear_algebra::vector::Vector3d &position,
                                   const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Function to convert Euler angles (yaw, pitch) to Cartesian direction vector
 * @param eulers the vector of Euler angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
eulersToCartesian(const math::geometric::orientation::Eulers &eulers);

/**
 * Function to convert yaw and pitch Euler angles to Cartesian direction vector
 * @param yaw, pitch define two of the Euler angle degrees of freedom
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
eulersToCartesian(double yaw,
                  double pitch,
                  const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Function to convert yaw and pitch Euler angles to spherical direction vector (horizontal, vertical, radial).
 * Angles are assumed to be in degrees.
 * @param yaw, pitch define two of the Euler angle degrees of freedom
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
eulersToSpherical(double yaw,
                  double pitch,
                  tAngleConversionFunction pConverter);

/**
 * Function to convert yaw and pitch Euler angles to spherical direction vector (horizontal, vertical, radial).
 * @param yaw, pitch define two of the Euler angle degrees of freedom
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
eulersToSpherical(double yaw,
                  double pitch,
                  const math::trigonometric::AngleUnitType &angleUnits,
                  tAngleConversionFunction pConverter);

/**
 * Function to convert Euler angles (yaw, pitch) to spherical direction vector (horizontal, vertical, radial).
 * The roll degree of freedom is ignored.
 * @param eulers     the vector of Euler angles
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
eulersToSpherical(const math::geometric::orientation::Eulers &eulers,
                  tAngleConversionFunction pConverter);

/**
 * Convert spherical negative elevation acceleration (measured from x-y plane to +z) to vertical angle
 * acceleration with respect to zenith (measured from +z)
 */
EXPORT_STEM double negativeElevationAccelerationToZenithAcceleration(double negativeElevationAcceleration);

/**
 * Convert spherical acceleration described with respect to negative elevation (measured from x-y plane to +z)
 * to vertical angle acceleration with respect to zenith (measured from +z)
 */
EXPORT_STEM void
negativeElevationAccelerationToZenithAcceleration(math::linear_algebra::vector::Vector3d &acceleration);

/**
 * Convert spherical negative elevation rate (measured from x-y plane to +z) to vertical angle rate with
 * respect to zenith (measured from +z)
 */
EXPORT_STEM double negativeElevationRateToZenithRate(double negativeElevationRate);

/**
 * Convert spherical velocity described with respect to negative elevation (measured from x-y plane to +z) to
 * vertical angle acceleration with respect to zenith (measured from +z)
 */
EXPORT_STEM void negativeElevationRateToZenithRate(math::linear_algebra::vector::Vector3d &velocity);

/**
 * Convert spherical negative elevation (measured from x-y plane to -z) to vertical angle described with
 * respect to zenith (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM double negativeElevationToZenith(double negativeElevation,
                                             const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Convert spherical position described with respect to negative elevation (measured from x-y plane to -z) to
 * vertical angle described with respect to zenith (measured from +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM void
negativeElevationToZenith(math::linear_algebra::vector::Vector3d &position,
                          const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates (angles assumed to be in
 * degrees)
 * @param position   the spherical input vector
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
sphericalToCartesian(const math::linear_algebra::vector::Vector3d &position,
                     tAngleConversionFunction pConverter);

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates
 * @param position   the spherical input vector
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
sphericalToCartesian(const math::linear_algebra::vector::Vector3d &position,
                     const math::trigonometric::AngleUnitType &angleUnits,
                     tAngleConversionFunction pConverter);

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates (angles assumed to be in
 * degrees)
 * @param horizontal, vertical, radial the input spherical coordinates
 * @param pConverter                   a pointer to a function that defines how to convert the resultant
 *                                     azimuth (measured counter-clockwise from +x), zenith (measured from +z
 *                                     to -z) to user-defined horizontal, vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
sphericalToCartesian(double horizontal,
                     double vertical,
                     double radial,
                     tAngleConversionFunction pConverter);

/**
 * Calculate Cartesian coordinates for the corresponding spherical coordinates
 * @param horizontal, vertical, radial the input spherical coordinates
 * @param angleUnits                   the units of the input rotational quantities, degrees or Radians
 * @param pConverter                   a pointer to a function that defines how to convert the resultant
 *                                     azimuth (measured counter-clockwise from +x), zenith (measured from +z
 *                                     to -z) to user-defined horizontal, vertical angles
 */
EXPORT_STEM math::linear_algebra::vector::Vector3d
sphericalToCartesian(double horizontal,
                     double vertical,
                     double radial,
                     const math::trigonometric::AngleUnitType &angleUnits,
                     tAngleConversionFunction pConverter);

/**
 * Function to convert spherical direction vector (horizontal, vertical, radial) to Euler angles (yaw, pitch).
 * The roll degree of freedom is zeroed on output. Angles are assumed to be in degrees.
 * @param position   the spherical input vector
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::geometric::orientation::Eulers
sphericalToEulers(const math::linear_algebra::vector::Vector3d &position,
                  tAngleConversionFunction pConverter);

/**
 * Function to convert spherical direction vector (horizontal, vertical, radial) to Euler angles (yaw, pitch).
 * The roll degree of freedom is zeroed on output.
 * @param position   the spherical input vector
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 * @param pConverter a pointer to a function that defines how to convert the resultant azimuth (measured
 *                   counter-clockwise from +x), zenith (measured from +z to -z) to user-defined horizontal,
 *                   vertical angles
 */
EXPORT_STEM math::geometric::orientation::Eulers
sphericalToEulers(const math::linear_algebra::vector::Vector3d &position,
                  const math::trigonometric::AngleUnitType &angleUnits,
                  tAngleConversionFunction pConverter);

/**
 * Function to convert horizontal, vertical direction angles to Euler angles (yaw, pitch). The roll degree of
 * freedom is zeroed on output. Angles are assumed to be in degrees.
 * @param horizontal, vertical the input spherical coordinates
 * @param pConverter           a pointer to a function that defines how to convert the resultant azimuth
 *                             (measured counter-clockwise from +x), zenith (measured from +z to -z) to
 *                             user-defined horizontal, vertical angles
 */
EXPORT_STEM math::geometric::orientation::Eulers
sphericalToEulers(double horizontal,
                  double vertical,
                  tAngleConversionFunction pConverter);

/**
 * Function to convert horizontal, vertical direction angles to Euler angles (yaw, pitch). The roll degree of
 * freedom is zeroed on output.
 * @param horizontal, vertical the input spherical coordinates
 * @param angleUnits           the units of the input rotational quantities, degrees or Radians
 * @param pConverter           a pointer to a function that defines how to convert the resultant azimuth
 *                             (measured counter-clockwise from +x), zenith (measured from +z to -z) to
 *                             user-defined horizontal, vertical angles
 */
EXPORT_STEM math::geometric::orientation::Eulers
sphericalToEulers(double horizontal,
                  double vertical,
                  const math::trigonometric::AngleUnitType &angleUnits,
                  tAngleConversionFunction pConverter);

/**
 * Convert spherical zenith acceleration (measured from +z) to vertical angle acceleration described with
 * respect to elevation (measured from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM double zenithAccelerationToElevationAcceleration(double zenithAcceleration);

/**
 * Convert spherical acceleration described with respect to zenith (measured from +z) to vertical angle
 * acceleration with respect to elevation (measured from x-y plane toward +z)
 */
EXPORT_STEM void
zenithAccelerationToElevationAcceleration(math::linear_algebra::vector::Vector3d &acceleration);

/**
 * Convert spherical zenith acceleration (measured from +z) to vertical angle acceleration described with
 * respect to negative elevation (measured from x-y plane toward -z)
 */
EXPORT_STEM double zenithAccelerationToNegativeElevationAcceleration(double zenithAcceleration);

/**
 * Convert spherical acceleration described with respect to zenith (measured from +z) to vertical angle
 * acceleration with respect to negative elevation (measured from x-y plane toward +z)
 */
EXPORT_STEM void
zenithAccelerationToNegativeElevationAcceleration(math::linear_algebra::vector::Vector3d &acceleration);

/**
 * Convert spherical zenith rate (measured from +z) to vertical angle rate described with respect to elevation
 * (measured from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM double zenithRateToElevationRate(double zenithRate);

/**
 * Convert spherical velocity described with respect to zenith (measured from +z) to vertical angle velocity
 * with respect to negative elevation (measured from x-y plane toward +z)
 */
EXPORT_STEM void zenithRateToElevationRate(math::linear_algebra::vector::Vector3d &velocity);

/**
 * Convert spherical zenith rate (measured from +z) to vertical angle rate described with respect to negative
 * elevation (measured from x-y plane toward -z)
 */
EXPORT_STEM double zenithRateToNegativeElevationRate(double zenithRate);

/**
 * Convert spherical velocity described with respect to zenith (measured from +z) to vertical angle velocity
 * with respect to elevation (measured from x-y plane toward +z)
 */
EXPORT_STEM void zenithRateToNegativeElevationRate(math::linear_algebra::vector::Vector3d &velocity);

/**
 * Convert spherical zenith (measured from +z) to vertical angle described with respect to elevation (measured
 * from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM double
zenithToElevation(double zenith,
                  const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Convert spherical position described with respect to zenith (measured from +z) to vertical angle described
 * with respect to elevation (measured from x-y plane toward +z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM void
zenithToElevation(math::linear_algebra::vector::Vector3d &position,
                  const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Convert spherical zenith (measured from +z) to vertical angle described with respect to negative elevation
 * (measured from x-y plane toward -z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM double
zenithToNegativeElevation(double zenith,
                          const math::trigonometric::AngleUnitType &angleUnits);

/**
 * Convert spherical position described with respect to zenith (measured from +z) to vertical angle described
 * with respect to negative elevation (measured from x-y plane toward -z)
 * @param angleUnits the units of the input rotational quantities, degrees or Radians
 */
EXPORT_STEM void
zenithToNegativeElevation(math::linear_algebra::vector::Vector3d &position,
                          const math::trigonometric::AngleUnitType &angleUnits);

}

}

#endif
