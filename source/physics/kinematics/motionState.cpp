#include "angle_unit_type.h"
#include "cartesianMotionState.h"
#include "coordinateSystem.h"
#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "euler_state_derivative_type.h"
#include "eulers.h"
#include "frameState.h"
#include "lowOrderPolynomialSolver.h"
#include "matrix3x3.h"
#include "motionState.h"
#include "projectedKinematicState.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "referenceFrame.h"
#include "spherical_conversion_type.h"
#include "sphericalMotionState.h"
#include "state_derivative_type.h"
#include "vector3d.h"
#include <cstring>
#include <string>
#include <typeinfo>

// using namespace declarations
using namespace attributes::concrete;
using namespace math::expression::polynomial::solvers;
using namespace math::geometric::orientation;
using namespace math::linear_algebra::vector;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace math::trigonometric;
using namespace utilities;

// if a copy-assign motion state caching option is not specified in preprocessor configuration, then define
// the following
#ifndef COPY_ASSIGN_MOTION_STATE_CACHING_OPTION
#define COPY_ASSIGN_MOTION_STATE_CACHING_OPTION true
#endif

// if a copy-assign motion state transform debugging option is not specified in preprocessor configuration,
// then define the following
#ifndef COPY_ASSIGN_MOTION_STATE_TRANSFORM_DEBUGGING_OPTION
#define COPY_ASSIGN_MOTION_STATE_TRANSFORM_DEBUGGING_OPTION true
#endif

// if a default conversion azimuth, zenith acceleration conversion function is not specified in preprocessor
// configuration, then define the following
#ifndef DEFAULT_FROM_AZIMUTH_ZENITH_ACCELERATION_CONVERTER
#define DEFAULT_FROM_AZIMUTH_ZENITH_ACCELERATION_CONVERTER zenithAccelerationToAccelerationElevation
#endif

// if a default conversion azimuth, zenith conversion function is not specified in preprocessor configuration,
// then define the following
#ifndef DEFAULT_FROM_AZIMUTH_ZENITH_CONVERTER
#define DEFAULT_FROM_AZIMUTH_ZENITH_CONVERTER zenithToElevation
#endif

// if a default conversion azimuth, zenith rate conversion function is not specified in preprocessor
// configuration, then define the following
#ifndef DEFAULT_FROM_AZIMUTH_ZENITH_RATE_CONVERTER
#define DEFAULT_FROM_AZIMUTH_ZENITH_RATE_CONVERTER zenithRateToRateElevation
#endif

// if a default motion state transform caching option is not specified in preprocessor configuration, then
// define the following
#ifndef DEFAULT_MOTION_STATE_CACHING_OPTION
#define DEFAULT_MOTION_STATE_CACHING_OPTION false
#endif

// if a default motion state transform debugging option is not specified in preprocessor configuration, then
// define the following
#ifndef DEFAULT_MOTION_STATE_TRANSFORM_DEBUGGING_OPTION
#define DEFAULT_MOTION_STATE_TRANSFORM_DEBUGGING_OPTION false
#endif

// if a default conversion azimuth, zenith acceleration conversion function is not specified in preprocessor
// configuration, then define the following
#ifndef DEFAULT_TO_AZIMUTH_ZENITH_ACCELERATION_CONVERTER
#define DEFAULT_TO_AZIMUTH_ZENITH_ACCELERATION_CONVERTER elevationAccelerationToZenithAcceleration
#endif

// if a default conversion azimuth, zenith conversion function is not specified in preprocessor configuration,
// then define the following
#ifndef DEFAULT_TO_AZIMUTH_ZENITH_CONVERTER
#define DEFAULT_TO_AZIMUTH_ZENITH_CONVERTER elevationToZenith
#endif

// if a default conversion azimuth, zenith rate conversion function is not specified in preprocessor
// configuration, then define the following
#ifndef DEFAULT_TO_AZIMUTH_ZENITH_RATE_CONVERTER
#define DEFAULT_TO_AZIMUTH_ZENITH_RATE_CONVERTER elevationRateToZenithRate
#endif

