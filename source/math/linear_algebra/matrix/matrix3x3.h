#ifndef MATRIX3X3_H
#define MATRIX3X3_H

#include "angle_unit_type.h"
#include "export_library.h"
#include "rotation_type.h"
#include "serializable.h"
#include "swappable.h"
#include "vector3d.h"
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
namespace number_systems { namespace complex { class Quat; } }

namespace linear_algebra
{

namespace matrix
{

/**
 * Class for 3x3 matrix storage and operations
 */
class Matrix3x3
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<Matrix3x3>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<Matrix3x3>
#endif
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using Eulers = geometric::orientation::Eulers;
    using Quat = number_systems::complex::Quat;
    using RotationType = geometric::orientation::RotationType;
    using Vector3d = linear_algebra::vector::Vector3d;

    /**
     * Constructor
     */
    EXPORT_STEM Matrix3x3(void);

    /**
     * Constructor
     */
    EXPORT_STEM Matrix3x3(const Vector3d &row0,
                          const Vector3d &row1,
                          const Vector3d &row2);

    /**
     * Constructor
     */
    EXPORT_STEM Matrix3x3(double row0col0, double row0col1, double row0col2,
                          double row1col0, double row1col1, double row1col2,
                          double row2col0, double row2col1, double row2col2);

    /**
     * Constructor
     */
    EXPORT_STEM Matrix3x3(double matrix[3][3]);

    /**
     * Copy constructor
     */
    EXPORT_STEM Matrix3x3(const Matrix3x3 &matrix);

    /**
     * Move constructor
     */
    EXPORT_STEM Matrix3x3(Matrix3x3 &&matrix);

    /**
     * Destructor
     */
    EXPORT_STEM ~Matrix3x3(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator = (const Matrix3x3 &matrix);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator = (Matrix3x3 &&matrix);

    /**
     * Subscript operator
     */
    EXPORT_STEM Vector3d &operator [] (int index);

    /**
     * Subscript operator
     */
    EXPORT_STEM Vector3d operator [] (int index) const;

    /**
     * Function call operator
     */
    EXPORT_STEM Matrix3x3 &operator () (const Vector3d &row0,
                                        const Vector3d &row1,
                                        const Vector3d &row2);

    /**
     * Function call operator
     */
    EXPORT_STEM Matrix3x3 &operator () (double row0col0, double row0col1, double row0col2,
                                        double row1col0, double row1col1, double row1col2,
                                        double row2col0, double row2col1, double row2col2);

    /**
     * Addition operator
     */
    EXPORT_STEM Matrix3x3 operator + (const Matrix3x3 &matrix) const;

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator += (const Matrix3x3 &matrix);

    /**
     * Subtraction operator
     */
    EXPORT_STEM Matrix3x3 operator - (const Matrix3x3 &matrix) const;

    /**
     * Subtraction assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator -= (const Matrix3x3 &matrix);

    /**
     * Multiplication operator
     */
    EXPORT_STEM Matrix3x3 operator * (const Matrix3x3 &matrix) const;

    /**
     * Multiplication operator
     */
    EXPORT_STEM Vector3d operator * (const Vector3d &vec) const;

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Vector3d operator * (const Vector3d &lhs,
                                            const Matrix3x3 &rhs);

    /**
     * Multiplication operator
     */
    EXPORT_STEM Matrix3x3 operator * (double value) const;

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Matrix3x3 operator * (double lhs,
                                             const Matrix3x3 &rhs);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator *= (const Matrix3x3 &matrix);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator *= (double value);

    /**
     * Division operator
     */
    EXPORT_STEM Matrix3x3 operator / (double value) const;

    /**
     * Division assignment operator
     */
    EXPORT_STEM Matrix3x3 &operator /= (double value);

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const Matrix3x3 &matrix);

