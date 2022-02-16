#ifndef MOTION_STATE_H
#define MOTION_STATE_H

#include "cloneable.h"
#include "coordinate_type.h"
#include "export_library.h"
#include "initializable.h"
#include "loggable.h"
#include "reflective.h"
#include "serializable.h"
#include "spherical_conversion_type.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "time_reference_type.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#include "xml_writable.h"
#endif
#include <memory>

// if a default frame state is not specified in preprocessor configuration, then define the following
#ifndef DEFAULT_FRAME_STATE
#define DEFAULT_FRAME_STATE "default"
#endif

// forward declarations
namespace containers { template<typename EntryType,
                                typename IdType,
                                typename TimeType,
                                IdType (EntryType::*)(void) const,
                                TimeType (EntryType::*)(void) const,
                                EntryType * (EntryType::*)(void) const,
                                void (*)(EntryType *&)>
                                class IdentifierAndTimeSortedContainer; }
namespace math { namespace geometric { namespace orientation { struct EulerAxisType;
                                                               class Eulers; } }
                 namespace linear_algebra { namespace vector { class Vector3d; } }
                 namespace trigonometric { struct AngleUnitType; } }

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace physics
{

namespace kinematics
{

// forward declarations
class CartesianMotionState;
struct EulerAccelerationAxisType;
struct EulerRateAxisType;
struct EulerStateDerivativeType;
class FrameState;
class KinematicState;
class ReferenceFrame;
class SphericalMotionState;
struct StateDerivativeType;

/**
 * Typedef declarations
 */
typedef void (*tMotionStateAngleConversionFunction)(SphericalMotionState *);

/**
 * This class stores and manipulates the motion state of a spatially-orientable physical object
 */
class MotionState
: public attributes::interfaces::Cloneable<MotionState>,
  public attributes::interfaces::Initializable,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  public attributes::concrete::OutputStreamable<MotionState>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
  public attributes::concrete::StaticMutexMappable<MotionState, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<MotionState>,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<MotionState>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<MotionState>
#endif
{
public:

    /**
     * Friend declarations
     */
    template<typename EntryType,
             typename IdType,
             typename TimeType,
             IdType (EntryType::*)(void) const,
             TimeType (EntryType::*)(void) const,
             EntryType * (EntryType::*)(void) const,
             void (*)(EntryType *&)>
    friend class containers::IdentifierAndTimeSortedContainer;
    friend class ReferenceFrame;

    /**
     * Type alias declarations
     */
    using AngleUnitType = math::trigonometric::AngleUnitType;
    using EulerAxisType = math::geometric::orientation::EulerAxisType;
    using Eulers = math::geometric::orientation::Eulers;
    using Vector3d = math::linear_algebra::vector::Vector3d;

    /**
     * Typedef declarations
     */
    typedef std::map<std::string, std::pair<std::unique_ptr<FrameState>,
                                            std::unique_ptr<KinematicState>>> tTransformationCache;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

protected:

    /**
     * Construct an empty motion state object
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct an empty motion state object at the specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    explicit EXPORT_STEM MotionState(double t,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position at the specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(double t,
                            const Vector3d &position,
                            const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position and initial velocity at the specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(double t,
                            const Vector3d &position,
                            const Vector3d &velocity,
                            const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, constant acceleration at the
     * specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(double t,
                            const Vector3d &position,
                            const Vector3d &velocity,
                            const Vector3d &acceleration,
                            const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, constant acceleration and Euler
     * orientation at the specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(double t,
                            const Vector3d &position,
                            const Vector3d &velocity,
                            const Vector3d &acceleration,
                            const Eulers &eulers,
                            const AngleUnitType &angleUnits,
                            const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, constant acceleration, Euler
     * orientation and Euler rates at the specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(double t,
                            const Vector3d &position,
                            const Vector3d &velocity,
                            const Vector3d &acceleration,
                            const Eulers &eulers,
                            const Eulers &eulerRates,
                            const AngleUnitType &angleUnits,
                            const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, constant acceleration, Euler
     * orientation, Euler rates, and Euler accelerations at the specified time
     * @param t     the time at which the motion state is initially defined
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM MotionState(double t,
                            const Vector3d &position,
                            const Vector3d &velocity,
                            const Vector3d &acceleration,
                            const Eulers &eulers,
                            const Eulers &eulerRates,
                            const Eulers &eulerAccelerations,
                            const AngleUnitType &angleUnits,
                            const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Copy constructor
     */
    EXPORT_STEM MotionState(const MotionState &motionState);

    /**
     * Move constructor
     */
    EXPORT_STEM MotionState(MotionState &&motionState);

public:

    /**
     * Construct an empty motion state object
     */
    EXPORT_STEM virtual ~MotionState(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM virtual MotionState &operator = (const MotionState &motionState);

    /**
     * Move assignment operator
     */
    EXPORT_STEM MotionState &operator = (MotionState &&motionState);

public:

    /**
     * Equality operator overload
     */
    EXPORT_STEM virtual bool operator == (const MotionState &motionState);

    /**
     * Subscript operator
     * @param type an enumeration which takes on one of the following values:
     *             = EulerAccelerationAxisType
     *             = EulerAxisType
     *             = EulerRateAxisType
     *             = EulerStateType
     *             = StateDerivativeType
     */
    template<typename Enum>
    inline auto &operator [] (Enum &&type)
    {
        return get(std::forward<Enum>(type));
    }

    /**
     * Subscript operator
     * @param type an enumeration which takes on one of the following values:
     *             = EulerAccelerationAxisType
     *             = EulerAxisType
     *             = EulerRateAxisType
     *             = EulerStateType
     *             = StateDerivativeType
     */
    template<typename Enum>
    inline auto operator [] (Enum &&type) const
    {
        return get(std::forward<Enum>(type));
    }

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const MotionState &motionState);

    /**
     * Assign the contents of one motion state to another
     * @param pMotionState a pointer to the source motion state
     */
    EXPORT_STEM virtual bool assign(const MotionState *pMotionState) = 0;

protected:

    /**
     * Function to cache the current object's motion state in the transformation cache
     */
    EXPORT_STEM virtual void cacheCurrentState(void) final;

public:

    /**
     * Get motion state transformation caching enabled/disabled
     */
    EXPORT_STEM virtual bool cacheTransformationsEnabled(void) const final;

    /**
     * Set motion state transformation caching enabled/disabled
     */
    EXPORT_STEM virtual void cacheTransformationsEnabled(bool bEnabled) final;

    /**
     * Calculate the scalar acceleration
     */
    EXPORT_STEM virtual double calcAcceleration(void) const = 0;

    /**
     * Calculate this object's instantaneous angular acceleration vector with respect to its body axis. The
     * resultant angular acceleration vector is described in the current object's frame of reference
     */
    EXPORT_STEM virtual Vector3d calcAngularAcceleration(void) const final;

    /**
     * Calculate this object's instantaneous angular velocity vector with respect to its body axis. The
     * resultant angular velocity vector is described in the current object's frame of reference
     */
    EXPORT_STEM virtual Vector3d calcAngularVelocity(void) const final;

    /**
     * Calculate the time at which the current object's motion state approaches the null motion state at the
     * desired range. The time returned represents the closest time, whether it be in the future or past, at
     * which the motion state approaches the given range. If the motion state does not approach the specified
     * range, the subroutine returns infinity
     */
    EXPORT_STEM virtual double calcApproachTime(double range) const = 0;

    /**
     * Calculate the time at which the current object's motion state approaches another motion state at the
     * desired range. The time returned represents the closest time, whether it be in the future or past, at
     * which the motion states approach one another at the given range. If the motion states do not approach
     * one another within the specified range, the subroutine returns infinity
     */
    EXPORT_STEM virtual double calcApproachTime(double range,
                                                const MotionState &motionState) const;

    /**
     * Calculate the Cartesian motion state from the current object's spherical motion state at the initial
     * time the state was defined
     * @param pCartesianMotionState the object in which the resultant Cartesian state will be stored
     */
    EXPORT_STEM virtual bool calcCartesianState(CartesianMotionState *pCartesianMotionState) const final;

    /**
     * Calculate the Cartesian motion state from the current object's spherical motion state at the specified
     * time
     * @param pCartesianMotionState the object in which the resultant Cartesian state will be stored
     * @param t                     the time to which the resultant motion state will be projected
     */
    EXPORT_STEM virtual bool calcCartesianState(CartesianMotionState *pCartesianMotionState,
                                                double t) const;

    /**
     * Calculate the distance from the trajectory defined by this object's motion state to the intersection on
     * a plane normal to the specified direction vector (not necessarily aligned with its velocity vector). The
     * time at which this plane intersection is projected to occur is returned via the first argument. The sign
     * of the distance returned will indicate whether the current object's motion state is moving toward the
     * plane (positive) or moving away from the plane (negative)
     * @param tPenetration the time at which the current object's motion state penetrates the plane normal to
     *                     the input direction vector
     * @param direction    the direction vector along which the motion state will be projected
     */
    EXPORT_STEM virtual double calcDistanceToPlaneIntersection(double &tPenetration,
                                                               const Vector3d &direction) const final;

    /**
     * Calculate the distance from the trajectory defined by this object's motion state to the intersection on
     * a plane normal to the instantaneous velocity vector of another motion state. The time at which this
     * plane intersection is projected to occur is returned via the first argument. The sign of the distance
     * returned will indicate whether the motion states are moving away from one another (negative) or moving
     * toward one another (positive). In the event that the input motion state's velocity vector magnitude is
     * zero, the orientation yaw, pitch Euler angles will be used to determine the direction along which the
     * current object's motion state will be projected
     * @param tPenetration the time at which the current object's motion state penetrates the plane normal to
     *                     the input motion state's direction of travel
     * @param motionState  a reference to a motion state object
     */
    EXPORT_STEM virtual double calcDistanceToPlaneIntersection(double &tPenetration,
                                                               const MotionState &motionState) const final;

    /**
     * Calculate the line of sight distance and approach time with respect to the null motion state
     * @param tApproach the calculated time at which the line of sight distance is projected to be achieved
     */
    EXPORT_STEM virtual double calcLineOfSightDistance(double &tApproach) const final;

    /**
     * Calculate the minimum approach distance achieved with respect to another motion state
     * @param tApproach the calculated time at which the minimum distance is achieved
     */
    EXPORT_STEM virtual double calcMinimumApproach(double &tApproach) const = 0;

    /**
     * Calculate the minimum approach distance achieved with respect to another motion state
     * @param tApproach   the calculated time at which the minimum distance is achieved
     * @param motionState a reference to a motion state object
     */
    EXPORT_STEM virtual double calcMinimumApproach(double &tApproach,
                                                   const MotionState &motionState) const;

    /**
     * Calculate the range
     */
    EXPORT_STEM virtual double calcRange(void) const final;

    /**
     * Calculate the range at time t
     */
    EXPORT_STEM virtual double calcRange(double t) const = 0;

    /**
     * Calculate the range between two motion states
     */
    EXPORT_STEM virtual double calcRange(const MotionState &motionState) const final;

    /**
     * Calculate the range to another motion state at time t
     */
    EXPORT_STEM virtual double calcRange(double t,
                                         const MotionState &motionState) const = 0;

    /**
     * Calculate the range acceleration at time t
     */
    EXPORT_STEM virtual double calcRangeAcceleration(double t) const = 0;

    /**
     * Calculate the range acceleration between two motion states
     */
    EXPORT_STEM virtual double calcRangeAcceleration(const MotionState &motionState) const final;

    /**
     * Calculate the range acceleration with respect to another motion state at time t
     */
    EXPORT_STEM virtual double calcRangeAcceleration(double t,
                                                     const MotionState &motionState) const = 0;

    /**
     * Calculate the range rate at time t
     */
    EXPORT_STEM virtual double calcRangeRate(double t) const = 0;

    /**
     * Calculate the range rate between two motion states
     */
    EXPORT_STEM virtual double calcRangeRate(const MotionState &motionState) const final;

    /**
     * Calculate the range rate with respect to another motion state at time t
     */
    EXPORT_STEM virtual double calcRangeRate(double t,
                                             const MotionState &motionState) const = 0;

    /**
     * Calculate the scalar speed
     */
    EXPORT_STEM virtual double calcSpeed(void) const final;

    /**
     * Calculate the scalar speed at time t
     */
    EXPORT_STEM virtual double calcSpeed(double t) const = 0;

    /**
     * Calculate the spherical motion state with angles in degrees from the current object's Cartesian motion
     * state
     * @param pSphericalMotionState the object in which the resultant spherical state will be stored
     */
    EXPORT_STEM virtual bool calcSphericalState(SphericalMotionState *pSphericalMotionState) const final;

    /**
     * Calculate the spherical motion state with angles in degrees from the current object's Cartesian motion
     * state at the specified time
     * @param pSphericalMotionState the object in which the resultant spherical state will be stored
     * @param t                     the time to which the resultant motion state will be projected
     */
    EXPORT_STEM virtual bool calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                                double t) const final;

    /**
     * Calculate the spherical motion state from the current object's Cartesian motion state at the initial
     * time the state was defined
     * @param pSphericalMotionState the object in which the resultant spherical state will be stored
     * @param angleUnits            the desired angle output units, Degrees or Radians
     */
    EXPORT_STEM virtual bool calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                                const AngleUnitType &angleUnits) const final;

    /**
     * Calculate the spherical motion state from the current object's Cartesian motion state at the specified
     * time
     * @param pSphericalMotionState the object in which the resultant spherical state will be stored
     * @param angleUnits            the desired angle output units, Degrees or Radians
     * @param t                     the time to which the resultant motion state will be projected
     */
    EXPORT_STEM virtual bool calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                                const AngleUnitType &angleUnits,
                                                double t) const;

    /**
     * Clear the transformation cache
     */
    EXPORT_STEM virtual void clearTransformationCache(void) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual MotionState *clone(void) const override = 0;

    /**
     * clone() function
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState> cloneShared(void) const final;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits);

public:

    /**
     * Create a motion state within the given frame of reference
     * @param pFrame a pointer to the target reference frame
     * @param state  the perturbation state of this object's reference frame
     * @param type   the coordinate type
     */
    static EXPORT_STEM MotionState *create(ReferenceFrame *pFrame,
                                           const std::string &state = DEFAULT_FRAME_STATE,
                                           const CoordinateType &type = CoordinateType::Cartesian);

    /**
     * Create a motion state within the given frame of reference
     * @param pFrame a pointer to the target reference frame
     * @param type   the coordinate type
     * @param state  the perturbation state of this object's reference frame
     */
    static EXPORT_STEM MotionState *create(ReferenceFrame *pFrame,
                                           const CoordinateType &type,
                                           const std::string &state = DEFAULT_FRAME_STATE);