namespace physics
{

namespace kinematics
{

// explicit template instantiations
template MotionState *MotionState::transformCloneToFrame(ReferenceFrame *,
                                                         const std::string &,
                                                         bool) const;
template MotionState *MotionState::transformCloneToFrame(ReferenceFrame *,
                                                         const char *,
                                                         bool) const;
template MotionState *MotionState::transformCloneToFrame(ReferenceFrame *,
                                                         const std::string &,
                                                         double t) const;
template MotionState *MotionState::transformCloneToFrame(ReferenceFrame *,
                                                         const char *,
                                                         double t) const;
template std::shared_ptr<MotionState> MotionState::transformSharedCloneToFrame(ReferenceFrame *,
                                                                               const std::string &,
                                                                               bool) const;
template std::shared_ptr<MotionState> MotionState::transformSharedCloneToFrame(ReferenceFrame *,
                                                                               const char *,
                                                                               bool) const;
template std::shared_ptr<MotionState> MotionState::transformSharedCloneToFrame(ReferenceFrame *,
                                                                               const std::string &,
                                                                               double t) const;
template std::shared_ptr<MotionState> MotionState::transformSharedCloneToFrame(ReferenceFrame *,
                                                                               const char *,
                                                                               double t) const;
template bool MotionState::transformToFrame(ReferenceFrame *,
                                            const std::string &,
                                            bool);
template bool MotionState::transformToFrame(ReferenceFrame *,
                                            const char *,
                                            bool);
template bool MotionState::transformToFrame(ReferenceFrame *,
                                            const std::string &,
                                            double t);
template bool MotionState::transformToFrame(ReferenceFrame *,
                                            const char *,
                                            double t);

// static member initializations
bool MotionState::m_bDefaultTransformCachingOption = DEFAULT_MOTION_STATE_CACHING_OPTION;
bool MotionState::m_bDefaultTransformDebuggingOption = DEFAULT_MOTION_STATE_TRANSFORM_DEBUGGING_OPTION;
tMotionStateAngleConversionFunction
MotionState::m_pDefaultFromAzimuthZenithConverter = DEFAULT_FROM_AZIMUTH_ZENITH_CONVERTER;
tMotionStateAngleConversionFunction
MotionState::m_pDefaultToAzimuthZenithConverter = DEFAULT_TO_AZIMUTH_ZENITH_CONVERTER;

/**
 * Construct an empty motion state object
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAngleUnits(AngleUnitType::Degrees);
    m_pState->setTime(0.0);
}

/**
 * Construct an empty motion state object at the specified time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAngleUnits(AngleUnitType::Degrees);
    m_pState->setTime(t);
}

/**
 * Construct a motion state with the given position at the specified time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const Vector3d &position,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAngleUnits(AngleUnitType::Degrees);
    m_pState->setPosition(position);
    m_pState->setTime(t);
}

/**
 * Construct a motion state with the given position and initial velocity at the specified time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const Vector3d &position,
                         const Vector3d &velocity,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAngleUnits(AngleUnitType::Degrees);
    m_pState->setPosition(position);
    m_pState->setTime(t);
    m_pState->setVelocity(velocity);
}

/**
 * Construct a motion state with the given position, initial velocity, constant acceleration at the specified
 * time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const Vector3d &position,
                         const Vector3d &velocity,
                         const Vector3d &acceleration,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAcceleration(acceleration);
    m_pState->setAngleUnits(AngleUnitType::Degrees);
    m_pState->setPosition(position);
    m_pState->setTime(t);
    m_pState->setVelocity(velocity);
}

/**
 * Construct a motion state with the given position, initial velocity, constant acceleration and Euler
 * orientation at the specified time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const Vector3d &position,
                         const Vector3d &velocity,
                         const Vector3d &acceleration,
                         const Eulers &eulers,
                         const AngleUnitType &angleUnits,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAcceleration(acceleration);
    m_pState->setEulers(eulers);
    m_pState->setPosition(position);
    m_pState->setTime(t);
    m_pState->setVelocity(velocity);
    m_pState->convertAngleUnits(angleUnits);
}

/**
 * Construct a motion state with the given position, initial velocity, constant acceleration, Euler orientation
 * and Euler rates at the specified time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const Vector3d &position,
                         const Vector3d &velocity,
                         const Vector3d &acceleration,
                         const Eulers &eulers,
                         const Eulers &eulerRates,
                         const AngleUnitType &angleUnits,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAcceleration(acceleration);
    m_pState->setEulerRates(eulerRates);
    m_pState->setEulers(eulers);
    m_pState->setPosition(position);
    m_pState->setTime(t);
    m_pState->setVelocity(velocity);
    m_pState->convertAngleUnits(angleUnits);
}

/**
 * Construct a motion state with the given position, initial velocity, constant acceleration, Euler
 * orientation, Euler rates, and Euler accelerations at the specified time
 * @param t     the time at which the motion state is initially defined
 * @param state the perturbation state of this object's reference frame
 */
MotionState::MotionState(double t,
                         const Vector3d &position,
                         const Vector3d &velocity,
                         const Vector3d &acceleration,
                         const Eulers &eulers,
                         const Eulers &eulerRates,
                         const Eulers &eulerAccelerations,
                         const AngleUnitType &angleUnits,
                         const std::string &state)
: m_bCacheTransformsEnabled(m_bDefaultTransformCachingOption),
  m_bDebugTransformsEnabled(m_bDefaultTransformDebuggingOption),
  m_frameState(state),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(m_pDefaultFromAzimuthZenithConverter),
  m_pState(ProjectedKinematicState::create()),
  m_pToAzimuthZenithConverter(m_pDefaultToAzimuthZenithConverter)
{
    setFrame(ReferenceFrame::getWorldFrame());

    m_pState->setAcceleration(acceleration);
    m_pState->setEulerAccelerations(eulerAccelerations);
    m_pState->setEulerRates(eulerRates);
    m_pState->setEulers(eulers);
    m_pState->setPosition(position);
    m_pState->setTime(t);
    m_pState->setVelocity(velocity);
    m_pState->convertAngleUnits(angleUnits);
}

/**
 * Copy constructor
 */
MotionState::MotionState(const MotionState &motionState)
: m_bCacheTransformsEnabled(false),
  m_bDebugTransformsEnabled(false),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(nullptr),
  m_pState(nullptr),
  m_pToAzimuthZenithConverter(nullptr)
{
    setFrame(ReferenceFrame::getWorldFrame());

    MotionState::operator = (motionState);
}

/**
 * Move constructor
 */
MotionState::MotionState(MotionState &&motionState)
: m_bCacheTransformsEnabled(false),
  m_bDebugTransformsEnabled(false),
  m_pFrame(nullptr),
  m_pFromAzimuthZenithConverter(nullptr),
  m_pState(nullptr),
  m_pToAzimuthZenithConverter(nullptr)
{
    operator = (std::move(motionState));
}

/**
 * Construct an empty motion state object
 */
MotionState::~MotionState(void)
{
    if (m_pFrame != nullptr)
        m_pFrame->removeMotionState(this);

    if (m_pState != nullptr)
        delete m_pState;
}

/**
 * Copy assignment operator
 */
MotionState &MotionState::operator = (const MotionState &motionState)
{
    if (&motionState != this)
    {
#if COPY_ASSIGN_MOTION_STATE_CACHING_OPTION
        m_bCacheTransformsEnabled = motionState.m_bCacheTransformsEnabled;
#endif
#if COPY_ASSIGN_MOTION_STATE_TRANSFORM_DEBUGGING_OPTION
        m_bDebugTransformsEnabled = motionState.m_bDebugTransformsEnabled;
#endif
        m_frameState = motionState.m_frameState;

        setFrame(motionState.getFrame());

        m_pFromAzimuthZenithConverter = motionState.m_pFromAzimuthZenithConverter;

        if (motionState.m_pState != nullptr)
        {
            if (m_pState != nullptr)
            {
                delete m_pState;
                m_pState = nullptr;
            }

            m_pState = motionState.m_pState->clone();
        }

        if (m_pState == nullptr)
            m_pState = ProjectedKinematicState::create();

        m_pToAzimuthZenithConverter = motionState.m_pToAzimuthZenithConverter;

        // clear this object's transformation cache
        if (m_bCacheTransformsEnabled)
            clearTransformationCache();
    }

    return *this;
}

/**
 * Move assignment operator
 */
MotionState &MotionState::operator = (MotionState &&motionState)
{
    if (&motionState != this)
    {
        motionState.swap(*this);
    }

    return *this;
}

/**
 * Equality operator overload
 */
bool MotionState::operator == (const MotionState &motionState)
{
    bool bEqual = isEqual(motionState, 0.0);
    if (bEqual)
        bEqual = (m_bCacheTransformsEnabled == motionState.m_bCacheTransformsEnabled);

    if (bEqual)
        bEqual = (m_bDebugTransformsEnabled == motionState.m_bDebugTransformsEnabled);

    if (bEqual)
        bEqual = (m_pFromAzimuthZenithConverter == motionState.m_pFromAzimuthZenithConverter);

    if (bEqual)
        bEqual = (m_pToAzimuthZenithConverter == motionState.m_pToAzimuthZenithConverter);

    return bEqual;
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const MotionState &motionState)
{
    return motionState.print(stream);
}

/**
 * Function to cache the current object's motion state in the transformation cache
 */
void MotionState::cacheCurrentState(void)
{
    if (m_bCacheTransformsEnabled && m_pFrame != nullptr && !m_pFrame->getName().empty())
    {
        auto *pFrameState = m_pFrame->getFrameState();
        if (pFrameState != nullptr)
            m_transformationCache[m_pFrame->getName()] = std::make_pair(std::unique_ptr<FrameState>(
                                                                        pFrameState->clone()),
                                                                        std::unique_ptr<KinematicState>(
                                                                        m_pState->clone()));
    }
}

/**
 * Get motion state transformation caching enabled/disabled
 */
bool MotionState::cacheTransformationsEnabled(void) const
{
    return m_bCacheTransformsEnabled;
}

/**
 * Set motion state transformation caching enabled/disabled
 */
void MotionState::cacheTransformationsEnabled(bool bEnabled)
{
    m_bCacheTransformsEnabled = bEnabled;
}

/**
 * Calculate this object's instantaneous angular acceleration vector with respect to its body axis. The
 * resultant angular acceleration vector is described in the current object's frame of reference
 */
Vector3d MotionState::calcAngularAcceleration(void) const
{
    return m_pState->getEulers().calcBodyAccelerations(m_pState->getEulerRates(),
                                                       m_pState->getEulerAccelerations());
}

/**
 * Calculate this object's instantaneous angular velocity vector with respect to its body axis. The resultant
 * angular velocity vector is described in the current object's frame of reference
 */
Vector3d MotionState::calcAngularVelocity(void) const
{
    return m_pState->getEulers().calcBodyRates(m_pState->getEulerRates());
}

/**
 * Calculate the time at which the current object's motion state approaches another motion state at the desired
 * range. The time returned represents the closest time, whether it be in the future or past, at which the
 * motion states approach one another at the given range. If the motion states do not approach one another
 * within the specified range, the subroutine returns infinity
 */
double MotionState::calcApproachTime(double range,
                                     const MotionState &motionState) const
{
    auto *pMotionState = &motionState;
    if (!pMotionState->isDescribedInFrame(m_pFrame))
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Motion states are not defined within the same frame of reference!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    // perform transformations, if necessary, to describe the input motion state within a Cartesian
    // coordinate system and within the current object's frame of reference
    if (!pMotionState->isDescribedInFrame(m_pFrame) ||
        pMotionState->getCoordinateType() != CoordinateType::Cartesian)
        pMotionState = pMotionState->transformClone(m_pFrame, CoordinateType::Cartesian);

    // perform transformations, if necessary, to describe the current object's motion state within a
    // Cartesian coordinate system
    auto *pThisMotionState = this;
    if (pThisMotionState->getCoordinateType() != CoordinateType::Cartesian)
        pThisMotionState = pThisMotionState->transformCloneToCoordinateSystem(CoordinateType::Cartesian);

    // if the following is true, no need to proceed...
    if (pThisMotionState->calcRange(m_pState->getTime(), *pMotionState) == range)
        return m_pState->getTime();

    auto &&acceleration = pThisMotionState->getAcceleration() - pMotionState->getAcceleration();
    auto &&position = pThisMotionState->getPosition() - pMotionState->getPosition(m_pState->getTime());
    auto &&velocity = pThisMotionState->getVelocity() - pMotionState->getVelocity(m_pState->getTime());

    auto a = 0.25 * acceleration.dot(acceleration);
    auto b = acceleration.dot(velocity);
    auto c = acceleration.dot(position) + velocity.dot(velocity);
    auto d = 2 * velocity.dot(position);
    auto e = position.dot(position) - range * range;

    double tApproach = 0.0;
    std::complex<double> roots[4];
    LowOrderPolynomialSolver polynomialSolver;
    auto rootsType = polynomialSolver.solve(a, b, c, d, e, roots);
    if (rootsType == LowOrderPolynomialSolver::RealRoots ||
        rootsType == LowOrderPolynomialSolver::RealAndComplexRoots)
    {
        // pick the time that is closest to the relative minimum approach time
        auto dt = std::numeric_limits<double>::max();
        for (int i = 0; i < 4; ++i)
        {
            auto root = roots[i].real();
            if (roots[i].imag() == 0.0 && std::fabs(root) < dt && root != 0.0)
                dt = root;
        }

        tApproach = m_pState->getTime() + dt;
    }
    else // all of the roots were imaginary, motion states do not come within the specified approach range
        tApproach = std::numeric_limits<double>::max();

    // if transformations had to be performed, dynamic memory was allocated and has to be deleted
    if (pMotionState != &motionState)
        delete pMotionState;

    if (pThisMotionState != this)
        delete pThisMotionState;

    return tApproach;
}

/**
 * Calculate the Cartesian motion state from the current object's spherical motion state
 * @param pCartesianMotionState the object in which the resultant Cartesian state will be stored
 */
bool MotionState::calcCartesianState(CartesianMotionState *pCartesianMotionState) const
{
    return calcCartesianState(pCartesianMotionState, m_pState->getTime());
}

/**
 * Calculate the Cartesian motion state from the current object's spherical motion state at the specified time
 * @param pCartesianMotionState the object in which the resultant Cartesian state will be stored
 * @param t                     the time to which the resultant motion state will be projected
 */
bool MotionState::calcCartesianState(CartesianMotionState *pCartesianMotionState,
                                     double t) const
{
    bool bSuccess = (pCartesianMotionState != nullptr);
    if (bSuccess)
    {
        auto *pSphericalMotionState = dynamic_cast<const SphericalMotionState *>(this);
        if (pSphericalMotionState != nullptr)
            pSphericalMotionState->calcCartesianState(pCartesianMotionState);
        else if (this != pCartesianMotionState)
            *pCartesianMotionState = (*this);

        pCartesianMotionState->update(t - m_pState->getTime());
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input argument is null!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return bSuccess;
}

/**
 * Calculate the distance from the trajectory defined by this object's motion state to the intersection on a
 * plane normal to the specified direction vector (not necessarily aligned with its velocity vector). The time
 * at which this plane intersection is projected to occur is returned via the first argument. The sign of the
 * distance returned will indicate whether the current object's motion state is moving toward the plane
 * (positive) or moving away from the plane (negative)
 * @param tPenetration the time at which the current object's motion state penetrates the plane normal to the
 *                     input direction vector
 * @param direction    the direction vector along which the motion state will be projected
 */
double MotionState::calcDistanceToPlaneIntersection(double &tPenetration,
                                                    const Vector3d &direction) const
{
    // project this object's motion state onto the input motion state
    CartesianMotionState thisMotionState(*this);
    thisMotionState.projectOnto(direction);

    return thisMotionState.calcLineOfSightDistance(tPenetration);
}

/**
 * Calculate the distance from the trajectory defined by this object's motion state to the intersection on a
 * plane normal to the instantaneous velocity vector of another motion state. The time at which this plane
 * intersection is projected to occur is returned via the first argument. The sign of the distance returned
 * will indicate whether the motion states are moving away from one another (negative) or moving toward one
 * another (positive). In the event that the input motion state's velocity vector magnitude is zero, the
 * orientation yaw, pitch Euler angles will be used to determine the direction along which the current object's
 * motion state will be projected
 * @param tPenetration the time at which the current object's motion state penetrates the plane normal to the
 *                     input motion state's direction of travel
 * @param motionState  a reference to a motion state object
 */
double MotionState::calcDistanceToPlaneIntersection(double &tPenetration,
                                                    const MotionState &motionState) const
{
    // project this object's motion state onto the input motion state
    CartesianMotionState thisMotionState(*this);
    thisMotionState.projectOnto(motionState);

    return thisMotionState.calcLineOfSightDistance(tPenetration);
}

/**
 * Calculate the line of sight distance and approach time with respect to the null motion state
 * @param tApproach the calculated time at which the line of sight distance is projected to be achieved
 */
double MotionState::calcLineOfSightDistance(double &tApproach) const
{
    auto range = calcRange(m_pState->getTime());
    auto rangeAccel = calcRangeAcceleration(m_pState->getTime());
    auto rangeRate = calcRangeRate(m_pState->getTime());

    std::complex<double> root1, root2;
    LowOrderPolynomialSolver polynomialSolver;
    auto rootsType = polynomialSolver.solve(0.5 * rangeAccel, rangeRate, range, root1, root2);
    if (rootsType == LowOrderPolynomialSolver::RealRoots ||
        rootsType == LowOrderPolynomialSolver::ComplexRoots) // there may be a very small imaginary
                                                             // component if iterative solver was invoked...
    {
        auto dt1 = root1.real();
        auto dt2 = root2.real();

        tApproach = m_pState->getTime();
        if (rangeAccel != 0.0)
            tApproach += (std::fabs(dt1) < std::fabs(dt2)) ? dt1 : dt2;
        else if (rangeRate != 0.0)
            tApproach += dt1;
    }
    else
        tApproach = std::numeric_limits<double>::max();

    auto &&position = getPosition(m_pState->getTime()) - getPosition(tApproach);
    auto distance = position.magnitude();

    return rangeRate > 0 ? -distance : distance;
}

/**
 * Calculate the minimum approach distance achieved with respect to another motion state
 * @param tApproach   the calculated time at which the minimum distance is achieved
 * @param motionState a reference to a motion state object
 */
double MotionState::calcMinimumApproach(double &tApproach,
                                        const MotionState &motionState) const
{
    auto *pMotionState = &motionState;
    if (!pMotionState->isDescribedInFrame(m_pFrame))
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Motion states are not defined within the same frame of reference!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    // perform transformations, if necessary, to describe the input motion state within a Cartesian
    // coordinate system and within the current object's frame of reference
    if (!pMotionState->isDescribedInFrame(m_pFrame) ||
        pMotionState->getCoordinateType() != CoordinateType::Cartesian)
        pMotionState = pMotionState->transformClone(m_pFrame, CoordinateType::Cartesian);

    // perform transformations, if necessary, to describe the current object's motion state within a
    // Cartesian coordinate system
    auto *pThisMotionState = this;
    if (pThisMotionState->getCoordinateType() != CoordinateType::Cartesian)
        pThisMotionState = pThisMotionState->transformCloneToCoordinateSystem(CoordinateType::Cartesian);

    auto &&acceleration = pThisMotionState->getAcceleration() - pMotionState->getAcceleration();
    auto position = pThisMotionState->getPosition() - pMotionState->getPosition(m_pState->getTime());
    auto &&velocity = pThisMotionState->getVelocity() - pMotionState->getVelocity(m_pState->getTime());

    auto a = 0.5 * acceleration.dot(acceleration);
    auto b = 1.5 * velocity.dot(acceleration);
    auto c = velocity.dot(velocity) + position.dot(acceleration);
    auto d = position.dot(velocity);

    std::complex<double> roots[3];
    LowOrderPolynomialSolver polynomialSolver;
    auto rootsType = polynomialSolver.solve(a, b, c, d, roots);
    if (rootsType == LowOrderPolynomialSolver::RealRoots ||
        rootsType == LowOrderPolynomialSolver::RealAndComplexRoots)
    {
        // pick the delta time that yields the smallest range
        auto pMinNormSq = std::numeric_limits<double>::max();
        auto dt = std::numeric_limits<double>::max();
        for (int i = 0; i < 3; ++i)
        {
            auto root = roots[i].real();
            if (roots[i].imag() == 0.0 && std::fabs(root) < dt && root != 0.0)
            {
                auto t = m_pState->getTime() + root;
                position = pThisMotionState->getPosition(t) - pMotionState->getPosition(t);
                auto pNormSq = position.normSquared();
                if (pNormSq < pMinNormSq)
                {
                    dt = root;
                    pMinNormSq = pNormSq;
                }
            }
        }

        tApproach = m_pState->getTime() + dt;
    }
    else // roots shouldn't be imaginary...
        tApproach = std::numeric_limits<double>::max();

    // finally, calculate the range at tApproach
    auto range = pThisMotionState->calcRange(tApproach, *pMotionState);

    // if transformations had to be performed, dynamic memory was allocated and has to be deleted
    if (pMotionState != &motionState)
        delete pMotionState;

    if (pThisMotionState != this)
        delete pThisMotionState;

    return range;
}

/**
 * Calculate the range
 */
double MotionState::calcRange(void) const
{
    return calcRange(m_pState->getTime());
}

/**
 * Calculate the range between two points motion states
 */
double MotionState::calcRange(const MotionState &motionState) const
{
    return calcRange(m_pState->getTime(), motionState);
}

/**
 * Calculate the range acceleration between two motion states
 */
double MotionState::calcRangeAcceleration(const MotionState &motionState) const
{
    return calcRangeAcceleration(m_pState->getTime(), motionState);
}

/**
 * Calculate the range rate between two motion states
 */
double MotionState::calcRangeRate(const MotionState &motionState) const
{
    return calcRangeRate(m_pState->getTime(), motionState);
}

/**
 * Calculate the scalar speed
 */
double MotionState::calcSpeed(void) const
{
    return calcSpeed(m_pState->getTime());
}

/**
 * Calculate the spherical motion state with angles in degrees from the current object's Cartesian motion state
 * @param pSphericalMotionState the object in which the resultant spherical state will be stored
 */
bool MotionState::calcSphericalState(SphericalMotionState *pSphericalMotionState) const
{
    return calcSphericalState(pSphericalMotionState, m_pState->getAngleUnits(), m_pState->getTime());
}

/**
 * Calculate the spherical motion state with angles in degrees from the current object's Cartesian motion state
 * at the specified time
 * @param pSphericalMotionState the object in which the resultant spherical state will be stored
 * @param t                     the time to which the resultant motion state will be projected
 */
bool MotionState::calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                     double t) const
{
    return calcSphericalState(pSphericalMotionState, m_pState->getAngleUnits(), t);
}

/**
 * Calculate the spherical motion state from the current object's Cartesian motion state
 * @param pSphericalMotionState the object in which the resultant spherical state will be stored
 * @param angleUnits            the desired angle output units, Degrees or Radians
 */
bool MotionState::calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                     const AngleUnitType &angleUnits) const
{
    return calcSphericalState(pSphericalMotionState, angleUnits, m_pState->getTime());
}

/**
 * Calculate the spherical motion state from the current object's Cartesian motion state at the specified time
 * @param pSphericalMotionState the object in which the resultant spherical state will be stored
 * @param angleUnits            the desired angle output units, Degrees or Radians
 * @param t                     the time to which the resultant motion state will be projected
 */
bool MotionState::calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                     const AngleUnitType &angleUnits, double t) const
{
    bool bSuccess = (pSphericalMotionState != nullptr);
    if (bSuccess)
    {
        auto *pCartesianMotionState = dynamic_cast<const CartesianMotionState *>(this);
        if (pCartesianMotionState != nullptr)
            pCartesianMotionState->calcSphericalState(pSphericalMotionState);
        else if (this != pSphericalMotionState)
            *pSphericalMotionState = *this;

        pSphericalMotionState->update(t - m_pState->getTime());
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input argument is null!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return bSuccess;
}

/**
 * Clear the transformation cache
 */
void MotionState::clearTransformationCache(void)
{
    m_transformationCache.clear();
}

/**
 * clone() function
 */
std::shared_ptr<MotionState> MotionState::cloneShared(void) const
{
    return std::shared_ptr<MotionState>(clone());
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void MotionState::convertAngleUnits(const AngleUnitType &angleUnits)
{
    m_pState->convertAngleUnits(angleUnits);
}

/**
 * Create a motion state within the given frame of reference
 * @param pFrame a pointer to the target reference frame
 * @param state  the perturbation state of this object's reference frame
 * @param type   the coordinate type
 */
MotionState *MotionState::create(ReferenceFrame *pFrame,
                                 const std::string &state,
                                 const CoordinateType &type)
{
    return create(pFrame, type, state);
}

/**
 * Create a motion state within the given frame of reference
 * @param pFrame a pointer to the target reference frame
 * @param type   the coordinate type
 * @param state  the perturbation state of this object's reference frame
 */
MotionState *MotionState::create(ReferenceFrame *pFrame,
                                 const CoordinateType &type,
                                 const std::string &state)
{
    MotionState *pMotionState = nullptr;
    switch (type)
    {
        case CoordinateType::Cartesian:
        pMotionState = CartesianMotionState::create(pFrame, state);
        break;

        case CoordinateType::Spherical:
        pMotionState = SphericalMotionState::create(pFrame, state);
        break;

        default:
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Unsupported coordinate system type.\n",
               getQualifiedStaticMethodName(__func__, MotionState));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return pMotionState;
}
#ifdef RAPID_XML
/**
 * Create a motion state within the given frame of reference using data from the specified XML element. If the
 * coordinate type is omitted in the XML input, a Cartesian coordinate type will be assumed; upon success,
 * function will return non-null
 */
MotionState *MotionState::createFromXML(xml_node<> *pNode,
                                        ReferenceFrame *pFrame)
{
    MotionState *pMotionState = nullptr;
    if (pNode != nullptr && std::strcmp(pNode->name(), "motionState") == 0)
    {
        CoordinateType coordType = CoordinateType::Cartesian;
        auto *pCoordinateSystemNode = pNode->first_node("coordinateSystem");
        if (pCoordinateSystemNode != nullptr)
            coordType = CoordinateType(pCoordinateSystemNode->value());

        pMotionState = create(pFrame, coordType);
        bool bSuccess = (pMotionState != nullptr);
        if (bSuccess)
        {
            bSuccess = pMotionState->readFromXML(pNode);
            if (!bSuccess)
            {
                delete pMotionState;
                pMotionState = nullptr;
            }
        }
    }

    return pMotionState;
}
#endif
/**
 * Create a shared motion state within the given frame of reference
 * @param pFrame a pointer to the target reference frame
 * @param state  the perturbation state of this object's reference frame
 * @param type   the coordinate type
 */
std::shared_ptr<MotionState> MotionState::createShared(ReferenceFrame *pFrame,
                                                       const std::string &state,
                                                       const CoordinateType &type)
{
    return std::shared_ptr<MotionState>(MotionState::create(pFrame, state, type));
}

/**
 * Create a shared motion state within the given frame of reference
 * @param pFrame a pointer to the target reference frame
 * @param type   the coordinate type
 * @param state  the perturbation state of this object's reference frame
 */
std::shared_ptr<MotionState> MotionState::createShared(ReferenceFrame *pFrame,
                                                       const CoordinateType &type,
                                                       const std::string &state)
{
    return std::shared_ptr<MotionState>(MotionState::create(pFrame, type, state));
}
#ifdef RAPID_XML
/**
 * Create a shared motion state within the given frame of reference using data from the specified XML element.
 * If the coordinate type is omitted in the XML input, a Cartesian coordinate type will be assumed; upon
 * success, function will return non-null
 */
std::shared_ptr<MotionState> MotionState::createSharedFromXML(xml_node<char> *pNode,
                                                              ReferenceFrame *pFrame)
{
    return std::shared_ptr<MotionState>(MotionState::createFromXML(pNode, pFrame));
}
#endif
/**
 * Query this object's flag to enable/disable motion state transformation debugging mode
 */
bool MotionState::debugTransformsEnabled(void) const
{
    return m_bDebugTransformsEnabled;
}

/**
 * Set this object's flag to enable/disable motion state transformation debugging mode
 */
void MotionState::debugTransformsEnabled(bool bEnabled)
{
    m_bDebugTransformsEnabled = bEnabled;
}

/**
 * Function to deserialize this object's data
 */
std::istream &MotionState::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_bCacheTransformsEnabled, sizeof(bool));
        stream.read((char *)&m_bDebugTransformsEnabled, sizeof(bool));
        m_pState->deserialize(stream);
        std::getline(stream, m_frameState, '\0');

        std::string frameName;
        std::getline(stream, frameName, '\0');

        // find a reference frame in the tree that also contains the current object's motion state
        if (m_pFrame != nullptr)
        {
            auto *pRoot = m_pFrame->getRoot();
            if (pRoot != nullptr)
            {
                auto *pFrame = pRoot->findFrame(frameName);
                if (pFrame != nullptr)
                    setFrame(pFrame);
            }
        }
    }

    return stream;
}

