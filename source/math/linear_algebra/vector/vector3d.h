#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "angle_unit_type.h"
#include "export_library.h"
#include "reflective.h"
#include "serializable.h"
#include "swappable.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#include "xml_writable.h"
#endif
#include <iostream>

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

// forward declarations
namespace number_systems { namespace complex { class Quat; } }

namespace linear_algebra
{

// forward declarations
namespace matrix { class Matrix3x3; }

namespace vector
{

// forward declarations
class Vector2d;

/**
 * This class represents a three dimensional vector
 */
class Vector3d
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<Vector3d>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<Vector3d>
#endif
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using Matrix3x3 = matrix::Matrix3x3;
    using Quat = number_systems::complex::Quat;

    /**
     * Constructor
     */
    EXPORT_STEM Vector3d(void);

    /**
     * Constructor
     */
    EXPORT_STEM Vector3d(double x,
                         double y,
                         double z);

    /**
     * Constructor
     */
    EXPORT_STEM Vector3d(const double vec[3]);

    /**
     * Constructor
     */
    EXPORT_STEM Vector3d(const Vector2d &vec);

    /**
     * Copy constructor
     */
    EXPORT_STEM Vector3d(const Vector3d &vec);

    /**
     * Move constructor
     */
    EXPORT_STEM Vector3d(Vector3d &&vec);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Vector3d(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Vector3d &operator = (const Vector3d &vec);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Vector3d &operator = (Vector3d &&vec);

    /**
     * Assignment operator (assign imaginary part of quaternion to vector)
     */
    EXPORT_STEM virtual Vector3d &operator = (const Quat &quat) final;

    /**
     * Assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator = (double value) final;

    /**
     * Implicit conversion to double pointer
     */
    EXPORT_STEM virtual operator double * (void) final;

    /**
     * Function call operator
     */
    EXPORT_STEM virtual void operator () (double x,
                                          double y,
                                          double z) final;

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual double &operator [] (int index) final;

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual double operator [] (int index) const final;

    /**
     * Addition operator
     */
    EXPORT_STEM virtual Vector3d operator + (const Vector3d &vec) const final;

    /**
     * Addition operator
     */
    EXPORT_STEM virtual Vector3d operator + (double value) const final;

    /**
     * Addition operator
     */
    friend EXPORT_STEM Vector3d operator + (double lhs,
                                            const Vector3d &rhs);

    /**
     * Addition operator
     */
    friend EXPORT_STEM Vector3d operator + (double lhs[3],
                                            const Vector3d &rhs);

    /**
     * Addition assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator += (const Vector3d &vec) final;

    /**
     * Addition assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator += (double value) final;

    /**
     * Unary minus operator
     */
    EXPORT_STEM virtual Vector3d operator - (void) const final;

    /**
     * Subtraction operator
     */
    EXPORT_STEM virtual Vector3d operator - (const Vector3d &vec) const final;

    /**
     * Subtraction operator
     */
    EXPORT_STEM virtual Vector3d operator - (double value) const final;

    /**
     * Subtraction operator
     */
    friend EXPORT_STEM Vector3d operator - (double lhs,
                                            const Vector3d &rhs);

    /**
     * Subtraction operator
     */
    friend EXPORT_STEM Vector3d operator - (double lhs[3],
                                            const Vector3d &rhs);

    /**
     * Subtraction assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator -= (const Vector3d &vec) final;

    /**
     * Subtraction on assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator -= (double value) final;

    /**
     * Multiplication operator
     */
    EXPORT_STEM virtual Vector3d operator * (const Vector3d &vec) const final;

    /**
     * Multiplication operator
     */
    EXPORT_STEM virtual Vector3d operator * (double value) const final;

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Vector3d operator * (double lhs,
                                            const Vector3d &rhs);

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Vector3d operator * (double lhs[3],
                                            const Vector3d &rhs);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator *= (const Vector3d &vec) final;

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator *= (double value) final;

    /**
     * Division operator
     */
    EXPORT_STEM virtual Vector3d operator / (const Vector3d &vec) const final;

    /**
     * Division operator
     */
    EXPORT_STEM virtual Vector3d operator / (double value) const final;

    /**
     * Division operator
     */
    friend EXPORT_STEM Vector3d operator / (double lhs,
                                            const Vector3d &rhs);

    /**
     * Division operator
     */
    friend EXPORT_STEM Vector3d operator / (double lhs[3],
                                            const Vector3d &rhs);

    /**
     * Division assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator /= (const Vector3d &vec) final;

    /**
     * Division assignment operator
     */
    EXPORT_STEM virtual Vector3d &operator /= (double value) final;

    /**
     * Equality operator
     */
    EXPORT_STEM virtual bool operator == (double value) const final;

    /**
     * Equality operator
     */
    friend EXPORT_STEM bool operator == (double lhs,
                                         const Vector3d &rhs);

    /**
     * Equality operator
     */
    EXPORT_STEM virtual bool operator == (const Vector3d &vec) const final;

    /**
     * Inequality operator
     */
    EXPORT_STEM virtual bool operator != (double value) const final;

    /**
     * Inequality operator
     */
    friend EXPORT_STEM bool operator != (double lhs,
                                         const Vector3d &rhs);

    /**
     * Inequality operator
     */
    EXPORT_STEM virtual bool operator != (const Vector3d &vec) const final;

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const Vector3d &vec);

