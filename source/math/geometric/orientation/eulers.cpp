#include "eulers.h"
#include "quat.h"
#include "matrix3x3.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "trigonometry.h"

// using namespace declarations
#ifdef RAPID_XML
using namespace rapidxml;
#endif

// using namespace declarations
using namespace math::linear_algebra::matrix;
using namespace math::linear_algebra::vector;
using namespace math::number_systems::complex;
using namespace math::trigonometric;

namespace math
{

namespace geometric
{

namespace orientation
{

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, Degrees or Radians
 */
Eulers::Eulers(const AngleUnitType &angleUnits)
: Eulers(0.0, 0.0, 0.0, angleUnits)
{
//  setRoll(0.0);
//  setPitch(0.0);
//  setYaw(0.0);
//  setAngleUnits(angleUnits);
}

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, Degrees or Radians
 */
Eulers::Eulers(double roll,
               double pitch,
               double yaw,
               const AngleUnitType &angleUnits)
: Vector3d(roll, pitch, yaw),
  m_angleUnits(angleUnits)
{

}

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, Degrees or Radians
 */
Eulers::Eulers(const Vector3d &eulers,
               const AngleUnitType &angleUnits)
: Eulers(eulers[EulerAxisType::Roll],
         eulers[EulerAxisType::Pitch],
         eulers[EulerAxisType::Yaw],
         angleUnits)
{

}

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, Degrees or Radians
 */
Eulers::Eulers(const double eulers[3],
               const AngleUnitType &angleUnits)
: Eulers(eulers[EulerAxisType::Roll],
         eulers[EulerAxisType::Pitch],
         eulers[EulerAxisType::Yaw],
         angleUnits)
{

}

/**
 * Copy constructor
 */
Eulers::Eulers(const Eulers &eulers)
: Vector3d(eulers)
{
    operator = (eulers);
}

/**
 * Move constructor
 */
Eulers::Eulers(Eulers &&eulers)
{
    operator = (std::move(eulers));
}

/**
 * Destructor
 */
Eulers::~Eulers(void)
{

}

/**
 * Copy assignment operator
 */
Eulers &Eulers::operator = (const Eulers &eulers)
{
    if (&eulers != this)
    {
        Vector3d::operator = (eulers);

        m_angleUnits = eulers.m_angleUnits;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Eulers &Eulers::operator = (Eulers &&eulers)
{
    if (&eulers != this)
    {
        Vector3d::operator = (std::move(eulers));

        m_angleUnits = std::move(eulers.m_angleUnits);
    }

    return *this;
}

/**
 * Function to calculate body Euler accelerations in radians from this object's Euler angles and input Euler
 * rates and accelerations
 */
Vector3d Eulers::calcBodyAccelerations(const Eulers &eulerRates,
                                       const Eulers &eulerAccelerations) const
{
    Vector3d alpha;
    auto rollAcc = eulerAccelerations.getRoll(AngleUnitType::Radians);
    auto rollRate = eulerRates.getRoll(AngleUnitType::Radians);
    auto pitchAcc = eulerAccelerations.getPitch(AngleUnitType::Radians);
    auto pitchRate = eulerRates.getPitch(AngleUnitType::Radians);
    auto yawAcc = eulerAccelerations.getYaw(AngleUnitType::Radians);
    auto yawRate = eulerRates.getYaw(AngleUnitType::Radians);
    if (rollAcc != 0.0 || rollRate != 0.0 ||
        pitchAcc != 0.0 || pitchRate != 0.0 ||
        yawAcc != 0.0 || yawRate != 0.0)
    {
        auto roll = m_vec[EulerAxisType::Roll];
        auto pitch = m_vec[EulerAxisType::Pitch];
        if (roll != 0.0 || pitch != 0.0)
        {
            auto cRoll = trigonometric::cos(roll, m_angleUnits);
            auto sRoll = trigonometric::sin(roll, m_angleUnits);
            auto cPitch = trigonometric::cos(pitch, m_angleUnits);
            auto sPitch = trigonometric::sin(pitch, m_angleUnits);

            // the equations that follow this comment block can be expanded into the following:
//          alpha[0] = rollAcc - yawAcc * sPitch - yawRate * pitchRate * cPitch;
//          alpha[1] = yawAcc * cPitch * sRoll + pitchAcc * cRoll - yawRate * pitchRate * sPitch * sRoll -
//                     pitchRate * rollRate * sRoll + yawRate * rollRate * cPitch * cRoll;
//          alpha[2] = yawAcc * cPitch * cRoll - pitchAcc * sRoll - yawRate * pitchRate * sPitch * cRoll -
//                     pitchRate * rollRate * cRoll - yawRate * rollRate * cPitch * sRoll;

            // compute p (alpha[0]), q (alpha[1]), r (alpha[2]) derivatives
            auto rollRate_cPitch = rollRate * cPitch;
            auto pitchRate_rollRate = pitchRate * rollRate;
            auto yawAcc_cPitch = yawAcc * cPitch;
            auto yawRate_pitchRate = yawRate * pitchRate;
            auto term1 = yawAcc_cPitch - yawRate_pitchRate * sPitch - pitchRate_rollRate;
            auto term2 = pitchAcc + yawRate * rollRate_cPitch;

            alpha[0] = rollAcc - yawAcc * sPitch - yawRate_pitchRate * cPitch; // pd
            alpha[1] = term1 * sRoll + term2 * cRoll; // qd
            alpha[2] = term1 * cRoll - term2 * sRoll; // rd
        }
        else
        {
            alpha[0] = rollAcc - yawRate * pitchRate; // pd
            alpha[1] = pitchAcc + yawRate * rollRate; // qd
            alpha[2] = yawAcc - pitchRate * rollRate; // rd
        }
    }

    return alpha;
}

/**
 * Function to calculate body Euler rates in radians from this object's Euler angles and input Euler rates
 */
Vector3d Eulers::calcBodyRates(const Eulers &eulerRates) const
{
    Vector3d omega;
    auto rollRate = eulerRates.getRoll(AngleUnitType::Radians);
    auto pitchRate = eulerRates.getPitch(AngleUnitType::Radians);
    auto yawRate = eulerRates.getYaw(AngleUnitType::Radians);
    if (rollRate != 0.0 || pitchRate != 0.0 || yawRate != 0.0)
    {
        auto roll = m_vec[EulerAxisType::Roll];
        auto pitch = m_vec[EulerAxisType::Pitch];
        if (roll != 0.0 || pitch != 0.0)
        {
            auto cRoll = trigonometric::cos(roll, m_angleUnits);
            auto sRoll = trigonometric::sin(roll, m_angleUnits);
            auto cPitch = trigonometric::cos(pitch, m_angleUnits);
            auto sPitch = trigonometric::sin(pitch, m_angleUnits);
            auto yawRate_cPitch = yawRate * cPitch;

            omega[0] = rollRate - yawRate * sPitch; // p
            omega[1] = yawRate_cPitch * sRoll + pitchRate * cRoll; // q
            omega[2] = yawRate_cPitch * cRoll - pitchRate * sRoll; // r
        }
        else
        {
            omega[0] = rollRate; // p
            omega[1] = pitchRate; // q
            omega[2] = yawRate; // r
        }
    }

    return omega;
}

/**
 * Function to calculate Euler accelerations from this object's body Euler angles and input body rates and
 * accelerations
 * @param omega a 3d vector that contains the body Euler rates in radians
 * @param alpha a 3d vector that contains the body Euler accelerations in radians
 */
Eulers Eulers::calcEulerAccelerations(const Vector3d &omega,
                                      const Vector3d &alpha) const
{
    return calcEulerAccelerations(omega, alpha, m_angleUnits);
}

/**
 * Function to calculate Euler accelerations from this object's body Euler angles and input body rates and
 * accelerations
 * @param omega      a 3d vector that contains the body Euler rates in radians
 * @param alpha      a 3d vector that contains the body Euler accelerations in radians
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
Eulers Eulers::calcEulerAccelerations(const Vector3d &omega,
                                      const Vector3d &alpha,
                                      const AngleUnitType &angleUnits) const
{
    // TODO: implement a switch statement that switches on Euler sequence. For now, this handles only ZYX
    //       convention
    Eulers eulerAccelerations(m_angleUnits);

    auto p = omega[0]; // body roll rate
    auto pd = alpha[0]; // body roll rate
    auto q = omega[1]; // body pitch rate
    auto qd = alpha[1]; // body pitch rate
    auto r = omega[2]; // body yaw rate
    auto rd = alpha[2]; // body yaw rate
    if (p != 0.0 || pd != 0.0 || q != 0.0 || qd != 0.0 || r != 0.0 || rd != 0.0)
    {
        auto roll = m_vec[EulerAxisType::Roll];
        auto pitch = m_vec[EulerAxisType::Pitch];
        eulerAccelerations.setAngleUnits(AngleUnitType::Radians);
        if (roll != 0.0 || pitch != 0.0)
        {
            auto cRoll = trigonometric::cos(roll, m_angleUnits);
            auto sRoll = trigonometric::sin(roll, m_angleUnits);
            auto cPitch = trigonometric::cos(pitch, m_angleUnits);
            auto sPitch = trigonometric::sin(pitch, m_angleUnits);

            if (std::fabs(cPitch) < 1.0e-10)
                cPitch = (cPitch < 0.0 ? -1.0 : 1.0) * 1.0e-10;

            // first calculate the Euler rates
            auto yawRate = (q * sRoll + r * cRoll) / cPitch;
            auto rollRate = p + yawRate * sPitch;
            auto pitchRate = q * cRoll - r * sRoll;

            // the equations that follow this comment block can be expanded into the following:
//          eulerAccelerations[Yaw] = pitchRate * (q * sRoll + r * cRoll) * sPitch / cPitch / cPitch
//                                  + (rd * cRoll - r * rollRate * sRoll
//                                  + qd * sRoll + q * rollRate * cRoll) / cPitch;
//          eulerAccelerations[Roll] = pd + eulerAccelerations[Yaw] * sPitch + yawRate * pitchRate * cPitch;
//          eulerAccelerations[Pitch] = -rd * sRoll - r * rollRate * cRoll + qd * cRoll - q * rollRate * sRoll;

            auto rollRate_cRoll = rollRate * cRoll;
            auto rollRate_sRoll = rollRate * sRoll;
            eulerAccelerations[EulerAxisType::Yaw] = (pitchRate * (rollRate - p)
                                                   + (rd * cRoll - r * rollRate_sRoll
                                                   + qd * sRoll + q * rollRate_cRoll)) / cPitch;
            eulerAccelerations[EulerAxisType::Roll] = pd + eulerAccelerations[EulerAxisType::Yaw] * sPitch +
                                                      yawRate * pitchRate * cPitch;
            eulerAccelerations[EulerAxisType::Pitch] = -rd * sRoll - r * rollRate_cRoll + qd * cRoll -
                                                       q * rollRate_sRoll;
        }
        else
        {
            eulerAccelerations[EulerAxisType::Roll] = pd + r * q;
            eulerAccelerations[EulerAxisType::Pitch] = qd - r * p;
            eulerAccelerations[EulerAxisType::Yaw] = rd + q * p;
        }

        eulerAccelerations.convertAngleUnits(angleUnits);
    }

    return eulerAccelerations;
}

/**
 * Function to calculate Euler rates from this object's body Euler angles and input body rates
 * @param omega a 3d vector that contains the body Euler rates in radians
 */
Eulers Eulers::calcEulerRates(const Vector3d &omega) const
{
    return calcEulerRates(omega, m_angleUnits);
}

/**
 * Function to calculate Euler rates from this object's body Euler angles and input body rates
 * @param omega      a 3d vector that contains the body Euler rates in radians
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
Eulers Eulers::calcEulerRates(const Vector3d &omega,
                              const AngleUnitType &angleUnits) const
{
    // TODO: implement a switch statement that switches on Euler sequence. For now, this handles only ZYX
    //       convention
    Eulers eulerRates(m_angleUnits);

    auto p = omega[0]; // body roll rate
    auto q = omega[1]; // body pitch rate
    auto r = omega[2]; // body yaw rate
    if (p != 0.0 || q != 0.0 || r != 0.0)
    {
        auto roll = m_vec[EulerAxisType::Roll];
        auto pitch = m_vec[EulerAxisType::Pitch];
        eulerRates.setAngleUnits(AngleUnitType::Radians);
        if (roll != 0.0 || pitch != 0.0)
        {
            auto cRoll = trigonometric::cos(roll, m_angleUnits);
            auto sRoll = trigonometric::sin(roll, m_angleUnits);
            auto cPitch = trigonometric::cos(pitch, m_angleUnits);
            auto sPitch = trigonometric::sin(pitch, m_angleUnits);

            if (std::fabs(cPitch) < 1.0e-10)
                cPitch = (cPitch < 0.0 ? -1.0 : 1.0) * 1.0e-10;

            eulerRates[EulerAxisType::Yaw] = (q * sRoll + r * cRoll) / cPitch;
            eulerRates[EulerAxisType::Roll] = p + eulerRates[EulerAxisType::Yaw] * sPitch;
            eulerRates[EulerAxisType::Pitch] = q * cRoll - r * sRoll;
        }
        else
        {
            eulerRates[EulerAxisType::Roll] = p;
            eulerRates[EulerAxisType::Pitch] = q;
            eulerRates[EulerAxisType::Yaw] = r;
        }

        eulerRates.convertAngleUnits(angleUnits);
    }

    return eulerRates;
}

/**
 * Function to calculate this object's angles in the interval [0, 360) or [0, 2 * pi)
 */
Eulers Eulers::calcPrincipal(void) const
{
    Eulers eulers(*this);

    return eulers.makePrincipal();
}

/**
 * Calculate a quaternion from this object's Euler angles
 * @param rotationType enumeration specifying whether the rotation quaternion is to be calculated using a
 *                     passive or an active interpretation (Active by default). In a passive interpretation,
 *                     the resultant quaternion transforms a vector from one frame to another, while an active
 *                     interpretation produces a quaternion that rotates a vector through an indicated sequence
 *                     of Euler angles within the same frame.
 */
Quat Eulers::calcQuaternion(const RotationType &rotationType) const
{
    // TODO: implement a switch statement that switches on Euler sequence. For now, this handles only ZYX
    //       convention

    auto roll = m_vec[EulerAxisType::Roll];
    auto pitch = m_vec[EulerAxisType::Pitch];
    auto yaw = m_vec[EulerAxisType::Yaw];

    auto cHalfRoll = trigonometric::cos(0.5 * roll, m_angleUnits);
    auto sHalfRoll = trigonometric::sin(0.5 * roll, m_angleUnits);
    auto cHalfPitch = trigonometric::cos(0.5 * pitch, m_angleUnits);
    auto sHalfPitch = trigonometric::sin(0.5 * pitch, m_angleUnits);
    auto cHalfYaw = trigonometric::cos(0.5 * yaw, m_angleUnits);
    auto sHalfYaw = trigonometric::sin(0.5 * yaw, m_angleUnits);

//  Quat qRoll(cHalfRoll, sHalfRoll, 0, 0);
//  Quat qPitch(cHalfPitch, 0, sHalfPitch, 0);
//  Quat qYaw(cHalfYaw, 0, 0, sHalfYaw);

//  Quat quat = qYaw * qPitch * qRoll;
    Quat quat(cHalfYaw * cHalfPitch * cHalfRoll + sHalfYaw * sHalfPitch * sHalfRoll,
              cHalfYaw * cHalfPitch * sHalfRoll - sHalfYaw * sHalfPitch * cHalfRoll,
              cHalfYaw * sHalfPitch * cHalfRoll + sHalfYaw * cHalfPitch * sHalfRoll,
              sHalfYaw * cHalfPitch * cHalfRoll - cHalfYaw * sHalfPitch * sHalfRoll);

    if (rotationType == RotationType::Passive)
        quat.conjugate();

//  Quat quat = qRoll.calcConjugate() * qPitch.calcConjugate() * qYaw.calcConjugate();
//  if (rotationType == Active)
//      quat.conjugate();

    return quat;
}

/**
 * Calculate a rotation matrix from this object's Euler angles
 * @param rotationType enumeration specifying whether the rotation matrix is to be calculated using a passive
 *                     or an active interpretation (Active by default). In a passive interpretation, the
 *                     resultant rotation matrix transforms a vector from one frame to another, while an active
 *                     interpretation produces a rotation matrix that rotates a vector through an indicated
 *                     sequence of Euler angles within the same frame.
 */
Matrix3x3 Eulers::calcRotationMatrix(const RotationType &rotationType) const
{
    // TODO: implement a switch statement that switches on Euler sequence. For now, this handles only ZYX
    //       convention

    auto roll = m_vec[EulerAxisType::Roll];
    auto pitch = m_vec[EulerAxisType::Pitch];
    auto yaw = m_vec[EulerAxisType::Yaw];

    auto cRoll = trigonometric::cos(roll, m_angleUnits);
    auto sRoll = trigonometric::sin(roll, m_angleUnits);
    auto cPitch = trigonometric::cos(pitch, m_angleUnits);
    auto sPitch = trigonometric::sin(pitch, m_angleUnits);
    auto cYaw = trigonometric::cos(yaw, m_angleUnits);
    auto sYaw = trigonometric::sin(yaw, m_angleUnits);

    Matrix3x3 matrix;
    matrix[0][0] = cPitch * cYaw;
    matrix[0][1] = sRoll * sPitch * cYaw - cRoll * sYaw;
    matrix[0][2] = cRoll * sPitch * cYaw + sRoll * sYaw;
    matrix[1][0] = cPitch * sYaw;
    matrix[1][1] = sRoll * sPitch * sYaw + cRoll * cYaw;
    matrix[1][2] = cRoll * sPitch * sYaw - sRoll * cYaw;
    matrix[2][0] = -sPitch;
    matrix[2][1] = sRoll * cPitch;
    matrix[2][2] = cRoll * cPitch;

    if (rotationType == RotationType::Passive)
    {
        std::swap(matrix[0][1], matrix[1][0]);
        std::swap(matrix[0][2], matrix[2][0]);
        std::swap(matrix[1][2], matrix[2][1]);
    }

    return matrix;
}

/**
 * Convert the units of this object's Euler quantities (Degrees or Radians)
 */
void Eulers::convertAngleUnits(const AngleUnitType &angleUnits)
{
    if (m_angleUnits != angleUnits)
    {
        double cnv = 1.0;
        if (angleUnits == AngleUnitType::Degrees && m_angleUnits == AngleUnitType::Radians)
            cnv = RADIANS_TO_DEGREES;
        else if (angleUnits == AngleUnitType::Radians && m_angleUnits == AngleUnitType::Degrees)
            cnv = DEGREES_TO_RADIANS;

        if (cnv != 1.0)
        {
            m_vec[EulerAxisType::Roll] *= cnv;
            m_vec[EulerAxisType::Pitch] *= cnv;
            m_vec[EulerAxisType::Yaw] *= cnv;
        }

        m_angleUnits = angleUnits;
    }
}

/**
 * Function to deserialize this object's data
 */
std::istream &Eulers::deserialize(std::istream &stream)
{
    Vector3d::deserialize(stream);

    if (stream)
    {
        stream.read((char *)&m_angleUnits, sizeof(int));
    }

    return stream;
}

/**
 * Get the value of the Euler specified by EulerAxisType
 */
double &Eulers::get(const EulerAxisType &type)
{
    return Vector3d::get(type);
}

/**
 * Get the value of the Euler specified by EulerAxisType
 */
double Eulers::get(const EulerAxisType &type) const
{
    return get(type, m_angleUnits);
}

/**
 * Get the value of the Euler specified by EulerAxisType
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double Eulers::get(const EulerAxisType &type,
                   const AngleUnitType &angleUnits) const
{
    auto value = m_vec[type];
    if (m_angleUnits != angleUnits)
    {
        double cnv = 1.0;
        if (angleUnits == AngleUnitType::Degrees && m_angleUnits == AngleUnitType::Radians)
            cnv = RADIANS_TO_DEGREES;
        else if (angleUnits == AngleUnitType::Radians && m_angleUnits == AngleUnitType::Degrees)
            cnv = DEGREES_TO_RADIANS;

        if (cnv != 1.0)
            value *= cnv;
    }

    return value;
}

/**
 * Get angle units (Degrees or Radians)
 */
AngleUnitType Eulers::getAngleUnits(void) const
{
    return m_angleUnits;
}

/**
 * Get the name of this class
 */
std::string Eulers::getClassName(void) const
{
    return "Eulers";
}

/**
 * Get this object's pitch angle
 */
double &Eulers::getPitch(void)
{
    return m_vec[EulerAxisType::Pitch];
}

/**
 * Get this object's pitch angle
 */
double Eulers::getPitch(void) const
{
    return getPitch(m_angleUnits);
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double Eulers::getPitch(const AngleUnitType &angleUnits) const
{
    return get(EulerAxisType::Pitch, angleUnits);
}

/**
 * Get this object's roll angle
 */
double &Eulers::getRoll(void)
{
    return m_vec[EulerAxisType::Roll];
}

/**
 * Get this object's roll angle
 */
double Eulers::getRoll(void) const
{
    return getRoll(m_angleUnits);
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double Eulers::getRoll(const AngleUnitType &angleUnits) const
{
    return get(EulerAxisType::Roll, angleUnits);
}

/**
 * Get this object's yaw angle
 */
double &Eulers::getYaw(void)
{
    return m_vec[EulerAxisType::Yaw];
}

/**
 * Get this object's yaw angle
 */
double Eulers::getYaw(void) const
{
    return getYaw(m_angleUnits);
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double Eulers::getYaw(const AngleUnitType &angleUnits) const
{
    return get(EulerAxisType::Yaw, angleUnits);
}

/**
 * Determines whether or not two sets of Euler angles are equivalent within the specified tolerance. Function
 * uses the principal value of the angles to perform the test.
 */
bool Eulers::isEqual(const Eulers &eulers,
                     double tol) const
{
    return calcPrincipal().Vector3d::isEqual(eulers.calcPrincipal(), tol);
}

/**
 * Function to restrict this object's angles to the interval [0, 360) or [0, 2 * pi)
 */
Eulers &Eulers::makePrincipal(void)
{
    double upperLimit = 360.0;
    if (m_angleUnits == AngleUnitType::Radians)
        upperLimit *= DEGREES_TO_RADIANS;

    for (int i = 0; i < 3; ++i)
    {
        m_vec[i] = std::fmod(m_vec[i], upperLimit);
        m_vec[i] = std::fmod(m_vec[i] + upperLimit, upperLimit);
        if (m_vec[i] > 0.5 * upperLimit)
            m_vec[i] -= upperLimit;
    }

    return *this;
}

/**
 * Output stream print function
 */
std::ostream &Eulers::print(std::ostream &stream) const
{
    Vector3d::print(stream);

    if (stream)
    {
        stream << " (" << std::string(m_angleUnits) << ")";
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool Eulers::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pYawNode = pNode->first_node("yaw");
        if (pYawNode != nullptr)
            m_vec[EulerAxisType::Yaw] = std::stod(pYawNode->value());

        auto *pPitchNode = pNode->first_node("pitch");
        if (pPitchNode != nullptr)
            m_vec[EulerAxisType::Pitch] = std::stod(pPitchNode->value());

        auto *pRollNode = pNode->first_node("roll");
        if (pRollNode != nullptr)
            m_vec[EulerAxisType::Roll] = std::stod(pRollNode->value());
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &Eulers::serialize(std::ostream &stream) const
{
    Vector3d::serialize(stream);

    if (stream)
    {
        stream.write((const char *)&m_angleUnits, sizeof(int));
    }

    return stream;
}

/**
 * Set the value of the Euler specified by EulerAxisType
 */
void Eulers::set(const EulerAxisType &type,
                 double value)
{
    return set(type, value, m_angleUnits);
}

/**
 * Set the value of the Euler specified by EulerAxisType
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void Eulers::set(const EulerAxisType &type,
                 double value,
                 const AngleUnitType &angleUnits)
{
    if (m_angleUnits != angleUnits)
    {
        double cnv = 1.0;
        if (angleUnits == AngleUnitType::Degrees && m_angleUnits == AngleUnitType::Radians)
            cnv = DEGREES_TO_RADIANS;
        else if (angleUnits == AngleUnitType::Radians && m_angleUnits == AngleUnitType::Degrees)
            cnv = RADIANS_TO_DEGREES;

        if (cnv != 1.0)
            value *= cnv;
    }

    m_vec[type] = value;
}

/**
 * Set angle units (Degrees or Radians)
 */
void Eulers::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
}

/**
 * Set this object's pitch angle
 */
void Eulers::setPitch(double pitch)
{
    setPitch(pitch, m_angleUnits);
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void Eulers::setPitch(double pitch,
                      const AngleUnitType &angleUnits)
{
    set(EulerAxisType::pitch(), pitch, angleUnits);
}

/**
 * Set this object's roll angle
 */
void Eulers::setRoll(double roll)
{
    setRoll(roll, m_angleUnits);
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void Eulers::setRoll(double roll,
                     const AngleUnitType &angleUnits)
{
    set(EulerAxisType::roll(), roll, angleUnits);
}

/**
 * Set this object's yaw angle
 */
void Eulers::setYaw(double yaw)
{
    setYaw(yaw, m_angleUnits);
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void Eulers::setYaw(double yaw,
                    const AngleUnitType &angleUnits)
{
    set(EulerAxisType::yaw(), yaw, angleUnits);
}

/**
 * Swap function
 */
void Eulers::swap(Eulers &eulers)
{
    Vector3d::swap(eulers);

    std::swap(m_angleUnits, eulers.m_angleUnits);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool Eulers::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto *pYawNode = pDocument->allocate_node(node_element, "yaw");
            auto *pYawString = pDocument->allocate_string(std::to_string(m_vec[0]).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pYawString);
            pYawNode->append_node(pDataNode);
            pNode->append_node(pYawNode);
        }

        if (bSuccess)
        {
            auto *pPitchNode = pDocument->allocate_node(node_element, "pitch");
            auto *pPitchString = pDocument->allocate_string(std::to_string(m_vec[1]).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pPitchString);
            pPitchNode->append_node(pDataNode);
            pNode->append_node(pPitchNode);
        }

        if (bSuccess)
        {
            auto *pRollNode = pDocument->allocate_node(node_element, "roll");
            auto *pRollString = pDocument->allocate_string(std::to_string(m_vec[2]).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pRollString);
            pRollNode->append_node(pDataNode);
            pNode->append_node(pRollNode);
        }
    }

    return bSuccess;
}
#endif

}

}

}
