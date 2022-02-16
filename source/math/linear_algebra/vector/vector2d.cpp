#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "trigonometry.h"
#include "vector2d.h"
#include "vector3d.h"
#include <iomanip>

// using namespace declarations
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
Vector2d::Vector2d(void)
{
    m_vec[0] = 0.0;
    m_vec[1] = 0.0;
}

/**
 * Constructor
 */
Vector2d::Vector2d(double x,
                   double y)
{
    m_vec[0] = x;
    m_vec[1] = y;
}

/**
 * Constructor
 */
Vector2d::Vector2d(const double vec[2])
{
    m_vec[0] = vec[0];
    m_vec[1] = vec[1];
}

/**
 * Copy constructor
 */
Vector2d::Vector2d(const Vector2d &vec)
{
    operator = (vec);
}

/**
 * Move constructor
 */
Vector2d::Vector2d(Vector2d &&vec)
{
    operator = (std::move(vec));
}

/**
 * Destructor
 */
Vector2d::~Vector2d(void)
{

}

/**
 * Copy assignment operator
 */
Vector2d &Vector2d::operator = (const Vector2d &vec)
{
    if (&vec != this)
    {
        m_vec[0] = vec.m_vec[0];
        m_vec[1] = vec.m_vec[1];
    }

    return *this;
}

/**
 * Move assignment operator
 */
Vector2d &Vector2d::operator = (Vector2d &&vec)
{
    if (&vec != this)
    {
        vec.swap(*this);
    }

    return *this;
}

/**
 * Assignment operator
 */
Vector2d &Vector2d::operator = (double value)
{
    m_vec[0] = value;
    m_vec[1] = value;

    return *this;
}

/**
 * Implicit conversion to Vector3d
 */
Vector2d::operator Vector3d (void)
{
    Vector3d vec(m_vec[0],
                 m_vec[1],
                 0.0);

    return vec;
}

/**
 * Function call operator
 */
void Vector2d::operator () (double x,
                            double y)
{
    m_vec[0] = x;
    m_vec[1] = y;
}

/**
 * Subscript operator
 */
