#include "eulers.h"
#include "matrix3x3.h"
#include "quat.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "trigonometry.h"
#include <cstdlib>
#include <iomanip>

// using namespace declarations
using namespace math::geometric::orientation;
using namespace math::linear_algebra::matrix;
using namespace math::linear_algebra::vector;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace math
{

namespace number_systems
{

namespace complex
{

/**
 * Constructor
 */
Quat::Quat(void)
: m_w(0.0),
  m_x(0.0),
  m_y(0.0),
  m_z(0.0)
{

}

/**
 * Constructor
 */
Quat::Quat(double w,
           double x,
           double y,
           double z)
: m_w(w),
  m_x(x),
  m_y(y),
  m_z(z)
{

}

/**
 * Constructor
 */
Quat::Quat(double w,
           const Vector3d &vec)
: m_w(w),
  m_x(vec[0]),
  m_y(vec[1]),
  m_z(vec[2])
{

}

/**
 * Copy constructor
 */
Quat::Quat(const Quat &quat)
{
    operator = (quat);
}

/**
 * Move constructor
 */
Quat::Quat(Quat &&quat)
{
    operator = (std::move(quat));
}

/**
 * Destructor
 */
Quat::~Quat(void)
{

}

/**
 * Copy assignment operator
 */
Quat &Quat::operator = (const Quat &quat)
{
    if (&quat != this)
    {
        m_w = quat.m_w;
        m_x = quat.m_x;
        m_y = quat.m_y;
        m_z = quat.m_z;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Quat &Quat::operator = (Quat &&quat)
{
    if (&quat != this)
    {
        quat.swap(*this);
    }

    return *this;
}

/**
 * Function call operator
 */
Quat Quat::operator () (double w,
                        double x,
                        double y,
                        double z)
{
    set(w, x, y, z);

    return *this;
}

/**
 * Subscript operator
 */
double &Quat::operator [] (int index)
{
    switch (index)
    {
        case 0: return m_w;
        case 1: return m_x;
        case 2: return m_y;
        case 3: return m_z;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Subscript operator
 */
double Quat::operator [] (int index) const
{
    return const_cast<Quat *>(this)->operator [] (index);
}

/**
 * Addition operator
 */
Quat Quat::operator + (const Quat &quat) const
{
    return Quat(m_w + quat.m_w, m_x + quat.m_x, m_y + quat.m_y, m_z + quat.m_z);
}

/**
 * Addition assignment operator
 */
Quat &Quat::operator += (const Quat &quat)
{
    m_w += quat.m_w;
    m_x += quat.m_x;
    m_y += quat.m_y;
    m_z += quat.m_z;

    return *this;
}

/**
 * Unary minus operator
 */
Quat Quat::operator - (void) const
{
    return calcAdditiveInverse();
}

/**
 * Subtraction operator
 */
Quat Quat::operator - (const Quat &quat) const
{
    return Quat(m_w - quat.m_w, m_x - quat.m_x, m_y - quat.m_y, m_z - quat.m_z);
}

/**
 * Subtraction assignment operator
 */
Quat &Quat::operator -= (const Quat &quat)
{
    m_w -= quat.m_w;
    m_x -= quat.m_x;
    m_y -= quat.m_y;
    m_z -= quat.m_z;

    return *this;
}

/**
 * Multiplication operator
 */
Quat Quat::operator * (const Quat &quat) const
{
    Quat result(*this);

    return result *= quat;
}

/**
 * Multiplication operator
 */
Quat Quat::operator * (const Vector3d &vec) const
{
    Quat result(*this);

    return result *= vec;
}

/**
 * Multiplication operator
 */
Quat Quat::operator * (double value) const
{
    Quat result(*this);

    return result *= value;
}

/**
 * Multiplication operator
 */
Quat operator * (const Vector3d &vec,
                 const Quat &quat)
{
    Quat result(0, vec[0], vec[1], vec[2]);

    return result *= quat;
}

/**
 * Multiplication operator
 */
Quat operator * (double value,
                 const Quat &quat)
{
    Quat result(quat);

    return result *= value;
}

/**
 * Multiplication assignment operator
 */
Quat &Quat::operator *= (const Quat &quat)
{
    postMultiply(quat);

    return *this;
}

/**
 * Multiplication assignment operator
 */
Quat &Quat::operator *= (const Vector3d &vec)
{
    auto w = m_w, x = m_x, y = m_y, z = m_z;

    m_w = -x * vec[0] - y * vec[1] - z * vec[2];
    m_x =  w * vec[0] + y * vec[2] - z * vec[1];
    m_y =  w * vec[1] - x * vec[2] + z * vec[0];
    m_z =  w * vec[2] + x * vec[1] - y * vec[0];

    return *this;
}

/**
 * Multiplication assignment operator
 */
Quat &Quat::operator *= (double value)
{
    m_w *= value;
    m_x *= value;
    m_y *= value;
    m_z *= value;

    return *this;
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const Quat &quat)
{
    auto width = stream.width();

    stream << std::setw(3) << " { "
           << std::setw(width) << quat.m_w << ", "
           << std::setw(width) << quat.m_x << ", "
           << std::setw(width) << quat.m_y << ", "
           << std::setw(width) << quat.m_z << " }";

    return stream;
}

/**
 * Calculate the additive inverse
 */
Quat Quat::calcAdditiveInverse(void) const
{
    Quat quat(*this);

    return quat.negate();
}

/**
 * Calculate the conjugate quaternion
 */
Quat Quat::calcConjugate(void) const
{
    Quat quat(*this);

    return quat.conjugate();
}

/**
 * Calculate the derivative of this object's quaternion
 * @param omega a 3d vector that contains the body Euler rates in radians
 */
Quat Quat::calcDerivative(const Vector3d &omega) const
{
    Quat quat(*this);

    return quat.differentiate(omega);
}

/**
 * Calculate Euler angles from this object's quaternion
 * @param angleUnits   the desired angle units of the return value, Degrees or Radians
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (Active by default). In a passive interpretation, the
 *                     resultant quaternion transforms a vector from one frame to another, while an active
 *                     interpretation produces a quaternion that rotates a vector through an indicated sequence
 *                     of Euler angles within the same frame.
 */
Eulers Quat::calcEulers(const AngleUnitType &angleUnits,
                        const RotationType &rotationType) const
{
    Eulers eulers(angleUnits);

    calcEulers(eulers[EulerAxisType::Roll],
               eulers[EulerAxisType::Pitch],
               eulers[EulerAxisType::Yaw],
               angleUnits,
               rotationType);

    return eulers;
}

/**
 * Calculate Euler angles from this object's quaternion
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (Active by default, angles assumed to be in degrees). In a
 *                     passive interpretation, the resultant quaternion transforms a vector from one frame to
 *                     another, while an active interpretation produces a quaternion that rotates a vector
 *                     through an indicated sequence of Euler angles within the same frame.
 */
Eulers Quat::calcEulers(const RotationType &rotationType) const
{
    Eulers eulers(AngleUnitType::Degrees);

    calcEulers(eulers[EulerAxisType::Roll],
               eulers[EulerAxisType::Pitch],
               eulers[EulerAxisType::Yaw],
               rotationType);

    return eulers;
}

/**
 * Calculate Euler angles from this object's quaternion
 * @param angleUnits   the desired angle output units, Degrees or Radians
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (Active by default). In a passive interpretation, the
 *                     resultant quaternion transforms a vector from one frame to another, while an active
 *                     interpretation produces a quaternion that rotates a vector through an indicated sequence
 *                     of Euler angles within the same frame.
 */
void Quat::calcEulers(double &roll,
                      double &pitch,
                      double &yaw,
                      const AngleUnitType &angleUnits,
                      const RotationType &rotationType) const
{
    auto &&quat = calcNormalization();
    if (rotationType == RotationType::Passive)
        quat.conjugate();

    auto w = quat[0], x = quat[1], y = quat[2], z = quat[3];
    auto ww = w * w, wx = w * x, wy = w * y, wz = w * z;
    auto xx = x * x, xy = x * y, xz = x * z;
    auto yy = y * y, yz = y * z;
    auto zz = z * z;

    roll = std::atan2(2 * (yz + wx), ww - xx - yy + zz);
    pitch = -trigonometric::asin(2 * (xz - wy));
    yaw = std::atan2(2 * (xy + wz), ww + xx - yy - zz);

    if (angleUnits == AngleUnitType::Degrees)
    {
        roll *= RADIANS_TO_DEGREES;
        pitch *= RADIANS_TO_DEGREES;
        yaw *= RADIANS_TO_DEGREES;
    }
}

/**
 * Calculate Euler angles from this object's quaternion
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (Active by default, angles assumed to be in degrees). In a
 *                     passive interpretation, the resultant quaternion transforms a vector from one frame to
 *                     another, while an active interpretation produces a quaternion that rotates a vector
 *                     through an indicated sequence of Euler angles within the same frame.
 */
void Quat::calcEulers(double &roll,
                      double &pitch,
                      double &yaw,
                      const RotationType &rotationType) const
{
    calcEulers(roll, pitch, yaw, AngleUnitType::Degrees, rotationType);
}

/**
 * Calculate the inverse quaternion
 */
Quat Quat::calcInverse(void) const
{
    Quat quat(*this);

    return quat.invert();
}

/**
 * Calculate the normalized quaternion and return the result
 */
Quat Quat::calcNormalization(void) const
{
    Quat quat(*this);

    return quat.normalize();
}

/**
 * Calculate a rotation matrix from this object's quaternion
 */
Matrix3x3 Quat::calcRotationMatrix(void) const
{
    Quat quat(*this);
    quat.normalize();

    auto w = quat[0];
    auto x = quat[1];
    auto y = quat[2];
    auto z = quat[3];

    auto ww = w * w;
    auto wx = w * x;
    auto wy = w * y;
    auto wz = w * z;
    auto xx = x * x;
    auto xy = x * y;
    auto xz = x * z;
    auto yy = y * y;
    auto yz = y * z;
    auto zz = z * z;

    Matrix3x3 matrix;
    matrix[0][0] = ww + xx - (yy + zz);
    matrix[0][1] = 2 * (xy - wz);
    matrix[0][2] = 2 * (xz + wy);
    matrix[1][0] = 2 * (xy + wz);
    matrix[1][1] = ww - xx + yy - zz;
    matrix[1][2] = 2 * (yz - wx);
    matrix[2][0] = 2 * (xz - wy);
    matrix[2][1] = 2 * (yz + wx);
    matrix[2][2] = ww - (xx + yy) + zz;

    return matrix;
}

/**
 * Calculate the conjugate quaternion and store in the current object
 */
Quat &Quat::conjugate(void)
{
    m_x = -m_x;
    m_y = -m_y;
    m_z = -m_z;

    return *this;
}

/**
 * Function to deserialize this object's data
 */
std::istream &Quat::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_w, sizeof(double));
        stream.read((char *)&m_x, sizeof(double));
        stream.read((char *)&m_y, sizeof(double));
        stream.read((char *)&m_z, sizeof(double));
    }

    return stream;
}

/**
 * Differentiate this object's quaternion
 * @param omega a 3d vector that contains the body Euler rates in radians
 */
Quat &Quat::differentiate(const Vector3d &omega)
{
    auto p = omega[0]; // body roll rate
    auto q = omega[1]; // body pitch rate
    auto r = omega[2]; // body yaw rate
    if (p != 0.0 || q != 0.0 || r != 0.0)
    {
        auto w = m_w;
        auto x = m_x;
        auto y = m_y;
        auto z = m_z;

        m_w = -0.5 * (p * x + q * y + r * z);
        m_x =  0.5 * (p * w + q * z - r * y);
        m_y = -0.5 * (p * z - q * w - r * x);
        m_z =  0.5 * (p * y - q * x + r * w);
    }

    return *this;
}

/**
 * Get the elements of the quaternion
 */
void Quat::get(double &real,
               Vector3d &imag) const
{
    this->real(real);
    this->imag(imag);
}

/**
 * Get the elements of the quaternion
 */
void Quat::get(double &w,
               double &x,
               double &y,
               double &z) const
{
    real(w);

    x = m_x;
    y = m_y;
    z = m_z;
}

/**
 * Get the name of this class
 */
std::string Quat::getClassName(void) const
{
    return "Quat";
}

/**
 * Return the purely imaginary part of the quaternion
 */
Vector3d Quat::imag(void) const
{
    Vector3d vec(m_x, m_y, m_z);

    return vec;
}

/**
 * Get the imaginary elements of the quaternion
 */
void Quat::imag(Vector3d &imag) const
{
    imag[0] = m_x;
    imag[1] = m_y;
    imag[2] = m_z;
}

/**
 * Calculate the inverse quaternion and store in the current object
 */
Quat &Quat::invert(void)
{
    auto normSq = normSquared();

    conjugate();

    if (normSq > 0.0 && normSq != 1.0)
    {
        m_w /= normSq;
        m_x /= normSq;
        m_y /= normSq;
        m_z /= normSq;
    }

    return *this;
}

/**
 * Determines whether or not a quaternion is equal to a constant
 */
bool Quat::isEqual(double value) const
{
    return isEqual(value, 0.0);
}

/**
 * Determines whether or not a quaternion is equal to a constant within the specified tolerance
 */
bool Quat::isEqual(double value,
                   double tol) const
{
    bool bEqual = (std::fabs(m_w - value) <= tol &&
                   std::fabs(m_x - value) <= tol &&
                   std::fabs(m_y - value) <= tol &&
                   std::fabs(m_z - value) <= tol);

    return bEqual;
}

/**
 * Determines whether or not two quaternions are equivalent
 */
bool Quat::isEqual(const Quat &quat) const
{
    return isEqual(quat, 0.0);
}

/**
 * Determines whether or not two quaternions are equivalent within the specified tolerance
 */
bool Quat::isEqual(const Quat &quat,
                   double tol) const
{
    bool bEqual = (&quat == this);
    if (!bEqual)
        bEqual = (std::fabs(m_w - quat.m_w) <= tol &&
                  std::fabs(m_x - quat.m_x) <= tol &&
                  std::fabs(m_y - quat.m_y) <= tol &&
                  std::fabs(m_z - quat.m_z) <= tol);

    return bEqual;
}

/**
 * Negate the quaternion
 */
Quat &Quat::negate(void)
{
    m_w = -m_w;

    conjugate();

    return *this;
}

/**
 * Normalize the current object's quaternion
 */
Quat &Quat::normalize(void)
{
    double magnitude = 0.0;
    auto normSq = normSquared();
    if (normSq > 0.0 && normSq != 1.0)
        magnitude = std::sqrt(normSq);
    else if (normSq == 1.0)
        magnitude = 1.0;

    if (magnitude > 0.0 && magnitude != 1.0)
    {
        m_w /= magnitude;
        m_x /= magnitude;
        m_y /= magnitude;
        m_z /= magnitude;
    }

    return *this;
}

/**
 * Compute the squared two-norm of the quaternion
 */
double Quat::normSquared(void) const
{
    return m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z;
}

/**
 * Quaternion post-multiplication (post-multiply the current object by the input quaternion)
 */
void Quat::postMultiply(const Quat &quat)
{
    auto w = m_w, x = m_x, y = m_y, z = m_z;

    m_w = w * quat.m_w - x * quat.m_x - y * quat.m_y - z * quat.m_z;
    m_x = w * quat.m_x + x * quat.m_w + y * quat.m_z - z * quat.m_y;
    m_y = w * quat.m_y - x * quat.m_z + y * quat.m_w + z * quat.m_x;
    m_z = w * quat.m_z + x * quat.m_y - y * quat.m_x + z * quat.m_w;
}

/**
 * Quaternion pre-multiplication (pre-multiply the current object by the input quaternion)
 */
void Quat::preMultiply(const Quat &quat)
{
    auto w = m_w, x = m_x, y = m_y, z = m_z;

    m_w = quat.m_w * w - quat.m_x * x - quat.m_y * y - quat.m_z * z;
    m_x = quat.m_w * x + quat.m_x * w + quat.m_y * z - quat.m_z * y;
    m_y = quat.m_w * y - quat.m_x * z + quat.m_y * w + quat.m_z * x;
    m_z = quat.m_w * z + quat.m_x * y - quat.m_y * x + quat.m_z * w;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool Quat::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pW_Node = pNode->first_node("w");
        if (pW_Node != nullptr)
            m_w = std::stod(pW_Node->value());

        auto *pX_Node = pNode->first_node("i");
        if (pX_Node != nullptr)
            m_x = std::stod(pX_Node->value());

        auto *pY_Node = pNode->first_node("j");
        if (pY_Node != nullptr)
            m_y = std::stod(pY_Node->value());

        auto *pZ_Node = pNode->first_node("k");
        if (pZ_Node != nullptr)
            m_z = std::stod(pZ_Node->value());
    }

    return bSuccess;
}
#endif
/**
 * Return the purely real (scalar) part of the quaternion
 */
double Quat::real(void) const
{
    return m_w;
}

/**
 * Get the scalar (real) element of the quaternion
 */
void Quat::real(double &real) const
{
    real = m_w;
}

/**
 * Function to serialize this object's data
 */
std::ostream &Quat::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_w, sizeof(double));
        stream.write((const char *)&m_x, sizeof(double));
        stream.write((const char *)&m_y, sizeof(double));
        stream.write((const char *)&m_z, sizeof(double));
    }

    return stream;
}

