#include "math_constants.h"
#include "matrix3x3.h"
#include "quat.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "trigonometry.h"
#include "vector2d.h"
#include "vector3d.h"
#include <cstdlib>
#include <iomanip>
#include <limits>

// using namespace declarations
using namespace math::geometric;
using namespace math::linear_algebra::matrix;
using namespace math::number_systems::complex;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace math
{

namespace linear_algebra
{

namespace vector
{

/**
 * Constructor
 */
Vector3d::Vector3d(void)
{
    m_vec[0] = 0.0;
    m_vec[1] = 0.0;
    m_vec[2] = 0.0;
}

/**
 * Constructor
 */
Vector3d::Vector3d(double x,
                   double y,
                   double z)
{
    m_vec[0] = x;
    m_vec[1] = y;
    m_vec[2] = z;
}

/**
 * Constructor
 */
Vector3d::Vector3d(const double vec[3])
{
    m_vec[0] = vec[0];
    m_vec[1] = vec[1];
    m_vec[2] = vec[2];
}

/**
 * Constructor
 */
Vector3d::Vector3d(const Vector2d &vec)
{
    m_vec[0] = vec.getX();
    m_vec[1] = vec.getY();
    m_vec[2] = 0.0;
}

/**
 * Copy constructor
 */
Vector3d::Vector3d(const Vector3d &vec)
{
    operator = (vec);
}

/**
 * Move constructor
 */
Vector3d::Vector3d(Vector3d &&vec)
{
    operator = (std::move(vec));
}

/**
 * Destructor
 */
Vector3d::~Vector3d(void)
{

}

/**
 * Copy assignment operator
 */
Vector3d &Vector3d::operator = (const Vector3d &vec)
{
    if (&vec != this)
    {
        m_vec[0] = vec.m_vec[0];
        m_vec[1] = vec.m_vec[1];
        m_vec[2] = vec.m_vec[2];
    }

    return *this;
}

/**
 * Move assignment operator
 */
Vector3d &Vector3d::operator = (Vector3d &&vec)
{
    if (&vec != this)
    {
        vec.swap(*this);
    }

    return *this;
}

/**
 * Assignment operator (assign imaginary part of quaternion to vector)
 */
Vector3d &Vector3d::operator = (const Quat &quat)
{
    m_vec[0] = quat[1];
    m_vec[1] = quat[2];
    m_vec[2] = quat[3];

    return *this;
}

/**
 * Assignment operator
 */
Vector3d &Vector3d::operator = (double value)
{
    m_vec[0] = value;
    m_vec[1] = value;
    m_vec[2] = value;

    return *this;
}

/**
 * Implicit conversion to double pointer
 */
Vector3d::operator double * (void)
{
    return &m_vec[0];
}

/**
 * Function call operator
 */
void Vector3d::operator() (double x,
                           double y,
                           double z)
{
    m_vec[0] = x;
    m_vec[1] = y;
    m_vec[2] = z;
}

/**
 * Subscript operator
 */
double &Vector3d::operator [] (int index)
{
    if (index >= 0 && index < 3)
        return m_vec[index];

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Subscript operator
 */
double Vector3d::operator [] (int index) const
{
    return const_cast<Vector3d *>(this)->operator [] (index);
}

/**
 * Addition operator
 */
Vector3d Vector3d::operator + (const Vector3d &vec) const
{
    return Vector3d(m_vec[0] + vec.m_vec[0],
                    m_vec[1] + vec.m_vec[1],
                    m_vec[2] + vec.m_vec[2]);
}

/**
 * Addition operator
 */
Vector3d Vector3d::operator + (double value) const
{
    return Vector3d(m_vec[0] + value,
                    m_vec[1] + value,
                    m_vec[2] + value);
}

/**
 * Addition operator
 */
Vector3d operator + (double lhs,
                     const Vector3d &rhs)
{
    return Vector3d(lhs + rhs.m_vec[0],
                    lhs + rhs.m_vec[1],
                    lhs + rhs.m_vec[2]);
}

/**
 * Addition operator
 */
Vector3d operator + (double lhs[3],
                     const Vector3d &rhs)
{
    return Vector3d(lhs[0] + rhs.m_vec[0],
                    lhs[1] + rhs.m_vec[1],
                    lhs[2] + rhs.m_vec[2]);
}

/**
 * Addition assignment operator
 */
Vector3d &Vector3d::operator += (const Vector3d &vec)
{
    m_vec[0] += vec.m_vec[0];
    m_vec[1] += vec.m_vec[1];
    m_vec[2] += vec.m_vec[2];

    return *this;
}

/**
 * Addition assignment operator
 */
Vector3d &Vector3d::operator += (double value)
{
    m_vec[0] += value;
    m_vec[1] += value;
    m_vec[2] += value;

    return *this;
}

/**
 * Unary minus operator
 */
Vector3d Vector3d::operator - (void) const
{
    return calcAdditiveInverse();
}

/**
 * Subtraction operator
 */
Vector3d Vector3d::operator - (const Vector3d &vec) const
{
    return Vector3d(m_vec[0] - vec.m_vec[0],
                    m_vec[1] - vec.m_vec[1],
                    m_vec[2] - vec.m_vec[2]);
}

/**
 * Subtraction operator
 */
Vector3d Vector3d::operator - (double value) const
{
    return Vector3d(m_vec[0] - value,
                    m_vec[1] - value,
                    m_vec[2] - value);
}

/**
 * Subtraction operator
 */
Vector3d operator - (double lhs,
                     const Vector3d &rhs)
{
    return Vector3d(lhs - rhs.m_vec[0],
                    lhs - rhs.m_vec[1],
                    lhs - rhs.m_vec[2]);
}

/**
 * Subtraction operator
 */
Vector3d operator - (double lhs[3],
                     const Vector3d &rhs)
{
    return Vector3d(lhs[0] - rhs.m_vec[0],
                    lhs[1] - rhs.m_vec[1],
                    lhs[2] - rhs.m_vec[2]);
}

/**
 * Subtraction assignment operator
 */
Vector3d &Vector3d::operator -= (const Vector3d &vec)
{
    m_vec[0] -= vec.m_vec[0];
    m_vec[1] -= vec.m_vec[1];
    m_vec[2] -= vec.m_vec[2];

    return *this;
}

/**
 * Subtraction on assignment operator
 */
Vector3d &Vector3d::operator -= (double value)
{
    m_vec[0] -= value;
    m_vec[1] -= value;
    m_vec[2] -= value;

    return *this;
}

/**
 * Multiplication operator
 */
Vector3d Vector3d::operator * (const Vector3d &vec) const
{
    return Vector3d(m_vec[0] * vec.m_vec[0],
                    m_vec[1] * vec.m_vec[1],
                    m_vec[2] * vec.m_vec[2]);
}

/**
 * Multiplication operator
 */
Vector3d Vector3d::operator * (double value) const
{
    return Vector3d(m_vec[0] * value,
                    m_vec[1] * value,
                    m_vec[2] * value);
}

/**
 * Multiplication operator
 */
Vector3d operator * (double lhs,
                     const Vector3d &rhs)
{
    return Vector3d(lhs * rhs.m_vec[0],
                    lhs * rhs.m_vec[1],
                    lhs * rhs.m_vec[2]);
}

/**
 * Multiplication operator
 */
Vector3d operator * (double lhs[3],
                     const Vector3d &rhs)
{
    return Vector3d(lhs[0] * rhs.m_vec[0],
                    lhs[1] * rhs.m_vec[1],
                    lhs[2] * rhs.m_vec[2]);
}

/**
 * Multiplication assignment operator
 */
Vector3d &Vector3d::operator *= (const Vector3d &vec)
{
    m_vec[0] *= vec.m_vec[0];
    m_vec[1] *= vec.m_vec[1];
    m_vec[2] *= vec.m_vec[2];

    return *this;
}

/**
 * Multiplication assignment operator
 */
Vector3d &Vector3d::operator *= (double value)
{
    m_vec[0] *= value;
    m_vec[1] *= value;
    m_vec[2] *= value;

    return *this;
}

/**
 * Division operator
 */
Vector3d Vector3d::operator / (const Vector3d &vec) const
{
    return Vector3d(m_vec[0] / vec.m_vec[0],
                    m_vec[1] / vec.m_vec[1],
                    m_vec[2] / vec.m_vec[2]);
}

/**
 * Division operator
 */
Vector3d Vector3d::operator / (double value) const
{
    return Vector3d(m_vec[0] / value,
                    m_vec[1] / value,
                    m_vec[2] / value);
}

/**
 * Division operator
 */
Vector3d operator / (double lhs,
                     const Vector3d &rhs)
{
    return Vector3d(lhs / rhs.m_vec[0],
                    lhs / rhs.m_vec[1],
                    lhs / rhs.m_vec[2]);
}

/**
 * Division operator
 */
Vector3d operator / (double lhs[3],
                     const Vector3d &rhs)
{
    return Vector3d(lhs[0] / rhs.m_vec[0],
                    lhs[1] / rhs.m_vec[1],
                    lhs[2] / rhs.m_vec[2]);
}

/**
 * Division assignment operator
 */
Vector3d &Vector3d::operator /= (const Vector3d &vec)
{
    m_vec[0] /= vec.m_vec[0];
    m_vec[1] /= vec.m_vec[1];
    m_vec[2] /= vec.m_vec[2];

    return *this;
}

/**
 * Division assignment operator
 */
Vector3d &Vector3d::operator /= (double value)
{
    m_vec[0] /= value;
    m_vec[1] /= value;
    m_vec[2] /= value;

    return *this;
}

/**
 * Equality operator
 */
bool Vector3d::operator == (double value) const
{
    return m_vec[0] == value &&
           m_vec[1] == value &&
           m_vec[2] == value;
}

/**
 * Equality operator
 */
bool operator == (double lhs,
                  const Vector3d &rhs)
{
    return rhs.operator == (lhs);
}

/**
 * Equality operator
 */
bool Vector3d::operator == (const Vector3d &vec) const
{
    return isEqual(vec, 0.0);
}

/**
 * Inequality operator
 */
bool Vector3d::operator != (double value) const
{
    return !operator == (value);
}

/**
 * Inequality operator
 */
bool operator != (double lhs,
                  const Vector3d &rhs)
{
    return rhs.operator != (lhs);
}

/**
 * Inequality operator
 */
bool Vector3d::operator != (const Vector3d &vec) const
{
    return !operator == (vec);
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const Vector3d &vec)
{
    return vec.print(stream);
}

/**
 * Calculate the additive inverse
 */
Vector3d Vector3d::calcAdditiveInverse(void) const
{
    Vector3d vec(*this);

    return vec.negate();
}

/**
 * Calculate the angle between the current vector and the input vector
 */
double Vector3d::calcAngle(const Vector3d &vec,
                           const AngleUnitType &angleUnits) const
{
    auto &&lhs = vec.calcUnit();
    auto &&rhs = calcUnit();
    auto dotProd = lhs.dot(rhs);
    auto angle = trigonometric::acos(dotProd);

    if (angleUnits == AngleUnitType::Degrees)
        angle *= RADIANS_TO_DEGREES;

    return angle;
}

/**
 * Calculate the component of input vector in the direction of current vector object
 */
double Vector3d::calcComponent(const Vector3d &vec) const
{
    return dot(vec) / magnitude();
}

/**
 * Calculate the cross product of the current object's vector with the input vector. The values are expected to
 * be VECTORS, not 3-d points. The current object's vector is "CROSSed" into the input object's vector
 */
Vector3d Vector3d::calcCross(const Vector3d &vec) const
{
    return Vector3d(*this).cross(vec);
}

/**
 * Calculate the matrix outer product
 */
Matrix3x3 Vector3d::calcOuterProduct(const Vector3d &vec) const
{
    Matrix3x3 matrix;
    for (std::size_t i = 0; i < 3; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            matrix[i][j] = m_vec[i] * vec.m_vec[j];

    return matrix;
}

/**
 * Calculate the projection of the current vector object onto the input vector object
 */
Vector3d Vector3d::calcProjection(const Vector3d &vec) const
{
    return Vector3d(*this).project(vec);
}

/**
 * Calculate the rotation of the current vector object and return the result
 */
Vector3d Vector3d::calcRotated(double angle,
                               const Vector3d &axis,
                               const AngleUnitType &angleUnits) const
{
    Vector3d vec(*this);

    return vec.rotate(angle, axis, angleUnits);
}

/**
 * Calculate the rotation of the current vector by the specified quaternion and return the result
 */
Vector3d Vector3d::calcRotated(const Quat &quat) const
{
    Vector3d vec(*this);

    return vec.rotate(quat);
}

/**
 * Calculate the current object's vector scaled by a constant
 */
Vector3d Vector3d::calcScaled(double value) const
{
    Vector3d vec(*this);

    return vec.scale(value);
}

/**
 * Calculate the current object's vector scaled by a constant in each direction
 */
Vector3d Vector3d::calcScaled(double x,
                              double y,
                              double z) const
{
    Vector3d vec(*this);

    return vec.scale(x, y, z);
}

/**
 * Calculate the unit vector from the current object's vector
 */
Vector3d Vector3d::calcUnit(void) const
{
    Vector3d vec(*this);

    return vec.unitize();
}

/**
 * Calculate the cross product of the current object's vector with the input vector and store in the current
 * object. The values are expected to be VECTORS, not 3-d points. The current object's vector is "CROSSed"
 * into the input object's vector
 */
Vector3d &Vector3d::cross(const Vector3d &vec)
{
    auto x = m_vec[0];
    auto y = m_vec[1];
    auto z = m_vec[2];

    m_vec[0] = y * vec.m_vec[2] - z * vec.m_vec[1];
    m_vec[1] = z * vec.m_vec[0] - x * vec.m_vec[2];
    m_vec[2] = x * vec.m_vec[1] - y * vec.m_vec[0];

    return *this;
}

/**
 * Function to deserialize this object's data
 */
std::istream &Vector3d::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_vec[0], sizeof(double));
        stream.read((char *)&m_vec[1], sizeof(double));
        stream.read((char *)&m_vec[2], sizeof(double));
    }

    return stream;
}

/**
 * Return the dot product
 */
double Vector3d::dot(const Vector3d &vec) const
{
    return m_vec[0] * vec.m_vec[0] +
           m_vec[1] * vec.m_vec[1] +
           m_vec[2] * vec.m_vec[2];
}

/**
 * Get the elements of the vector
 */
void Vector3d::get(double &x,
                   double &y,
                   double &z) const
{
    x = m_vec[0];
    y = m_vec[1];
    z = m_vec[2];
}

/**
 * Get the elements of the vector
 */
void Vector3d::get(double vec[3]) const
{
    const_cast<Vector3d *>(this)->get(vec[0], vec[1], vec[2]);
}

/**
 * Get the element at specified index
 */
double &Vector3d::get(int index)
{
    return operator [] (index);
}

/**
 * Get the element at specified index
 */
double Vector3d::get(int index) const
{
    return operator [] (index);
}

/**
 * Get the name of this class
 */
std::string Vector3d::getClassName(void) const
{
    return "Vector3d";
}

/**
 * Return a copy of the current object's vector with elements in the reverse order
 */
Vector3d Vector3d::getReverse(void) const
{
    Vector3d vec(*this);

    return vec.reverse();
}

/**
 * Get this object's x component
 */
double &Vector3d::getX(void)
{
    return m_vec[0];
}

/**
 * Get this object's x component
 */
double Vector3d::getX(void) const
{
    return m_vec[0];
}

/**
 * Get this object's y component
 */
double &Vector3d::getY(void)
{
    return m_vec[1];
}

/**
 * Get this object's y component
 */
double Vector3d::getY(void) const
{
    return m_vec[1];
}

/**
 * Get this object's z component
 */
double &Vector3d::getZ(void)
{
    return m_vec[2];
}

/**
 * Get this object's z component
 */
double Vector3d::getZ(void) const
{
    return m_vec[2];
}

/**
 * Determines whether or not two vectors are equivalent within the specified tolerance
 */
bool Vector3d::isEqual(const Vector3d &vec,
                       double tol) const
{
    bool bEqual = (&vec == this);
    if (!bEqual)
    {
        bEqual = true; // assume they're equal
        for (std::size_t i = 0; bEqual && i < 3; ++i)
            bEqual = (std::fabs(m_vec[i] - vec.m_vec[i]) <= tol);
    }

    return bEqual;
}

/**
 * Compute magnitude of the vector
 */
double Vector3d::magnitude(void) const
{
    auto normSq = normSquared();
    if (normSq > 0.0 && normSq != 1.0)
        return std::sqrt(normSq);
    else if (normSq == 1.0)
        return 1.0;
    else
        return 0.0;
}

/**
 * Negate the vector
 */
Vector3d &Vector3d::negate(void)
{
    m_vec[0] = -m_vec[0];
    m_vec[1] = -m_vec[1];
    m_vec[2] = -m_vec[2];

    return *this;
}

/**
 * Compute the squared two-norm of the vector
 */
double Vector3d::normSquared(void) const
{
    return dot(*this);
}

/**
 * Output stream print function
 */
std::ostream &Vector3d::print(std::ostream &stream) const
{
    if (stream)
    {
        auto width = (int)stream.width();

        stream << std::setw(3) << " { ";
        for (std::size_t i = 0; i < 3; ++i)
            stream << std::setw(width) << m_vec[i] << (i != 2 ? ", " : "");

        stream << " }";
    }

    return stream;
}

/**
 * Project the current object's vector onto the input vector object and store in current object
 */
Vector3d &Vector3d::project(const Vector3d &vec)
{
    auto innerProduct = vec.dot(vec);
    if (innerProduct != 0.0)
    {
        auto scale = dot(vec) / innerProduct;

        m_vec[0] = scale * vec.m_vec[0];
        m_vec[1] = scale * vec.m_vec[1];
        m_vec[2] = scale * vec.m_vec[2];
    }

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool Vector3d::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pX_Node = pNode->first_node("x");
        if (pX_Node != nullptr)
            m_vec[0] = std::stod(pX_Node->value());

        auto *pY_Node = pNode->first_node("y");
        if (pY_Node != nullptr)
            m_vec[1] = std::stod(pY_Node->value());

        auto *pZ_Node = pNode->first_node("z");
        if (pZ_Node != nullptr)
            m_vec[2] = std::stod(pZ_Node->value());
    }

    return bSuccess;
}
#endif
/**
 * Function to reverse the vector elements
 */
Vector3d &Vector3d::reverse(void)
{
    std::swap(m_vec[0], m_vec[2]);

    return *this;
}

/**
 * Rotate the vector by the specified angle about the specified normal axis and store in the current object
 */
Vector3d &Vector3d::rotate(double angle,
                           const Vector3d &axis,
                           const AngleUnitType &angleUnits)
{
    auto x = m_vec[0];
    auto y = m_vec[1];
    auto z = m_vec[2];

    auto c = trigonometric::cos(angle, angleUnits);
    auto s = trigonometric::sin(angle, angleUnits);
    auto t = 1.0 - c;

    m_vec[0] = x * (t * axis[0] * axis[0] + c)
             + y * (t * axis[1] * axis[0] - axis[2] * s)
             + z * (t * axis[2] * axis[0] + axis[1] * s);

    m_vec[1] = x * (t * axis[0] * axis[1] + axis[2] * s)
             + y * (t * axis[1] * axis[1] + c)
             + z * (t * axis[2] * axis[1] - axis[0] * s);

    m_vec[2] = x * (t * axis[0] * axis[2] - axis[1] * s)
             + y * (t * axis[1] * axis[2] + axis[0] * s)
             + z * (t * axis[2] * axis[2] + c);

    return *this;
}

/**
 * Rotate the vector by the specified quaternion
 */
Vector3d &Vector3d::rotate(const Quat &quat)
{
    auto &&u = quat.imag(); // extract the imaginary part of the quaternion

    auto w = quat.real(); // extract the real part of the quaternion
    operator = (2.0 * dot(u) * u + operator * (w * w - u.dot(u)) - 2.0 * w * calcCross(u));

    return *this;
}

/**
 * Scale the vector by a constant and store in the current object
 */
Vector3d &Vector3d::scale(double value)
{
    return operator *= (value);
}

/**
 * Scale an element of this vector associated with the indicated index by a constant and store in the current
 * object
 */
bool Vector3d::scale(int index,
                     double value)
{
    bool bSuccess = (index >= 0 && index < 3);
    if (bSuccess)
        m_vec[index] *= value;
    else
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                                 "Invalid index.");
    }

    return bSuccess;
}

