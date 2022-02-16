#ifndef EULERS_H
#define EULERS_H

#include "euler_axis_type.h"
#include "rotation_type.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

namespace orientation
{

/**
 * This class contains methods to perform operations on sequences of Euler angles and associated quantities
 * (rates, accelerations, etc.)
 */
class Eulers
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<Eulers>,
  public linear_algebra::vector::Vector3d
{
private:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using EulerAxisType = geometric::orientation::EulerAxisType;
    using Matrix3x3 = linear_algebra::matrix::Matrix3x3;
    using Quat = number_systems::complex::Quat;
    using RotationType = geometric::orientation::RotationType;

    /**
     * Using declarations
     */
    using Vector3d::calcAngle;
    using Vector3d::calcComponent;
    using Vector3d::calcCross;
    using Vector3d::calcOuterProduct;
    using Vector3d::calcProjection;
    using Vector3d::calcRotated;
    using Vector3d::calcUnit;
    using Vector3d::cross;
    using Vector3d::dot;

public:

    using Vector3d::get;

private:

    using Vector3d::getX;
    using Vector3d::getY;
    using Vector3d::getZ;

public:

    using Vector3d::isEqual;

private:

    using Vector3d::magnitude;
    using Vector3d::normSquared;
    using Vector3d::project;
    using Vector3d::rotate;

public:

    using Vector3d::set;

private:

    using Vector3d::setX;
    using Vector3d::setY;
    using Vector3d::setZ;
    using Vector3d::swap;
    using Vector3d::unitize;

public:

    /**
     * Constructor
     * @param angleUnits the units of this object's Euler angles, Degrees or Radians
     */
    EXPORT_STEM Eulers(const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Constructor
     * @param angleUnits the units of this object's Euler angles, Degrees or Radians
     */
    EXPORT_STEM Eulers(double roll,
                       double pitch,
                       double yaw,
                       const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Constructor
     * @param angleUnits the units of this object's Euler angles, Degrees or Radians
     */
    EXPORT_STEM Eulers(const Vector3d &eulers,
                       const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Constructor
     * @param angleUnits the units of this object's Euler angles, Degrees or Radians
     */
    EXPORT_STEM Eulers(const double eulers[3],
                       const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Copy constructor
     */
    EXPORT_STEM Eulers(const Eulers &eulers);

    /**
     * Move constructor
     */
    EXPORT_STEM Eulers(Eulers &&eulers);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Eulers(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Eulers &operator = (const Eulers &eulers);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Eulers &operator = (Eulers &&eulers);

    /**
     * Function to calculate body Euler accelerations in radians from this object's Euler angles and input
     * Euler rates and accelerations
     */
    EXPORT_STEM Vector3d calcBodyAccelerations(const Eulers &eulerRates,
                                               const Eulers &eulerAccelerations) const;

    /**
     * Function to calculate body Euler rates in radians from this object's Euler angles and input Euler rates
     */
    EXPORT_STEM Vector3d calcBodyRates(const Eulers &eulerRates) const;

    /**
     * Function to calculate Euler accelerations from this object's body Euler angles and input body rates and
     * accelerations
     * @param omega a 3d vector that contains the body Euler rates in radians
     * @param alpha a 3d vector that contains the body Euler accelerations in radians
     */
    EXPORT_STEM Eulers calcEulerAccelerations(const Vector3d &omega,
                                              const Vector3d &alpha) const;

    /**
     * Function to calculate Euler accelerations from this object's body Euler angles and input body rates and
     * accelerations
     * @param omega      a 3d vector that contains the body Euler rates in radians
     * @param alpha      a 3d vector that contains the body Euler accelerations in radians
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM Eulers calcEulerAccelerations(const Vector3d &omega,
                                              const Vector3d &alpha,
                                              const AngleUnitType &angleUnits = AngleUnitType::Degrees) const;

    /**
     * Function to calculate Euler rates from this object's body Euler angles and input body rates
     * @param omega a 3d vector that contains the body Euler rates in radians
     */
    EXPORT_STEM Eulers calcEulerRates(const Vector3d &omega) const;

    /**
     * Function to calculate Euler rates from this object's body Euler angles and input body rates
     * @param omega      a 3d vector that contains the body Euler rates in radians
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM Eulers calcEulerRates(const Vector3d &omega,
                                      const AngleUnitType &angleUnits = AngleUnitType::Degrees) const;

    /**
     * Function to calculate this object's angles in the interval [0, 360) or [0, 2 * pi)
     */
    EXPORT_STEM Eulers calcPrincipal(void) const;

    /**
     * Calculate a quaternion from this object's Euler angles
     * @param rotationType enumeration specifying whether the rotation quaternion is to be calculated using a
     *                     passive or an active interpretation (Active by default). In a passive
     *                     interpretation, the resultant quaternion transforms a vector from one frame to
     *                     another, while an active interpretation produces a quaternion that rotates a vector
     *                     through an indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM Quat calcQuaternion(const RotationType &rotationType = RotationType::Active) const;

    /**
     * Calculate a rotation matrix from this object's Euler angles
     * @param rotationType enumeration specifying whether the rotation matrix is to be calculated using a
     *                     passive or an active interpretation (Active by default). In a passive
     *                     interpretation, the resultant rotation matrix transforms a vector from one frame to
     *                     another, while an active interpretation produces a rotation matrix that rotates a
     *                     vector through an indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM Matrix3x3 calcRotationMatrix(const RotationType &rotationType = RotationType::Active) const;

    /**
     * Convert the units of this object's Euler quantities (Degrees or Radians)
     */
    EXPORT_STEM void convertAngleUnits(const AngleUnitType &angleUnits);

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get the value of the Euler specified by EulerAxisType
     */
    EXPORT_STEM double &get(const EulerAxisType &type);

    /**
     * Get the value of the Euler specified by EulerAxisType
     */
    EXPORT_STEM double get(const EulerAxisType &type) const;

    /**
     * Get the value of the Euler specified by EulerAxisType
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM double get(const EulerAxisType &type,
                           const AngleUnitType &angleUnits) const;

    /**
     * Get angle units (Degrees or Radians)
     */
    EXPORT_STEM AngleUnitType getAngleUnits(void) const;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get this object's pitch angle
     */
    EXPORT_STEM double &getPitch(void);

    /**
     * Get this object's pitch angle
     */
    EXPORT_STEM double getPitch(void) const;

    /**
     * Get this object's pitch angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM double getPitch(const AngleUnitType &angleUnits) const;

    /**
     * Get this object's roll angle
     */
    EXPORT_STEM double &getRoll(void);

    /**
     * Get this object's roll angle
     */
    EXPORT_STEM double getRoll(void) const;

    /**
     * Get this object's roll angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM double getRoll(const AngleUnitType &angleUnits) const;

    /**
     * Get this object's yaw angle
     */
    EXPORT_STEM double &getYaw(void);

    /**
     * Get this object's yaw angle
     */
    EXPORT_STEM double getYaw(void) const;

    /**
     * Get this object's yaw angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM double getYaw(const AngleUnitType &angleUnits) const;

    /**
     * Determines whether or not two sets of Euler angles are equivalent within the specified tolerance.
     * Function uses the principal value of the angles to perform the test.
     */
    EXPORT_STEM bool isEqual(const Eulers &eulers,
                             double tol) const;

    /**
     * Function to restrict this object's angles to the interval [0, 360) or [0, 2 * pi)
     */
    EXPORT_STEM Eulers &makePrincipal(void);

    /**
     * Output stream print function
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set the value of the Euler specified by EulerAxisType
     */
    EXPORT_STEM void set(const EulerAxisType &type,
                         double value);

    /**
     * Set the value of the Euler specified by EulerAxisType
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM void set(const EulerAxisType &type,
                         double value,
                         const AngleUnitType &angleUnits);

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM void setAngleUnits(const AngleUnitType &angleUnits);

    /**
     * Set this object's pitch angle
     */
    EXPORT_STEM virtual void setPitch(double pitch) final;

    /**
     * Set this object's pitch angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitch(double pitch,
                                      const AngleUnitType &angleUnits) final;

    /**
     * Set this object's roll angle
     */
    EXPORT_STEM virtual void setRoll(double roll) final;

    /**
     * Set this object's roll angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRoll(double roll,
                                     const AngleUnitType &angleUnits) final;

    /**
     * Set this object's yaw angle
     */
    EXPORT_STEM virtual void setYaw(double yaw) final;

    /**
     * Set this object's yaw angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYaw(double yaw,
                                    const AngleUnitType &angleUnits) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Eulers &eulers) override final;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * enumeration specifies whether angles are in degrees or radians
     */
    AngleUnitType m_angleUnits;
};

}

}

}

#endif