/**
 * Set the elements of the quaternion
 */
void Quat::set(double w, double x, double y, double z)
{
    m_w = w;
    m_x = x;
    m_y = y;
    m_z = z;
}

/**
 * Set the elements of the quaternion
 */
void Quat::set(const double quat[4])
{
    set(quat[0], quat[1], quat[2], quat[3]);
}

/**
 * Set the elements of the quaternion
 */
void Quat::set(double real,
               const Vector3d &imag)
{
    set(real, imag[0], imag[1], imag[2]);
}

/**
 * Swap function
 */
void Quat::swap(Quat &quat)
{
    std::swap(m_w, quat.m_w);
    std::swap(m_x, quat.m_x);
    std::swap(m_y, quat.m_y);
    std::swap(m_z, quat.m_z);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool Quat::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto *pW_Node = pDocument->allocate_node(node_element, "w");
            auto *pW_String = pDocument->allocate_string(std::to_string(m_w).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pW_String);
            pW_Node->append_node(pDataNode);
            pNode->append_node(pW_Node);
        }

        if (bSuccess)
        {
            auto *pX_Node = pDocument->allocate_node(node_element, "i");
            auto *pX_String = pDocument->allocate_string(std::to_string(m_x).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pX_String);
            pX_Node->append_node(pDataNode);
            pNode->append_node(pX_Node);
        }

        if (bSuccess)
        {
            auto *pY_Node = pDocument->allocate_node(node_element, "j");
            auto *pY_String = pDocument->allocate_string(std::to_string(m_y).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pY_String);
            pY_Node->append_node(pDataNode);
            pNode->append_node(pY_Node);
        }

        if (bSuccess)
        {
            auto *pZ_Node = pDocument->allocate_node(node_element, "k");
            auto *pZ_String = pDocument->allocate_string(std::to_string(m_z).c_str());
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