double &Vector2d::operator [] (int index)
{
    if ((index & 0x1f) == index)
        return m_vec[index];

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Subscript operator
 */
double Vector2d::operator [] (int index) const
{
    return const_cast<Vector2d *>(this)->operator [] (index);
}

/**
 * Addition operator
 */
Vector2d Vector2d::operator + (const Vector2d &vec) const
{
    return Vector2d(m_vec[0] + vec.m_vec[0],
                    m_vec[1] + vec.m_vec[1]);
}

/**
 * Addition operator
 */
Vector2d Vector2d::operator + (double value) const
{
    return Vector2d(m_vec[0] + value,
                    m_vec[1] + value);
}

/**
 * Addition operator
 */
Vector2d operator + (double lhs,
                     const Vector2d &rhs)
{
    return Vector2d(lhs + rhs.m_vec[0],
                    lhs + rhs.m_vec[1]);
}

/**
 * Addition operator
 */
Vector2d operator + (double lhs[2],
                     const Vector2d &rhs)
{
    return Vector2d(lhs[0] + rhs.m_vec[0],
                    lhs[1] + rhs.m_vec[1]);
}

/**
 * Addition assignment operator
 */
Vector2d &Vector2d::operator += (const Vector2d &vec)
{
    m_vec[0] += vec.m_vec[0];
    m_vec[1] += vec.m_vec[1];

    return *this;
}

/**
 * Addition assignment operator
 */
Vector2d &Vector2d::operator += (double value)
{
    m_vec[0] += value;
    m_vec[1] += value;

    return *this;
}

/**
 * Unary minus operator
 */
Vector2d Vector2d::operator - (void) const
{
    return calcAdditiveInverse();
}

/**
 * Subtraction operator
 */
Vector2d Vector2d::operator - (const Vector2d &vec) const
{
    return Vector2d(m_vec[0] - vec.m_vec[0],
                    m_vec[1] - vec.m_vec[1]);
}

/**
 * Subtraction operator
 */
Vector2d Vector2d::operator - (double value) const
{
    return Vector2d(m_vec[0] - value,
                    m_vec[1] - value);
}

/**
 * Subtraction operator
 */
Vector2d operator - (double lhs, const Vector2d &rhs)
{
    return Vector2d(lhs - rhs.m_vec[0],
                    lhs - rhs.m_vec[1]);
}

/**
 * Subtraction operator
 */
Vector2d operator - (double lhs[2], const Vector2d &rhs)
{
    return Vector2d(lhs[0] - rhs.m_vec[0],
                    lhs[1] - rhs.m_vec[1]);
}

/**
 * Subtraction assignment operator
 */
Vector2d &Vector2d::operator -= (const Vector2d &vec)
{
    m_vec[0] -= vec.m_vec[0];
    m_vec[1] -= vec.m_vec[1];

    return *this;
}

/**
 * Subtraction on assignment operator
 */
Vector2d &Vector2d::operator -= (double value)
{
    m_vec[0] -= value;
    m_vec[1] -= value;

    return *this;
}

/**
 * Multiplication operator
 */
Vector2d Vector2d::operator * (const Vector2d &vec) const
{
    return Vector2d(m_vec[0] * vec.m_vec[0],
                    m_vec[1] * vec.m_vec[1]);
}

/**
 * Multiplication operator
 */
Vector2d Vector2d::operator * (double value) const
{
    return Vector2d(m_vec[0] * value,
                    m_vec[1] * value);
}

/**
 * Multiplication operator
 */
Vector2d operator * (double lhs,
                     const Vector2d &rhs)
{
    return Vector2d(lhs * rhs.m_vec[0],
                    lhs * rhs.m_vec[1]);
}

/**
 * Multiplication operator
 */
Vector2d operator * (double lhs[2], const Vector2d &rhs)
{
    return Vector2d(lhs[0] * rhs.m_vec[0],
                    lhs[1] * rhs.m_vec[1]);
}

/**
 * Multiplication assignment operator
 */
Vector2d &Vector2d::operator *= (const Vector2d &vec)
{
    m_vec[0] *= vec.m_vec[0];
    m_vec[1] *= vec.m_vec[1];

    return *this;
}

/**
 * Multiplication assignment operator
 */
Vector2d &Vector2d::operator *= (double value)
{
    m_vec[0] *= value;
    m_vec[1] *= value;

    return *this;
}

/**
 * Division operator
 */
Vector2d Vector2d::operator / (const Vector2d &vec) const
{
    return Vector2d(m_vec[0] / vec.m_vec[0],
                    m_vec[1] / vec.m_vec[1]);
}

/**
 * Division operator
 */
Vector2d Vector2d::operator / (double value) const
{
    return Vector2d(m_vec[0] / value,
                    m_vec[1] / value);
}

/**
 * Division operator
 */
Vector2d operator / (double lhs,
                     const Vector2d &rhs)
{
    return Vector2d(lhs / rhs.m_vec[0],
                    lhs / rhs.m_vec[1]);
}

/**
 * Division operator
 */
Vector2d operator / (double lhs[2],
                     const Vector2d &rhs)
{
    return Vector2d(lhs[0] / rhs.m_vec[0],
                    lhs[1] / rhs.m_vec[1]);
}

/**
 * Division assignment operator
 */
Vector2d &Vector2d::operator /= (const Vector2d &vec)
{
    m_vec[0] /= vec.m_vec[0];
    m_vec[1] /= vec.m_vec[1];

    return *this;
}

/**
 * Division assignment operator
 */
Vector2d &Vector2d::operator /= (double value)
{
    m_vec[0] /= value;
    m_vec[1] /= value;

    return *this;
}

/**
 * Equality operator
 */
bool Vector2d::operator == (double value) const
{
    return m_vec[0] == value &&
           m_vec[1] == value;
}

/**
 * Equality operator
 */
bool operator == (double lhs,
                  const Vector2d &rhs)
{
    return rhs.operator == (lhs);
}

/**
 * Equality operator
 */
bool Vector2d::operator == (const Vector2d &vec) const
{
    return isEqual(vec, 0.0);
}

/**
 * Inequality operator
 */
bool Vector2d::operator != (double value) const
{
    return !operator == (value);
}

/**
 * Inequality operator
 */
bool operator != (double lhs,
                  const Vector2d &rhs)
{
    return rhs.operator != (lhs);
}

/**
 * Inequality operator
 */
bool Vector2d::operator != (const Vector2d &vec) const
{
    return !operator == (vec);
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const Vector2d &vec)
{
    return vec.print(stream);
}

/**
 * Calculate the additive inverse
 */
Vector2d Vector2d::calcAdditiveInverse(void) const
{
    Vector2d vec(*this);

    return vec.negate();
}

/**
 * Calculate the angle between the current vector and the input vector
 */
double Vector2d::calcAngle(const Vector2d &vec,
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
double Vector2d::calcComponent(const Vector2d &vec) const
{
    return dot(vec) / magnitude();
}

/**
 * Calculate the projection of the current vector object onto the input vector object
 */
Vector2d Vector2d::calcProjection(const Vector2d &vec) const
{
    return Vector2d(*this).project(vec);
}

/**
 * Calculate the rotation of the current vector object and return the result
 */
Vector2d Vector2d::calcRotated(double angle,
                               const AngleUnitType &angleUnits) const
{
    Vector2d vec(*this);

    return vec.rotate(angle, angleUnits);
}

/**
 * Calculate the rotation of the current vector by the specified quaternion and return the result
 */
Vector3d Vector2d::calcRotated(const Quat &quat) const
{
    Vector3d vec(*this);

    return vec.rotate(quat);
}

/**
 * Calculate the rotation of the current vector object and return the result
 */
Vector3d Vector2d::calcRotated(double angle,
                               const Vector3d &axis,
                               const AngleUnitType &angleUnits) const
{
    Vector3d vec(*this);

    return vec.rotate(angle, axis, angleUnits);
}

/**
 * Calculate the current object's vector scaled by a constant
 */
Vector2d Vector2d::calcScaled(double value) const
{
    Vector2d vec(*this);

    return vec.scale(value);
}

/**
 * Calculate the current object's vector scaled by a constant in each direction
 */
Vector2d Vector2d::calcScaled(double x,
                              double y) const
{
    Vector2d vec(*this);

    return vec.scale(x, y);
}

/**
 * Calculate the unit vector from the current object's vector
 */
Vector2d Vector2d::calcUnit(void) const
{
    Vector2d vec(*this);

    return vec.unitize();
}

/**
 * Function to deserialize this object's data
 */
std::istream &Vector2d::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_vec[0], sizeof(double));
        stream.read((char *)&m_vec[1], sizeof(double));
    }

    return stream;
}