/**
 * Get position, velocity, or acceleration state vector
 */
Vector3d &MotionState::get(const StateDerivativeType &type)
{
    switch (type)
    {
        case StateDerivativeType::Acceleration: return m_pState->getAcceleration();
        case StateDerivativeType::Position: return m_pState->getPosition();
        case StateDerivativeType::Velocity: return m_pState->getVelocity();
        case StateDerivativeType::Unknown:
        // do nothing
        break;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Invalid StateDerivativeType.");
}

/**
 * Get position, velocity, or acceleration state vector (const overload)
 */
Vector3d MotionState::get(const StateDerivativeType &type) const
{
    switch (type)
    {
        case StateDerivativeType::Acceleration: return m_pState->getAcceleration();
        case StateDerivativeType::Position: return m_pState->getPosition();
        case StateDerivativeType::Velocity: return m_pState->getVelocity();
        case StateDerivativeType::Unknown:
        // do nothing
        break;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Invalid StateDerivativeType.");
}

/**
 * Get position, velocity, or acceleration state vector (const overload)
 */
void MotionState::get(const StateDerivativeType &type,
                      double state[3]) const
{
    get(type).get(state);
}

/**
 * Get roll, pitch, or yaw Euler acceleration
 */
double &MotionState::get(const EulerAccelerationAxisType &type)
{
    return m_pState->getEulerAccelerations().get(type);
}

/**
 * Get roll, pitch, or yaw Euler acceleration (const overload)
 */
double MotionState::get(const EulerAccelerationAxisType &type) const
{
    return m_pState->getEulerAccelerations().get(type);
}

/**
 * Get roll, pitch, or yaw Euler angle
 */
double &MotionState::get(const EulerAxisType &type)
{
    return m_pState->getEulers().get(type);
}

/**
 * Get roll, pitch, or yaw Euler angle (const overload)
 */
double MotionState::get(const EulerAxisType &type) const
{
    return m_pState->getEulers().get(type);
}

/**
 * Get roll, pitch, or yaw Euler rate
 */
double &MotionState::get(const EulerRateAxisType &type)
{
    return m_pState->getEulerRates().get(type);
}

/**
 * Get roll, pitch, or yaw Euler rate (const overload)
 */
double MotionState::get(const EulerRateAxisType &type) const
{
    return m_pState->getEulerRates().get(type);
}

/**
 * Get Euler, Euler rate, or Euler acceleration vector
 */
Eulers &MotionState::get(const EulerStateDerivativeType &type)
{
    switch (type)
    {
        case EulerStateDerivativeType::Accelerations: return m_pState->getEulerAccelerations();
        case EulerStateDerivativeType::Eulers: return m_pState->getEulers();
        case EulerStateDerivativeType::Rates: return m_pState->getEulerRates();
        case EulerStateDerivativeType::Unknown:
        // do nothing
        break;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Invalid EulerStateType.");
}

/**
 * Get Euler, Euler rate, or Euler acceleration vector (const overload)
 */
Eulers MotionState::get(const EulerStateDerivativeType &type) const
{
    switch (type)
    {
        case EulerStateDerivativeType::Accelerations: return m_pState->getEulerAccelerations();
        case EulerStateDerivativeType::Eulers: return m_pState->getEulers();
        case EulerStateDerivativeType::Rates: return m_pState->getEulerRates();
        case EulerStateDerivativeType::Unknown:
        // do nothing
        break;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Invalid EulerStateType.");
}

/**
 * Get Euler, Euler rate, or Euler acceleration vector
 */
void MotionState::get(const EulerStateDerivativeType &type,
                      double state[3]) const
{
    return get(type).get(state);
}

/**
 * Get the state acceleration
 */
Vector3d &MotionState::getAcceleration(void)
{
    return m_pState->getAcceleration();
}

/**
 * Get the state acceleration
 */
Vector3d MotionState::getAcceleration(void) const
{
    return m_pState->getAcceleration();
}

/**
 * Get angle units (Degrees or Radians)
 */
AngleUnitType MotionState::getAngleUnits(void) const
{
    return m_pState->getAngleUnits();
}

/**
 * Get the name of this class
 */
std::string MotionState::getClassName(void) const
{
    return "MotionState";
}

/**
 * Get a pointer to this object's member conversion function that defines how to convert standard definitions
 * of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a user-defined
 * horizontal, vertical angle convention
 */
tMotionStateAngleConversionFunction MotionState::getConversionFromAzimuthZenithFunction(void) const
{
    return m_pFromAzimuthZenithConverter;
}

/**
 * Get a pointer to this object's member function that defines how to convert a user-defined convention of
 * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
 * (measured from +z to -z) angles
 */
tMotionStateAngleConversionFunction MotionState::getConversionToAzimuthZenithFunction(void) const
{
    return m_pToAzimuthZenithConverter;
}

/**
 * Get a pointer to the global default conversion function that defines how to convert standard definitions of
 * spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a user-defined
 * horizontal, vertical angle convention
 */
tMotionStateAngleConversionFunction MotionState::getDefaultConversionFromAzimuthZenithFunction(void)
{
    return m_pDefaultFromAzimuthZenithConverter;
}

/**
 * Get a pointer to the global default function that defines how to convert a user-defined convention of
 * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
 * (measured from +z to -z) angles
 */
tMotionStateAngleConversionFunction MotionState::getDefaultConversionToAzimuthZenithFunction(void)
{
    return m_pDefaultToAzimuthZenithConverter;
}

/**
 * Get the global default transform caching option
 */
bool MotionState::getDefaultTransformCachingOption(void)
{
    return m_bDefaultTransformCachingOption;
}

/**
 * Get the Euler accelerations
 */
void MotionState::getEulerAccelerations(double &rollAccel,
                                        double &pitchAccel,
                                        double &yawAccel) const
{
    auto &&eulerAccelerations = m_pState->getEulerAccelerations();

    pitchAccel = eulerAccelerations.getPitch();
    rollAccel = eulerAccelerations.getRoll();
    yawAccel = eulerAccelerations.getYaw();
}

/**
 * Get the Euler accelerations (roll, pitch, yaw)
 */
void MotionState::getEulerAccelerations(double eulerAccelerations[3]) const
{
    getEulerRates(eulerAccelerations[EulerAccelerationAxisType::Roll],
                  eulerAccelerations[EulerAccelerationAxisType::Pitch],
                  eulerAccelerations[EulerAccelerationAxisType::Yaw]);
}

/**
 * Get the Euler accelerations
 */
Eulers &MotionState::getEulerAccelerations(void)
{
    return m_pState->getEulerAccelerations();
}

/**
 * Get the Euler accelerations
 */
Eulers MotionState::getEulerAccelerations(void) const
{
    return m_pState->getEulerAccelerations();
}

/**
 * Get the Euler rates
 */
void MotionState::getEulerRates(double &rollRate,
                                double &pitchRate,
                                double &yawRate) const
{
    getEulerRates(rollRate,
                  pitchRate,
                  yawRate,
                  m_pState->getTime());
}

/**
 * Get the Euler rates at the specified time
 */
void MotionState::getEulerRates(double &rollRate,
                                double &pitchRate,
                                double &yawRate,
                                double t) const
{
    m_pState->getEulerRates(rollRate,
                            pitchRate,
                            yawRate,
                            t);
}

/**
 * Get the Euler rates
 */
void MotionState::getEulerRates(double eulerRates[3]) const
{
    getEulerRates(eulerRates, m_pState->getTime());
}

/**
 * Get the Euler rates at the specified time
 */
void MotionState::getEulerRates(double eulerRates[3],
                                double t) const
{
    getEulerRates(eulerRates[EulerRateAxisType::Roll],
                  eulerRates[EulerRateAxisType::Pitch],
                  eulerRates[EulerRateAxisType::Yaw], t);
}

/**
 * Get the Euler rates
 */
Eulers &MotionState::getEulerRates(void)
{
    return m_pState->getEulerRates();
}

/**
 * Get the Euler rates
 */
Eulers MotionState::getEulerRates(void) const
{
    return m_pState->getEulerRates();
}

/**
 * Get the Euler rates at the specified time
 */
Eulers MotionState::getEulerRates(double t) const
{
    Eulers eulerRates(m_pState->getAngleUnits());

    getEulerRates(eulerRates, t);

    return eulerRates;
}

/**
 * Get the Euler angles
 */
void MotionState::getEulers(double &roll,
                            double &pitch,
                            double &yaw) const
{
    getEulers(roll, pitch, yaw, m_pState->getTime());
}

/**
 * Get the Euler angles at the specified time
 */
void MotionState::getEulers(double &roll,
                            double &pitch,
                            double &yaw,
                            double t) const
{
    m_pState->getEulers(roll, pitch, yaw, t);
}

/**
 * Get the Euler angles
 */
void MotionState::getEulers(double eulers[3]) const
{
    getEulers(eulers, m_pState->getTime());
}

/**
 * Get the Euler angles at the specified time
 */
void MotionState::getEulers(double eulers[3],
                            double t) const
{
    getEulers(eulers[EulerAxisType::Roll],
              eulers[EulerAxisType::Pitch],
              eulers[EulerAxisType::Yaw], t);
}

/**
 * Get the Euler angles
 */
Eulers &MotionState::getEulers(void)
{
    return m_pState->getEulers();
}

/**
 * Get the Euler angles
 */
Eulers MotionState::getEulers(void) const
{
    return m_pState->getEulers();
}

/**
 * Get the Euler angles at the specified time
 */
Eulers MotionState::getEulers(double t) const
{
    Eulers eulers(m_pState->getAngleUnits());

    getEulers(eulers, t);

    return eulers;
}

/**
 * Get the frame of reference in which this motion state is defined
 */
ReferenceFrame *MotionState::getFrame(void)
{
    return m_pFrame;
}

/**
 * Get the frame of reference in which this motion state is defined
 */
ReferenceFrame *MotionState::getFrame(void) const
{
    return m_pFrame;
}

/**
 * Return a string representing the coordinate reference frame and coordinate system in which this motion state
 * is described
 */
std::string MotionState::getFrameAndCoordinateSystem(void) const
{
    std::string frameAndCoordinateSystem;
    auto *pFrame = getFrame();
    if (pFrame != nullptr)
    {
        frameAndCoordinateSystem = pFrame->getName();
    }
    else
    {
        // this shouldn't happen
        frameAndCoordinateSystem = "null";

        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "This object's reference frame is null!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);

    }

    if (!frameAndCoordinateSystem.empty())
    {
        frameAndCoordinateSystem += "_";
    }

    frameAndCoordinateSystem += getCoordinateType().toString();

    return frameAndCoordinateSystem;
}

/**
 * Get the perturbation state of this object's reference frame
 */
std::string MotionState::getFrameState(void) const
{
    return m_frameState;
}

/**
 * Get the full kinematic state of this object
 */
KinematicState *MotionState::getKinematicState(void)
{
    return m_pState;
}

/**
 * Get the full kinematic state of this object
 */
KinematicState *MotionState::getKinematicState(void) const
{
    return m_pState;
}

/**
 * Get this object's pitch angle
 */
double MotionState::getPitch(void) const
{
    auto t = getTime();

    return getPitch(t);
}

/**
 * Get this object's pitch angle
 * @param t the time at which the return value will be computed
 */
double MotionState::getPitch(double t) const
{
    auto &&angleUnits = getAngleUnits();

    return getPitch(t, angleUnits);
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getPitch(const AngleUnitType &angleUnits) const
{
    auto t = getTime();

    return getPitch(t, angleUnits);
}

/**
 * Get this object's pitch angle
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getPitch(double t,
                             const AngleUnitType &angleUnits) const
{
    return m_pState->getPitch(angleUnits, t);
}

/**
 * Get this object's pitch acceleration
 */
double MotionState::getPitchAcceleration(void) const
{
    auto &&angleUnits = getAngleUnits();

    return getPitchAcceleration(angleUnits);
}

/**
 * Get this object's pitch acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getPitchAcceleration(const AngleUnitType &angleUnits) const
{
    return m_pState->getPitchAcceleration(angleUnits);
}

/**
 * Get this object's pitch rate
 */
double MotionState::getPitchRate(void) const
{
    auto t = getTime();

    return getPitchRate(t);
}

/**
 * Get this object's pitch rate
 * @param t the time at which the return value will be computed
 */
double MotionState::getPitchRate(double t) const
{
    auto &&angleUnits = getAngleUnits();

    return getPitchRate(t, angleUnits);
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getPitchRate(const AngleUnitType &angleUnits) const
{
    auto t = getTime();

    return getPitchRate(t, angleUnits);
}

/**
 * Get this object's pitch rate
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getPitchRate(double t,
                                 const AngleUnitType &angleUnits) const
{
    return m_pState->getPitchRate(angleUnits, t);
}

/**
 * Get the state position
 */
void MotionState::getPosition(double position[3]) const
{
    getPosition(position, m_pState->getTime());
}

/**
 * Get the state position
 */
Vector3d &MotionState::getPosition(void)
{
    return m_pState->getPosition();
}

/**
 * Get the state position
 */
Vector3d MotionState::getPosition(void) const
{
    return m_pState->getPosition();
}

/**
 * Get the state position at the specified time
 */
Vector3d MotionState::getPosition(double t) const
{
    Vector3d position;

    getPosition(position, t);

    return position;
}

/**
 * Get this object's roll angle
 */
double MotionState::getRoll(void) const
{
    auto t = getTime();

    return getRoll(t);
}

/**
 * Get this object's roll angle
 * @param t the time at which the return value will be computed
 */
double MotionState::getRoll(double t) const
{
    auto &&angleUnits = getAngleUnits();

    return getRoll(t, angleUnits);
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getRoll(const AngleUnitType &angleUnits) const
{
    auto t = getTime();

    return getRoll(t, angleUnits);
}

/**
 * Get this object's roll angle
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getRoll(double t,
                            const AngleUnitType &angleUnits) const
{
    return m_pState->getRoll(angleUnits, t);
}

/**
 * Get this object's roll acceleration
 */
double MotionState::getRollAcceleration(void) const
{
    auto &&angleUnits = getAngleUnits();

    return getRollRate(angleUnits);
}

/**
 * Get this object's roll acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getRollAcceleration(const AngleUnitType &angleUnits) const
{
    return m_pState->getRollAcceleration(angleUnits);
}

/**
 * Get this object's roll rate
 */
double MotionState::getRollRate(void) const
{
    auto t = getTime();

    return getRollRate(t);
}

/**
 * Get this object's roll rate
 * @param t the time at which the return value will be computed
 */
double MotionState::getRollRate(double t) const
{
    auto &&angleUnits = getAngleUnits();

    return getRollRate(t, angleUnits);
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getRollRate(const AngleUnitType &angleUnits) const
{
    auto t = getTime();

    return getRollRate(t, angleUnits);
}

/**
 * Get this object's roll rate
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getRollRate(double t,
                                const AngleUnitType &angleUnits) const
{
    return m_pState->getRollRate(angleUnits, t);
}

/**
 * Get the time
 */
double MotionState::getTime(void) const
{
    return m_pState->getTime();
}

/**
 * Get a reference to the transformation cache
 */
MotionState::tTransformationCache &MotionState::getTransformationCache(void)
{
    return m_transformationCache;
}

/**
 * Get the state velocity
 */
void MotionState::getVelocity(double velocity[3]) const
{
    getVelocity(velocity, m_pState->getTime());
}

/**
 * Get the state velocity
 */
Vector3d &MotionState::getVelocity(void)
{
    return m_pState->getVelocity();
}

/**
 * Get the state velocity
 */
Vector3d MotionState::getVelocity(void) const
{
    return m_pState->getVelocity();
}

/**
 * Get the state velocity at the specified time
 */
Vector3d MotionState::getVelocity(double t) const
{
    Vector3d velocity;

    getVelocity(velocity, t);

    return velocity;
}

/**
 * Get this object's yaw angle
 */
double MotionState::getYaw(void) const
{
    auto t = getTime();

    return getYaw(t);
}

/**
 * Get this object's yaw angle
 * @param t the time at which the return value will be computed
 */
double MotionState::getYaw(double t) const
{
    auto &&angleUnits = getAngleUnits();

    return getYaw(t, angleUnits);
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getYaw(const AngleUnitType &angleUnits) const
{
    auto t = getTime();

    return getYaw(t, angleUnits);
}

/**
 * Get this object's yaw angle
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getYaw(double t,
                           const AngleUnitType &angleUnits) const
{
    return m_pState->getYaw(angleUnits, t);
}

/**
 * Get this object's yaw acceleration
 */
double MotionState::getYawAcceleration(void) const
{
    auto &&angleUnits = getAngleUnits();

    return getYawAcceleration(angleUnits);
}

/**
 * Get this object's yaw acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getYawAcceleration(const AngleUnitType &angleUnits) const
{
    return m_pState->getYawAcceleration(angleUnits);
}

/**
 * Get this object's yaw rate
 */
double MotionState::getYawRate(void) const
{
    auto t = getTime();

    return getYawRate(t);
}

/**
 * Get this object's yaw rate
 * @param t the time at which the return value will be computed
 */
double MotionState::getYawRate(double t) const
{
    auto &&angleUnits = getAngleUnits();

    return getYawRate(t, angleUnits);
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getYawRate(const AngleUnitType &angleUnits) const
{
    auto t = getTime();

    return getYawRate(t, angleUnits);
}

/**
 * Get this object's yaw rate
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double MotionState::getYawRate(double t,
                               const AngleUnitType &angleUnits) const
{
    return m_pState->getYawRate(angleUnits, t);
}

/**
 * Initialization function
 */
bool MotionState::initialize(void)
{
    m_pState->setEulers(0.0, 0.0, 0.0);
    m_pState->setPosition(0.0, 0.0, 0.0);
    m_pState->setTime(0.0);

    initializeTimeDerivatives();

    return true;
}

/**
 * Initialization function to reset time-derivatives to zero
 */
void MotionState::initializeTimeDerivatives(void)
{
    m_pState->setAcceleration(0.0, 0.0, 0.0);
    m_pState->setEulerAccelerations(0.0, 0.0, 0.0);
    m_pState->setEulerRates(0.0, 0.0, 0.0);
    m_pState->setVelocity(0.0, 0.0, 0.0);
}

/**
 * Determines whether or not two motion states are defined in the same frame of reference belonging to the same
 * tree
 */
bool MotionState::inSameFrameAndTree(const MotionState &motionState) const
{
    return getFrame() == motionState.getFrame();
}

/**
 * Determines whether or not two motion states are defined in frames that belong to the same tree
 */
bool MotionState::inSameTree(const MotionState &motionState) const
{
    bool bSameFrame = false;
    if (m_pFrame != nullptr && motionState.m_pFrame != nullptr)
        bSameFrame = m_pFrame->isFamily(motionState.m_pFrame);

    return bSameFrame;
}

/**
 * Queries whether or not this motion state is described in Cartesian coordinates
 */
bool MotionState::isCartesian(void) const
{
    return dynamic_cast<const CartesianMotionState *>(this) != nullptr;
}

/**
 * Determines whether or not the current object is described in the specified frame of reference
 */
bool MotionState::isDescribedInFrame(ReferenceFrame *pFrame) const
{
    bool bSameFrame = (getFrame() == pFrame);
    if (!bSameFrame && m_pFrame != nullptr && pFrame != nullptr)
        bSameFrame = (pFrame->getName() == m_pFrame->getName());

    return bSameFrame;
}

/**
 * Determines whether or not two motion states are equivalent within the specified tolerance; this function
 * also tests for whether or not the motion states are defined in frames having the same name, but the
 * associated frames could actually belong to different trees.
 */
bool MotionState::isEqual(const MotionState &motionState,
                          double tol) const
{
    bool bEqual = (&motionState == this);
    if (!bEqual)
    {
        bool bSuccess = (motionState.m_pFrame != nullptr && m_pFrame != nullptr);
        if (bSuccess)
        {
            bEqual = (motionState.m_pFrame->getName() == m_pFrame->getName());
            if (bEqual)
                bEqual = m_pState->isEqual(*motionState.m_pState, tol);

            if (bEqual)
                bEqual = (m_frameState == motionState.m_frameState);
        }
    }

    return bEqual;
}

/**
 * Query whether or not this motion state is translationally or rotationally moving using the specified
 * tolerance
 */
bool MotionState::isMoving(double tol) const
{
    return isRotating() || isTranslating();
}

/**
 * Query whether or not this motion state is rotating using the specified tolerance
 */
bool MotionState::isRotating(double tol) const
{
    Vector3d zero(0.0, 0.0, 0.0);

    return !m_pState->getEulerAccelerations().isEqual(zero, tol) ||
           !m_pState->getEulerRates().isEqual(zero, tol);
}

/**
 * Queries whether or not this motion state is described in spherical coordinates
 */
bool MotionState::isSpherical(void) const
{
    return dynamic_cast<const SphericalMotionState *>(this) != nullptr;
}

/**
 * Queries whether or not this motion state is stationary (not rotating and not moving) using the specified
 * tolerance
 */
bool MotionState::isStationary(double tol) const
{
    return !isMoving(tol);
}

/**
 * Query whether or not this motion state is moving translationally using the specified tolerance
 */
bool MotionState::isTranslating(double tol) const
{
    Vector3d zero(0.0, 0.0, 0.0);

    return !m_pState->getAcceleration().isEqual(zero, tol) ||
           !m_pState->getVelocity().isEqual(zero, tol);
}

/**
 * Function to print the contents of the motion state
 */
std::ostream &MotionState::print(std::ostream &stream) const
{
    if (stream)
    {
        stream << "Time (s):                               { " << m_pState->getTime() << " }" << std::endl
               << "Angle Type:                             { " << m_pState->getAngleUnits() << " }"
               << std::endl
               << "Reference Frame:                        { " << m_pFrame->getName() << " }" << std::endl
               << "Eulers (roll, pitch, yaw):             " << m_pState->getEulers() << std::endl
               << "Euler Rates (roll, pitch, yaw):        " << m_pState->getEulerRates() << std::endl
               << "Euler Accelerations (roll, pitch, yaw):" << m_pState->getEulerAccelerations();
    }

    return stream;
}

/**
 * Project this object's motion state in the direction specified by the input direction vector
 */
void MotionState::projectOnto(const Vector3d &direction)
{
    // perform transformations, if necessary, to describe the input motion state within a Cartesian
    // coordinate system
    auto *pThisMotionState = this;
    if (pThisMotionState->getCoordinateType() != CoordinateType::Cartesian)
        pThisMotionState = pThisMotionState->transformCloneToCoordinateSystem(CoordinateType::Cartesian);

    (*pThisMotionState)[StateDerivativeType::Acceleration].project(direction);
    (*pThisMotionState)[StateDerivativeType::Position].project(direction);
    (*pThisMotionState)[StateDerivativeType::Velocity].project(direction);

    // if a transformation had to be performed, dynamic memory was allocated and has to be deleted
    if (pThisMotionState != this)
    {
        operator = (*pThisMotionState);

        delete pThisMotionState;
    }
}

/**
 * Project this object's motion state into the direction of the input motion state's velocity vector. If the
 * input motion state has no velocity, the orientation yaw, pitch Euler angles will be used to define the unit
 * direction vector along which the current object's motion state will be projected.
 */
void MotionState::projectOnto(const MotionState &motionState)
{
    // temporarily move the time at which this motion state is defined up to the input
    // motion state's time; this is done to prevent the motion state from being time-projected
    auto currentT = m_pState->getTime();
    m_pState->setTime(motionState.getTime());

    projectOnto(motionState, m_pState->getTime());

    m_pState->setTime(currentT); // set this motion state's time back to its original value
}

/**
 * Project this object's motion state at time t into the direction of the input motion state's velocity vector.
 * If the input motion state has no velocity, the orientation yaw, pitch Euler angles will be used to define
 * the unit direction vector along which the current object's motion state will be projected. This is a
 * temporal transformation; therefore all frames and motion states will be projected to time t
 */
void MotionState::projectOnto(const MotionState &motionState,
                              double t)
{
    auto *pThisMotionState = this;
    auto *pMotionState = &motionState;
    auto *pMotionStateFrame = pMotionState->getFrame();
    if (getFrame() == nullptr)
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Reference frame was not assigned to the current MotionState.\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }
    else if (pMotionStateFrame == nullptr)
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Reference frame was not assigned to the input MotionState.\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }
    else
    {
        if (!pThisMotionState->isDescribedInFrame(pMotionStateFrame))
        {
            StaticSynchronizable<MotionState>::lock(0);
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Motion states are not defined within the same frame of reference!\n",
                   getQualifiedMethodName(__func__));
            StaticSynchronizable<MotionState>::unlock(0);
        }

        // convert this object's motion state into the frame of the input motion state
        transformToFrame(pMotionStateFrame, t);

        // perform transformations, if necessary, to describe the input motion state within a Cartesian
        // coordinate system
        if (pMotionState->getCoordinateType() != CoordinateType::Cartesian)
            pMotionState = pMotionState->transformCloneToCoordinateSystem(CoordinateType::Cartesian);

        // perform transformations, if necessary, to describe the current object's motion state within a
        // Cartesian coordinate system
        auto *pThisMotionState = this;
        if (pThisMotionState->getCoordinateType() != CoordinateType::Cartesian)
            pThisMotionState = pThisMotionState->transformCloneToCoordinateSystem(CoordinateType::Cartesian);

        // project this object's motion state in the direction of the input motion state's velocity vector
        auto &&velocity = pMotionState->getVelocity(t);
        auto &&direction = velocity.calcUnit();
        if (direction.magnitude() == 0.0)
        {
//          StaticSynchronizable<MotionState>::lock(0);
//          logMsg(std::cout, LoggingLevel::Enum::Warning,
//                 "Input motion state has zero-magnitude velocity vector, orientation "
//                 "Eulers will be used to determine direction vector.\n",
//                 getQualifiedMethodName(__func__));
//          StaticSynchronizable<MotionState>::unlock(0);

            // construct a motion state from the input motion state's Euler angles
            auto &&eulers = pMotionState->getEulers(t);
            SphericalMotionState sphericalMotionState(m_frameState);
            sphericalMotionState.setAngleUnits(pMotionState->getAngleUnits());
            sphericalMotionState.setPosition(eulers.getYaw(), eulers.getPitch(), 1.0);

            auto *pConverter = pMotionState->getConversionFromAzimuthZenithFunction();
            sphericalMotionState.setConversionFromAzimuthZenithFunction(pConverter);

            // convert motion state to Cartesian coordinates to get the direction vector
            CartesianMotionState cartesianMotionState(m_frameState);
            sphericalMotionState.calcCartesianState(&cartesianMotionState);
            cartesianMotionState.getPosition(direction);
        }

        auto &&acceleration = pThisMotionState->getAcceleration() - pMotionState->getAcceleration();
        auto &&position = pThisMotionState->getPosition(t) - pMotionState->getPosition(t);
        velocity = pThisMotionState->getVelocity(t) - velocity;

        acceleration.project(direction);
        position.project(direction);
        velocity.project(direction);

        pThisMotionState->update(t);
        pThisMotionState->setAcceleration(acceleration);
        pThisMotionState->setPosition(position);
        pThisMotionState->setVelocity(velocity);

        // if transformations had to be performed, dynamic memory was allocated and has to be deleted
        if (pMotionState != &motionState)
            delete pMotionState;

        if (pThisMotionState != this)
        {
            operator = (*pThisMotionState);
            delete pThisMotionState;
        }
    }
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool MotionState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "motionState") == 0);
    if (bSuccess)
    {
        bSuccess = (m_pFrame != nullptr);
        if (!bSuccess)
        {
            StaticSynchronizable<MotionState>::lock(0);
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Reference frame was not assigned to the current MotionState!\n",
                   getQualifiedMethodName(__func__));
            StaticSynchronizable<MotionState>::unlock(0);
        }

        if (bSuccess)
        {
            auto *pKinematicStateNode = pNode->first_node("kinematicState");
            if (pKinematicStateNode != nullptr)
                bSuccess = m_pState->readFromXML(pKinematicStateNode);
        }

        if (bSuccess)
        {
            auto *pCoordinateSystemNode = pNode->first_node("coordinateSystem");
            if (pCoordinateSystemNode != nullptr)
            {
                CoordinateType coordType(pCoordinateSystemNode->value());
                bSuccess = (coordType == getCoordinateType());
                if (!bSuccess)
                {
                    StaticSynchronizable<MotionState>::lock(0);
                    logMsg(std::cout, LoggingLevel::Enum::Warning,
                           "The current object's coordinate type \"" + std::to_string(getCoordinateType()) +
                           "\" does not match the coordinate type \"" + std::to_string(coordType) +
                           "\" specified in XML input!\n",
                           getQualifiedMethodName(__func__));
                    StaticSynchronizable<MotionState>::unlock(0);
                }
            }
        }

        if (bSuccess)
        {
            auto *pRferenceFrameNode = pNode->first_node("referenceFrame");
            if (pRferenceFrameNode != nullptr)
            {
                auto &&frameName = pRferenceFrameNode->value();
                bSuccess = (std::strcmp(frameName, m_pFrame->getName().c_str()) == 0);
                if (!bSuccess)
                {
                    StaticSynchronizable<MotionState>::lock(0);
                    logMsg(std::cout, LoggingLevel::Enum::Warning,
                           "Attempting to read data defined in \"" + std::string(frameName) + "\" frame "
                           "of reference into the current object, which is defined in \"" +
                           m_pFrame->getName() + "\" frame of reference.\n",
                           getQualifiedMethodName(__func__));
                    StaticSynchronizable<MotionState>::unlock(0);
                }
            }
        }

        if (bSuccess)
        {
            auto *pCacheTransformsEnabledNode = pNode->first_node("cacheTransforms");
            if (pCacheTransformsEnabledNode != nullptr)
                cacheTransformationsEnabled(std::strcmp(pCacheTransformsEnabledNode->value(), "true") == 0);
//            else
//                m_bCacheTransformsEnabled = false;

            auto *pDebugTransformsEnabledNode = pNode->first_node("debugTransforms");
            if (pDebugTransformsEnabledNode != nullptr)
                debugTransformsEnabled(std::strcmp(pDebugTransformsEnabledNode->value(), "true") == 0);
//            else
//                m_bDebugTransformsEnabled = false;

            auto *pFrameStateNode = pNode->first_node("frameState");
            if (pFrameStateNode != nullptr)
                m_frameState = pFrameStateNode->value();
//            else
//                m_frameState = DEFAULT_FRAME_STATE;

            auto *pFromAzimuthZenithConverterNode = pNode->first_node("fromAzimuthZenithConversion");
            if (pFromAzimuthZenithConverterNode != nullptr)
            {
                auto &&name = pFromAzimuthZenithConverterNode->value();
                SphericalConversionType type(name);
                m_pFromAzimuthZenithConverter = SphericalMotionState::getSphericalConversionFunction(type);
            }

            auto *pToAzimuthZenithConverterElementNode = pNode->first_node("toAzimuthZenithConversion");
            if (pToAzimuthZenithConverterElementNode != nullptr)
            {
                auto &&name = pToAzimuthZenithConverterElementNode->value();
                SphericalConversionType type(name);
                m_pToAzimuthZenithConverter = SphericalMotionState::getSphericalConversionFunction(type);
            }
        }
    }

    return bSuccess;
}
#endif
#if 0
/**
 * Function to register a coordinate system conversion function pair
 * @param inputCoordinateSystem              the name of the source coordinate system
 * @param outputCoordinateSystem             the name of the destination coordinate system
 * @param coordinateSystemConversionFunction a pointer to the coordinate conversion function which takes the
 *                                           motion state from the source coordinate system to the destination
 *                                           coordinate system
 */
bool MotionState::registerCoordinateSystemConversionPair(const std::string &sourceCoordinateSystem,
                                                         const std::string &destCoordinateSystem,
                                                         tCoordinateSystemConversionFunction
                                                         coordinateSystemConversionFunction)
{
    bool bSuccess = (coordinateSystemConversionFunction != nullptr);
    if (bSuccess)
    {
        auto &&sourceDestNamePair = std::make_pair(sourceCoordinateSystem, destCoordinateSystem);

        m_coordinateSystemConversionFunctionMap.emplace(sourceDestNamePair,
                                                        coordinateSystemConversionFunction);
    }

    return bSuccess;
}
#endif
/**
 * Scale Euler angle accelerations in roll, pitch, or yaw
 */
void MotionState::scale(const EulerAccelerationAxisType &type,
                        double value)
{
    m_pState->getEulerAccelerations().scale(type, value);
}

/**
 * Scale Euler angle rates in roll, pitch, or yaw
 */
void MotionState::scale(const EulerRateAxisType &type,
                        double value)
{
    m_pState->getEulerRates().scale(type, value);
}

/**
 * Scale Euler angles in roll, pitch, or yaw
 */
void MotionState::scale(const EulerAxisType &type,
                        double value)
{
    m_pState->getEulers().scale(type, value);
}

/**
 * Scale the acceleration state vector
 */
void MotionState::scaleAcceleration(double value)
{
    m_pState->getAcceleration().scale(value);
}

/**
 * Scale the Euler acceleration state vector
 */
void MotionState::scaleEulerAccelerations(double value)
{
    m_pState->getEulerAccelerations().scale(value);
}

/**
 * Scale the Euler rate state vector
 */
void MotionState::scaleEulerRates(double value)
{
    m_pState->getEulerRates().scale(value);
}

/**
 * Scale the Euler state vector
 */
void MotionState::scaleEulers(double value)
{
    m_pState->getEulers().scale(value);
}

/**
 * Scale the position state vector
 */
void MotionState::scalePosition(double value)
{
    m_pState->getPosition().scale(value);
}

/**
 * Scale the velocity state vector
 */
void MotionState::scaleVelocity(double value)
{
    m_pState->getVelocity().scale(value);
}

/**
 * Function to serialize this object's data
 */
std::ostream &MotionState::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_bCacheTransformsEnabled, sizeof(bool));
        stream.write((const char *)&m_bDebugTransformsEnabled, sizeof(bool));
        m_pState->serialize(stream);
        stream << m_frameState << '\0';
        stream << m_pFrame->getName() << '\0';
    }

    return stream;
}

/**
 * Set roll, pitch, or yaw Euler acceleration
 */
void MotionState::set(const EulerAccelerationAxisType &type,
                      double eulerAcceleration)
{
    m_pState->getEulerAccelerations().set(type, eulerAcceleration);
}

/**
 * Set roll, pitch, or yaw Euler rate
 */
void MotionState::set(const EulerRateAxisType &type,
                      double eulerRate)
{
    m_pState->getEulerRates().set(type, eulerRate);
}

/**
 * Set Euler angle, rate, or acceleration vector
 */
void MotionState::set(const EulerStateDerivativeType &type,
                      const Eulers &state)
{
    switch (type)
    {
        case EulerStateDerivativeType::Accelerations:
        m_pState->setEulerAccelerations(state);
        m_pState->getEulerAccelerations().convertAngleUnits(m_pState->getAngleUnits());
        break;

        case EulerStateDerivativeType::Eulers:
        m_pState->setEulers(state);
        m_pState->getEulers().convertAngleUnits(m_pState->getAngleUnits());
        break;

        case EulerStateDerivativeType::Rates:
        m_pState->setEulerRates(state);
        m_pState->getEulerRates().convertAngleUnits(m_pState->getAngleUnits());
        break;

        case EulerStateDerivativeType::Unknown:
        // do nothing
        break;
    }
}

/**
 * Set roll, pitch, or yaw Euler angle
 */
void MotionState::set(const EulerAxisType &type,
                      double angle)
{
    m_pState->getEulers().set(type, angle);
}

/**
 * Set position, velocity, or acceleration state vector
 */
void MotionState::set(const StateDerivativeType &type,
                      const Vector3d &state)
{
    switch (type)
    {
        case StateDerivativeType::Acceleration:
        m_pState->setAcceleration(state);
        break;

        case StateDerivativeType::Position:
        m_pState->setPosition(state);
        break;

        case StateDerivativeType::Velocity:
        m_pState->setVelocity(state);
        break;

        case StateDerivativeType::Unknown:
        // do nothing
        break;
    }
}

/**
 * Set angle units (Degrees or Radians)
 */
void MotionState::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_pState->setAngleUnits(angleUnits);
}