#ifdef RAPID_XML
    /**
     * Create a motion state within the given frame of reference using data from the specified XML element. If
     * the coordinate type is omitted in the XML input, a Cartesian coordinate type will be assumed; upon
     * success, function will return non-null
     */
    static EXPORT_STEM MotionState *createFromXML(rapidxml::xml_node<char> *pNode,
                                                  ReferenceFrame *pFrame);
#endif
    /**
     * Create a shared motion state within the given frame of reference
     * @param pFrame a pointer to the target reference frame
     * @param state  the perturbation state of this object's reference frame
     * @param type   the coordinate type
     */
    static EXPORT_STEM std::shared_ptr<MotionState> createShared(ReferenceFrame *pFrame,
                                                                 const std::string &state =
                                                                 DEFAULT_FRAME_STATE,
                                                                 const CoordinateType &type =
                                                                 CoordinateType::Cartesian);

    /**
     * Create a shared motion state within the given frame of reference
     * @param pFrame a pointer to the target reference frame
     * @param type   the coordinate type
     * @param state  the perturbation state of this object's reference frame
     */
    static EXPORT_STEM std::shared_ptr<MotionState> createShared(ReferenceFrame *pFrame,
                                                                 const CoordinateType &type,
                                                                 const std::string &state =
                                                                 DEFAULT_FRAME_STATE);
