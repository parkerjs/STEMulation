#ifndef QUAT_H
#define QUAT_H

#include "angle_unit_type.h"
#include "export_library.h"
#include "rotation_type.h"
#include "serializable.h"
#include "swappable.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#include "xml_writable.h"
#endif
#include <cmath>
#include <iostream>

namespace math
{

// forward declarations
namespace geometric { namespace orientation { class Eulers; } }
namespace linear_algebra { namespace matrix { class Matrix3x3; }
                           namespace vector { class Vector3d; } }

namespace number_systems
{

namespace complex
{

/**
 * Class for quaternion storage and operations
 */
class Quat
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<Quat>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<Quat>
#endif
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using Eulers = geometric::orientation::Eulers;
    using Matrix3x3 = linear_algebra::matrix::Matrix3x3;
    using RotationType = geometric::orientation::RotationType;
    using Vector3d = linear_algebra::vector::Vector3d;

    /**
     * Constructor
     */
    EXPORT_STEM Quat(void);

    /**
     * Constructor
     */
    EXPORT_STEM Quat(double w,
                     double x = 0.0,
                     double y = 0.0,
                     double z = 0.0);

    /**
     * Constructor
     */
    EXPORT_STEM Quat(double w,
                     const Vector3d &vec);

    /**
     * Copy constructor
     */
    EXPORT_STEM Quat(const Quat &quat);

    /**
     * Move constructor
     */
    EXPORT_STEM Quat(Quat &&quat);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Quat(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Quat &operator = (const Quat &quat);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Quat &operator = (Quat &&quat);

    /**
     * Function call operator
     */
    EXPORT_STEM Quat operator () (double w,
                                  double x,
                                  double y,
                                  double z);

    /**
     * Subscript operator
     */
    EXPORT_STEM double &operator [] (int index);

    /**
     * Subscript operator
     */
    EXPORT_STEM double operator [] (int index) const;

    /**
     * Addition operator
     */
    EXPORT_STEM Quat operator + (const Quat &quat) const;

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Quat &operator += (const Quat &quat);

    /**
     * Unary minus operator
     */
    EXPORT_STEM Quat operator - (void) const;

    /**
     * Subtraction operator
     */
    EXPORT_STEM Quat operator - (const Quat &quat) const;

    /**
     * Subtraction assignment operator
     */
    EXPORT_STEM Quat &operator -= (const Quat &quat);

    /**
     * Multiplication operator
     */
    EXPORT_STEM Quat operator * (const Quat &quat) const;

    /**
     * Multiplication operator
     */
    EXPORT_STEM Quat operator * (const Vector3d &vec) const;

    /**
     * Multiplication operator
     */
    EXPORT_STEM Quat operator * (double value) const;

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Quat operator * (const Vector3d &vec,
                                        const Quat &quat);

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Quat operator * (double value,
                                        const Quat &quat);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Quat &operator *= (const Quat &quat);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Quat &operator *= (const Vector3d &quat);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Quat &operator *= (double value);

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const Quat &quat);

    /**
     * Calculate the additive inverse
     */
    EXPORT_STEM virtual Quat calcAdditiveInverse(void) const final;

    /**
     * Calculate the conjugate quaternion
     */
    EXPORT_STEM virtual Quat calcConjugate(void) const final;

    /**
     * Calculate the derivative of this object's quaternion
     * @param omega a 3d vector that contains the body Euler rates in radians
     */
    EXPORT_STEM virtual Quat calcDerivative(const Vector3d &omega) const final;