/**
 * Set a pointer to this object's member conversion function that defines how to convert standard definitions
 * of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a user-defined
 * horizontal, vertical angle convention
 */
void MotionState::setConversionFromAzimuthZenithFunction(tMotionStateAngleConversionFunction pConverter)
{
    m_pFromAzimuthZenithConverter = pConverter;
}

/**
 * Set a pointer to this object's member function that defines how to convert a user-defined convention of
 * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
 * (measured from +z to -z) angles
 */
void MotionState::setConversionToAzimuthZenithFunction(tMotionStateAngleConversionFunction pConverter)
{
    m_pToAzimuthZenithConverter = pConverter;
}

/**
 * Set a pointer to the global default conversion function that defines how to convert standard definitions of
 * spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a user-defined
 * horizontal, vertical angle convention
 */
void MotionState::setDefaultConversionFromAzimuthZenithFunction(tMotionStateAngleConversionFunction pConverter)
{
    m_pDefaultFromAzimuthZenithConverter = pConverter;
}

/**
 * Set a pointer to the global default function that defines how to convert a user-defined convention of
 * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
 * (measured from +z to -z) angles
 */
void MotionState::setDefaultConversionToAzimuthZenithFunction(tMotionStateAngleConversionFunction pConverter)
{
    m_pDefaultToAzimuthZenithConverter = pConverter;
}

