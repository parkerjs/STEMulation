#include "eulers.h"
#include "matrix3x3.h"
#include "quat.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "trigonometry.h"
#include <iomanip>
#include <iostream>

// using namespace declarations
using namespace math::geometric::orientation;
using namespace math::linear_algebra::vector;
using namespace math::number_systems::complex;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Constructor
 */
Matrix3x3::Matrix3x3(void)
: m_row0(0.0, 0.0, 0.0),
  m_row1(0.0, 0.0, 0.0),
  m_row2(0.0, 0.0, 0.0)
{

}

/**
 * Constructor
 */
Matrix3x3::Matrix3x3(const Vector3d &row0,
                     const Vector3d &row1,
                     const Vector3d &row2)
: m_row0(row0),
  m_row1(row1),
  m_row2(row2)
{

}

/**
 * Constructor
 */
Matrix3x3::Matrix3x3(double row0col0, double row0col1, double row0col2,
                     double row1col0, double row1col1, double row1col2,
                     double row2col0, double row2col1, double row2col2)
: m_row0(row0col0, row0col1, row0col2),
  m_row1(row1col0, row1col1, row1col2),
  m_row2(row2col0, row2col1, row2col2)
{

}

/**
 * Constructor
 */
Matrix3x3::Matrix3x3(double matrix[3][3])
: m_row0(matrix[0][0], matrix[0][1], matrix[0][2]),
  m_row1(matrix[1][0], matrix[1][1], matrix[1][2]),
  m_row2(matrix[2][0], matrix[2][1], matrix[2][2])
{

}

/**
 * Copy constructor
 */
Matrix3x3::Matrix3x3(const Matrix3x3 &matrix)
{
    operator = (matrix);
}

/**
 * Move constructor
 */
Matrix3x3::Matrix3x3(Matrix3x3 &&matrix)
{
    operator = (std::move(matrix));
}

/**
 * Destructor
 */
Matrix3x3::~Matrix3x3(void)
{

}

/**
 * Copy assignment operator
 */