#ifdef RAPID_XML
    /**
     * Create a shared motion state within the given frame of reference using data from the specified XML
     * element. If the coordinate type is omitted in the XML input, a Cartesian coordinate type will be
     * assumed; upon success, function will return non-null
     */
    static EXPORT_STEM std::shared_ptr<MotionState> createSharedFromXML(rapidxml::xml_node<char> *pNode,
                                                                        ReferenceFrame *pFrame);
#endif
    /**
     * Query this object's flag to enable/disable motion state transformation debugging mode
     */
    EXPORT_STEM virtual bool debugTransformsEnabled(void) const final;

    /**
     * Set this object's flag to enable/disable motion state transformation debugging mode
     */
    EXPORT_STEM virtual void debugTransformsEnabled(bool bEnabled) final;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get position, velocity, or acceleration state vector
     */
    EXPORT_STEM virtual Vector3d &get(const StateDerivativeType &type) final;

    /**
     * Get position, velocity, or acceleration state vector (const overload)
     */
    EXPORT_STEM virtual Vector3d get(const StateDerivativeType &type) const final;

    /**
     * Get position, velocity, or acceleration state vector (const overload)
     */
    EXPORT_STEM virtual void get(const StateDerivativeType &type,
                                 double state[3]) const final;

    /**
     * Get roll, pitch, or yaw Euler acceleration
     */
    EXPORT_STEM virtual double &get(const EulerAccelerationAxisType &type) final;

    /**
     * Get roll, pitch, or yaw Euler acceleration (const overload)
     */
    EXPORT_STEM virtual double get(const EulerAccelerationAxisType &type) const final;

    /**
     * Get roll, pitch, or yaw Euler angle
     */
    EXPORT_STEM virtual double &get(const EulerAxisType &type) final;

    /**
     * Get roll, pitch, or yaw Euler angle (const overload)
     */
    EXPORT_STEM virtual double get(const EulerAxisType &type) const final;

    /**
     * Get roll, pitch, or yaw Euler rate
     */
    EXPORT_STEM virtual double &get(const EulerRateAxisType &type) final;

    /**
     * Get roll, pitch, or yaw Euler rate (const overload)
     */
    EXPORT_STEM virtual double get(const EulerRateAxisType &type) const final;

    /**
     * Get Euler, Euler rate, or Euler acceleration vector
     */
    EXPORT_STEM virtual Eulers &get(const EulerStateDerivativeType &type) final;

    /**
     * Get Euler, Euler rate, or Euler acceleration vector (const overload)
     */
    EXPORT_STEM virtual Eulers get(const EulerStateDerivativeType &type) const final;

    /**
     * Get Euler, Euler rate, or Euler acceleration vector
     */
    EXPORT_STEM virtual void get(const EulerStateDerivativeType &type,
                                 double state[3]) const final;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double &,
                                             double &,
                                             double &) const = 0;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double acceleration[3]) const = 0;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual Vector3d &getAcceleration(void) final;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual Vector3d getAcceleration(void) const final;

    /**
     * Get angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual AngleUnitType getAngleUnits(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get a pointer to this object's member conversion function that defines how to convert standard
     * definitions of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z)
     * to a user-defined horizontal, vertical angle convention
     */
    EXPORT_STEM virtual tMotionStateAngleConversionFunction
    getConversionFromAzimuthZenithFunction(void) const final;

    /**
     * Get a pointer to this object's member function that defines how to convert a user-defined convention of
     * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
     * (measured from +z to -z) angles
     */
    EXPORT_STEM virtual tMotionStateAngleConversionFunction
    getConversionToAzimuthZenithFunction(void) const final;

    /**
     * Get the coordinate system in which this motion state is described
     */
    EXPORT_STEM virtual CoordinateType getCoordinateType(void) const = 0;

    /**
     * Get a pointer to the global default conversion function that defines how to convert standard definitions
     * of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a
     * user-defined horizontal, vertical angle convention
     */
    static EXPORT_STEM tMotionStateAngleConversionFunction getDefaultConversionFromAzimuthZenithFunction(void);

    /**
     * Get a pointer to the global default function that defines how to convert a user-defined convention of
     * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
     * (measured from +z to -z) angles
     */
    static EXPORT_STEM tMotionStateAngleConversionFunction getDefaultConversionToAzimuthZenithFunction(void);

    /**
     * Get the global default motion state transformation debugging mode option
     */
    static EXPORT_STEM bool getDefaultTransformDebuggingOption(void);

    /**
     * Get the global default transform caching option
     */
    static EXPORT_STEM bool getDefaultTransformCachingOption(void);

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual void getEulerAccelerations(double &rollAccel,
                                                   double &pitchAccel,
                                                   double &yawAccel) const final;

    /**
     * Get the Euler accelerations (roll, pitch, yaw)
     */
    EXPORT_STEM virtual void getEulerAccelerations(double eulerAccelerations[3]) const final;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual Eulers &getEulerAccelerations(void) final;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual Eulers getEulerAccelerations(void) const final;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual void getEulerRates(double &rollRate,
                                           double &pitchRate,
                                           double &yawRate) const final;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual void getEulerRates(double &rollRate,
                                           double &pitchRate,
                                           double &yawRate,
                                           double t) const final;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual void getEulerRates(double eulerRates[3]) const final;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual void getEulerRates(double eulerRates[3],
                                           double t) const final;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual Eulers &getEulerRates(void) final;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual Eulers getEulerRates(void) const final;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual Eulers getEulerRates(double t) const final;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual void getEulers(double &roll,
                                       double &pitch,
                                       double &yaw) const final;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual void getEulers(double &roll,
                                       double &pitch,
                                       double &yaw,
                                       double t) const final;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual void getEulers(double eulers[3]) const final;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual void getEulers(double eulers[3],
                                       double t) const final;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual Eulers &getEulers(void) final;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual Eulers getEulers(void) const final;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual Eulers getEulers(double t) const final;

    /**
     * Get the frame of reference in which this motion state is defined
     */
    EXPORT_STEM virtual ReferenceFrame *getFrame(void) final;

    /**
     * Get the frame of reference in which this motion state is defined
     */
    EXPORT_STEM virtual ReferenceFrame *getFrame(void) const final;

    /**
     * Return a string representing the coordinate reference frame and coordinate system in which this motion
     * state is described
     */
    EXPORT_STEM virtual std::string getFrameAndCoordinateSystem(void) const final;

    /**
     * Get the perturbation state of this object's reference frame
     */
    EXPORT_STEM virtual std::string getFrameState(void) const final;

    /**
     * Get the full kinematic state of this object
     */
    EXPORT_STEM virtual KinematicState *getKinematicState(void) final;

    /**
     * Get the full kinematic state of this object
     */
    EXPORT_STEM virtual KinematicState *getKinematicState(void) const final;

    /**
     * Get this object's pitch angle
     */
    EXPORT_STEM virtual double getPitch(void) const final;

    /**
     * Get this object's pitch angle
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitch(double t) const final;

    /**
     * Get this object's pitch angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitch(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's pitch angle
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitch(double t,
                                        const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's pitch acceleration
     */
    EXPORT_STEM virtual double getPitchAcceleration(void) const final;

    /**
     * Get this object's pitch acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitchAcceleration(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's pitch rate
     */
    EXPORT_STEM virtual double getPitchRate(void) const final;

    /**
     * Get this object's pitch rate
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitchRate(double t) const final;

    /**
     * Get this object's pitch rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitchRate(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's pitch rate
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitchRate(double t,
                                            const AngleUnitType &angleUnits) const final;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual void getPosition(double &,
                                         double &,
                                         double &) const = 0;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double &,
                                         double &,
                                         double &,
                                         double t) const = 0;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual void getPosition(double position[3]) const final;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double position[3],
                                         double t) const = 0;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual Vector3d &getPosition(void) final;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual Vector3d getPosition(void) const final;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual Vector3d getPosition(double t) const final;

    /**
     * Get this object's roll angle
     */
    EXPORT_STEM virtual double getRoll(void) const final;

    /**
     * Get this object's roll angle
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRoll(double t) const final;

    /**
     * Get this object's roll angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRoll(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's roll angle
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRoll(double t,
                                       const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's roll acceleration
     */
    EXPORT_STEM virtual double getRollAcceleration(void) const final;

    /**
     * Get this object's roll acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRollAcceleration(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's roll rate
     */
    EXPORT_STEM virtual double getRollRate(void) const final;

    /**
     * Get this object's roll rate
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRollRate(double t) const final;

    /**
     * Get this object's roll rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRollRate(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's roll rate
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRollRate(double t,
                                           const AngleUnitType &angleUnits) const final;

    /**
     * Get the time
     */
    EXPORT_STEM virtual double getTime(void) const final;