/**
 * Set the global default transform caching option
 */
void MotionState::setDefaultTransformCachingOption(bool bTransformCachingEnabled)
{
    m_bDefaultTransformCachingOption = bTransformCachingEnabled;
}

/**
 * Set Euler accelerations used to propagate Euler rates
 */
void MotionState::setEulerAccelerations(const Eulers &eulerAccelerations)
{
    setEulerAccelerations(eulerAccelerations[EulerAccelerationAxisType::Roll],
                          eulerAccelerations[EulerAccelerationAxisType::Pitch],
                          eulerAccelerations[EulerAccelerationAxisType::Yaw]);

    m_pState->getEulerAccelerations().convertAngleUnits(m_pState->getAngleUnits());
}

/**
 * Set Euler accelerations used to propagate Euler rates
 */
void MotionState::setEulerAccelerations(double rollAcceleration,
                                        double pitchAcceleration,
                                        double yawAcceleration)
{
    auto &eulerAccelerations = m_pState->getEulerAccelerations();

    eulerAccelerations.setPitch(pitchAcceleration);
    eulerAccelerations.setRoll(rollAcceleration);
    eulerAccelerations.setYaw(yawAcceleration);
}

/**
 * Set Euler rates used to propagate Euler angles
 */
void MotionState::setEulerRates(const Eulers &eulerRates)
{
    setEulerRates(eulerRates[EulerAccelerationAxisType::Roll],
                  eulerRates[EulerAccelerationAxisType::Pitch],
                  eulerRates[EulerAccelerationAxisType::Yaw]);

    m_pState->getEulerRates().convertAngleUnits(m_pState->getAngleUnits());
}