/**
 * Return the dot product
 */
double Vector2d::dot(const Vector2d &vec) const
{
    return m_vec[0] * vec.m_vec[0] +
           m_vec[1] * vec.m_vec[1];
}

/**
 * Get the elements of the vector
 */
void Vector2d::get(double &x,
                   double &y) const
{
    x = m_vec[0];
    y = m_vec[1];
}

/**
 * Get the elements of the vector
 */
void Vector2d::get(double vec[2]) const
{
    const_cast<Vector2d *>(this)->get(vec[0], vec[1]);
}

/**
 * Get the element at specified index
 */
double &Vector2d::get(int index)
{
    return operator [] (index);
}

/**
 * Get the element at specified index
 */
double Vector2d::get(int index) const
{
    return operator [] (index);
}

/**
 * Get the name of this class
 */
std::string Vector2d::getClassName(void) const
{
    return "Vector2d";
}

/**
 * Return a copy of the current object's vector with elements in the reverse order
 */
Vector2d Vector2d::getReverse(void) const
{
    Vector2d vec(*this);

    return vec.reverse();
}

/**
 * Get this object's x component
 */
double &Vector2d::getX(void)
{
    return m_vec[0];
}

/**
 * Get this object's x component
 */
double Vector2d::getX(void) const
{
    return m_vec[0];
}

/**
 * Get this object's y component
 */
double &Vector2d::getY(void)
{
    return m_vec[1];
}