    /**
     * Calculate the additive inverse
     */
    EXPORT_STEM virtual Vector3d calcAdditiveInverse(void) const final;

    /**
     * Calculate the angle between the current vector and the input vector
     */
    EXPORT_STEM virtual double calcAngle(const Vector3d &vec,
                                         const AngleUnitType &angleUnits = AngleUnitType::Degrees) const final;

    /**
     * Calculate the component of input vector in the direction of current vector object
     */
    EXPORT_STEM virtual double calcComponent(const Vector3d &vec) const final;

    /**
     * Calculate the cross product of the current object's vector with the input vector. The values are
     * expected to be VECTORS, not 3-d points. The current object's vector is "CROSSed" into the input object's
     * vector
     */
    EXPORT_STEM virtual Vector3d calcCross(const Vector3d &vec) const final;

    /**
     * Calculate the matrix outer product
     */
    EXPORT_STEM virtual Matrix3x3 calcOuterProduct(const Vector3d &vec) const final;

    /**
     * Calculate the projection of the current vector object onto the input vector object
     */
    EXPORT_STEM virtual Vector3d calcProjection(const Vector3d &vec) const final;

    /**
     * Calculate the rotation of the current vector object and return the result
     */
    EXPORT_STEM virtual Vector3d calcRotated(double angle,
                                             const Vector3d &axis,
                                             const AngleUnitType &angleUnits =
                                             AngleUnitType::Degrees) const final;

    /**
     * Calculate the rotation of the current vector by the specified quaternion and return the result
     */
    EXPORT_STEM virtual Vector3d calcRotated(const Quat &quat) const final;

    /**
     * Calculate the current object's vector scaled by a constant
     */
    EXPORT_STEM virtual Vector3d calcScaled(double value) const final;

    /**
     * Calculate the current object's vector scaled by a constant in each direction
     */
    EXPORT_STEM virtual Vector3d calcScaled(double x,
                                            double y,
                                            double z) const final;

    /**
     * Calculate the unit vector from the current object's vector
     */
    EXPORT_STEM virtual Vector3d calcUnit(void) const final;

    /**
     * Calculate the cross product of the current object's vector with the input vector and store in the
     * current object. The values are expected to be VECTORS, not 3-d points. The current object's vector is
     * "CROSSed" into the input object's vector
     */
    EXPORT_STEM virtual Vector3d &cross(const Vector3d &vec) final;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Return the dot product
     */
    EXPORT_STEM virtual double dot(const Vector3d &vec) const final;