Matrix3x3 &Matrix3x3::operator = (const Matrix3x3 &matrix)
{
    if (&matrix != this)
    {
        m_row0 = matrix.m_row0;
        m_row1 = matrix.m_row1;
        m_row2 = matrix.m_row2;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Matrix3x3 &Matrix3x3::operator = (Matrix3x3 &&matrix)
{
    if (&matrix != this)
    {
        matrix.swap(*this);
    }

    return *this;
}

/**
 * Subscript operator
 */
Vector3d &Matrix3x3::operator [] (int index)
{
    switch (index)
    {
        case 0: return m_row0;
        case 1: return m_row1;
        case 2: return m_row2;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Subscript operator
 */
Vector3d Matrix3x3::operator [] (int index) const
{
    return const_cast<Matrix3x3 *>(this)->operator [] (index);
}

/**
 * Function call operator
 */
Matrix3x3 &Matrix3x3::operator () (const Vector3d &row0,
                                   const Vector3d &row1,
                                   const Vector3d &row2)
{
    m_row0 = row0;
    m_row1 = row1;
    m_row2 = row2;

    return *this;
}

/**
 * Function call operator
 */
Matrix3x3 &Matrix3x3::operator () (double row0col0, double row0col1, double row0col2,
                                   double row1col0, double row1col1, double row1col2,
                                   double row2col0, double row2col1, double row2col2)
{
    m_row0.set(row0col0, row0col1, row0col2);
    m_row1.set(row1col0, row1col1, row1col2);
    m_row2.set(row2col0, row2col1, row2col2);

    return *this;
}

/**
 * Addition operator
 */
Matrix3x3 Matrix3x3::operator + (const Matrix3x3 &matrix) const
{
    Matrix3x3 result;

    result += (matrix);

    return result;
}

/**
 * Addition assignment operator
 */
Matrix3x3 &Matrix3x3::operator += (const Matrix3x3 &matrix)
{
    m_row0 += matrix.m_row0;
    m_row1 += matrix.m_row1;
    m_row2 += matrix.m_row1;

    return *this;
}

/**
 * Subtraction operator
 */
Matrix3x3 Matrix3x3::operator - (const Matrix3x3 &matrix) const
{
    Matrix3x3 result;

    result -= (matrix);

    return result;
}

/**
 * Subtraction assignment operator
 */
Matrix3x3 &Matrix3x3::operator -= (const Matrix3x3 &matrix)
{
    m_row0 -= matrix.m_row0;
    m_row1 -= matrix.m_row1;
    m_row2 -= matrix.m_row1;

    return *this;
}

/**
 * Multiplication operator
 */
Matrix3x3 Matrix3x3::operator * (const Matrix3x3 &matrix) const
{
    Matrix3x3 result(*this);

    result *= (matrix);

    return result;
}

/**
 * Multiplication operator
 */
Vector3d Matrix3x3::operator * (const Vector3d &vec) const
{
    Vector3d result;

    result[0] = m_row0[0] * vec[0] + m_row0[1] * vec[1] + m_row0[2] * vec[2];
    result[1] = m_row1[0] * vec[0] + m_row1[1] * vec[1] + m_row1[2] * vec[2];
    result[2] = m_row2[0] * vec[0] + m_row2[1] * vec[1] + m_row2[2] * vec[2];

    return result;
}

/**
 * Multiplication operator
 */
Vector3d operator * (const Vector3d &lhs,
                     const Matrix3x3 &rhs)
{
    Vector3d vec;

    vec[0] = lhs[0] * rhs.m_row0[0] + lhs[1] * rhs.m_row1[0] + lhs[2] * rhs.m_row2[0];
    vec[1] = lhs[0] * rhs.m_row0[1] + lhs[1] * rhs.m_row1[1] + lhs[2] * rhs.m_row2[1];
    vec[2] = lhs[0] * rhs.m_row0[2] + lhs[1] * rhs.m_row1[2] + lhs[2] * rhs.m_row2[2];

    return vec;
}

/**
 * Multiplication operator
 */
Matrix3x3 Matrix3x3::operator * (double value) const
{
    Matrix3x3 result(*this);

    result *= value;

    return result;
}

/**
 * Multiplication operator
 */
Matrix3x3 operator * (double lhs,
                      const Matrix3x3 &rhs)
{
    Matrix3x3 result(rhs);

    result *= lhs;

    return result;
}

/**
 * Multiplication assignment operator
 */
Matrix3x3 &Matrix3x3::operator *= (const Matrix3x3 &matrix)
{
    auto row0col0 = m_row0[0], row0col1 = m_row0[1], row0col2 = m_row0[2];
    auto row1col0 = m_row1[0], row1col1 = m_row1[1], row1col2 = m_row1[2];
    auto row2col0 = m_row2[0], row2col1 = m_row2[1], row2col2 = m_row2[2];

    m_row0[0] = row0col0 * matrix.m_row0[0] + row0col1 * matrix.m_row1[0] + row0col2 * matrix.m_row2[0];
    m_row0[1] = row0col0 * matrix.m_row0[1] + row0col1 * matrix.m_row1[1] + row0col2 * matrix.m_row2[1];
    m_row0[2] = row0col0 * matrix.m_row0[2] + row0col1 * matrix.m_row1[2] + row0col2 * matrix.m_row2[2];
    m_row1[0] = row1col0 * matrix.m_row0[0] + row1col1 * matrix.m_row1[0] + row1col2 * matrix.m_row2[0];
    m_row1[1] = row1col0 * matrix.m_row0[1] + row1col1 * matrix.m_row1[1] + row1col2 * matrix.m_row2[1];
    m_row1[2] = row1col0 * matrix.m_row0[2] + row1col1 * matrix.m_row1[2] + row1col2 * matrix.m_row2[2];
    m_row2[0] = row2col0 * matrix.m_row0[0] + row2col1 * matrix.m_row1[0] + row2col2 * matrix.m_row2[0];
    m_row2[1] = row2col0 * matrix.m_row0[1] + row2col1 * matrix.m_row1[1] + row2col2 * matrix.m_row2[1];
    m_row2[2] = row2col0 * matrix.m_row0[2] + row2col1 * matrix.m_row1[2] + row2col2 * matrix.m_row2[2];

    return *this;
}

/**
 * Multiplication assignment operator
 */
Matrix3x3 &Matrix3x3::operator *= (double value)
{
    return scale(value);
}

/**
 * Division operator
 */
Matrix3x3 Matrix3x3::operator / (double value) const
{
    Matrix3x3 result(*this);

    result /= value;

    return result;
}

/**
 * Division assignment operator
 */
Matrix3x3 &Matrix3x3::operator /= (double value)
{
    return scale(1.0 / value);
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const Matrix3x3 &matrix)
{
    auto width = stream.width();

    stream << std::setw(3) << " { ";
    stream << std::setw(width) << matrix[0][0] << " "
           << std::setw(width) << matrix[0][1] << " "
           << std::setw(width) << matrix[0][2] << std::endl << "   "
           << std::setw(width) << matrix[1][0] << " "
           << std::setw(width) << matrix[1][1] << " "
           << std::setw(width) << matrix[1][2] << std::endl << "   "
           << std::setw(width) << matrix[2][0] << " "
           << std::setw(width) << matrix[2][1] << " "
           << std::setw(width) << matrix[2][2] << " }";

    return stream;
}

/**
 * Calculate Euler angles from this object's rotation matrix
 * @param angleUnits   the desired angle units of the return value, Degrees or Radians
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (Active by default). In a passive interpretation, the
 *                     resultant rotation matrix transforms a vector from one frame to another, while an active
 *                     interpretation produces a rotation matrix that rotates a vector through an indicated
 *                     sequence of Euler angles within the same frame.
 */
Eulers Matrix3x3::calcEulers(const AngleUnitType &angleUnits,
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
 * Calculate Euler angles from this object's rotation matrix
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (angles assumed to be in degrees). In a passive
 *                     interpretation, the resultant rotation matrix transforms a vector from one frame to
 *                     another, while an active interpretation produces a rotation matrix that rotates a vector
 *                     through an indicated sequence of Euler angles within the same frame.
 */
Eulers Matrix3x3::calcEulers(const RotationType &rotationType) const
{
    Eulers eulers(AngleUnitType::Degrees);

    calcEulers(eulers[EulerAxisType::Roll],
               eulers[EulerAxisType::Pitch],
               eulers[EulerAxisType::Yaw],
               rotationType);

    return eulers;
}

/**
 * Calculate Euler angles from this object's rotation matrix
 * @param angleUnits   the desired angle output units, Degrees or Radians
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles (Active by default). In a passive interpretation, the
 *                     resultant rotation matrix transforms a vector from one frame to another, while an active
 *                     interpretation produces a rotation matrix that rotates a vector through an indicated
 *                     sequence of Euler angles within the same frame.
 */
void Matrix3x3::calcEulers(double &roll,
                           double &pitch,
                           double &yaw,
                           const AngleUnitType &angleUnits,
                           const RotationType &rotationType) const
{
    auto r01 = m_row0[1];
    auto r02 = m_row0[2];
    auto r10 = m_row1[0];
    auto r12 = m_row1[2];
    auto r20 = m_row2[0];
    auto r21 = m_row2[1];

    if (rotationType == RotationType::Passive)
    {
        std::swap(r01, r10);
        std::swap(r02, r20);
        std::swap(r12, r21);
    }

    auto r00 = m_row0[0];
    auto r11 = m_row1[1];
    auto r22 = m_row2[2];

    if (std::fabs(r20) < 1.0)
    {
        roll = std::atan2(r21, r22);
        pitch = -trigonometric::asin(r20);
        yaw = std::atan2(r10, r00);
    }
    else
    {
        roll = 0.0;
        pitch = (r20 >= 0.0) ? -0.5 * PI : 0.5 * PI;
        yaw = -std::atan2(r01, r11);
    }

    if (angleUnits == AngleUnitType::Degrees)
    {
        roll *= RADIANS_TO_DEGREES;
        pitch *= RADIANS_TO_DEGREES;
        yaw *= RADIANS_TO_DEGREES;
    }
}

/**
 * Calculate Euler angles from this object's rotation matrix
 * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
 *                     calculating the Euler angles. In a passive interpretation, the resultant rotation matrix
 *                     transforms a vector from one frame to another, while an active interpretation produces a
 *                     rotation matrix that rotates a vector through an indicated sequence of Euler angles
 *                     within the same frame.
 */
void Matrix3x3::calcEulers(double &roll,
                           double &pitch,
                           double &yaw,
                           const RotationType &rotationType) const
{
    calcEulers(roll, pitch, yaw, AngleUnitType::Degrees, rotationType);
}

/**
 * Calculate the inverse matrix
 */
Matrix3x3 Matrix3x3::calcInverse(void) const
{
    Matrix3x3 result(*this);

    return result.invert();
}

/**
 * Calculate a Quaternion from this object's rotation matrix
 * @param rotationType enumeration specifying whether the function assumes that the current object's rotation
 *                     matrix is an active or passive rotation in calculating the quaternion (Active by
 *                     default). In a passive interpretation, the resultant rotation matrix transforms a vector
 *                     from one frame to another, while an active interpretation produces a rotation matrix
 *                     that rotates a vector through an indicated sequence of Euler angles within the same
 *                     frame.
 */
Quat Matrix3x3::calcQuaternion(const RotationType &rotationType) const
{
    Quat quat;
    quat[0] = 0.5 * std::sqrt(m_row0[0] + m_row1[1] + m_row2[2] + 1.0);
    quat[1] = (m_row2[1] - m_row1[2]) / (4.0 * quat[0]);
    quat[2] = (m_row0[2] - m_row2[0]) / (4.0 * quat[0]);
    quat[3] = (m_row1[0] - m_row0[1]) / (4.0 * quat[0]);

    if (rotationType == RotationType::Passive)
        quat.conjugate();

    return quat;
}

/**
 * Calculate the scaled matrix
 */
Matrix3x3 Matrix3x3::calcScaled(double value) const
{
    Matrix3x3 result(*this);

    return result.scale(value);
}

/**
 * Calculate the transpose matrix
 */
Matrix3x3 Matrix3x3::calcTranspose(void) const
{
    Matrix3x3 result(*this);

    return result.transpose();
}

/**
 * Function to deserialize this object's data
 */
std::istream &Matrix3x3::deserialize(std::istream &stream)
{
    if (stream)
    {
        m_row0.deserialize(stream);
        m_row1.deserialize(stream);
        m_row2.deserialize(stream);
    }

    return stream;
}

/**
 * Compute the determinant of this matrix
 */
double Matrix3x3::determinant(void) const
{
    return m_row0[0] * m_row1[1] * m_row2[2] + m_row0[1] * m_row1[2] * m_row2[0] +
           m_row0[2] * m_row1[0] * m_row2[1] - m_row0[2] * m_row1[1] * m_row2[0] -
           m_row0[1] * m_row1[0] * m_row2[2] - m_row0[0] * m_row1[2] * m_row2[1];
}

/**
 * Get the name of this class
 */
std::string Matrix3x3::getClassName(void) const
{
    return "Matrix3x3";
}

/**
 * Create a 3x3 identity matrix
 */
Matrix3x3 Matrix3x3::identity(void)
{
    Matrix3x3 identity;

    identity.m_row0[0] = 1;
    identity.m_row1[1] = 1;
    identity.m_row2[2] = 1;

    return identity;
}

/**
 * Invert the current object's matrix and store in the current object
 */
Matrix3x3 &Matrix3x3::invert(void)
{
    auto det = determinant();
    auto row0col0 = m_row0[0], row0col1 = m_row0[1], row0col2 = m_row0[2];
    auto row1col0 = m_row1[0], row1col1 = m_row1[1], row1col2 = m_row1[2];
    auto row2col0 = m_row2[0], row2col1 = m_row2[1], row2col2 = m_row2[2];

    m_row0[0] = (row1col1 * row2col2 - row1col2 * row2col1) / det;
    m_row0[1] = (row0col2 * row2col1 - row0col1 * row2col2) / det;
    m_row0[2] = (row0col1 * row1col2 - row1col1 * row0col2) / det;
    m_row1[0] = (row2col0 * row1col2 - row1col0 * row2col2) / det;
    m_row1[1] = (row0col0 * row2col2 - row2col0 * row0col2) / det;
    m_row1[2] = (row1col0 * row0col2 - row0col0 * row1col2) / det;
    m_row2[0] = (row1col0 * row2col1 - row2col0 * row1col1) / det;
    m_row2[1] = (row2col0 * row0col1 - row0col0 * row2col1) / det;
    m_row2[2] = (row0col0 * row1col1 - row0col1 * row1col0) / det;

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool Matrix3x3::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pRow0_Node = pNode->first_node("row_0");
        if (pRow0_Node != nullptr)
            m_row0.readFromXML(pRow0_Node);

        auto *pRow1_Node = pNode->first_node("row_1");
        if (pRow1_Node != nullptr)
            m_row1.readFromXML(pRow1_Node);

        auto *pRow2_Node = pNode->first_node("row_2");
        if (pRow2_Node != nullptr)
            m_row2.readFromXML(pRow2_Node);
    }

    return bSuccess;
}
#endif
/**
 * Scale the matrix and store in the current object
 */
Matrix3x3 &Matrix3x3::scale(double value)
{
    m_row0.scale(value);
    m_row1.scale(value);
    m_row2.scale(value);

    return *this;
}

/**
 * Function to serialize this object's data
 */
std::ostream &Matrix3x3::serialize(std::ostream &stream) const
{
    if (stream)
    {
        m_row0.serialize(stream);
        m_row1.serialize(stream);
        m_row2.serialize(stream);
    }

    return stream;
}

/**
 * Swap function
 */
void Matrix3x3::swap(Matrix3x3 &matrix)
{
    m_row0.swap(matrix.m_row0);
    m_row1.swap(matrix.m_row1);
    m_row2.swap(matrix.m_row2);
}

/**
 * Swap two elements within the matrix
 */
bool Matrix3x3::swap(int i,
                     int j)
{
    bool bSuccess = (i >= 0 && j >= 0 && i <= 2 && j <=2);
    if (bSuccess)
        std::swap((*this)[i][j], (*this)[j][i]);

    return bSuccess;
}

/**
 * Convert Matrix3x3 object to a double array
 */
void Matrix3x3::toArray(double matrix[3][3]) const
{
    for (std::size_t i = 0; i < 3; ++i)
    {
        matrix[0][i] = m_row0[i];
        matrix[1][i] = m_row1[i];
        matrix[2][i] = m_row2[i];
    }
}

/**
 * Compute the transpose of this matrix and store in the current object
 */
Matrix3x3 &Matrix3x3::transpose(void)
{
    std::swap(m_row0[1], m_row1[0]);
    std::swap(m_row0[2], m_row2[0]);
    std::swap(m_row1[2], m_row2[1]);

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool Matrix3x3::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto *pRow0_Node = pDocument->allocate_node(node_element, "row_0)");
            bSuccess = m_row0.writeToXML(pRow0_Node);
            pNode->append_node(pRow0_Node);
        }

        if (bSuccess)
        {
            auto *pRow1_Node = pDocument->allocate_node(node_element, "row_1)");
            bSuccess = m_row0.writeToXML(pRow1_Node);
            pNode->append_node(pRow1_Node);
        }

        if (bSuccess)
        {
            auto *pRow2_Node = pDocument->allocate_node(node_element, "row_2)");
            bSuccess = m_row0.writeToXML(pRow2_Node);
            pNode->append_node(pRow2_Node);
        }
    }

    return bSuccess;
}
#endif
}

}

}