/**
 * Set Euler rates used to propagate Euler angles
 */
void MotionState::setEulerRates(double rollRate,
                                double pitchRate,
                                double yawRate)
{
    auto &eulerRates = m_pState->getEulerRates();

    eulerRates.setPitch(pitchRate);
    eulerRates.setRoll(rollRate);
    eulerRates.setYaw(yawRate);
}

/**
 * Set the Euler angles
 */
void MotionState::setEulers(const Eulers &eulers)
{
    setEulers(eulers[EulerAccelerationAxisType::Roll],
              eulers[EulerAccelerationAxisType::Pitch],
              eulers[EulerAccelerationAxisType::Yaw]);

    m_pState->getEulers().convertAngleUnits(m_pState->getAngleUnits());
}

/**
 * Set the Euler angles
 */
void MotionState::setEulers(double roll,
                            double pitch,
                            double yaw)
{
    auto &eulers = getEulers();

    eulers.setPitch(pitch);
    eulers.setRoll(roll);
    eulers.setYaw(yaw);
}

/**
 * Set the frame of reference in which this motion state is defined
 */
bool MotionState::setFrame(ReferenceFrame *pFrame)
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess && pFrame != m_pFrame)
    {
        pFrame->addMotionState(this);
        m_pFrame = pFrame;
    }

    return bSuccess;
}

/**
 * Set the perturbation state of this object's reference frame
 * @param state the perturbation state of this object's reference frame
 */
void MotionState::setFrameState(const std::string &state)
{
    m_frameState = state;
}

/**
 * Set the full kinematic state of this object; upon success, this object takes ownership of the input argument
 * and becomes responsible for its destruction
 */
bool MotionState::setKinematicState(KinematicState *pState)
{
    bool bSuccess = (pState != nullptr);
    if (bSuccess)
    {
        if (m_pState != nullptr && pState != m_pState)
        {
            delete m_pState;
            m_pState = pState;
        }
    }

    return bSuccess;
}

/**
 * Set this object's pitch angle
 */
