#ifndef VECTOR2D_H
#define VECTOR2D_H

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

namespace vector
{

// forward declarations
class Vector3d;

/**
 * This class represents a two dimensional vector
 */
class Vector2d
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<Vector2d>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<Vector2d>
#endif
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using Quat = number_systems::complex::Quat;

    /**
     * Constructor
     */
    EXPORT_STEM Vector2d(void);

    /**
     * Constructor
     */
    EXPORT_STEM Vector2d(double x,
                         double y);

    /**
     * Constructor
     */
    EXPORT_STEM Vector2d(const double vec[2]);

    /**
     * Copy constructor
     */
    EXPORT_STEM Vector2d(const Vector2d &vec);

    /**
     * Move constructor
     */
    EXPORT_STEM Vector2d(Vector2d &&vec);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Vector2d(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Vector2d &operator = (const Vector2d &vec);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Vector2d &operator = (Vector2d &&vec);

    /**
     * Assignment operator
     */
    EXPORT_STEM Vector2d &operator = (double value);

    /**
     * Implicit conversion to Vector3d
     */
    EXPORT_STEM operator Vector3d (void);

    /**
     * Function call operator
     */
    EXPORT_STEM void operator () (double x,
                                  double y);

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
    EXPORT_STEM Vector2d operator + (const Vector2d &vec) const;

    /**
     * Addition operator
     */
    EXPORT_STEM Vector2d operator + (double value) const;

    /**
     * Addition operator
     */
    friend EXPORT_STEM Vector2d operator + (double lhs,
                                            const Vector2d &rhs);

    /**
     * Addition operator
     */
    friend EXPORT_STEM Vector2d operator + (double lhs[2],
                                            const Vector2d &rhs);

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Vector2d &operator += (const Vector2d &vec);

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Vector2d &operator += (double value);

    /**
     * Unary minus operator
     */
    EXPORT_STEM Vector2d operator - (void) const;

    /**
     * Subtraction operator
     */
    EXPORT_STEM Vector2d operator - (const Vector2d &vec) const;

    /**
     * Subtraction operator
     */
    EXPORT_STEM Vector2d operator - (double value) const;

    /**
     * Subtraction operator
     */
    friend EXPORT_STEM Vector2d operator - (double lhs,
                                            const Vector2d &rhs);

    /**
     * Subtraction operator
     */
    friend EXPORT_STEM Vector2d operator - (double lhs[2],
                                            const Vector2d &rhs);

    /**
     * Subtraction assignment operator
     */
    EXPORT_STEM Vector2d &operator -= (const Vector2d &vec);

    /**
     * Subtraction on assignment operator
     */
    EXPORT_STEM Vector2d &operator -= (double value);

    /**
     * Multiplication operator
     */
    EXPORT_STEM Vector2d operator * (const Vector2d &vec) const;

    /**
     * Multiplication operator
     */
    EXPORT_STEM Vector2d operator * (double value) const;

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Vector2d operator * (double lhs,
                                            const Vector2d &rhs);

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Vector2d operator * (double lhs[2],
                                            const Vector2d &rhs);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Vector2d &operator *= (const Vector2d &vec);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Vector2d &operator *= (double value);

    /**
     * Division operator
     */
    EXPORT_STEM Vector2d operator / (const Vector2d &vec) const;

    /**
     * Division operator
     */
    EXPORT_STEM Vector2d operator / (double value) const;

    /**
     * Division operator
     */
    friend EXPORT_STEM Vector2d operator / (double lhs,
                                            const Vector2d &rhs);

    /**
     * Division operator
     */
    friend EXPORT_STEM Vector2d operator / (double lhs[2],
                                            const Vector2d &rhs);

    /**
     * Division assignment operator
     */
    EXPORT_STEM Vector2d &operator /= (const Vector2d &vec);

    /**
     * Division assignment operator
     */
    EXPORT_STEM Vector2d &operator /= (double value);

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (double value) const;

    /**
     * Equality operator
     */
    friend EXPORT_STEM bool operator == (double lhs,
                                         const Vector2d &rhs);

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (const Vector2d &vec) const;

    /**
     * Inequality operator
     */
    EXPORT_STEM bool operator != (double value) const;

    /**
     * Inequality operator
     */
    friend EXPORT_STEM bool operator != (double lhs,
                                         const Vector2d &rhs);

    /**
     * Inequality operator
     */
    EXPORT_STEM bool operator != (const Vector2d &vec) const;

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const Vector2d &vec);