/**
 * Get this object's y component
 */
double Vector2d::getY(void) const
{
    return m_vec[1];
}

/**
 * Determines whether or not two vectors are equivalent within the specified tolerance
 */
bool Vector2d::isEqual(const Vector2d &vec,
                       double tol) const
{
    bool bEqual = (&vec == this);
    if (!bEqual)
    {
        bEqual = (std::fabs(m_vec[0] - vec.m_vec[0]) <= tol) &&
                 (std::fabs(m_vec[1] - vec.m_vec[1]) <= tol);
    }

    return bEqual;
}

/**
 * Compute magnitude of the vector
 */
double Vector2d::magnitude(void) const
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
Vector2d &Vector2d::negate(void)
{
    m_vec[0] = -m_vec[0];
    m_vec[1] = -m_vec[1];

    return *this;
}

/**
 * Compute the squared two-norm of the vector
 */
double Vector2d::normSquared(void) const
{
    return dot(*this);
}

/**
 * Output stream print function
 */
std::ostream &Vector2d::print(std::ostream &stream) const
{
    if (stream)
    {
        auto width = (int)stream.width();

        stream << std::setw(3) << " { "
               << std::setw(width) << m_vec[0] << ", "
               << std::setw(width) << m_vec[1] << " }";
    }

    return stream;
}

/**
 * Project the current object's vector onto the input vector object and store in current object
 */
Vector2d &Vector2d::project(const Vector2d &vec)
{
    auto innerProduct = vec.dot(vec);
    if (innerProduct != 0.0)
    {
        auto scale = dot(vec) / innerProduct;

        m_vec[0] = scale * vec.m_vec[0];
        m_vec[1] = scale * vec.m_vec[1];
    }

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool Vector2d::readFromXML(xml_node<> *pNode)
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
    }

    return bSuccess;
}
#endif
/**
 * Function to reverse the vector elements
 */
Vector2d &Vector2d::reverse(void)
{
    std::swap(m_vec[0], m_vec[1]);

    return *this;
}

/**
 * Rotate the vector by the specified angle about the specified normal axis and store in the current object
 */
Vector2d &Vector2d::rotate(double angle,
                           const AngleUnitType &angleUnits)
{
    auto x = m_vec[0];
    auto y = m_vec[1];

    auto c = trigonometric::cos(angle, angleUnits);
    auto s = trigonometric::sin(angle, angleUnits);

    m_vec[0] = x * c - y * s;
    m_vec[1] = x * s + y * c;

    return *this;
}

/**
 * Scale the vector by a constant and store in the current object
 */
Vector2d &Vector2d::scale(double value)
{
    return operator *= (value);
}

/**
 * Scale the vector by a constant in each direction and store in the current object
 */
Vector2d &Vector2d::scale(double x,
                          double y)
{
    m_vec[0] *= x;
    m_vec[1] *= y;

    return *this;
}

/**
 * Function to serialize this object's data
 */
std::ostream &Vector2d::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_vec[0], sizeof(double));
        stream.write((const char *)&m_vec[1], sizeof(double));
    }

    return stream;
}

/**
 * Set the elements of the vector
 */
void Vector2d::set(double x,
                   double y)
{
    m_vec[0] = x;
    m_vec[1] = y;
}

/**
 * Set the elements of the vector
 */
void Vector2d::set(const double vec[2])
{
    set(vec[0], vec[1]);
}

/**
 * Set this object's x component
 */
void Vector2d::setX(double x)
{
    m_vec[0] = x;
}

/**
 * Set this object's y component
 */
void Vector2d::setY(double y)
{
    m_vec[1] = y;
}

/**
 * Swap function
 */
void Vector2d::swap(Vector2d &vec)
{
    std::swap(m_vec[0], vec.m_vec[0]);
    std::swap(m_vec[1], vec.m_vec[1]);
}

/**
 * Compute the unit vector and store in the current object
 */
Vector2d &Vector2d::unitize(void)
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
bool Vector2d::writeToXML(xml_node<> *pNode)
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
    }

    return bSuccess;
}
#endif
}

}

}