    /**
     * Calculate Euler angles from this object's rotation matrix
     * @param angleUnits   the desired angle units of the return value, Degrees or Radians
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (Active by default). In a passive interpretation, the
     *                     resultant rotation matrix transforms a vector from one frame to another, while an
     *                     active interpretation produces a rotation matrix that rotates a vector through an
     *                     indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual Eulers calcEulers(const AngleUnitType &angleUnits = AngleUnitType::Degrees,
                                          const RotationType &rotationType = RotationType::Active) const final;

    /**
     * Calculate Euler angles from this object's rotation matrix
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (angles assumed to be in degrees). In a passive
     *                     interpretation, the resultant rotation matrix transforms a vector from one frame to
     *                     another, while an active interpretation produces a rotation matrix that rotates a
     *                     vector through an indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual Eulers calcEulers(const RotationType &rotationType) const final;

    /**
     * Calculate Euler angles from this object's rotation matrix
     * @param angleUnits   the desired angle output units, Degrees or Radians
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles (Active by default). In a passive interpretation, the
     *                     resultant rotation matrix transforms a vector from one frame to another, while an
     *                     active interpretation produces a rotation matrix that rotates a vector through an
     *                     indicated sequence of Euler angles within the same frame.
     */
    EXPORT_STEM virtual void calcEulers(double &roll,
                                        double &pitch,
                                        double &yaw,
                                        const AngleUnitType &angleUnits = AngleUnitType::Degrees,
                                        const RotationType &rotationType = RotationType::Active) const final;

    /**
     * Calculate Euler angles from this object's rotation matrix
     * @param rotationType enumeration specifying whether the function assumes an active or passive rotation in
     *                     calculating the Euler angles. In a passive interpretation, the resultant rotation
     *                     matrix transforms a vector from one frame to another, while an active interpretation
     *                     produces a rotation matrix that rotates a vector through an indicated sequence of
     *                     Euler angles within the same frame.
     */
    EXPORT_STEM virtual void calcEulers(double &roll,
                                        double &pitch,
                                        double &yaw,
                                        const RotationType &rotationType) const final;

    /**
     * Calculate the inverse matrix
     */
    EXPORT_STEM Matrix3x3 calcInverse(void) const;

    /**
     * Calculate a Quaternion from this object's rotation matrix
     * @param rotationType enumeration specifying whether the function assumes that the current object's
     *                     rotation matrix is an active or passive rotation in calculating the quaternion
     *                     (Active by default). In a passive interpretation, the resultant rotation matrix
     *                     transforms a vector from one frame to another, while an active interpretation
     *                     produces a rotation matrix that rotates a vector through an indicated sequence of
     *                     Euler angles within the same frame.
     */
    EXPORT_STEM virtual Quat calcQuaternion(const RotationType &rotationType =
                                            RotationType::Active) const final;

    /**
     * Calculate the scaled matrix
     */
    EXPORT_STEM virtual Matrix3x3 calcScaled(double value) const final;

    /**
     * Calculate the transpose matrix
     */
    EXPORT_STEM virtual Matrix3x3 calcTranspose(void) const final;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Compute the determinant of this matrix
     */
    EXPORT_STEM virtual double determinant(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Create a 3x3 identity matrix
     */
    static EXPORT_STEM Matrix3x3 identity(void);

    /**
     * Invert the current object's matrix and store in the current object
     */
    EXPORT_STEM virtual Matrix3x3 &invert(void) final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Scale the matrix and store in the current object
     */
    EXPORT_STEM virtual Matrix3x3 &scale(double value) final;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Matrix3x3 &matrix) override final;

    /**
     * Swap two elements within the matrix
     */
    EXPORT_STEM virtual bool swap(int i,
                                  int j) final;

    /**
     * Convert Matrix3x3 object to a double array
     */
    EXPORT_STEM virtual void toArray(double matrix[3][3]) const final;

    /**
     * Compute the transpose of this matrix and store in the current object
     */
    EXPORT_STEM virtual Matrix3x3 &transpose(void) final;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * 3-d vector object represents the first row of this 3x3 matrix
     */
    Vector3d m_row0;

    /**
     * 3-d vector object represents the second row of this 3x3 matrix
     */
    Vector3d m_row1;

    /**
     * 3-d vector object represents the third row of this 3x3 matrix
     */
    Vector3d m_row2;
};

}

}

}

#endif
