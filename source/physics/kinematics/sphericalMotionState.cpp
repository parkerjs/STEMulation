#include "angle_unit_type.h"
#include "cartesianMotionState.h"
#include "kinematicState.h"
#include "math_constants.h"
#include "referenceFrame.h"
#include "spherical_acceleration_axis_type.h"
#include "spherical_position_axis_type.h"
#include "spherical_velocity_axis_type.h"
#include "sphericalMotionState.h"
#include "trigonometry.h"

// using namespace declarations
using namespace math;
using namespace math::trigonometric;

namespace physics
{

namespace kinematics
{
#if 0
/**
 * Convert spherical motion state with vertical angle, rate, and acceleration described with respect to
 * elevation (measured from x-y plane to +z) to vertical angle, rate, and acceleration described with respect
 * to zenith (measured from +z)
 */
bool bRegisteredSphericalToCartesianConversion =
MotionState::registerCoordinateSystemConversionPair(CoordinateType(CoordinateType::Enum::Spherical),
                                                    CoordinateType(CoordinateType::Enum::Cartesian),
[] (const MotionState *pInputMotionState, MotionState *pOutputMotionState, double t)
{
    auto *pCartesianMotionState = dynamic_cast<CartesianMotionState *>(pOutputMotionState);
    auto *pSphericalMotionState = dynamic_cast<const SphericalMotionState *>(pInputMotionState);
    bool bSuccess = (pCartesianMotionState != nullptr && pSphericalMotionState != nullptr);
    if (bSuccess)
        bSuccess = pSphericalMotionState->calcCartesianState(pCartesianMotionState, t);

    return bSuccess;
});

/**
 * Register coordinate conversion function that maps spherical to spherical
 */
bool bRegisteredSphericalToSphericalConversion =
MotionState::registerCoordinateSystemConversionPair(CoordinateType(CoordinateType::Enum::Spherical),
                                                    CoordinateType(CoordinateType::Enum::Spherical),
[] (const MotionState *pInputMotionState, MotionState *pOutputMotionState, double t)
{
    if (pInputMotionState != pOutputMotionState)
    {
        auto *pInputSphericalMotionState = dynamic_cast<const SphericalMotionState *>(pInputMotionState);
        auto *pOutputSphericalMotionState = dynamic_cast<SphericalMotionState *>(pOutputMotionState);
        if (pInputSphericalMotionState != nullptr && pOutputSphericalMotionState != nullptr)
            *pOutputSphericalMotionState = *pInputSphericalMotionState;
        else
            return false;
    }

    return true;
});
#endif
/**
 * Convert spherical motion state with vertical angle, rate, and acceleration described with respect to
 * elevation (measured from x-y plane to +z) to vertical angle, rate, and acceleration described with respect
 * to zenith (measured from +z)
 */
void elevationToZenith(SphericalMotionState *pSphericalMotionState)
{
    if (pSphericalMotionState != nullptr)
    {
        auto elevation = pSphericalMotionState->get(SphericalPositionAxisType::Vertical);
        auto elevationAcceleration = pSphericalMotionState->get(SphericalAccelerationAxisType::Vertical);
        auto elevationRate = pSphericalMotionState->get(SphericalVelocityAxisType::Vertical);

        auto angleShift = PI_OVER_TWO;
        if (pSphericalMotionState->getAngleUnits() == AngleUnitType::Degrees)
            angleShift *= RADIANS_TO_DEGREES;

        auto zenith = angleShift - elevation;
        auto zenithAcceleration = -elevationAcceleration;
        auto zenithRate = -elevationRate;

        pSphericalMotionState->set(SphericalAccelerationAxisType::Vertical, zenithAcceleration);
        pSphericalMotionState->set(SphericalPositionAxisType::Vertical, zenith);
        pSphericalMotionState->set(SphericalVelocityAxisType::Vertical, zenithRate);
    }
}

/**
 * Convert spherical motion state with vertical angle, rate, and acceleration described with respect to
 * negative elevation (measured from x-y plane to -z) to vertical angle, rate, and acceleration described with
 * respect to zenith (measured from +z)
 */
void negativeElevationToZenith(SphericalMotionState *pSphericalMotionState)
{
    if (pSphericalMotionState != nullptr)
    {
        auto negativeElevation = pSphericalMotionState->get(SphericalPositionAxisType::Vertical);

        auto angleShift = PI_OVER_TWO;
        if (pSphericalMotionState->getAngleUnits() == AngleUnitType::Degrees)
            angleShift *= RADIANS_TO_DEGREES;

        auto zenith = angleShift + negativeElevation;

        pSphericalMotionState->set(SphericalPositionAxisType::Vertical, zenith);
    }
}

/**
 * Convert spherical motion state with vertical angle, angular rate, and angular acceleration described with
 * respect to zenith (measured from +z) to vertical angle, rate, and acceleration described with respect to
 * elevation (measured from x-y plane toward +z)
 */
void zenithToElevation(SphericalMotionState *pSphericalMotionState)
{
    if (pSphericalMotionState != nullptr)
    {
        auto zenith = pSphericalMotionState->get(SphericalPositionAxisType::Vertical);
        auto zenithAcceleration = pSphericalMotionState->get(SphericalAccelerationAxisType::Vertical);
        auto zenithRate = pSphericalMotionState->get(SphericalVelocityAxisType::Vertical);

        auto angleShift = PI_OVER_TWO;
        if (pSphericalMotionState->getAngleUnits() == AngleUnitType::Degrees)
            angleShift *= RADIANS_TO_DEGREES;

        auto elevation = angleShift - zenith;
        auto elevationAcceleration = -zenithAcceleration;
        auto elevationRate = -zenithRate;

        pSphericalMotionState->set(SphericalAccelerationAxisType::Vertical, elevationAcceleration);
        pSphericalMotionState->set(SphericalPositionAxisType::Vertical, elevation);
        pSphericalMotionState->set(SphericalVelocityAxisType::Vertical, elevationRate);
    }
}

/**
 * Convert spherical motion state with vertical angle, angular rate, and angular acceleration described with
 * respect to zenith (measured from +z) to vertical angle, rate, and acceleration described with respect to
 * negative elevation (measured from x-y plane toward -z)
 */
void zenithToNegativeElevation(SphericalMotionState *pSphericalMotionState)
{
    if (pSphericalMotionState != nullptr)
    {
        auto zenith = pSphericalMotionState->get(SphericalPositionAxisType::Vertical);

        auto angleShift = PI_OVER_TWO;
        if (pSphericalMotionState->getAngleUnits() == AngleUnitType::Degrees)
            angleShift *= RADIANS_TO_DEGREES;

        auto negativeElevation = zenith - angleShift;

        pSphericalMotionState->set(SphericalPositionAxisType::Vertical, negativeElevation);
    }
}

/**
 * Construct an empty motion state object
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(const std::string &state)
: MotionState(state)
{

}

/**
 * Construct an empty motion state at the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const std::string &state)
: MotionState(t, state)
{

}

/**
 * Construct a motion state with the given spherical position at the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const Vector3d &position,
                                           const std::string &state)
: MotionState(t, position, state)
{

}

/**
 * Construct a motion state with the given spherical position and initial velocity at the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const std::string &state)
: MotionState(t, position, velocity, state)
{

}

/**
 * Construct a motion state with the given spherical position, initial velocity, and constant acceleration at
 * the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const Vector3d &acceleration,
                                           const std::string &state)
: MotionState(t, position, velocity, acceleration, state)
{

}

/**
 * Construct a motion state with the given spherical position, initial velocity, constant acceleration and
 * Euler orientation at the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const Vector3d &acceleration,
                                           const Eulers &eulers,
                                           const AngleUnitType &angleUnits,
                                           const std::string &state)
: MotionState(t, position, velocity, acceleration, eulers, angleUnits, state)
{

}

/**
 * Construct a motion state with the given spherical position, initial velocity, constant acceleration, Euler
 * orientation and Euler rates at the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const Vector3d &acceleration,
                                           const Eulers &eulers,
                                           const Eulers &eulerRates,
                                           const AngleUnitType &angleUnits,
                                           const std::string &state)
: MotionState(t, position, velocity, acceleration, eulers, eulerRates, angleUnits, state)
{

}

/**
 * Construct a motion state with the given spherical position, initial velocity, constant acceleration, Euler
 * orientation, Euler rates, and Euler accelerations at the specified time
 * @param state the perturbation state of this object's reference frame
 */
SphericalMotionState::SphericalMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const Vector3d &acceleration,
                                           const Eulers &eulers,
                                           const Eulers &eulerRates,
                                           const Eulers &eulerAccelerations,
                                           const AngleUnitType &angleUnits,
                                           const std::string &state)
: MotionState(t, position, velocity, acceleration, eulers, eulerRates, eulerAccelerations, angleUnits, state)
{

}

/**
 * Construct a spherical motion state from an abstract motion state constant reference
 */
SphericalMotionState::SphericalMotionState(const MotionState &motionState)
{
    if (motionState.isSpherical())
        operator = (dynamic_cast<const SphericalMotionState &>(motionState));
    else
    {
        cacheTransformationsEnabled(motionState.cacheTransformationsEnabled());
        debugTransformsEnabled(motionState.debugTransformsEnabled());
        setConversionFromAzimuthZenithFunction(motionState.getConversionFromAzimuthZenithFunction());
        setConversionToAzimuthZenithFunction(motionState.getConversionToAzimuthZenithFunction());
        motionState.calcSphericalState(this);
    }
}

/**
 * Construct a spherical motion state from a Cartesian motion state
 */
SphericalMotionState::SphericalMotionState(const CartesianMotionState &cartesianMotionState)
{
    SphericalMotionState sphericalMotionState(cartesianMotionState.getFrameState());

    sphericalMotionState.cacheTransformationsEnabled(cartesianMotionState.cacheTransformationsEnabled());
    sphericalMotionState.debugTransformsEnabled(cartesianMotionState.debugTransformsEnabled());

    auto *pFromConverter = cartesianMotionState.getConversionFromAzimuthZenithFunction();
    auto *pToConverter = cartesianMotionState.getConversionToAzimuthZenithFunction();

    sphericalMotionState.setConversionFromAzimuthZenithFunction(pFromConverter);
    sphericalMotionState.setConversionToAzimuthZenithFunction(pToConverter);
    cartesianMotionState.calcSphericalState(&sphericalMotionState);

    operator = (sphericalMotionState);
}

/**
 * Copy constructor
 */
SphericalMotionState::SphericalMotionState(const SphericalMotionState &motionState)
: MotionState(motionState)
{

}

/**
 * Move constructor
 */
SphericalMotionState::SphericalMotionState(SphericalMotionState &&motionState)
: MotionState(std::move(motionState))
{

}

/**
 * Destructor
 */
SphericalMotionState::~SphericalMotionState(void)
{

}

/**
 * Assignment operator
 */
SphericalMotionState &SphericalMotionState::operator = (const MotionState &motionState)
{
    if (&motionState != this)
    {
        assign(&motionState);
    }

    return *this;
}

/**
 * Copy assignment operator
 */
SphericalMotionState &SphericalMotionState::operator = (const SphericalMotionState &motionState)
{
    if (&motionState != this)
    {
        MotionState::operator = (motionState);
    }

    return *this;
}

/**
 * Move assignment operator
 */
SphericalMotionState &SphericalMotionState::operator = (SphericalMotionState &&motionState)
{
    if (&motionState != this)
    {
        motionState.swap(*this);
    }

    return *this;
}

/**
 * Assign the contents of one motion state to another
 * @param pMotionState a pointer to the source motion state
 */
bool SphericalMotionState::assign(const MotionState *pMotionState)
{
    bool bSuccess = (pMotionState != this);
    if (bSuccess)
    {
        if (pMotionState->isCartesian())
        {
            cacheTransformationsEnabled(pMotionState->cacheTransformationsEnabled());
            debugTransformsEnabled(pMotionState->debugTransformsEnabled());
            setConversionFromAzimuthZenithFunction(pMotionState->getConversionFromAzimuthZenithFunction());
            setConversionToAzimuthZenithFunction(pMotionState->getConversionToAzimuthZenithFunction());
            pMotionState->calcSphericalState(this);
        }
        else if (pMotionState->isSpherical())
        {
            auto &&sphericalMotionState = dynamic_cast<const SphericalMotionState &>(*pMotionState);

            MotionState::operator = (sphericalMotionState);
        }
    }

    return bSuccess;
}

/**
 * Calculate the scalar acceleration
 */
double SphericalMotionState::calcAcceleration(void) const
{
    return m_pState->getAcceleration().get(SphericalAccelerationAxisType::Radial);

}

/**
 * Calculate the time at which the current object's motion state approaches the null motion state at the
 * desired range. The time returned represents the closest time, whether it be in the future or past, at which
 * the motion state approaches the given range. If the motion state does not approach the specified range, the
 * subroutine returns infinity
 */
double SphericalMotionState::calcApproachTime(double range) const
{
    CartesianMotionState motionState(m_pState->getTime(), m_frameState);
    motionState.setFrame(m_pFrame);

    return MotionState::calcApproachTime(range, motionState);
}

/**
 * Calculate the Cartesian motion state from the current object's spherical motion state at the specified time
 * @param pCartesianMotionState the object in which the resultant Cartesian state will be stored
 * @param t                     the time to which the resultant motion state will be projected
 */
bool SphericalMotionState::calcCartesianState(CartesianMotionState *pCartesianMotionState,
                                              double t) const
{
    bool bSuccess = (pCartesianMotionState != nullptr);
    if (bSuccess)
    {
        auto *pSphericalMotionState = this;
        if (m_pToAzimuthZenithConverter != nullptr)
        {
            pSphericalMotionState = new SphericalMotionState(*this);
            (*m_pToAzimuthZenithConverter)(const_cast<SphericalMotionState *>(pSphericalMotionState));
        }
/*
        \verbatim
        // original equations:
        double x = r * sinZe * cosAz;
        double y = r * sinZe * sinAz;
        double z = r * cosZe;
        double xd = rd * sinZe * cosAz + r * zed * cosZe * cosAz - r * azd * sinZe * sinAz;
        double yd = rd * sinZe * sinAz + r * zed * cosZe * sinAz + r * azd * sinZe * cosAz;
        double zd = rd * cosZe         - r * zed * sinZe;
        double xdd = (rdd - r * (zed * zed + azd * azd)) * sinZe * cosAz
                   - (2 * rd * azd + r * azdd) * sinZe * sinAz
                   + (2 * rd * zed + r * zedd) * cosZe * cosAz
                   - (2 * r * zed * azd) * cosZe * sinAz;
        double ydd = (rdd - r * (zed * zed + azd * azd)) * sinZe * sinAz
                   + (2 * rd * azd + r * azdd) * sinZe * cosAz
                   + (2 * rd * zed + r * zedd) * cosZe * sinAz
                   + (2 * r * zed * azd) * cosZe * cosAz;
        double zdd = (rdd - r * zed * zed) * cosZe - (2 * rd * zed + r * zedd) * sinZe;
        \endverbatim
 */
        auto angleUnits = m_pState->getAngleUnits();
        double az = 0.0, r = 0.0, rSinZe = 0.0, x = 0.0, y = 0.0, z = 0.0, ze = 0.0;
        double cosAz = 1.0, sinAz = 0.0, cosZe = 1.0, sinZe = 0.0;
        auto &&pos = pSphericalMotionState->getPosition(t);
        if (pos != 0.0)
        {
            pos.get(az, ze, r);

            cosAz = trigonometric::cos(az, angleUnits);
            sinAz = trigonometric::sin(az, angleUnits);
            cosZe = trigonometric::cos(ze, angleUnits);
            sinZe = trigonometric::sin(ze, angleUnits);

            if (pos[SphericalPositionAxisType::Radial] != 0.0)
            {
                rSinZe = r * sinZe;
                x = rSinZe * cosAz;
                y = rSinZe * sinAz;
                z = r * cosZe;
            }
        }

        double azd = 0.0, rd = 0.0, xd = 0.0, yd = 0.0, zd = 0.0, zed = 0.0;
        auto &&vel = pSphericalMotionState->getVelocity(t);
        bool bMotionStateHasVelocity = (vel != 0.0);
        if (bMotionStateHasVelocity)
        {
            vel.get(azd, zed, rd);
            if (angleUnits == AngleUnitType::Degrees)
            {
                azd *= DEGREES_TO_RADIANS;
                zed *= DEGREES_TO_RADIANS;
            }

            auto rdSinZe = rd * sinZe;
            auto rZedCosZe = z * zed;
            auto rAzdSinZe = rSinZe * azd;
            xd = (rdSinZe + rZedCosZe) * cosAz - rAzdSinZe * sinAz;
            yd = (rdSinZe + rZedCosZe) * sinAz + rAzdSinZe * cosAz;
            zd = rd * cosZe - r * zed * sinZe;
        }

        double xdd = 0.0, ydd = 0.0, zdd = 0.0;
        auto &&acc = pSphericalMotionState->getAcceleration();
        bool bMotionStateHasAcceleration = (acc != 0.0);
        if (bMotionStateHasVelocity || bMotionStateHasAcceleration)
        {
            auto azdd = acc[SphericalAccelerationAxisType::Horizontal];
            auto zedd = acc[SphericalAccelerationAxisType::Vertical];
            if (angleUnits == AngleUnitType::Degrees)
            {
                azdd *= DEGREES_TO_RADIANS;
                zedd *= DEGREES_TO_RADIANS;
            }

            auto rdd = acc[SphericalAccelerationAxisType::Radial];
            double rAzd = 0.0, rAzdSq = 0.0, rZedSq = 0.0, rdd_rZedSq_rAzdSqSinZe = rdd, twoRdZed_rZedd = 0.0;
            auto twoRdAzd_rAzddSinZe = 0.0, twoR_ZedAzdCosZe = 0.0;
            if (bMotionStateHasVelocity)
            {
                rAzd = r * azd;
                rAzdSq = rAzd * azd;
                rZedSq = r * zed * zed;
                rdd_rZedSq_rAzdSqSinZe -= rAzdSq + rZedSq;
                twoRdZed_rZedd = 2 * rd * zed;
                twoRdAzd_rAzddSinZe = 2 * rd * azd;
                twoR_ZedAzdCosZe = 2 * rAzd * zed * cosZe;
            }

            if (bMotionStateHasAcceleration)
            {
                twoRdZed_rZedd += r * zedd;
                twoRdAzd_rAzddSinZe += r * azdd;
            }

            auto twoRdZed_rZeddCosZe = twoRdZed_rZedd * cosZe;
            twoRdAzd_rAzddSinZe *= sinZe;
            rdd_rZedSq_rAzdSqSinZe *= sinZe;

            xdd = (rdd_rZedSq_rAzdSqSinZe + twoRdZed_rZeddCosZe) * cosAz
                - (twoRdAzd_rAzddSinZe + twoR_ZedAzdCosZe) * sinAz;
            ydd = (rdd_rZedSq_rAzdSqSinZe + twoRdZed_rZeddCosZe) * sinAz
                + (twoRdAzd_rAzddSinZe + twoR_ZedAzdCosZe) * cosAz;
            zdd = (rdd - rZedSq) * cosZe - twoRdZed_rZedd * sinZe;
        }

        auto &&eulerAccelerations = pSphericalMotionState->getEulerAccelerations();
        auto &&eulerRates = pSphericalMotionState->getEulerRates(t);
        auto &&eulers = pSphericalMotionState->getEulers(t);

        pCartesianMotionState->setAcceleration(xdd, ydd, zdd);
        pCartesianMotionState->setAngleUnits(pSphericalMotionState->getAngleUnits());
        pCartesianMotionState->setEulerAccelerations(eulerAccelerations);
        pCartesianMotionState->setEulerRates(eulerRates);
        pCartesianMotionState->setEulers(eulers);
        pCartesianMotionState->setPosition(x, y, z);
        pCartesianMotionState->setFrame(m_pFrame);
        pCartesianMotionState->setFrameState(m_frameState);
        pCartesianMotionState->setTime(t);
        pCartesianMotionState->setVelocity(xd, yd, zd);

        // set the angle units of the Cartesian motion state to those of the target angle units
        pCartesianMotionState->convertAngleUnits(angleUnits);

        if (pSphericalMotionState != this)
            delete pSphericalMotionState;
    }

    return bSuccess;
}

/**
 * Calculate the minimum approach distance achieved with respect to another motion state
 * @param tApproach the calculated time at which the minimum distance is achieved
 */
double SphericalMotionState::calcMinimumApproach(double &tApproach) const
{
    CartesianMotionState motionState(m_pState->getTime(), m_frameState);
    motionState.setFrame(m_pFrame);

    return MotionState::calcMinimumApproach(tApproach, motionState);
}

/**
 * Calculate the range at time t
 */
double SphericalMotionState::calcRange(double t) const
{
    return calcRange(t, SphericalMotionState(m_pState->getTime(), m_frameState));
}

/**
 * Calculate the range with respect to another motion state at time t
 */
double SphericalMotionState::calcRange(double t,
                                       const MotionState &motionState) const
{
    CartesianMotionState thisCartesianMotionState(*this);

    if (motionState.isCartesian())
        return thisCartesianMotionState.calcRange(motionState);
    else
    {
        auto &&sphericalMotionState = static_cast<const SphericalMotionState &>(motionState);
        CartesianMotionState cartesianMotionState(sphericalMotionState);

        return thisCartesianMotionState.calcRange(cartesianMotionState);
    }
}

/**
 * Calculate the range acceleration at time t
 */
double SphericalMotionState::calcRangeAcceleration(double t) const
{
    return calcRangeAcceleration(t, SphericalMotionState(m_pState->getTime(), m_frameState));
}

/**
 * Calculate the range acceleration with respect to another motion state at time t
 */
double SphericalMotionState::calcRangeAcceleration(double t,
                                                   const MotionState &motionState) const
{
    CartesianMotionState thisCartesianMotionState(*this);

    if (motionState.isCartesian())
        return thisCartesianMotionState.calcRangeAcceleration(motionState);
    else
    {
        auto &&sphericalMotionState = static_cast<const SphericalMotionState &>(motionState);
        CartesianMotionState cartesianMotionState(sphericalMotionState);

        return thisCartesianMotionState.calcRangeAcceleration(cartesianMotionState);
    }
}

/**
 * Calculate the range rate at time t
 */
double SphericalMotionState::calcRangeRate(double t) const
{
    return calcRangeRate(t, SphericalMotionState(m_pState->getTime(), m_frameState));
}

/**
 * Calculate the range rate with respect to another motion state at time t
 */
double SphericalMotionState::calcRangeRate(double t,
                                           const MotionState &motionState) const
{
    CartesianMotionState thisCartesianMotionState(*this);

    if (motionState.isCartesian())
        return thisCartesianMotionState.calcRangeRate(motionState);
    else
    {
        auto &&sphericalMotionState = static_cast<const SphericalMotionState &>(motionState);
        CartesianMotionState cartesianMotionState(sphericalMotionState);

        return thisCartesianMotionState.calcRangeRate(cartesianMotionState);
    }
}

/**
 * Calculate the scalar speed at time t
 */
double SphericalMotionState::calcSpeed(double t) const
{
    return m_pState->getVelocity().get(SphericalVelocityAxisType::Radial);
}

/**
 * clone() function
 */
SphericalMotionState *SphericalMotionState::clone(void) const
{
    return new SphericalMotionState(*this);
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void SphericalMotionState::convertAngleUnits(const AngleUnitType &angleUnits)
{
    if (m_pState->getAngleUnits() != angleUnits)
    {
        double cnv = 1.0;
        if (angleUnits == AngleUnitType::Degrees && m_pState->getAngleUnits() == AngleUnitType::Radians)
            cnv = RADIANS_TO_DEGREES;
        else if (angleUnits == AngleUnitType::Radians && m_pState->getAngleUnits() == AngleUnitType::Degrees)
            cnv = DEGREES_TO_RADIANS;

        if (cnv != 1.0)
        {
            auto &acceleration = m_pState->getAcceleration();
            acceleration[SphericalAccelerationAxisType::Horizontal] *= cnv;
            acceleration[SphericalAccelerationAxisType::Vertical] *= cnv;

            auto &position = m_pState->getPosition();
            position[SphericalPositionAxisType::Horizontal] *= cnv;
            position[SphericalPositionAxisType::Vertical] *= cnv;

            auto &velocity = m_pState->getVelocity();
            velocity[SphericalVelocityAxisType::Horizontal] *= cnv;
            velocity[SphericalVelocityAxisType::Vertical] *= cnv;
        }

        MotionState::convertAngleUnits(angleUnits);
    }
}

/**
 * Create a spherical motion state within the given frame of reference
 * @param pFrame the frame of reference in which the new motion state will be created
 * @param state  the perturbation state of this object's reference frame
 */
SphericalMotionState *SphericalMotionState::create(ReferenceFrame *pFrame,
                                                   const std::string &state)
{
    SphericalMotionState *pMotionState = nullptr;
    if (pFrame != nullptr)
    {
        pMotionState = new SphericalMotionState(pFrame->getTime(state), state);
        pMotionState->setFrame(pFrame);

        return pMotionState;
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        std::cout << "Warning from " << getQualifiedStaticMethodName(__func__, SphericalMotionState) << ": "
                  << "Pointer to ReferenceFrame object must not be null."
                  << std::endl << std::endl;
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return nullptr;
}

/**
 * Get spherical acceleration in horizontal, vertical, radial axis
 */
double &SphericalMotionState::get(const SphericalAccelerationAxisType &type)
{
    return m_pState->getAcceleration().get(type);
}

/**
 * Get spherical acceleration in horizontal, vertical, radial axis
 */
double SphericalMotionState::get(const SphericalAccelerationAxisType &type) const
{
    return m_pState->getAcceleration().get(type);
}

/**
 * Get spherical position in horizontal, vertical, radial axis
 */
double &SphericalMotionState::get(const SphericalPositionAxisType &type)
{
    return m_pState->getPosition().get(type);
}

/**
 * Get spherical position in horizontal, vertical, radial axis
 */
double SphericalMotionState::get(const SphericalPositionAxisType &type) const
{
    return m_pState->getPosition().get(type);
}

/**
 * Get spherical velocity in horizontal, vertical, radial axis
 */
double &SphericalMotionState::get(const SphericalVelocityAxisType &type)
{
    return m_pState->getVelocity().get(type);
}

/**
 * Get spherical velocity in horizontal, vertical, radial axis
 */
double SphericalMotionState::get(const SphericalVelocityAxisType &type) const
{
    return m_pState->getVelocity().get(type);
}

/**
 * Get the state acceleration
 */
void SphericalMotionState::getAcceleration(double &horizontalAcc,
                                           double &verticalAcc,
                                           double &radialAcc) const
{
    m_pState->getAcceleration().get(horizontalAcc, verticalAcc, radialAcc);
}

/**
 * Get the state acceleration
 */
void SphericalMotionState::getAcceleration(double acceleration[3]) const
{
    getAcceleration(acceleration[SphericalAccelerationAxisType::Horizontal],
                    acceleration[SphericalAccelerationAxisType::Vertical],
                    acceleration[SphericalAccelerationAxisType::Radial]);
}

/**
 * Get the name of this class
 */
std::string SphericalMotionState::getClassName(void) const
{
    return "SphericalMotionState";
}

/**
 * Get the coordinate system in which this motion state is described
 */
CoordinateType SphericalMotionState::getCoordinateType(void) const
{
    return CoordinateType::Spherical;
}

/**
 * Get the state position
 */
void SphericalMotionState::getPosition(double &horizontal,
                                       double &vertical,
                                       double &radial) const
{
    getPosition(horizontal, vertical, radial, m_pState->getTime());
}

/**
 * Get the state position at the specified time
 */
void SphericalMotionState::getPosition(double &horizontal,
                                       double &vertical,
                                       double &radial,
                                       double t) const
{
    double position[3] = { 0.0, 0.0, 0.0 };
    m_pState->getPosition(position, t);

    horizontal = position[SphericalPositionAxisType::Horizontal];
    vertical = position[SphericalPositionAxisType::Vertical];
    radial = position[SphericalPositionAxisType::Radial];
}

/**
 * Get the state position at the specified time
 */
void SphericalMotionState::getPosition(double position[3],
                                       double t) const
{
    getPosition(position[SphericalPositionAxisType::Horizontal],
                position[SphericalPositionAxisType::Vertical],
                position[SphericalPositionAxisType::Radial], t);
}

/**
 * Get a pointer to a conversion functor that corresponds to the specified azimuth, zenith spherical conversion
 * type; Upon success, function returns non-null
 */
tMotionStateAngleConversionFunction
SphericalMotionState::getSphericalConversionFunction(const SphericalConversionType &type)
{
    static std::map<SphericalConversionType, tMotionStateAngleConversionFunction> sphericalConversionMap =
    { { SphericalConversionType::Enum::ElevationToZenith, elevationToZenith },
      { SphericalConversionType::Enum::NegativeElevationToZenith, negativeElevationToZenith },
      { SphericalConversionType::Enum::ZenithToElevation, zenithToElevation },
      { SphericalConversionType::Enum::ZenithToNegativeElevation, zenithToNegativeElevation }
    };

    auto &&itFunction = sphericalConversionMap.find(type);
    if (itFunction != sphericalConversionMap.cend())
        return itFunction->second;
    else
        return nullptr;
}

/**
 * Get an enumerated type that corresponds to the conversion functor used to convert standard definitions of
 * spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a user- defined
 * horizontal, vertical angle convention
 */
SphericalConversionType
SphericalMotionState::getSphericalConversionType(tMotionStateAngleConversionFunction pConverter)
{
    static std::map<tMotionStateAngleConversionFunction, SphericalConversionType> sphericalConversionMap =
    { { elevationToZenith, SphericalConversionType::Enum::ElevationToZenith },
      { negativeElevationToZenith, SphericalConversionType::Enum::NegativeElevationToZenith },
      { zenithToElevation, SphericalConversionType::Enum::ZenithToElevation },
      { zenithToNegativeElevation, SphericalConversionType::Enum::ZenithToNegativeElevation }
    };

    auto &&itType = sphericalConversionMap.find(pConverter);
    if (itType != sphericalConversionMap.cend())
        return itType->second;
    else
        return SphericalConversionType::Enum::Unknown;
}

/**
 * Get the state velocity
 */
void SphericalMotionState::getVelocity(double &horizontalRate,
                                       double &verticalRate,
                                       double &radialRate) const
{
    getVelocity(horizontalRate, verticalRate, radialRate, m_pState->getTime());
}

/**
 * Get the state velocity at the specified time
 */
void SphericalMotionState::getVelocity(double &horizontalRate,
                                       double &verticalRate,
                                       double &radialRate,
                                       double t) const
{
    double velocity[3] = { 0.0, 0.0, 0.0 };
    m_pState->getVelocity(velocity, t);

    horizontalRate = velocity[SphericalVelocityAxisType::Horizontal];
    verticalRate = velocity[SphericalVelocityAxisType::Vertical];
    radialRate = velocity[SphericalVelocityAxisType::Radial];
}

/**
 * Get the state velocity at the specified time
 */
void SphericalMotionState::getVelocity(double velocity[3],
                                       double t) const
{
    getVelocity(velocity[SphericalVelocityAxisType::Horizontal],
                velocity[SphericalVelocityAxisType::Vertical],
                velocity[SphericalVelocityAxisType::Radial], t);
}

/**
 * Function to print the contents of the motion state
 */
std::ostream &SphericalMotionState::print(std::ostream &stream) const
{
    MotionState::print(stream);

    if (stream)
    {
        stream << std::endl;
        stream << "Spherical Position (h, v, r):          " << m_pState->getPosition() << std::endl
               << "Spherical Velocity (hd, vd, rd):       " << m_pState->getVelocity() << std::endl
               << "Spherical Acceleration (hdd, vdd, rdd):" << m_pState->getAcceleration();
    }

    return stream;
}

/**
 * Return the motion state projected to time t
 */
SphericalMotionState SphericalMotionState::project(double t) const
{
    SphericalMotionState sphericalMotionState(*this);

    sphericalMotionState.update(t - m_pState->getTime());

    return sphericalMotionState;
}

/**
 * Scale the acceleration in horizontal, vertical, and radial
 */
void SphericalMotionState::scale(const SphericalAccelerationAxisType &type,
                                 double value)
{
    m_pState->getAcceleration().scale(type, value);
}

/**
 * Scale position in horizontal, vertical, and radial
 */
void SphericalMotionState::scale(const SphericalPositionAxisType &type,
                                 double value)
{
    m_pState->getPosition().scale(type, value);
}

/**
 * Scale velocity in horizontal, vertical, and radial
 */
void SphericalMotionState::scale(const SphericalVelocityAxisType &type,
                                 double value)
{
    m_pState->getVelocity().scale(type, value);
}

/**
 * Set spherical acceleration in horizontal, vertical, radial
 */
void SphericalMotionState::set(const SphericalAccelerationAxisType &type,
                               double acceleration)
{
    m_pState->getAcceleration().set(type, acceleration);
}

/**
 * Set spherical position in horizontal, vertical, radial
 */
void SphericalMotionState::set(const SphericalPositionAxisType &type,
                               double position)
{
    m_pState->getPosition().set(type, position);
}

/**
 * Set spherical velocity in horizontal, vertical, radial
 */
void SphericalMotionState::set(const SphericalVelocityAxisType &type,
                               double velocity)
{
    m_pState->getVelocity().set(type, velocity);
}

/**
 * Set the state acceleration
 */
void SphericalMotionState::setAcceleration(const Vector3d &acceleration)
{
    setAcceleration(acceleration[SphericalAccelerationAxisType::Horizontal],
                    acceleration[SphericalAccelerationAxisType::Vertical],
                    acceleration[SphericalAccelerationAxisType::Radial]);
}

/**
 * Set the state acceleration
 */
void SphericalMotionState::setAcceleration(double horizontalAcc,
                                           double verticalAcc,
                                           double radialAcc)
{
    m_pState->getAcceleration().set(horizontalAcc,
                                    verticalAcc,
                                    radialAcc);
}

/**
 * Set the state initial position
 */
void SphericalMotionState::setPosition(const Vector3d &position)
{
    setPosition(position[SphericalPositionAxisType::Horizontal],
                position[SphericalPositionAxisType::Vertical],
                position[SphericalPositionAxisType::Radial]);
}

/**
 * Set the state initial position
 */
void SphericalMotionState::setPosition(double horizontal,
                                       double vertical,
                                       double radial)
{
    m_pState->getPosition().set(horizontal,
                                vertical,
                                radial);
}

/**
 * Set the state initial velocity
 */
void SphericalMotionState::setVelocity(const Vector3d &velocity)
{
    setVelocity(velocity[SphericalVelocityAxisType::Horizontal],
                velocity[SphericalVelocityAxisType::Vertical],
                velocity[SphericalVelocityAxisType::Radial]);
}

/**
 * Set the state initial velocity
 */
void SphericalMotionState::setVelocity(double horizontalRate,
                                       double verticalRate,
                                       double radialRate)
{
    m_pState->getVelocity().set(horizontalRate,
                                verticalRate,
                                radialRate);
}

}

}