void MotionState::setPitch(double pitch)
{
    auto &&angleUnits = getAngleUnits();

    setPitch(pitch, angleUnits);
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setPitch(double pitch,
                           const AngleUnitType &angleUnits)
{
    m_pState->setPitch(pitch, angleUnits);
}

/**
 * Set this object's pitch acceleration
 */
void MotionState::setPitchAcceleration(double pitchAcceleration)
{
    auto &&angleUnits = getAngleUnits();

    setPitchAcceleration(pitchAcceleration, angleUnits);
}

/**
 * Set this object's pitch acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setPitchAcceleration(double pitchAcceleration,
                                       const AngleUnitType &angleUnits)
{
    m_pState->setPitchAcceleration(pitchAcceleration, angleUnits);
}

/**
 * Set this object's pitch rate
 */
void MotionState::setPitchRate(double pitchRate)
{
    auto &&angleUnits = getAngleUnits();

    setPitchRate(pitchRate, angleUnits);
}

/**
 * Set this object's pitch rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setPitchRate(double pitchRate,
                               const AngleUnitType &angleUnits)
{
    m_pState->setPitchRate(pitchRate, angleUnits);
}

/**
 * Set this object's roll angle
 */
void MotionState::setRoll(double roll)
{
    auto &&angleUnits = getAngleUnits();

    setRoll(roll, angleUnits);
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setRoll(double roll,
                          const AngleUnitType &angleUnits)
{
    m_pState->setRoll(roll, angleUnits);
}

/**
 * Set this object's roll acceleration
 */
void MotionState::setRollAcceleration(double rollAcceleration)
{
    auto &&angleUnits = getAngleUnits();

    setRollAcceleration(rollAcceleration, angleUnits);
}

/**
 * Set this object's roll acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setRollAcceleration(double rollAcceleration,
                                      const AngleUnitType &angleUnits)
{
    m_pState->setRollAcceleration(rollAcceleration, angleUnits);
}

/**
 * Set this object's roll rate
 */
void MotionState::setRollRate(double rollRate)
{
    auto &&angleUnits = getAngleUnits();

    setRollRate(rollRate, angleUnits);
}

/**
 * Set this object's roll rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setRollRate(double rollRate,
                              const AngleUnitType &angleUnits)
{
    m_pState->setRollRate(rollRate, angleUnits);
}

/**
 * Set the time at which the initial position and velocity are defined for the state
 */
void MotionState::setTime(double t0)
{
    m_pState->setTime(t0);
}

/**
 * Set this object's yaw angle
 */
void MotionState::setYaw(double yaw)
{
    auto &&angleUnits = getAngleUnits();

    setYaw(yaw, angleUnits);
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setYaw(double yaw,
                         const AngleUnitType &angleUnits)
{
    m_pState->setYaw(yaw, angleUnits);
}

/**
 * Set this object's yaw acceleration
 */
void MotionState::setYawAcceleration(double yawAcceleration)
{
    auto &&angleUnits = getAngleUnits();

    setYawAcceleration(yawAcceleration, angleUnits);
}

/**
 * Set this object's yaw acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setYawAcceleration(double yawAcceleration,
                                     const AngleUnitType &angleUnits)
{
    m_pState->setYawAcceleration(yawAcceleration);
}

/**
 * Set this object's yaw rate
 */
void MotionState::setYawRate(double yawRate)
{
    auto &&angleUnits = getAngleUnits();

    setYawRate(yawRate, angleUnits);
}

/**
 * Set this object's yaw rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void MotionState::setYawRate(double yawRate,
                             const AngleUnitType &angleUnits)
{
    m_pState->setYawRate(yawRate, angleUnits);
}

/**
 * Swap function
 */
void MotionState::swap(MotionState &motionState)
{
    Loggable<std::string, std::ostream>::swap(motionState);

    std::swap(m_bCacheTransformsEnabled, motionState.m_bCacheTransformsEnabled);
    std::swap(m_bDebugTransformsEnabled, motionState.m_bDebugTransformsEnabled);
    m_frameState.swap(motionState.m_frameState);
    std::swap(m_pFrame, motionState.m_pFrame);
    std::swap(m_pFromAzimuthZenithConverter, motionState.m_pFromAzimuthZenithConverter);
    std::swap(m_pState, motionState.m_pState);
    std::swap(m_pToAzimuthZenithConverter, motionState.m_pToAzimuthZenithConverter);
    m_transformationCache.swap(motionState.m_transformationCache);
}

/**
 * Convert this object's motion state into a ReferenceFrame. Upon success, returns a dynamically allocated
 * ReferenceFrame object. Fails if the current object's motion state is not defined within a valid (non- null)
 * reference frame. It is up to the caller to destroy the reference frame object by making a call to
 * ReferenceFrame::delete()
 */
ReferenceFrame *MotionState::toReferenceFrame(const std::string &name) const
{
    ReferenceFrame *pFrame = nullptr;
    if (m_pFrame != nullptr)
    {
        pFrame = m_pFrame->findChild(name);
        if (pFrame == nullptr)
            pFrame = m_pFrame->createChild(name);

        toReferenceFrame(pFrame);
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Reference frame was not assigned to the current MotionState.\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return pFrame;
}

/**
 * Convert this object's motion state into a ReferenceFrame. Upon success, the function populates the input
 * reference frame, if non-null
 */
bool MotionState::toReferenceFrame(ReferenceFrame *pFrame) const
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        const CartesianMotionState *pCartesianMotionState = nullptr;
        bool bCartesian = isCartesian();
        if (!bCartesian)
            pCartesianMotionState = new CartesianMotionState(*this);
        else
            pCartesianMotionState = dynamic_cast<const CartesianMotionState *>(this);

        pFrame->setAcceleration(pCartesianMotionState->getAcceleration(), m_frameState);
        pFrame->setAngleUnits(pCartesianMotionState->getAngleUnits(), m_frameState);
        pFrame->setParent(m_pFrame);
        pFrame->setOrientation(pCartesianMotionState->getEulers(), m_frameState);
        pFrame->setOrigin(pCartesianMotionState->getPosition(), m_frameState);
        pFrame->setRotationalAccelerations(pCartesianMotionState->getEulerAccelerations(), m_frameState);
        pFrame->setRotationalRates(pCartesianMotionState->getEulerRates(), m_frameState);
        pFrame->setVelocity(pCartesianMotionState->getVelocity(), m_frameState);
        pFrame->setTime(pCartesianMotionState->getTime(), m_frameState);

        if (!bCartesian)
            delete pCartesianMotionState;
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input reference frame object is null, transformation failed!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return bSuccess;
}

/**
 * Convert this object's motion state into a shared ReferenceFrame. Upon success, returns a dynamically
 * allocated ReferenceFrame object. Fails if the current object's motion state is not defined within a valid
 * (non- null) reference frame. It is up to the caller to destroy the reference frame object by making a call
 * to ReferenceFrame::delete()
 */
std::shared_ptr<ReferenceFrame> MotionState::toSharedReferenceFrame(const std::string &name) const
{
    return std::shared_ptr<ReferenceFrame>(MotionState::toReferenceFrame(name));
}

/**
 * Spatially (and optionally temporally) transform a clone of the current object's motion state from its
 * current frame of reference and/or coordinate system to the target frame of reference and/or coordinate
 * system (by default, the transformation is spatial only); upon success, function returns a non-null pointer
 * to a dynamically-allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param coordType the coordinate system in which the motion state will be described
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
MotionState *MotionState::transformClone(ReferenceFrame *pFrame,
                                         const CoordinateType &coordType,
                                         bool bTemporal) const
{
    auto *pMotionState = transformCloneToFrame(pFrame, bTemporal);
    if (pMotionState != nullptr && pMotionState->getCoordinateType() != coordType)
    {
        auto *pTempMotionState = pMotionState;
        pMotionState = pMotionState->transformCloneToCoordinateSystem(coordType);
        if (pTempMotionState != nullptr)
            delete pTempMotionState;
    }

    return pMotionState;
}

/**
 * Spatially and temporally transform a clone of the current object's motion state from its current frame of
 * reference and/or coordinate system to the target frame of reference and/or coordinate system. Since this
 * transformation is temporal, the motion state and all frames through which it is transformed will be
 * projected to the specified time; upon success, function returns a non-null pointer to a dynamically
 * allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param coordType the coordinate system in which the motion state will be described
 * @param t         the time to which the frames and motion states will be projected
 */
MotionState *MotionState::transformClone(ReferenceFrame *pFrame,
                                         const CoordinateType &coordType,
                                         double t) const
{
    auto *pMotionState = transformCloneToFrame(pFrame, t);
    if (pMotionState != nullptr && pMotionState->getCoordinateType() != coordType)
    {
        auto *pTempMotionState = pMotionState;
        pMotionState = pMotionState->transformCloneToCoordinateSystem(coordType);
        if (pTempMotionState != nullptr)
            delete pTempMotionState;
    }

    return pMotionState;
}

/**
 * Transform the frame state of a clone of the current object's motion state to that which is specified; the
 * function works by first transforming this object's state to the root reference frame and then it transforms
 * the state down the chain using the target reference frame state
 * @param state the perturbation state of this object's reference frame
 */
MotionState *MotionState::transformCloneFrameState(const std::string &state)
{
    auto *pMotionState = clone();
    if (pMotionState != nullptr)
        pMotionState->transformFrameState(state);

    return pMotionState;
}

/**
 * Transform a clone of the current object's motion state from its current coordinate system into the desired
 * coordinate system; upon success, function returns a non-null pointer to a dynamically allocated motion state
 * @param coordType the coordinate system in which the motion state will be described
 */
MotionState *MotionState::transformCloneToCoordinateSystem(const CoordinateType &coordType) const
{
    MotionState *pMotionState = nullptr;
    if (m_pFrame != nullptr)
    {
        switch (coordType)
        {
            case CoordinateType::Cartesian:
            pMotionState = MotionState::create(m_pFrame, coordType, m_frameState);
            calcCartesianState(dynamic_cast<CartesianMotionState *>(pMotionState));
            break;

            case CoordinateType::Spherical:
            pMotionState = MotionState::create(m_pFrame, coordType, m_frameState);
            calcSphericalState(dynamic_cast<SphericalMotionState *>(pMotionState));
            break;

            default:
            StaticSynchronizable<MotionState>::lock(0);
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Reference frame was not assigned to the current MotionState.\n",
                   getQualifiedMethodName(__func__));
            StaticSynchronizable<MotionState>::unlock(0);
        }
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Reference frame was not assigned to the current MotionState.\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return pMotionState;
}

/**
 * Spatially (and optionally temporally) transform a clone of the current object's motion state from its
 * current frame of reference to the target frame of reference (by default, the transformation is spatial
 * only); upon success, function returns a non-null pointer to a dynamically-allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
MotionState *MotionState::transformCloneToFrame(ReferenceFrame *pFrame,
                                                bool bTemporal) const
{
    MotionState *pMotionState = nullptr;
    if (pFrame == nullptr)
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input reference frame object is null, transformation failed!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }
    else
    {
        pMotionState = clone();
        if (pMotionState != nullptr)
            pMotionState->transformToFrame(pFrame, bTemporal);
    }

    return pMotionState;
}

/**
 * Spatially (and optionally temporally) transform a clone of the current object's motion and frame states from
 * the current frame of reference and state to the target frame of reference and state (by default, the
 * transformation is spatial only); upon success, function returns a non-null pointer to a
 * dynamically-allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param state     the perturbation state of this object's reference frame
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
template<typename T,
         typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                 std::is_same<T, const char *>::value, int>::type>
MotionState *MotionState::transformCloneToFrame(ReferenceFrame *pFrame,
                                                T state,
                                                bool bTemporal) const
{
    auto *pMotionState = clone();
    if (pMotionState != nullptr)
        pMotionState->transformToFrame(pFrame, state, bTemporal);

    return pMotionState;
}

/**
 * Spatially and temporally transform a clone of the current object's motion state from its current frame of
 * reference to the target frame of reference; upon success, function returns a non-null pointer to a
 * dynamically-allocated motion state
 * @param pFrame a pointer to the target reference frame
 * @param t      the time to which the frames and motion states will be projected
 */
MotionState *MotionState::transformCloneToFrame(ReferenceFrame *pFrame,
                                                double t) const
{
    MotionState *pMotionState = nullptr;
    if (pFrame == nullptr)
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input reference frame object is null, transformation failed!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }
    else
    {
        pMotionState = clone();
        if (pMotionState != nullptr)
            pMotionState->transformToFrame(pFrame, t);
    }

    return pMotionState;
}

/**
 * Spatially and temporally transform a clone of the current object's motion state and frame from the current
 * frame of reference and state to the target frame of reference and state; upon success, function returns a
 * non-null pointer to a dynamically-allocated motion state
 * @param pFrame a pointer to the target reference frame
 * @param state  the perturbation state of this object's reference frame
 * @param t      the time to which the frames and motion states will be projected
 */
template<typename T,
         typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                 std::is_same<T, const char *>::value, int>::type>
MotionState *MotionState::transformCloneToFrame(ReferenceFrame *pFrame,
                                                T state,
                                                double t) const
{
    auto *pMotionState = clone();
    if (pMotionState != nullptr)
        pMotionState->transformToFrame(pFrame, state, t);

    return pMotionState;
}

/**
 * Transform this motion state's frame state to that which is specified; the function works by first
 * transforming this object's state to the root reference frame, and then it transforms the state down the
 * chain using the target reference frame state
 * @param state the perturbation state of this object's reference frame
 */
bool MotionState::transformFrameState(const std::string &state)
{
    return transformToFrame(m_pFrame, state, false);
}

/**
 * Spatially (and optionally temporally) transform a shared clone of the current object's motion state from its
 * current frame of reference and/or coordinate system to the target frame of reference and/or coordinate
 * system (by default, the transformation is spatial only); upon success, function returns a non-null pointer
 * to a dynamically-allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param coordType the coordinate system in which the motion state will be described
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
std::shared_ptr<MotionState> MotionState::transformSharedClone(ReferenceFrame *pFrame,
                                                               const CoordinateType &coordType,
                                                               bool bTemporal) const
{
    return std::shared_ptr<MotionState>(MotionState::transformClone(pFrame, coordType, bTemporal));
}

/**
 * Spatially and temporally transform a shared clone of the current object's motion state from its current
 * frame of reference and/or coordinate system to the target frame of reference and/or coordinate system. Since
 * this transformation is temporal, the motion state and all frames through which it is transformed will be
 * projected to the specified time; upon success, function returns a non-null pointer to a dynamically
 * allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param coordType the coordinate system in which the motion state will be described
 * @param t         the time to which the frames and motion states will be projected
 */
std::shared_ptr<MotionState> MotionState::transformSharedClone(ReferenceFrame *pFrame,
                                                               const CoordinateType &coordType,
                                                               double t) const
{
    return std::shared_ptr<MotionState>(MotionState::transformClone(pFrame, coordType, t));
}

/**
 * Transform the frame state of a shared clone of the current object's motion state to that which is specified;
 * the function works by first transforming this object's state to the root reference frame, and then it
 * transforms the state down the chain using the target reference frame state
 * @param state the perturbation state of this object's reference frame
 */
std::shared_ptr<MotionState>
MotionState::transformSharedCloneFrameState(const std::string &state)
{
    return std::shared_ptr<MotionState>(MotionState::transformCloneFrameState(state));
}

/**
 * Transform a shared clone of the current object's motion state from its current coordinate system into the
 * desired coordinate system; upon success, function returns a non-null pointer to a dynamically allocated
 * motion state
 * @param coordType the coordinate system in which the motion state will be described
 */
std::shared_ptr<MotionState>
MotionState::transformSharedCloneToCoordinateSystem(const CoordinateType &coordType) const
{
    return std::shared_ptr<MotionState>(MotionState::transformCloneToCoordinateSystem(coordType));
}

/**
 * Spatially (and optionally temporally) transform a shared clone of the current object's motion state from its
 * current frame of reference to the target frame of reference (by default, the transformation is spatial
 * only); upon success, function returns a non-null pointer to a dynamically-allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
std::shared_ptr<MotionState> MotionState::transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                                                      bool bTemporal) const
{
    return std::shared_ptr<MotionState>(MotionState::transformCloneToFrame(pFrame, bTemporal));
}

/**
 * Spatially (and optionally temporally) transform a shared clone of the current object's motion and frame
 * states from the current frame of reference and state to the target frame of reference and state (by default,
 * the transformation is spatial only); upon success, function returns a non-null pointer to a
 * dynamically-allocated motion state
 * @param pFrame    a pointer to the target reference frame
 * @param state     the perturbation state of this object's reference frame
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
template<typename T,
         typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                 std::is_same<T, const char *>::value, int>::type>
std::shared_ptr<MotionState>
MotionState::transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                         T state,
                                         bool bTemporal) const
{
    return std::shared_ptr<MotionState>(MotionState::transformCloneToFrame(pFrame, state, bTemporal));
}

/**
 * Spatially and temporally transform a shared clone of the current object's motion state from its current
 * frame of reference to the target frame of reference; upon success, function returns a non-null pointer to a
 * dynamically-allocated motion state
 * @param pFrame a pointer to the target reference frame
 * @param t      the time to which the frames and motion states will be projected
 */
std::shared_ptr<MotionState> MotionState::transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                                                      double t) const
{
    return std::shared_ptr<MotionState>(MotionState::transformCloneToFrame(pFrame, t));
}

/**
 * Spatially and temporally transform a shared clone of the current object's motion and frame states from the
 * current frame of reference and state to the target frame of reference and state; upon success, function
 * returns a non-null pointer to a dynamically-allocated motion state
 * @param pFrame a pointer to the target reference frame
 * @param state  the perturbation state of this object's reference frame
 * @param t      the time to which the frames and motion states will be projected
 */
template<typename T,
         typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                 std::is_same<T, const char *>::value, int>::type>
std::shared_ptr<MotionState>
MotionState::transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                         T state,
                                         double t) const
{
    return std::shared_ptr<MotionState>(MotionState::transformCloneToFrame(pFrame, state, t));
}

