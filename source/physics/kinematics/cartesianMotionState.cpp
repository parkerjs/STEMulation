#include "acceleration_axis_type.h"
#include "cartesianMotionState.h"
#include "kinematicState.h"
#include "position_axis_type.h"
#include "referenceFrame.h"
#include "sphericalMotionState.h"
#include "trigonometry.h"
#include "velocity_axis_type.h"

// using namespace declarations
using namespace math;
using namespace math::linear_algebra::vector;
using namespace utilities;

namespace physics
{

namespace kinematics
{
#if 0
static bool bRegisteredCartesianToSphericalConversion =
MotionState::registerCoordinateSystemConversionPair(CoordinateType(CoordinateType::Enum::Cartesian),
                                                    CoordinateType(CoordinateType::Enum::Spherical),
[] (const MotionState *pInputMotionState, MotionState *pOutputMotionState, double t)
{
    auto *pCartesianMotionState = dynamic_cast<const CartesianMotionState *>(pInputMotionState);
    auto *pSphericalMotionState = dynamic_cast<SphericalMotionState *>(pOutputMotionState);
    bool bSuccess = (pCartesianMotionState != nullptr && pSphericalMotionState != nullptr);
    if (bSuccess)
        bSuccess = pCartesianMotionState->calcSphericalState(pSphericalMotionState, t);

    return bSuccess;
});

/**
 * Register coordinate conversion function that maps Cartesian to Cartesian
 */
static bool bRegisteredCartesianToCartesianConversion =
MotionState::registerCoordinateSystemConversionPair(CoordinateType(CoordinateType::Enum::Cartesian),
                                                    CoordinateType(CoordinateType::Enum::Cartesian),
[] (const MotionState *pInputMotionState, MotionState *pOutputMotionState, double t)
{
    if (pInputMotionState != pOutputMotionState)
    {
        auto *pInputCartesianMotionState = dynamic_cast<const CartesianMotionState *>(pInputMotionState);
        auto *pOutputCartesianMotionState = dynamic_cast<CartesianMotionState *>(pOutputMotionState);
        if (pInputCartesianMotionState != nullptr && pOutputCartesianMotionState != nullptr)
            *pOutputCartesianMotionState = *pInputCartesianMotionState;
        else
            return false;
    }

    return true;
});

template<typename ... Args>
static bool bRegistered = MotionState::registerMotionStateFactory(CoordinateType::Enum::Cartesian,
                                                                  CartesianMotionState::create<Args ...>);
#endif
/**
 * Construct an empty motion state object
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(const std::string &state)
: MotionState(state)
{

}

/**
 * Construct an empty motion state at the specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
                                           const std::string &state)
: MotionState(t, state)
{

}

/**
 * Construct a motion state with the given position at the specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
                                           const Vector3d &position,
                                           const std::string &state)
: MotionState(t, position, state)
{

}

/**
 * Construct a motion state with the given position and initial velocity at the specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const std::string &state)
: MotionState(t, position, velocity, state)
{

}

/**
 * Construct a motion state with the given position, initial velocity, and constant acceleration at the
 * specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
                                           const Vector3d &position,
                                           const Vector3d &velocity,
                                           const Vector3d &acceleration,
                                           const std::string &state)
: MotionState(t, position, velocity, acceleration, state)
{

}

/**
 * Construct a motion state with the given position, initial velocity, constant acceleration and Euler
 * orientation at the specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
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
 * Construct a motion state with the given position, initial velocity, constant acceleration, Euler orientation
 * and Euler rates at the specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
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
 * Construct a motion state with the given position, initial velocity, constant acceleration, Euler
 * orientation, Euler rates, and Euler accelerations at the specified time
 * @param state the perturbation state of this object's reference frame
 */
CartesianMotionState::CartesianMotionState(double t,
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
 * Construct a Cartesian motion state from an abstract motion state constant reference
 */
CartesianMotionState::CartesianMotionState(const MotionState &motionState)
{
    if (motionState.isCartesian())
        operator = (dynamic_cast<const CartesianMotionState &>(motionState));
    else
    {
        cacheTransformationsEnabled(motionState.cacheTransformationsEnabled());
        debugTransformsEnabled(motionState.debugTransformsEnabled());
        setConversionFromAzimuthZenithFunction(motionState.getConversionFromAzimuthZenithFunction());
        setConversionToAzimuthZenithFunction(motionState.getConversionToAzimuthZenithFunction());
        motionState.calcCartesianState(this);
    }
}

/**
 * Construct a Cartesian motion state from a spherical motion state
 */
CartesianMotionState::CartesianMotionState(const SphericalMotionState &sphericalMotionState)
{
    CartesianMotionState cartesianMotionState(sphericalMotionState.getFrameState());

    cartesianMotionState.cacheTransformationsEnabled(sphericalMotionState.cacheTransformationsEnabled());
    cartesianMotionState.debugTransformsEnabled(sphericalMotionState.debugTransformsEnabled());

    auto *pFromConverter = sphericalMotionState.getConversionFromAzimuthZenithFunction();
    auto *pToConverter = sphericalMotionState.getConversionToAzimuthZenithFunction();

    cartesianMotionState.setConversionFromAzimuthZenithFunction(pFromConverter);
    cartesianMotionState.setConversionToAzimuthZenithFunction(pToConverter);
    sphericalMotionState.calcCartesianState(&cartesianMotionState);

    operator = (cartesianMotionState);
}

/**
 * Copy constructor
 */
CartesianMotionState::CartesianMotionState(const CartesianMotionState &motionState)
: MotionState(motionState)
{

}

/**
 * Move constructor
 */
CartesianMotionState::CartesianMotionState(CartesianMotionState &&motionState)
: MotionState(std::move(motionState))
{

}

/**
 * Destructor
 */
CartesianMotionState::~CartesianMotionState(void)
{

}

/**
 * Assignment operator
 */
CartesianMotionState &CartesianMotionState::operator = (const MotionState &motionState)
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
CartesianMotionState &CartesianMotionState::operator = (const CartesianMotionState &motionState)
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
CartesianMotionState &CartesianMotionState::operator = (CartesianMotionState &&motionState)
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
bool CartesianMotionState::assign(const MotionState *pMotionState)
{
    bool bSuccess = (pMotionState != nullptr && pMotionState != this);
    if (bSuccess)
    {
        if (pMotionState->isSpherical())
        {
            cacheTransformationsEnabled(pMotionState->cacheTransformationsEnabled());
            debugTransformsEnabled(pMotionState->debugTransformsEnabled());
            setConversionFromAzimuthZenithFunction(pMotionState->getConversionFromAzimuthZenithFunction());
            setConversionToAzimuthZenithFunction(pMotionState->getConversionToAzimuthZenithFunction());
            pMotionState->calcCartesianState(this);
        }
        else if (pMotionState->isCartesian())
        {
            MotionState::operator = (dynamic_cast<const CartesianMotionState &>(*pMotionState));
        }
    }

    return bSuccess;
}

/**
 * Calculate the scalar acceleration
 */
double CartesianMotionState::calcAcceleration(void) const
{
    return m_pState->getAcceleration().magnitude();
}

/**
 * Calculate the time at which the current object's motion state approaches the null motion state at the
 * desired range. The time returned represents the closest time, whether it be in the future or past, at which
 * the motion state approaches the given range. If the motion state does not approach the specified range, the
 * subroutine returns infinity
 */
double CartesianMotionState::calcApproachTime(double range) const
{
    CartesianMotionState motionState(m_pState->getTime(), m_frameState);
    motionState.setFrame(m_pFrame);

    return MotionState::calcApproachTime(range, motionState);
}

/**
 * Calculate the minimum approach distance achieved with respect to another motion state
 * @param tApproach the calculated time at which the minimum distance is achieved
 */
double CartesianMotionState::calcMinimumApproach(double &tApproach) const
{
    CartesianMotionState motionState(m_pState->getTime(), m_frameState);
    motionState.setFrame(m_pFrame);

    return MotionState::calcMinimumApproach(tApproach, motionState);
}

/**
 * Calculate the range at time t
 */
double CartesianMotionState::calcRange(double t) const
{
    return calcRange(t, CartesianMotionState(m_pState->getTime(), m_frameState));
}

/**
 * Calculate the range with respect to another motion state at time t
 */
double CartesianMotionState::calcRange(double t,
                                       const MotionState &motionState) const
{
    double dp[3], r = 0.0;
    double posA[3], posB[3];

    getPosition(posA, t);
    motionState.getPosition(posB, t);

    for (int i = 0; i < 3; ++i)
    {
        dp[i] = posA[i] - posB[i];
        r += dp[i] * dp[i];
    }

    if (r > 0)
        r = std::sqrt(r);

    return r;
}

/**
 * Calculate the range acceleration at time t
 */
double CartesianMotionState::calcRangeAcceleration(double t) const
{
    return calcRangeAcceleration(t, CartesianMotionState(m_pState->getTime(), m_frameState));
}

/**
 * Calculate the range acceleration with respect to another motion state at time t
 */
double CartesianMotionState::calcRangeAcceleration(double t,
                                                   const MotionState &motionState) const
{
    double r = 0.0, rr = 0.0, ra = 0.0;
    double da[3], dp[3], dv[3];
    double accA[3], accB[3], posA[3], posB[3], velA[3], velB[3];

    getAcceleration(accA);
    getPosition(posA, t);
    getVelocity(velA, t);
    motionState.getAcceleration(accB);
    motionState.getPosition(posB, t);
    motionState.getVelocity(velB, t);

    for (int i = 0; i < 3; ++i)
    {
        da[i] = accA[i] - accB[i];
        dp[i] = posA[i] - posB[i];
        dv[i] = velA[i] - velB[i];
        ra += dv[i] * dv[i] + da[i] * dp[i];
        rr += dp[i] * dv[i];
        r += dp[i] * dp[i];
    }

    r = std::sqrt(r);
    rr = r > 0.0 ? rr / r : 0.0;
    ra = r > 0.0 ? (ra - rr * rr) / r : 0.0;

    return ra;
}

/**
 * Calculate the range rate at time t
 */
double CartesianMotionState::calcRangeRate(double t) const
{
    return calcRangeRate(t, CartesianMotionState(m_pState->getTime(), m_frameState));
}

/**
 * Calculate the range rate with respect to another motion state at time t
 */
double CartesianMotionState::calcRangeRate(double t,
                                           const MotionState &motionState) const
{
    double r = 0.0, rr = 0;
    double dp[3], dv[3];
    double posA[3], posB[3], velA[3], velB[3];

    getPosition(posA, t);
    getVelocity(velA, t);
    motionState.getPosition(posB, t);
    motionState.getVelocity(velB, t);

    for (int i = 0; i < 3; ++i)
    {
        dp[i] = posA[i] - posB[i];
        dv[i] = velA[i] - velB[i];
        rr += dp[i] * dv[i];
        r += dp[i] * dp[i];
    }

    rr = r > 0.0 ? rr / std::sqrt(r) : 0.0;

    return rr;
}

/**
 * Calculate the scalar speed at time t
 */
double CartesianMotionState::calcSpeed(double t) const
{
    return getVelocity(t).magnitude();
}

/**
 * Calculate the spherical motion state from the current object's Cartesian motion state at the specified time
 * @param pSphericalMotionState the object in which the resultant spherical state will be stored
 * @param angleUnits            the desired angle output units, Degrees or Radians
 * @param t                     the time to which the resultant motion state will be projected
 */
bool CartesianMotionState::calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                              const AngleUnitType &angleUnits,
                                              double t) const
{
    bool bSuccess = (pSphericalMotionState != nullptr);
    if (bSuccess)
    {
        // angular orientation, velocity, and acceleration will initially be in radians
        pSphericalMotionState->setAngleUnits(AngleUnitType::Radians);

        double x, y, z;
        getPosition(x, y, z, t);
        if (x == 0.0 && y == 0.0)
        {
//          StaticSynchronizable<MotionState>::lock(0);
//          logMsg(std::cout, LoggingLevel::Enum::Warning,
//                 "The motion state position passes through a singularity, "
//                 "conversion to spherical coordinate system may be inaccurate!\n",
//                 getQualifiedMethodName(__func__));
//          StaticSynchronizable<MotionState>::unlock(0);
        }

        double rxy = 0.0, rxySq = 0.0;
        if (x != 0.0 || y != 0.0 || z != 0.0)
        {
            rxySq = x * x + y * y;
            rxy = std::sqrt(rxySq);
        }

        double r = 0.0;
        if (rxySq != 0.0 || z != 0.0)
            r = std::sqrt(rxySq + z * z);

        double az = 0.0, ze = 0.0;
        if (r != 0.0)
        {
            az = std::atan2(y, x);
            ze = trigonometric::acos(z / r);
        }

        bool bMotionStateHasVelocity = false;
        double azd = 0.0, rd = 0.0, xd = 0.0, yd = 0.0, zd = 0.0, zed = 0.0;
        if (r != 0.0)
        {
            getVelocity(xd, yd, zd, t);

            bMotionStateHasVelocity = (xd != 0.0 || yd != 0.0 || zd != 0.0);
            if (bMotionStateHasVelocity)
            {
                rd = (x * xd + y * yd + z * zd) / r;
                if (rxy != 0.0)
                {
                    azd = (x * yd - xd * y) / rxySq;
                    zed = (rd * z - r * zd) / (rxy * r);
                }
            }
        }

        double azdd = 0.0, rdd = 0.0, zedd = 0.0;
        if (r != 0.0)
        {
            double xdd, ydd, zdd;
            getAcceleration(xdd, ydd, zdd);

            bool bMotionStateHasAcceleration = (xdd != 0.0 || ydd != 0.0 || zdd != 0.0);
            if (bMotionStateHasVelocity || bMotionStateHasAcceleration)
            {
                if (bMotionStateHasVelocity)
                    rdd = xd * xd + yd * yd + zd * zd - rd * rd;

                if (bMotionStateHasAcceleration)
                    rdd += x * xdd + y * ydd + z * zdd;

                rdd /= r;

                if (rxy != 0.0)
                {
                    double rxyd = 0.0;
                    if (bMotionStateHasVelocity)
                    {
                        rxyd = x * xd + y * yd;
                        azdd = -2.0 * rxyd * azd;
                        rxyd /= rxy;
                        zedd = -zed * (r * rxyd + rxy * rd);
                    }

                    if (bMotionStateHasAcceleration)
                    {
                        azdd += x * ydd - xdd * y;
                        zedd -= r * zdd;
                    }

                    azdd /= rxySq;
                    zedd += rdd * z;
                    zedd /= rxy * r;
                }
                else
                    zedd = -zed * rd / r;
            }
        }

        pSphericalMotionState->setAcceleration(azdd, zedd, rdd);
        pSphericalMotionState->setPosition(az, ze, r);
        pSphericalMotionState->setVelocity(azd, zed, rd);

        // need to convert radians to the angle units of current object...
        pSphericalMotionState->convertAngleUnits(m_pState->getAngleUnits());

        auto &&eulerAccelerations = getEulerAccelerations();
        auto &&eulerRates = getEulerRates(t);
        auto &&eulers = getEulers(t);

        pSphericalMotionState->setEulerAccelerations(eulerAccelerations);
        pSphericalMotionState->setEulerRates(eulerRates);
        pSphericalMotionState->setEulers(eulers);
        pSphericalMotionState->setFrame(m_pFrame);
        pSphericalMotionState->setFrameState(m_frameState);
        pSphericalMotionState->setTime(t);

        // convert current angular units of the spherical
        // motion state to those of the target angular units...
        pSphericalMotionState->convertAngleUnits(angleUnits);

        auto *pConverter = pSphericalMotionState->getConversionFromAzimuthZenithFunction();
        if (pConverter != nullptr)
            (*pConverter)(pSphericalMotionState);
    }

    return bSuccess;
}

/**
 * clone() function
 */
CartesianMotionState *CartesianMotionState::clone(void) const
{
    return new CartesianMotionState(*this);
}

/**
 * Create a Cartesian motion state within the given frame of reference
 * @param pFrame the frame of reference in which the new motion state will be created
 * @param state  the perturbation state of this object's reference frame
 */
CartesianMotionState *CartesianMotionState::create(ReferenceFrame *pFrame,
                                                   const std::string &state)
{
    CartesianMotionState *pMotionState = nullptr;
    if (pFrame != nullptr)
    {
        pMotionState = new CartesianMotionState(pFrame->getTime(state), state);
        pMotionState->setFrame(pFrame);

        return pMotionState;
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Pointer to ReferenceFrame object must not be null.\n",
               getQualifiedStaticMethodName(__func__, CartesianMotionState));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return nullptr;
}

/**
 * Get Cartesian acceleration in x, y, or z
 */
double &CartesianMotionState::get(const AccelerationAxisType &type)
{
    return m_pState->getAcceleration().get(type);
}

/**
 * Get Cartesian acceleration in x, y, or z (const overload)
 */
double CartesianMotionState::get(const AccelerationAxisType &type) const
{
    return m_pState->getAcceleration().get(type);
}

/**
 * Get Cartesian position in x, y, or z
 */
double &CartesianMotionState::get(const PositionAxisType &type)
{
    return m_pState->getPosition().get(type);
}

/**
 * Get Cartesian position in x, y, or z (const overload)
 */
double CartesianMotionState::get(const PositionAxisType &type) const
{
    return m_pState->getPosition().get(type);
}

/**
 * Get Cartesian velocity in x, y, or z
 */
double &CartesianMotionState::get(const VelocityAxisType &type)
{
    return m_pState->getVelocity().get(type);
}

/**
 * Get Cartesian velocity in x, y, or z (const overload)
 */
double CartesianMotionState::get(const VelocityAxisType &type) const
{
    return m_pState->getVelocity().get(type);
}

/**
 * Get the state acceleration
 */
void CartesianMotionState::getAcceleration(double &xAcceleration,
                                           double &yAcceleration,
                                           double &zAcceleration) const
{
    m_pState->getAcceleration().get(xAcceleration,
                                    yAcceleration,
                                    zAcceleration);
}

/**
 * Get the state acceleration
 */
void CartesianMotionState::getAcceleration(double acceleration[3]) const
{
    getAcceleration(acceleration[AccelerationAxisType::X],
                    acceleration[AccelerationAxisType::Y],
                    acceleration[AccelerationAxisType::Z]);
}

/**
 * Get the name of this class
 */
std::string CartesianMotionState::getClassName(void) const
{
    return "CartesianMotionState";
}

/**
 * Get the coordinate system in which this motion state is described
 */
CoordinateType CartesianMotionState::getCoordinateType(void) const
{
    return CoordinateType::Cartesian;
}

/**
 * Get the state position
 */
void CartesianMotionState::getPosition(double &xPosition,
                                       double &yPosition,
                                       double &zPosition) const
{
    getPosition(xPosition, yPosition, zPosition, m_pState->getTime());
}

/**
 * Get the state position at the specified time
 */
void CartesianMotionState::getPosition(double &xPosition,
                                       double &yPosition,
                                       double &zPosition,
                                       double t) const
{
    double position[3] = { 0.0, 0.0, 0.0 };
    m_pState->getPosition(position, t);

    xPosition = position[PositionAxisType::X];
    yPosition = position[PositionAxisType::Y];
    zPosition = position[PositionAxisType::Z];
}

/**
 * Get the state position at the specified time
 */
void CartesianMotionState::getPosition(double position[3],
                                       double t) const
{
    getPosition(position[PositionAxisType::X],
                position[PositionAxisType::Y],
                position[PositionAxisType::Z], t);
}

/**
 * Get the state velocity
 */
void CartesianMotionState::getVelocity(double &xVelocity,
                                       double &yVelocity,
                                       double &zVelocity) const
{
    getVelocity(xVelocity, yVelocity, zVelocity, m_pState->getTime());
}

/**
 * Get the state velocity at the specified time
 */
void CartesianMotionState::getVelocity(double &xVelocity,
                                       double &yVelocity,
                                       double &zVelocity,
                                       double t) const
{
    double velocity[3] = { 0.0, 0.0, 0.0 };
    m_pState->getVelocity(velocity, t);

    xVelocity = velocity[VelocityAxisType::X];
    yVelocity = velocity[VelocityAxisType::Y];
    zVelocity = velocity[VelocityAxisType::Z];
}

/**
 * Get the state velocity at the specified time
 */
void CartesianMotionState::getVelocity(double velocity[3],
                                       double t) const
{
    getVelocity(velocity[VelocityAxisType::X],
                velocity[VelocityAxisType::Y],
                velocity[VelocityAxisType::Z], t);
}

/**
 * Function to print the contents of the motion state
 */
std::ostream &CartesianMotionState::print(std::ostream &stream) const
{
    MotionState::print(stream);

    if (stream)
    {
        stream << std::endl;
        stream << "Cartesian Position (x, y, z):          " << m_pState->getPosition() << std::endl
               << "Cartesian Velocity (xd, yd, zd):       " << m_pState->getVelocity() << std::endl
               << "Cartesian Acceleration (xdd, ydd, zdd):" << m_pState->getAcceleration();
    }

    return stream;
}

/**
 * Return the motion state projected to time t
 */
CartesianMotionState CartesianMotionState::project(double t) const
{
    CartesianMotionState cartesianMotionState(*this);

    cartesianMotionState.update(t - m_pState->getTime());

    return cartesianMotionState;
}

/**
 * Scale the acceleration in x, y, or z
 */
void CartesianMotionState::scale(const AccelerationAxisType &type,
                                 double value)
{
    m_pState->getAcceleration().scale(type, value);
}

/**
 * Scale position in x, y, or z
 */
void CartesianMotionState::scale(const PositionAxisType &type,
                                 double value)
{
    m_pState->getPosition().scale(type, value);
}

/**
 * Scale velocity in x, y, or z
 */
void CartesianMotionState::scale(const VelocityAxisType &type,
                                 double value)
{
    m_pState->getVelocity().scale(type, value);
}

/**
 * Set Cartesian acceleration in x, y, or z
 */
void CartesianMotionState::set(const AccelerationAxisType &type,
                               double acceleration)
{
    m_pState->getAcceleration().set(type, acceleration);
}

/**
 * Set Cartesian position in x, y, or z
 */
void CartesianMotionState::set(const PositionAxisType &type,
                               double position)
{
    m_pState->getPosition().set(type, position);
}

/**
 * Set Cartesian velocity in x, y, or z
 */
void CartesianMotionState::set(const VelocityAxisType &type,
                               double velocity)
{
    m_pState->getVelocity().set(type, velocity);
}

/**
 * Set the state acceleration
 */
void CartesianMotionState::setAcceleration(const Vector3d &acceleration)
{
    setAcceleration(acceleration[AccelerationAxisType::X],
                    acceleration[AccelerationAxisType::Y],
                    acceleration[AccelerationAxisType::Z]);
}

/**
 * Set the state acceleration
 */
void CartesianMotionState::setAcceleration(double xAcceleration,
                                           double yAcceleration,
                                           double zAcceleration)
{
    m_pState->getAcceleration().set(xAcceleration,
                                    yAcceleration,
                                    zAcceleration);
}

/**
 * Set the state initial position
 */
void CartesianMotionState::setPosition(const Vector3d &position)
{
    setPosition(position[PositionAxisType::X],
                position[PositionAxisType::Y],
                position[PositionAxisType::Z]);
}

/**
 * Set the state initial position
 */
void CartesianMotionState::setPosition(double xPosition,
                                       double yPosition,
                                       double zPosition)
{
    m_pState->getPosition().set(xPosition,
                                yPosition,
                                zPosition);
}

/**
 * Set the state initial velocity
 */
void CartesianMotionState::setVelocity(const Vector3d &velocity)
{
    setVelocity(velocity[VelocityAxisType::X],
                velocity[VelocityAxisType::Y],
                velocity[VelocityAxisType::Z]);
}

/**
 * Set the state initial velocity
 */
void CartesianMotionState::setVelocity(double xVelocity,
                                       double yVelocity,
                                       double zVelocity)
{
    m_pState->getVelocity().set(xVelocity,
                                yVelocity,
                                zVelocity);
}

}

}