    /**
     * Calculate Euler angles from this object's quaternion
     * @param angleUnits   the desired angle units of the return value, Degrees or Radians
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (Active by default). In a passive interpretation, the
     *                     resultant quaternion transforms a vector from one frame to another, while an active
     *                     interpretation produces a quaternion that rotates a vector through an indicated
     *                     sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual Eulers calcEulers(const AngleUnitType &angleUnits = AngleUnitType::Degrees,
                                          const RotationType &rotationType = RotationType::Active) const final;

    /**
     * Calculate Euler angles from this object's quaternion
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (Active by default, angles assumed to be in degrees).
     *                     In a passive interpretation, the resultant quaternion transforms a vector from one
     *                     frame to another, while an active interpretation produces a quaternion that rotates
     *                     a vector through an indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual Eulers calcEulers(const RotationType &rotationType) const final;

    /**
     * Calculate Euler angles from this object's quaternion
     * @param angleUnits   the desired angle output units, Degrees or Radians
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (Active by default). In a passive interpretation, the
     *                     resultant quaternion transforms a vector from one frame to another, while an active
     *                     interpretation produces a quaternion that rotates a vector through an indicated
     *                     sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual void calcEulers(double &roll,
                                        double &pitch,
                                        double &yaw,
                                        const AngleUnitType &angleUnits = AngleUnitType::Degrees,
                                        const RotationType &rotationType = RotationType::Active) const final;

    /**
     * Calculate Euler angles from this object's quaternion
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (Active by default, angles assumed to be in degrees).
     *                     In a passive interpretation, the resultant quaternion transforms a vector from one
     *                     frame to another, while an active interpretation produces a quaternion that rotates
     *                     a vector through an indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual void calcEulers(double &roll,
                                        double &pitch,
                                        double &yaw,
                                        const RotationType &rotationType) const final;

    /**
     * Calculate the inverse quaternion
     */
    EXPORT_STEM virtual Quat calcInverse(void) const final;

    /**
     * Calculate the normalized quaternion and return the result
     */
    EXPORT_STEM virtual Quat calcNormalization(void) const final;

    /**
     * Calculate a rotation matrix from this object's quaternion
     */
    EXPORT_STEM virtual Matrix3x3 calcRotationMatrix(void) const final;

    /**
     * Calculate the conjugate quaternion and store in the current object
     */
    EXPORT_STEM virtual Quat &conjugate(void) final;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Differentiate this object's quaternion
     * @param omega a 3d vector that contains the body Euler rates in radians
     */
    EXPORT_STEM virtual Quat &differentiate(const Vector3d &omega) final;

    /**
     * Get the elements of the quaternion
     */
    EXPORT_STEM virtual void get(double &real,
                                 Vector3d &imag) const final;

    /**
     * Get the elements of the quaternion
     */
    EXPORT_STEM virtual void get(double &w,
                                 double &x,
                                 double &y,
                                 double &z) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Return the purely imaginary part of the quaternion
     */
    EXPORT_STEM virtual Vector3d imag(void) const final;

    /**
     * Get the imaginary elements of the quaternion
     */
    EXPORT_STEM virtual void imag(Vector3d &imag) const final;

    /**
     * Calculate the inverse quaternion and store in the current object
     */
    EXPORT_STEM virtual Quat &invert(void) final;

    /**
     * Determines whether or not a quaternion is equal to a constant
     */
    EXPORT_STEM virtual bool isEqual(double value) const final;

    /**
     * Determines whether or not a quaternion is equal to a constant within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(double value,
                                     double tol) const final;

    /**
     * Determines whether or not two quaternions are equivalent
     */
    EXPORT_STEM virtual bool isEqual(const Quat &quat) const final;

    /**
     * Determines whether or not two quaternions are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const Quat &quat,
                                     double tol) const final;

    /**
     * Negate the quaternion
     */
    EXPORT_STEM virtual Quat &negate(void) final;

    /**
     * Normalize the current object's quaternion
     */
    EXPORT_STEM virtual Quat &normalize(void) final;

    /**
     * Compute the squared two-norm of the quaternion
     */
    EXPORT_STEM virtual double normSquared(void) const final;

    /**
     * Quaternion post-multiplication (post-multiply the current object by the input quaternion)
     */
    EXPORT_STEM virtual void postMultiply(const Quat &quat) final;

    /**
     * Quaternion pre-multiplication (pre-multiply the current object by the input quaternion)
     */
    EXPORT_STEM virtual void preMultiply(const Quat &quat) final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Return the purely real (scalar) part of the quaternion
     */
    EXPORT_STEM virtual double real(void) const final;

    /**
     * Get the scalar (real) element of the quaternion
     */
    EXPORT_STEM virtual void real(double &real) const final;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set the elements of the quaternion
     */
    EXPORT_STEM virtual void set(double w,
                                 double x,
                                 double y,
                                 double z) final;

    /**
     * Set the elements of the quaternion
     */
    EXPORT_STEM virtual void set(const double quat[4]) final;

    /**
     * Set the elements of the quaternion
     */
    EXPORT_STEM virtual void set(double real,
                                 const Vector3d &imag) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Quat &quat) override;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * the real scalar component
     */
    double m_w;

    /**
     * the complex i unit vector
     */
    double m_x;

    /**
     * the complex j unit vector
     */
    double m_y;

    /**
     * the complex k unit vector
     */
    double m_z;
};

}

}

}

#endif