    /**
     * Get the elements of the vector
     */
    EXPORT_STEM virtual void get(double &x,
                                 double &y,
                                 double &z) const final;

    /**
     * Get the elements of the vector
     */
    EXPORT_STEM virtual void get(double vec[3]) const final;

    /**
     * Get the element at specified index
     */
    EXPORT_STEM virtual double &get(int index) final;

    /**
     * Get the element at specified index
     */
    EXPORT_STEM virtual double get(int index) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Return a copy of the current object's vector with elements in the reverse order
     */
    EXPORT_STEM virtual Vector3d getReverse(void) const final;

    /**
     * Get this object's x component
     */
    EXPORT_STEM virtual double &getX(void) final;

    /**
     * Get this object's x component
     */
    EXPORT_STEM virtual double getX(void) const final;

    /**
     * Get this object's y component
     */
    EXPORT_STEM virtual double &getY(void) final;

    /**
     * Get this object's y component
     */
    EXPORT_STEM virtual double getY(void) const final;

    /**
     * Get this object's z component
     */
    EXPORT_STEM virtual double &getZ(void) final;

    /**
     * Get this object's z component
     */
    EXPORT_STEM virtual double getZ(void) const final;

    /**
     * Determines whether or not two vectors are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const Vector3d &vec,
                                     double tol) const final;

    /**
     * Compute magnitude of the vector
     */
    EXPORT_STEM virtual double magnitude(void) const final;

    /**
     * Negate the vector
     */
    EXPORT_STEM virtual Vector3d &negate(void) final;

    /**
     * Compute the squared two-norm of the vector
     */
    EXPORT_STEM virtual double normSquared(void) const final;

    /**
     * Output stream print function
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const;

    /**
     * Project the current object's vector onto the input vector object and store in current object
     */
    EXPORT_STEM virtual Vector3d &project(const Vector3d &vec) final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Function to reverse the vector elements
     */
    EXPORT_STEM virtual Vector3d &reverse(void) final;

    /**
     * Rotate the vector by the specified angle about the specified normal axis and store in the current object
     */
    EXPORT_STEM virtual Vector3d &rotate(double angle,
                                         const Vector3d &axis,
                                         const AngleUnitType &angleUnits = AngleUnitType::Degrees) final;

    /**
     * Rotate the vector by the specified quaternion
     */
    EXPORT_STEM virtual Vector3d &rotate(const Quat &quat) final;

    /**
     * Scale the vector by a constant and store in the current object
     */
    EXPORT_STEM virtual Vector3d &scale(double value) final;

    /**
     * Scale an element of this vector associated with the indicated index by a constant and store in the
     * current object
     */
    EXPORT_STEM virtual bool scale(int index,
                                   double value) final;

    /**
     * Scale the vector by a constant in each direction and store in the current object
     */
    EXPORT_STEM virtual Vector3d &scale(double x,
                                        double y,
                                        double z) final;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set an element of this vector associated with the indicated index
     */
    EXPORT_STEM virtual bool set(int index,
                                 double value) final;

    /**
     * Set the elements of the vector
     */
    EXPORT_STEM virtual void set(double x,
                                 double y,
                                 double z) final;

    /**
     * Set the elements of the vector
     */
    EXPORT_STEM virtual void set(const double vec[3]) final;

    /**
     * Set this object's x component
     */
    EXPORT_STEM virtual void setX(double x) final;

    /**
     * Set this object's y component
     */
    EXPORT_STEM virtual void setY(double y) final;

    /**
     * Set this object's z component
     */
    EXPORT_STEM virtual void setZ(double z) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Vector3d &vec) override final;

    /**
     * Convert this object to a skew symmetric matrix
     */
    EXPORT_STEM virtual Matrix3x3 toSkewSymmetricMatrix(void) const final;

    /**
     * Compute the unit vector and store in the current object
     */
    EXPORT_STEM virtual Vector3d &unitize(void) final;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
protected:

    /**
     * array to store x, y, z components of this 3-d vector
     */
    double m_vec[3];
};

}

}

}

#endif