/**
 * Scale the vector by a constant in each direction and store in the current object
 */
Vector3d &Vector3d::scale(double x,
                          double y,
                          double z)
{
    m_vec[0] *= x;
    m_vec[1] *= y;
    m_vec[2] *= z;

    return *this;
}

/**
 * Function to serialize this object's data
 */
std::ostream &Vector3d::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_vec[0], sizeof(double));
        stream.write((const char *)&m_vec[1], sizeof(double));
        stream.write((const char *)&m_vec[2], sizeof(double));
    }

    return stream;
}

/**
 * Set an element of this vector associated with the indicated index
 */
bool Vector3d::set(int index,
                   double value)
{
    bool bSuccess = (index >= 0 && index < 3);
    if (bSuccess)
        m_vec[index] = value;
    else
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                                 "Invalid index.");
    }

    return bSuccess;
}

/**
 * Set the elements of the vector
 */
void Vector3d::set(double x,
                   double y,
                   double z)
{
    m_vec[0] = x;
    m_vec[1] = y;
    m_vec[2] = z;
}

/**
 * Set the elements of the vector
 */
void Vector3d::set(const double vec[3])
{
    set(vec[0], vec[1], vec[2]);
}

/**
 * Set this object's x component
 */
void Vector3d::setX(double x)
{
    m_vec[0] = x;
}