protected:

    /**
     * Get a reference to the transformation cache
     */
    EXPORT_STEM virtual tTransformationCache &getTransformationCache(void) final;

public:

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual void getVelocity(double &,
                                         double &,
                                         double &) const = 0;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double &,
                                         double &,
                                         double &,
                                         double t) const = 0;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3]) const final;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3],
                                         double t) const = 0;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual Vector3d &getVelocity(void) final;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual Vector3d getVelocity(void) const final;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual Vector3d getVelocity(double t) const final;

    /**
     * Get this object's yaw angle
     */
    EXPORT_STEM virtual double getYaw(void) const final;

    /**
     * Get this object's yaw angle
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYaw(double t) const final;

    /**
     * Get this object's yaw angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYaw(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's yaw angle
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYaw(double t,
                                      const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's yaw acceleration
     */
    EXPORT_STEM virtual double getYawAcceleration(void) const final;

    /**
     * Get this object's yaw acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYawAcceleration(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's yaw rate
     */
    EXPORT_STEM virtual double getYawRate(void) const final;

    /**
     * Get this object's yaw rate
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYawRate(double t) const final;

    /**
     * Get this object's yaw rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYawRate(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's yaw rate
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYawRate(double t,
                                          const AngleUnitType &angleUnits) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Initialization function to reset time-derivatives to zero
     */
    EXPORT_STEM virtual void initializeTimeDerivatives(void) final;

    /**
     * Determines whether or not two motion states are defined in the same frame of reference belonging to the
     * same tree
     */
    EXPORT_STEM virtual bool inSameFrameAndTree(const MotionState &motionState) const;

    /**
     * Determines whether or not two motion states are defined in frames that belong to the same tree
     */
    EXPORT_STEM virtual bool inSameTree(const MotionState &motionState) const;

    /**
     * Queries whether or not this motion state is described in Cartesian coordinates
     */
    EXPORT_STEM virtual bool isCartesian(void) const;

    /**
     * Determines whether or not the current object is described in the specified frame of reference
     */
    EXPORT_STEM virtual bool isDescribedInFrame(ReferenceFrame *pFrame) const final;

    /**
     * Determines whether or not two motion states are equivalent within the specified tolerance; this function
     * also tests for whether or not the motion states are defined in frames having the same name, but the
     * associated frames could actually belong to different trees.
     */
    EXPORT_STEM virtual bool isEqual(const MotionState &motionState,
                                     double tol) const;

    /**
     * Query whether or not this motion state is translationally or rotationally moving using the specified
     * tolerance
     */
    EXPORT_STEM virtual bool isMoving(double tol = 1.0e-8) const final;

    /**
     * Query whether or not this motion state is rotating using the specified tolerance
     */
    EXPORT_STEM virtual bool isRotating(double tol = 1.0e-8) const final;

    /**
     * Queries whether or not this motion state is described in spherical coordinates
     */
    EXPORT_STEM virtual bool isSpherical(void) const;

    /**
     * Queries whether or not this motion state is stationary (not rotating and not moving) using the specified
     * tolerance
     */
    EXPORT_STEM virtual bool isStationary(double tol = 1.0e-8) const final;

    /**
     * Query whether or not this motion state is moving translationally using the specified tolerance
     */
    EXPORT_STEM virtual bool isTranslating(double tol = 1.0e-8) const final;

    /**
     * Function to print the contents of the motion state
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * Project this object's motion state in the direction specified by the input direction vector
     */
    EXPORT_STEM virtual void projectOnto(const Vector3d &direction) final;

    /**
     * Project this object's motion state into the direction of the input motion state's velocity vector. If
     * the input motion state has no velocity, the orientation yaw, pitch Euler angles will be used to define
     * the unit direction vector along which the current object's motion state will be projected.
     */
    EXPORT_STEM virtual void projectOnto(const MotionState &motionState) final;

    /**
     * Project this object's motion state at time t into the direction of the input motion state's velocity
     * vector. If the input motion state has no velocity, the orientation yaw, pitch Euler angles will be used
     * to define the unit direction vector along which the current object's motion state will be projected.
     * This is a temporal transformation; therefore all frames and motion states will be projected to time t
     */
    EXPORT_STEM virtual void projectOnto(const MotionState &motionState,
                                         double t) final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