/**
 * Spatially (and optionally temporally) transform this object's motion state defined in its current frame of
 * reference to the input frame of reference (by default, the transformation is spatial only)
 * @param pFrame    a pointer to the target reference frame
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
bool MotionState::transformToFrame(ReferenceFrame *pFrame,
                                   bool bTemporal)
{
    bool bSuccess = (pFrame != nullptr && m_pFrame != nullptr);
    if (bSuccess)
    {
        auto pFrameSearchFunc = bTemporal ? &ReferenceFrame::findCommonAncestorFrame
                                          : &ReferenceFrame::findLeastCommonRootFrame;

        auto *pCommonFrame = pFrame;
        auto *pThisCommonFrame = (*m_pFrame.*pFrameSearchFunc)(pCommonFrame);
        bSuccess = (pCommonFrame != nullptr && pThisCommonFrame != nullptr);
        if (bSuccess)
        {
            // first transform to common root frame
            if (bTemporal)
            {
                auto t = pFrame->getTime();
                bSuccess = pThisCommonFrame->transformToFrame(this, t, m_frameState);
            }
            else
                bSuccess = pThisCommonFrame->transformToFrame(this, m_frameState);

            // now transform to target frame
            setFrame(pCommonFrame);
            if (bSuccess)
                bSuccess = pFrame->transformToFrame(this, bTemporal, m_frameState);
        }
        else
        {
            StaticSynchronizable<MotionState>::lock(0);
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "A common frame of reference could not be found.\n",
                   getQualifiedMethodName(__func__));
            StaticSynchronizable<MotionState>::unlock(0);
        }
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input and/or current object reference frame is null, transformation failed!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return bSuccess;
}

/**
 * Spatially (and optionally temporally) transform this object's motion and frame states defined in the current
 * frame of reference and state to the input frame of reference and state (by default, the transformation is
 * spatial only)
 * @param pFrame    a pointer to the target reference frame
 * @param state     the perturbation state of this object's reference frame
 * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
 *                  spatial-only transformation will occur (time at which frames and motion state are defined
 *                  is not considered). If true, the motion state and all frames through which it is
 *                  transformed will be projected to the time at which the target frame is defined.
 */
template<typename T,
         typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                 std::is_same<T, const char *>::value, int>::type>
bool MotionState::transformToFrame(ReferenceFrame *pFrame,
                                   T state,
                                   bool bTemporal)
{
    bool bSuccess = true; // assume success
    if (m_frameState != state)
    {
        bSuccess = (m_pFrame != nullptr);
        if (bSuccess)
        {
            auto *pRootFrame = m_pFrame->getRoot();
            bSuccess = (pRootFrame != nullptr);
            if (bSuccess)
            {
                // first transform to the root reference frame
                bSuccess = transformToFrame(pRootFrame, bTemporal);
                if (bSuccess)
                    setFrameState(state);
            }
        }
    }

    if (bSuccess)
    {
        // now transform to the target reference frame in the specified state
        bSuccess = transformToFrame(pFrame, bTemporal);
    }

    return bSuccess;
}

/**
 * Spatially and temporally transform this object's motion state defined in its current frame of reference to
 * the input frame of reference. Since this transformation is temporal, the motion state and all frames through
 * which it is transformed will be projected to the specified time
 * @param pFrame a pointer to the target reference frame
 * @param t      the time to which the frames and motion states will be projected
 */
bool MotionState::transformToFrame(ReferenceFrame *pFrame,
                                   double t)
{
    bool bSuccess = (pFrame != nullptr && m_pFrame != nullptr);
    if (bSuccess)
    {
        auto *pCommonAncestor = pFrame;
        auto *pThisCommonAncestor = m_pFrame->findCommonAncestorFrame(pCommonAncestor);
        bSuccess = (pCommonAncestor != nullptr && pThisCommonAncestor != nullptr);
        if (bSuccess)
        {
            // first transform to common root frame
            bSuccess = pThisCommonAncestor->transformToFrame(this, t, m_frameState);

            // now transform to target frame
            setFrame(pCommonAncestor);
            if (bSuccess)
                bSuccess = pFrame->transformToFrame(this, t, m_frameState);
        }
        else
        {
            StaticSynchronizable<MotionState>::lock(0);
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "A common frame of reference could not be found.\n",
                   getQualifiedMethodName(__func__));
            StaticSynchronizable<MotionState>::unlock(0);
        }
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Input and/or current object reference frame is null, transformation failed!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return bSuccess;
}

/**
 * Spatially and temporally transform this object's motion and frame states defined in the current frame of
 * reference and state to the input frame of reference and state. Since this transformation is temporal, the
 * motion state and all frames through which it is transformed will be projected to the specified time
 * @param pFrame a pointer to the target reference frame
 * @param state  the perturbation state of this object's reference frame
 * @param t      the time to which the frames and motion states will be projected
 */
template<typename T,
         typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                 std::is_same<T, const char *>::value, int>::type>
bool MotionState::transformToFrame(ReferenceFrame *pFrame,
                                   T state,
                                   double t)
{
    bool bSuccess = true; // assume success
    if (m_frameState != state)
    {
        bSuccess = (m_pFrame != nullptr);
        if (bSuccess)
        {
            auto *pRootFrame = m_pFrame->getRoot();
            bSuccess = (pRootFrame != nullptr);
            if (bSuccess)
            {
                // first transform to the root reference frame
                bSuccess = transformToFrame(pRootFrame);
                if (bSuccess)
                    setFrameState(state);
            }
        }
    }

    if (bSuccess)
    {
        // now transform to the target reference frame in the specified state
        bSuccess = transformToFrame(pFrame);
    }

    return bSuccess;
}

/**
 * Update the motion state with the specified time. This function will update all quantities within the motion
 * state that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void MotionState::update(double t,
                         const TimeReferenceType &timeReference)
{
    if (timeReference == TimeReferenceType::Delta)
        t += m_pState->getTime();

    // order matters!
    if (t != m_pState->getTime())
    {
        m_pState->setPosition(getPosition(t));
        m_pState->setVelocity(getVelocity(t));
        m_pState->setEulers(getEulers(t));
        m_pState->setEulerRates(getEulerRates(t));
        m_pState->setTime(t);
    }
}

/**
 * Update the motion state Euler rates only using the specified time
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void MotionState::updateEulerRates(double t,
                                   const TimeReferenceType &timeReference)
{
    if (timeReference == TimeReferenceType::Delta)
        t += m_pState->getTime();

    if (t != m_pState->getTime())
        m_pState->setEulerRates(getEulerRates(t));
}

/**
 * Update the motion state Euler angles only using the specified time
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void MotionState::updateEulers(double t,
                               const TimeReferenceType &timeReference)
{
    if (timeReference == TimeReferenceType::Delta)
        t += m_pState->getTime();

    if (t != m_pState->getTime())
        m_pState->setEulers(getEulers(t));
}

/**
 * Update the motion state position only using the specified time
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void MotionState::updatePosition(double t,
                                 const TimeReferenceType &timeReference)
{
    if (timeReference == TimeReferenceType::Delta)
        t += m_pState->getTime();

    if (t != m_pState->getTime())
        m_pState->setPosition(getPosition(t));
}

/**
 * Update the motion state time using the specified delta time
 * @param dt the time update interval (s)
 */
void MotionState::updateTime(double dt)
{
    if (dt != 0.0)
        m_pState->setTime(m_pState->getTime() + dt);
}

/**
 * Update the motion state velocity only using the specified time
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void MotionState::updateVelocity(double t,
                                 const TimeReferenceType &timeReference)
{
    if (timeReference == TimeReferenceType::Delta)
        t += m_pState->getTime();

    if (t != m_pState->getTime())
        m_pState->setVelocity(getVelocity(t));
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool MotionState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && m_pFrame != nullptr && std::strcmp(pNode->name(), "motionState") == 0);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            std::ostringstream stream;
            stream << std::boolalpha << m_bCacheTransformsEnabled;
            auto *pCacheTransformsEnabledNode = pDocument->allocate_node(node_element, "cacheTransforms");
            auto *pCacheTransformsEnabledString = pDocument->allocate_string(stream.str().c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pCacheTransformsEnabledString);
            pCacheTransformsEnabledNode->append_node(pDataNode);
            pNode->append_node(pCacheTransformsEnabledNode);
        }

        if (bSuccess)
        {
            std::ostringstream stream;
            stream << std::boolalpha << m_bDebugTransformsEnabled;
            auto *pDebugTransformsEnabledNode = pDocument->allocate_node(node_element, "debugTransforms");
            auto *pDebugTransformsEnabledString = pDocument->allocate_string(stream.str().c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pDebugTransformsEnabledString);
            pDebugTransformsEnabledNode->append_node(pDataNode);
            pNode->append_node(pDebugTransformsEnabledNode);
        }

        if (bSuccess)
        {
            auto *pCoordinateSystemNode = pDocument->allocate_node(node_element, "coordinateSystem");
            auto *pCoordinateSystemString = pDocument->allocate_string(std::string(getCoordinateType()).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pCoordinateSystemString);
            pCoordinateSystemNode->append_node(pDataNode);
            pNode->append_node(pCoordinateSystemNode);
        }

        if (bSuccess)
        {
            auto *pFrameStateNode = pDocument->allocate_node(node_element, "frameState");
            auto *pDataNode = pDocument->allocate_node(node_data, m_frameState.c_str());
            pFrameStateNode->append_node(pDataNode);
            pNode->append_node(pFrameStateNode);
        }

        if (bSuccess)
        {
            auto &&type = SphericalMotionState::getSphericalConversionType(m_pFromAzimuthZenithConverter);
            auto *pFromAzimuthZenithConverterNode =
                  pDocument->allocate_node(node_element, "fromAzimuthZenithConversion");
            auto *pFromAzimuthZenithConverterString = pDocument->allocate_string(std::string(type).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pFromAzimuthZenithConverterString);
            pFromAzimuthZenithConverterNode->append_node(pDataNode);
            pNode->append_node(pFromAzimuthZenithConverterNode);
        }

        if (bSuccess)
        {
            auto *pKinematicStateNode = pDocument->allocate_node(node_element, "kinematicState");
            bSuccess = m_pState->writeToXML(pKinematicStateNode);
            pNode->append_node(pKinematicStateNode);
        }

        if (bSuccess)
        {
            auto *pReferenceFrameNode = pDocument->allocate_node(node_element, "referenceFrame");
            auto *pDataNode = pDocument->allocate_node(node_data, m_pFrame->getName().c_str());
            pReferenceFrameNode->append_node(pDataNode);
            pNode->append_node(pReferenceFrameNode);
        }

        if (bSuccess)
        {
            auto &&type = SphericalMotionState::getSphericalConversionType(m_pToAzimuthZenithConverter);
            auto *pToAzimuthZenithConverterNode =
                  pDocument->allocate_node(node_element, "toAzimuthZenithConversion");
            auto *pToAzimuthZenithConverterString = pDocument->allocate_string(std::string(type).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pToAzimuthZenithConverterString);
            pToAzimuthZenithConverterNode->append_node(pDataNode);
            pNode->append_node(pToAzimuthZenithConverterNode);
        }
    }

    return bSuccess;
}
#endif
}

}