/**
 * Set this object's y component
 */
void Vector3d::setY(double y)
{
    m_vec[1] = y;
}

/**
 * Set this object's z component
 */
void Vector3d::setZ(double z)
{
    m_vec[2] = z;
}

/**
 * Swap function
 */
void Vector3d::swap(Vector3d &vec)
{
    std::swap(m_vec[0], vec.m_vec[0]);
    std::swap(m_vec[1], vec.m_vec[1]);
    std::swap(m_vec[2], vec.m_vec[2]);
}

/**
 * Convert this object to a skew symmetric matrix
 */
Matrix3x3 Vector3d::toSkewSymmetricMatrix(void) const
{
    Matrix3x3 matrix;

    matrix[0][1] = -m_vec[2];
    matrix[0][2] = m_vec[1];
    matrix[1][2] = -m_vec[0];
    matrix[1][0] = -matrix[0][1];
    matrix[2][0] = -matrix[0][2];
    matrix[2][1] = -matrix[1][2];

    return matrix;
}

/**
 * Compute the unit vector and store in the current object
 */
Vector3d &Vector3d::unitize(void)
{
    double mag = magnitude();
    if (mag > 0.0 && mag != 1.0)
        operator /= (mag);

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool Vector3d::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto *pX_Node = pDocument->allocate_node(node_element, "x");
            auto *pX_String = pDocument->allocate_string(std::to_string(m_vec[0]).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pX_String);
            pX_Node->append_node(pDataNode);
            pNode->append_node(pX_Node);
        }

        if (bSuccess)
        {
            auto *pY_Node = pDocument->allocate_node(node_element, "y");
            auto *pY_String = pDocument->allocate_string(std::to_string(m_vec[1]).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pY_String);
            pY_Node->append_node(pDataNode);
            pNode->append_node(pY_Node);
        }

        if (bSuccess)
        {
            auto *pZ_Node = pDocument->allocate_node(node_element, "z");
            auto *pZ_String = pDocument->allocate_string(std::to_string(m_vec[2]).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pZ_String);
            pZ_Node->append_node(pDataNode);
            pNode->append_node(pZ_Node);
        }
    }

    return bSuccess;
}
#endif
}

}

}