    /**
     * Calculate the additive inverse
     */
    EXPORT_STEM Vector2d calcAdditiveInverse(void) const;

    /**
     * Calculate the angle between the current vector and the input vector
     */
    EXPORT_STEM double calcAngle(const Vector2d &vec,
                                 const AngleUnitType &angleUnits = AngleUnitType::Degrees) const;

    /**
     * Calculate the component of input vector in the direction of current vector object
     */
    EXPORT_STEM double calcComponent(const Vector2d &vec) const;

    /**
     * Calculate the projection of the current vector object onto the input vector object
     */
    EXPORT_STEM Vector2d calcProjection(const Vector2d &vec) const;

    /**
     * Calculate the rotation of the current vector object and return the result
     */
    EXPORT_STEM Vector2d calcRotated(double angle,
                                     const AngleUnitType &angleUnits = AngleUnitType::Degrees) const;

    /**
     * Calculate the rotation of the current vector by the specified quaternion and return the result
     */
    EXPORT_STEM Vector3d calcRotated(const Quat &quat) const;

    /**
     * Calculate the rotation of the current vector object and return the result
     */
    EXPORT_STEM Vector3d calcRotated(double angle,
                                     const Vector3d &axis,
                                     const AngleUnitType &angleUnits = AngleUnitType::Degrees) const;

    /**
     * Calculate the current object's vector scaled by a constant
     */
    EXPORT_STEM Vector2d calcScaled(double value) const;

    /**
     * Calculate the current object's vector scaled by a constant in each direction
     */
    EXPORT_STEM Vector2d calcScaled(double x,
                                    double y) const;

    /**
     * Calculate the unit vector from the current object's vector
     */
    EXPORT_STEM Vector2d calcUnit(void) const;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Return the dot product
     */
    EXPORT_STEM double dot(const Vector2d &vec) const;

    /**
     * Get the elements of the vector
     */
    EXPORT_STEM void get(double &x,
                         double &y) const;

    /**
     * Get the elements of the vector
     */
    EXPORT_STEM void get(double vec[2]) const;

    /**
     * Get the element at specified index
     */
    EXPORT_STEM double &get(int index);

    /**
     * Get the element at specified index
     */
    EXPORT_STEM double get(int index) const;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Return a copy of the current object's vector with elements in the reverse order
     */
    EXPORT_STEM Vector2d getReverse(void) const;

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
     * Determines whether or not two vectors are equivalent within the specified tolerance
     */
    EXPORT_STEM bool isEqual(const Vector2d &vec,
                             double tol) const;

    /**
     * Compute magnitude of the vector
     */
    EXPORT_STEM double magnitude(void) const;

    /**
     * Negate the vector
     */
    EXPORT_STEM Vector2d &negate(void);

    /**
     * Compute the squared two-norm of the vector
     */
    EXPORT_STEM double normSquared(void) const;

    /**
     * Output stream print function
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const;

    /**
     * Project the current object's vector onto the input vector object and store in current object
     */
    EXPORT_STEM Vector2d &project(const Vector2d &vec);
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Function to reverse the vector elements
     */
    EXPORT_STEM Vector2d &reverse(void);

    /**
     * Rotate the vector by the specified angle about the specified normal axis and store in the current object
     */
    EXPORT_STEM Vector2d &rotate(double angle,
                                 const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Scale the vector by a constant and store in the current object
     */
    EXPORT_STEM Vector2d &scale(double value);

    /**
     * Scale the vector by a constant in each direction and store in the current object
     */
    EXPORT_STEM Vector2d &scale(double x,
                                double y);

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set the elements of the vector
     */
    EXPORT_STEM void set(double x,
                         double y);

    /**
     * Set the elements of the vector
     */
    EXPORT_STEM void set(const double vec[2]);

    /**
     * Set this object's x component
     */
    EXPORT_STEM virtual void setX(double x) final;

    /**
     * Set this object's y component
     */
    EXPORT_STEM virtual void setY(double y) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Vector2d &vec) override final;

    /**
     * Compute the unit vector and store in the current object
     */
    EXPORT_STEM Vector2d &unitize(void);
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
protected:

    /**
     * array to store x, y components of this 2d-vector
     */
    double m_vec[2];
};

}

}

}

#endif