#if 0
    /**
     * Function to register a coordinate system conversion function pair
     * @param sourceCoordinateSystem             the name of the source coordinate system
     * @param destCoordinateSystem               the name of the destination coordinate system
     * @param coordinateSystemConversionFunction a pointer to the coordinate conversion function which takes
     *                                           the motion state from the source coordinate system to the
     *                                           destination coordinate system
     */
    static bool registerCoordinateSystemConversionPair(const std::string &sourceCoordinateSystem,
                                                       const std::string &destCoordinateSystem,
                                                       tCoordinateSystemConversionFunction
                                                       coordinateSystemConversionFunction);

    /**
     * Function to register a name-motion state object factory pair
     * @param coordType the coordinate system associated with the factory
     * @param factory   the name of the factory used to instantiate the object
     */
    template<typename ... Args>
    inline static void registerMotionStateFactory(const CoordinateType &coordType,
                                                  tMotionStateFactory<Args ...> factory)
    {
        if (factory != nullptr)
        {
            auto &&factoryMap = getFactoryMap();
            factoryMap[coordType] = factory;
        }
    }
#endif
    /**
     * Scale Euler angle accelerations in roll, pitch, or yaw
     */
    EXPORT_STEM virtual void scale(const EulerAccelerationAxisType &type,
                                   double value) final;

    /**
     * Scale Euler angle rates in roll, pitch, or yaw
     */
    EXPORT_STEM virtual void scale(const EulerRateAxisType &type,
                                   double value) final;

    /**
     * Scale Euler angles in roll, pitch, or yaw
     */
    EXPORT_STEM virtual void scale(const EulerAxisType &type,
                                   double value) final;

    /**
     * Scale the acceleration state vector
     */
    EXPORT_STEM virtual void scaleAcceleration(double value) final;

    /**
     * Scale the Euler acceleration state vector
     */
    EXPORT_STEM virtual void scaleEulerAccelerations(double value) final;

    /**
     * Scale the Euler rate state vector
     */
    EXPORT_STEM virtual void scaleEulerRates(double value) final;

    /**
     * Scale the Euler state vector
     */
    EXPORT_STEM virtual void scaleEulers(double value) final;

    /**
     * Scale the position state vector
     */
    EXPORT_STEM virtual void scalePosition(double value) final;

    /**
     * Scale the velocity state vector
     */
    EXPORT_STEM virtual void scaleVelocity(double value) final;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set roll, pitch, or yaw Euler acceleration
     */
    EXPORT_STEM virtual void set(const EulerAccelerationAxisType &type,
                                 double eulerAcceleration) final;

    /**
     * Set roll, pitch, or yaw Euler rate
     */
    EXPORT_STEM virtual void set(const EulerRateAxisType &type,
                                 double eulerRate) final;

    /**
     * Set Euler angle, rate, or acceleration vector
     */
    EXPORT_STEM virtual void set(const EulerStateDerivativeType &type,
                                 const Eulers &state) final;

    /**
     * Set roll, pitch, or yaw Euler angle
     */
    EXPORT_STEM virtual void set(const EulerAxisType &type,
                                 double angle) final;

    /**
     * Set position, velocity, or acceleration state vector
     */
    EXPORT_STEM virtual void set(const StateDerivativeType &type,
                                 const Vector3d &state) final;

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(const Vector3d &acceleration) = 0;

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(double,
                                             double,
                                             double) = 0;

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) final;

    /**
     * Set a pointer to this object's member conversion function that defines how to convert standard
     * definitions of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z)
     * to a user-defined horizontal, vertical angle convention
     */
    EXPORT_STEM virtual void setConversionFromAzimuthZenithFunction(tMotionStateAngleConversionFunction
                                                                    pConverter) final;

    /**
     * Set a pointer to this object's member function that defines how to convert a user-defined convention of
     * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
     * (measured from +z to -z) angles
     */
    EXPORT_STEM virtual void setConversionToAzimuthZenithFunction(tMotionStateAngleConversionFunction
                                                                  pConverter) final;

    /**
     * Set a pointer to the global default conversion function that defines how to convert standard definitions
     * of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a
     * user-defined horizontal, vertical angle convention
     */
    static EXPORT_STEM void setDefaultConversionFromAzimuthZenithFunction(tMotionStateAngleConversionFunction
                                                                          pConverter);

    /**
     * Set a pointer to the global default function that defines how to convert a user-defined convention of
     * spherical horizontal, vertical angles to standard azimuth (measured counter-clockwise from +x), zenith
     * (measured from +z to -z) angles
     */
    static EXPORT_STEM void setDefaultConversionToAzimuthZenithFunction(tMotionStateAngleConversionFunction
                                                                        pConverter);

    /**
     * Set the global default motion state transformation debugging mode option
     */
    static EXPORT_STEM void setDefaultTransformsDebuggingOption(void);

    /**
     * Set the global default transform caching option
     */
    static EXPORT_STEM void setDefaultTransformCachingOption(bool bTransformCachingEnabled);

    /**
     * Set Euler accelerations used to propagate Euler rates
     */
    EXPORT_STEM virtual void setEulerAccelerations(const Eulers &eulerAccelerations) final;

    /**
     * Set Euler accelerations used to propagate Euler rates
     */
    EXPORT_STEM virtual void setEulerAccelerations(double rollAcceleration,
                                                   double pitchAcceleration,
                                                   double yawAcceleration) final;

    /**
     * Set Euler rates used to propagate Euler angles
     */
    EXPORT_STEM virtual void setEulerRates(const Eulers &eulerRates) final;

    /**
     * Set Euler rates used to propagate Euler angles
     */
    EXPORT_STEM virtual void setEulerRates(double rollRate,
                                           double pitchRate,
                                           double yawRate) final;

    /**
     * Set the Euler angles
     */
    EXPORT_STEM virtual void setEulers(const Eulers &eulers) final;

    /**
     * Set the Euler angles
     */
    EXPORT_STEM virtual void setEulers(double roll,
                                       double pitch,
                                       double yaw) final;

    /**
     * Set the frame of reference in which this motion state is defined
     */
    EXPORT_STEM virtual bool setFrame(ReferenceFrame *pFrame) final;

    /**
     * Set the perturbation state of this object's reference frame
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM virtual void setFrameState(const std::string &state) final;

    /**
     * Set the full kinematic state of this object; upon success, this object takes ownership of the input
     * argument and becomes responsible for its destruction
     */
    EXPORT_STEM virtual bool setKinematicState(KinematicState *pState) final;

    /**
     * Set this object's pitch angle
     */
    EXPORT_STEM virtual void setPitch(double pitch) final;

    /**
     * Set this object's pitch angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitch(double pitch,
                                      const AngleUnitType &angleUnits) final;

    /**
     * Set this object's pitch acceleration
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration) final;

    /**
     * Set this object's pitch acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration,
                                                  const AngleUnitType &angleUnits) final;

    /**
     * Set this object's pitch rate
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate) final;

    /**
     * Set this object's pitch rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate,
                                          const AngleUnitType &angleUnits) final;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(const Vector3d &position) = 0;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(double,
                                         double,
                                         double) = 0;

    /**
     * Set this object's roll angle
     */
    EXPORT_STEM virtual void setRoll(double roll) final;

    /**
     * Set this object's roll angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRoll(double roll,
                                     const AngleUnitType &angleUnits) final;

    /**
     * Set this object's roll acceleration
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration) final;

    /**
     * Set this object's roll acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration,
                                                 const AngleUnitType &angleUnits) final;

    /**
     * Set this object's roll rate
     */
    EXPORT_STEM virtual void setRollRate(double rollRate) final;

    /**
     * Set this object's roll rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRollRate(double rollRate,
                                         const AngleUnitType &angleUnits) final;

    /**
     * Set the time at which the initial position and velocity are defined for the state
     */
    EXPORT_STEM virtual void setTime(double t0) final;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(const Vector3d &velocity) = 0;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(double,
                                         double,
                                         double) = 0;

    /**
     * Set this object's yaw angle
     */
    EXPORT_STEM virtual void setYaw(double yaw) final;

    /**
     * Set this object's yaw angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYaw(double yaw,
                                    const AngleUnitType &angleUnits) final;

    /**
     * Set this object's yaw acceleration
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration) final;

    /**
     * Set this object's yaw acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration,
                                                const AngleUnitType &angleUnits) final;

    /**
     * Set this object's yaw rate
     */
    EXPORT_STEM virtual void setYawRate(double yawRate) final;

    /**
     * Set this object's yaw rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYawRate(double yawRate,
                                        const AngleUnitType &angleUnits) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(MotionState &motionState) override final;

    /**
     * Convert this object's motion state into a ReferenceFrame. Upon success, returns a dynamically allocated
     * ReferenceFrame object. Fails if the current object's motion state is not defined within a valid (non-
     * null) reference frame. It is up to the caller to destroy the reference frame object by making a call to
     * ReferenceFrame::delete()
     */
    EXPORT_STEM virtual ReferenceFrame *toReferenceFrame(const std::string &name = "") const final;

    /**
     * Convert this object's motion state into a ReferenceFrame. Upon success, the function populates the input
     * reference frame, if non-null.
     */
    EXPORT_STEM virtual bool toReferenceFrame(ReferenceFrame *pFrame) const final;

    /**
     * Convert this object's motion state into a shared ReferenceFrame. Upon success, returns a dynamically
     * allocated ReferenceFrame object. Fails if the current object's motion state is not defined within a
     * valid (non- null) reference frame. It is up to the caller to destroy the reference frame object by
     * making a call to ReferenceFrame::delete()
     */
    EXPORT_STEM virtual std::shared_ptr<ReferenceFrame>
    toSharedReferenceFrame(const std::string &name = "") const final;

    /**
     * Spatially (and optionally temporally) transform a clone of the current object's motion state from its
     * current frame of reference and/or coordinate system to the target frame of reference and/or coordinate
     * system (by default, the transformation is spatial only); upon success, function returns a non-null
     * pointer to a dynamically-allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param coordType the coordinate system in which the motion state will be described
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    EXPORT_STEM virtual MotionState *transformClone(ReferenceFrame *pFrame,
                                                    const CoordinateType &coordType,
                                                    bool bTemporal = false) const final;

    /**
     * Spatially and temporally transform a clone of the current object's motion state from its current frame
     * of reference and/or coordinate system to the target frame of reference and/or coordinate system. Since
     * this transformation is temporal, the motion state and all frames through which it is transformed will be
     * projected to the specified time; upon success, function returns a non-null pointer to a dynamically
     * allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param coordType the coordinate system in which the motion state will be described
     * @param t         the time to which the frames and motion states will be projected
     */
    EXPORT_STEM virtual MotionState *transformClone(ReferenceFrame *pFrame,
                                                    const CoordinateType &coordType,
                                                    double t) const final;

    /**
     * Transform the frame state of a clone of the current object's motion state to that which is specified;
     * the function works by first transforming this object's state to the root reference frame and then it
     * transforms the state down the chain using the target reference frame state
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM virtual MotionState *transformCloneFrameState(const std::string &state) final;

    /**
     * Transform a clone of the current object's motion state from its current coordinate system into the
     * desired coordinate system; upon success, function returns a non-null pointer to a dynamically allocated
     * motion state
     * @param coordType the coordinate system in which the motion state will be described
     */
    EXPORT_STEM virtual MotionState *transformCloneToCoordinateSystem(const CoordinateType &coordType) const final;

    /**
     * Spatially (and optionally temporally) transform a clone of the current object's motion state from its
     * current frame of reference to the target frame of reference (by default, the transformation is spatial
     * only); upon success, function returns a non-null pointer to a dynamically-allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    EXPORT_STEM virtual MotionState *transformCloneToFrame(ReferenceFrame *pFrame,
                                                           bool bTemporal = false) const final;

    /**
     * Spatially (and optionally temporally) transform a clone of the current object's motion and frame states
     * from the current frame of reference and state to the target frame of reference and state (by default,
     * the transformation is spatial only); upon success, function returns a non-null pointer to a
     * dynamically-allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param state     the perturbation state of this object's reference frame
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    template<typename T,
             typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                     std::is_same<T, const char *>::value, int>::type = 0>
    EXPORT_STEM MotionState *transformCloneToFrame(ReferenceFrame *pFrame,
                                                   T state,
                                                   bool bTemporal = false) const;

    /**
     * Spatially and temporally transform a clone of the current object's motion state from its current frame
     * of reference to the target frame of reference; upon success, function returns a non-null pointer to a
     * dynamically-allocated motion state
     * @param pFrame a pointer to the target reference frame
     * @param t      the time to which the frames and motion states will be projected
     */
    EXPORT_STEM virtual MotionState *transformCloneToFrame(ReferenceFrame *pFrame,
                                                           double t) const final;

    /**
     * Spatially and temporally transform a clone of the current object's motion state and frame from the
     * current frame of reference and state to the target frame of reference and state; upon success, function
     * returns a non-null pointer to a dynamically-allocated motion state
     * @param pFrame a pointer to the target reference frame
     * @param state  the perturbation state of this object's reference frame
     * @param t      the time to which the frames and motion states will be projected
     */
    template<typename T,
             typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                     std::is_same<T, const char *>::value, int>::type = 0>
    EXPORT_STEM MotionState *transformCloneToFrame(ReferenceFrame *pFrame,
                                                   T state,
                                                   double t) const;

    /**
     * Transform this motion state's frame state to that which is specified; the function works by first
     * transforming this object's state to the root reference frame, and then it transforms the state down the
     * chain using the target reference frame state
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM virtual bool transformFrameState(const std::string &state) final;

    /**
     * Spatially (and optionally temporally) transform a shared clone of the current object's motion state from
     * its current frame of reference and/or coordinate system to the target frame of reference and/or
     * coordinate system (by default, the transformation is spatial only); upon success, function returns a
     * non-null pointer to a dynamically-allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param coordType the coordinate system in which the motion state will be described
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState>
    transformSharedClone(ReferenceFrame *pFrame,
                         const CoordinateType &coordType,
                         bool bTemporal = false) const final;

    /**
     * Spatially and temporally transform a shared clone of the current object's motion state from its current
     * frame of reference and/or coordinate system to the target frame of reference and/or coordinate system.
     * Since this transformation is temporal, the motion state and all frames through which it is transformed
     * will be projected to the specified time; upon success, function returns a non-null pointer to a
     * dynamically allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param coordType the coordinate system in which the motion state will be described
     * @param t         the time to which the frames and motion states will be projected
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState>
    transformSharedClone(ReferenceFrame *pFrame,
                         const CoordinateType &coordType,
                         double t) const final;

    /**
     * Transform the frame state of a shared clone of the current object's motion state to that which is
     * specified; the function works by first transforming this object's state to the root reference frame, and
     * then it transforms the state down the chain using the target reference frame state
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState>
    transformSharedCloneFrameState(const std::string &state) final;

    /**
     * Transform a shared clone of the current object's motion state from its current coordinate system into
     * the desired coordinate system; upon success, function returns a non-null pointer to a dynamically
     * allocated motion state
     * @param coordType the coordinate system in which the motion state will be described
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState>
    transformSharedCloneToCoordinateSystem(const CoordinateType &coordType) const final;

    /**
     * Spatially (and optionally temporally) transform a shared clone of the current object's motion state from
     * its current frame of reference to the target frame of reference (by default, the transformation is
     * spatial only); upon success, function returns a non-null pointer to a dynamically-allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState>
    transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                bool bTemporal = false) const final;

    /**
     * Spatially (and optionally temporally) transform a shared clone of the current object's motion and frame
     * states from the current frame of reference and state to the target frame of reference and state (by
     * default, the transformation is spatial only); upon success, function returns a non-null pointer to a
     * dynamically-allocated motion state
     * @param pFrame    a pointer to the target reference frame
     * @param state     the perturbation state of this object's reference frame
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    template<typename T,
             typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                     std::is_same<T, const char *>::value, int>::type = 0>
    EXPORT_STEM std::shared_ptr<MotionState>
    transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                T state,
                                bool bTemporal = false) const;

    /**
     * Spatially and temporally transform a shared clone of the current object's motion state from its current
     * frame of reference to the target frame of reference; upon success, function returns a non-null pointer
     * to a dynamically-allocated motion state
     * @param pFrame a pointer to the target reference frame
     * @param t      the time to which the frames and motion states will be projected
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState>
    transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                double t) const final;

    /**
     * Spatially and temporally transform a shared clone of the current object's motion and frame states from
     * the current frame of reference and state to the target frame of reference and state; upon success,
     * function returns a non-null pointer to a dynamically-allocated motion state
     * @param pFrame a pointer to the target reference frame
     * @param state  the perturbation state of this object's reference frame
     * @param t      the time to which the frames and motion states will be projected
     */
    template<typename T,
             typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                     std::is_same<T, const char *>::value, int>::type = 0>
    EXPORT_STEM std::shared_ptr<MotionState>
    transformSharedCloneToFrame(ReferenceFrame *pFrame,
                                T state,
                                double t) const;

    /**
     * Spatially (and optionally temporally) transform this object's motion state defined in its current frame
     * of reference to the input frame of reference (by default, the transformation is spatial only)
     * @param pFrame    a pointer to the target reference frame
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    EXPORT_STEM bool transformToFrame(ReferenceFrame *pFrame,
                                      bool bTemporal = false);

    /**
     * Spatially (and optionally temporally) transform this object's motion and frame states defined in the
     * current frame of reference and state to the input frame of reference and state (by default, the
     * transformation is spatial only)
     * @param pFrame    a pointer to the target reference frame
     * @param state     the perturbation state of this object's reference frame
     * @param bTemporal flag indicating whether or not a temporal transformation will take place. If false, a
     *                  spatial-only transformation will occur (time at which frames and motion state are
     *                  defined is not considered). If true, the motion state and all frames through which it
     *                  is transformed will be projected to the time at which the target frame is defined.
     */
    template<typename T,
             typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                     std::is_same<T, const char *>::value, int>::type = 0>
    EXPORT_STEM bool transformToFrame(ReferenceFrame *pFrame,
                                      T state,
                                      bool bTemporal = false);

    /**
     * Spatially and temporally transform this object's motion state defined in its current frame of reference
     * to the input frame of reference. Since this transformation is temporal, the motion state and all frames
     * through which it is transformed will be projected to the specified time
     * @param pFrame a pointer to the target reference frame
     * @param t      the time to which the frames and motion states will be projected
     */
    EXPORT_STEM virtual bool transformToFrame(ReferenceFrame *pFrame,
                                              double t) final;

    /**
     * Spatially and temporally transform this object's motion and frame states defined in the current frame of
     * reference and state to the input frame of reference and state. Since this transformation is temporal,
     * the motion state and all frames through which it is transformed will be projected to the specified time
     * @param pFrame a pointer to the target reference frame
     * @param state  the perturbation state of this object's reference frame
     * @param t      the time to which the frames and motion states will be projected
     */
    template<typename T,
             typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value ||
                                     std::is_same<T, const char *>::value, int>::type = 0>
    EXPORT_STEM bool transformToFrame(ReferenceFrame *pFrame,
                                      T state,
                                      double t);

    /**
     * Update the motion state with the specified time. This function will update all quantities within the
     * motion state that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void update(double t,
                                    const TimeReferenceType &timeReference = TimeReferenceType::Absolute) final;

    /**
     * Update the motion state Euler rates only using the specified time
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void updateEulerRates(double t,
                                              const TimeReferenceType &timeReference =
                                              TimeReferenceType::Delta) final;

    /**
     * Update the motion state Euler angles only using the specified time
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void updateEulers(double t,
                                          const TimeReferenceType &timeReference =
                                          TimeReferenceType::Delta) final;

    /**
     * Update the motion state position only using the specified time
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void updatePosition(double t,
                                            const TimeReferenceType &timeReference =
                                            TimeReferenceType::Delta) final;

    /**
     * Update the motion state time using the specified delta time
     * @param dt the time update interval (s)
     */
    EXPORT_STEM virtual void updateTime(double dt) final;

    /**
     * Update the motion state velocity only using the specified time
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void updateVelocity(double t,
                                            const TimeReferenceType &timeReference =
                                            TimeReferenceType::Delta) final;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
protected:

    /**
     * flag to enable/disable transform caching
     */
    bool m_bCacheTransformsEnabled;

    /**
     * flag to enable/disable motion state transformation debugging mode
     */
    bool m_bDebugTransformsEnabled;

    /**
     * default flag to enable/disable transform caching
     */
    static bool m_bDefaultTransformCachingOption;

    /**
     * default flag to enable/disable motion state transformation debugging mode
     */
    static bool m_bDefaultTransformDebuggingOption;

    /**
     * state identifies the current state of this object's reference frame
     */
    std::string m_frameState;

    /**
     * the default conversion function describing how to convert angular quantities from azimuth, zenith
     */
    static tMotionStateAngleConversionFunction m_pDefaultFromAzimuthZenithConverter;

    /**
     * the default conversion function describing how to convert angular quantities to azimuth, zenith
     */
    static tMotionStateAngleConversionFunction m_pDefaultToAzimuthZenithConverter;

    /**
     * the reference frame in which this motion state is defined
     */
    ReferenceFrame *m_pFrame;

    /**
     * the conversion function describing how to convert angular quantities from azimuth, zenith
     */
    tMotionStateAngleConversionFunction m_pFromAzimuthZenithConverter;

    /**
     * this object's state's kinematic state
     */
    KinematicState *m_pState;

    /**
     * the default conversion function describing how to convert angular quantities to azimuth, zenith
     */
    tMotionStateAngleConversionFunction m_pToAzimuthZenithConverter;

    /**
     * map of cached motion state transformations
     */
    tTransformationCache m_transformationCache;
};

}

}

#endif
