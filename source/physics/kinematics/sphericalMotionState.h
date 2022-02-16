#ifndef SPHERICAL_MOTION_STATE_H
#define SPHERICAL_MOTION_STATE_H

#include "motionState.h"

namespace physics
{

namespace kinematics
{

// forward declarations
struct SphericalAccelerationAxisType;
class SphericalMotionState;
struct SphericalPositionAxisType;
struct SphericalVelocityAxisType;

/**
 * forward declarations of spherical coordinate conversion functions for vertical angles, rates, accelerations
 */
extern EXPORT_STEM void elevationToZenith(SphericalMotionState *pSphericalMotionState);
extern EXPORT_STEM void negativeElevationToZenith(SphericalMotionState *pSphericalMotionState);
extern EXPORT_STEM void zenithToElevation(SphericalMotionState *pSphericalMotionState);
extern EXPORT_STEM void zenithToNegativeElevation(SphericalMotionState *pSphericalMotionState);

/**
 * This class stores and manipulates the spherical motion state of a spatially-
 *                              orientable physical object
 */
class SphericalMotionState
: public MotionState,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Using declarations
     */
    using MotionState::operator [];
    using MotionState::calcApproachTime;
    using MotionState::calcCartesianState;
    using MotionState::calcMinimumApproach;
    using MotionState::calcRange;
    using MotionState::calcRangeAcceleration;
    using MotionState::calcRangeRate;
    using MotionState::calcSpeed;
    using MotionState::get;
    using MotionState::getAcceleration;
    using MotionState::getPosition;
    using MotionState::getVelocity;
    using MotionState::scale;
    using MotionState::set;
#if 0
protected: // TODO: will eventually disable direct construction in favor of using create() function
#endif
    /**
     * Construct an empty motion state object
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct an empty motion state at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    explicit EXPORT_STEM SphericalMotionState(double t,
                                              const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given spherical position at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(double t,
                                     const Vector3d &position,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given spherical position and initial velocity at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given spherical position, initial velocity, and constant acceleration
     * at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given spherical position, initial velocity, constant acceleration and
     * Euler orientation at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const Eulers &eulers,
                                     const AngleUnitType &angleUnits,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given spherical position, initial velocity, constant acceleration,
     * Euler orientation and Euler rates at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const Eulers &eulers,
                                     const Eulers &eulerRates,
                                     const AngleUnitType &angleUnits,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given spherical position, initial velocity, constant acceleration,
     * Euler orientation, Euler rates, and Euler accelerations at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM SphericalMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const Eulers &eulers,
                                     const Eulers &eulerRates,
                                     const Eulers &eulerAccelerations,
                                     const AngleUnitType &angleUnits,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a spherical motion state from an abstract motion state constant reference
     */
    EXPORT_STEM SphericalMotionState(const MotionState &motionState);

    /**
     * Construct a spherical motion state from a Cartesian motion state
     */
    EXPORT_STEM SphericalMotionState(const CartesianMotionState &cartesianMotionState);

    /**
     * Copy constructor
     */
    EXPORT_STEM SphericalMotionState(const SphericalMotionState &sphericalMotionState);

    /**
     * Move constructor
     */
    EXPORT_STEM SphericalMotionState(SphericalMotionState &&motionState);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~SphericalMotionState(void) override;

    /**
     * Assignment operator
     */
    EXPORT_STEM virtual SphericalMotionState &operator = (const MotionState &motionState) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM SphericalMotionState &operator = (const SphericalMotionState &motionState);

    /**
     * Move assignment operator
     */
    EXPORT_STEM SphericalMotionState &operator = (SphericalMotionState &&motionState);

    /**
     * Subscript operator
     * @param type an enumeration which takes on one of the following values:
     *             = SphericalAccelerationAxisType
     *             = SphericalPositionAxisType
     *             = SphericalVelocityAxisType
     */
    template<typename Enum>
    inline auto &operator [] (Enum &&type)
    {
        return get(std::forward<Enum>(type));
    }

    /**
     * Subscript operator
     * @param type an enumeration which takes on one of the following values:
     *             = SphericalAccelerationAxisType
     *             = SphericalPositionAxisType
     *             = SphericalVelocityAxisType
     */
    template<typename Enum>
    inline auto operator [] (Enum &&type) const
    {
        return get(std::forward<Enum>(type));
    }

    /**
     * Assign the contents of one motion state to another
     * @param pMotionState a pointer to the source motion state
     */
    EXPORT_STEM virtual bool assign(const MotionState *pMotionState) override;

    /**
     * Calculate the scalar acceleration
     */
    EXPORT_STEM virtual double calcAcceleration(void) const override;

    /**
     * Calculate the time at which the current object's motion state approaches the null motion state at the
     * desired range. The time returned represents the closest time, whether it be in the future or past, at
     * which the motion state approaches the given range. If the motion state does not approach the specified
     * range, the subroutine returns infinity
     */
    EXPORT_STEM virtual double calcApproachTime(double range) const override;

    /**
     * Calculate the Cartesian motion state from the current object's spherical motion state at the specified
     * time
     * @param pCartesianMotionState the object in which the resultant Cartesian state will be stored
     * @param t                     the time to which the resultant motion state will be projected
     */
    EXPORT_STEM virtual bool calcCartesianState(CartesianMotionState *pCartesianMotionState,
                                                double t) const override;

    /**
     * Calculate the minimum approach distance achieved with respect to another motion state
     * @param tApproach the calculated time at which the minimum distance is achieved
     */
    EXPORT_STEM virtual double calcMinimumApproach(double &tApproach) const override;

    /**
     * Calculate the range at time t
     */
    EXPORT_STEM virtual double calcRange(double t) const override;

    /**
     * Calculate the range with respect to another motion state at time t
     */
    EXPORT_STEM virtual double calcRange(double t,
                                         const MotionState &motionState) const override;

    /**
     * Calculate the range acceleration at time t
     */
    EXPORT_STEM virtual double calcRangeAcceleration(double t) const override;

    /**
     * Calculate the range acceleration with respect to another motion state at time t
     */
    EXPORT_STEM virtual double calcRangeAcceleration(double t,
                                                     const MotionState &motionState) const override;

    /**
     * Calculate the range rate at time t
     */
    EXPORT_STEM virtual double calcRangeRate(double t) const override;

    /**
     * Calculate the range rate with respect to another motion state at time t
     */
    EXPORT_STEM virtual double calcRangeRate(double t,
                                             const MotionState &motionState) const override;

    /**
     * Calculate the scalar speed at time t
     */
    EXPORT_STEM virtual double calcSpeed(double t) const override;

    /**
     * clone() function
     */
    EXPORT_STEM virtual SphericalMotionState *clone(void) const override;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) override;

    /**
     * Create a spherical motion state within the given frame of reference
     * @param pFrame the frame of reference in which the new motion state will be created
     * @param state  the perturbation state of this object's reference frame
     */
    static EXPORT_STEM SphericalMotionState *create(ReferenceFrame *pFrame,
                                                    const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Get spherical acceleration in horizontal, vertical, radial axis
     */
    EXPORT_STEM double &get(const SphericalAccelerationAxisType &type);

    /**
     * Get spherical acceleration in horizontal, vertical, radial axis
     */
    EXPORT_STEM double get(const SphericalAccelerationAxisType &type) const;

    /**
     * Get spherical position in horizontal, vertical, radial axis
     */
    EXPORT_STEM double &get(const SphericalPositionAxisType &type);

    /**
     * Get spherical position in horizontal, vertical, radial axis
     */
    EXPORT_STEM double get(const SphericalPositionAxisType &type) const;

    /**
     * Get spherical velocity in horizontal, vertical, radial axis
     */
    EXPORT_STEM double &get(const SphericalVelocityAxisType &type);

    /**
     * Get spherical velocity in horizontal, vertical, radial axis
     */
    EXPORT_STEM double get(const SphericalVelocityAxisType &type) const;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double &horizontalAcc,
                                             double &verticalAcc,
                                             double &radialAcc) const override;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double acceleration[3]) const override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the coordinate system in which this motion state is described
     */
    EXPORT_STEM virtual CoordinateType getCoordinateType(void) const override;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual void getPosition(double &horizontal,
                                         double &vertical,
                                         double &radial) const override;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double &horizontal,
                                         double &vertical,
                                         double &radial,
                                         double t) const override;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double position[3],
                                         double t) const override;

    /**
     * Get a pointer to a conversion functor that corresponds to the specified azimuth, zenith spherical
     * conversion type; Upon success, function returns non-null
     */
    static EXPORT_STEM tMotionStateAngleConversionFunction
    getSphericalConversionFunction(const SphericalConversionType &type);

    /**
     * Get an enumerated type that corresponds to the conversion functor used to convert standard definitions
     * of spherical azimuth (measured counter-clockwise from +x), zenith (measured from +z to -z) to a user-
     * defined horizontal, vertical angle convention
     */
    static EXPORT_STEM SphericalConversionType
    getSphericalConversionType(tMotionStateAngleConversionFunction pConverter);

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual void getVelocity(double &horizontalRate,
                                         double &verticalRate,
                                         double &radialRate) const override;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double &horizontalRate,
                                         double &verticalRate,
                                         double &radialRate,
                                         double t) const override;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3],
                                         double t) const override;

    /**
     * Function to print the contents of the motion state
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * Return the motion state projected to time t
     */
    EXPORT_STEM SphericalMotionState project(double t) const;

    /**
     * Scale the acceleration in horizontal, vertical, and radial
     */
    EXPORT_STEM void scale(const SphericalAccelerationAxisType &type,
                           double value);

    /**
     * Scale position in horizontal, vertical, and radial
     */
    EXPORT_STEM void scale(const SphericalPositionAxisType &type,
                           double value);

    /**
     * Scale velocity in horizontal, vertical, and radial
     */
    EXPORT_STEM void scale(const SphericalVelocityAxisType &type,
                           double value);

    /**
     * Set spherical acceleration in horizontal, vertical, radial
     */
    EXPORT_STEM void set(const SphericalAccelerationAxisType &type,
                         double acceleration);

    /**
     * Set spherical position in horizontal, vertical, radial
     */
    EXPORT_STEM void set(const SphericalPositionAxisType &type,
                         double position);

    /**
     * Set spherical velocity in horizontal, vertical, radial
     */
    EXPORT_STEM void set(const SphericalVelocityAxisType &type,
                         double velocity);

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(const Vector3d &acceleration) override;

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(double horizontalAcc,
                                             double verticalAcc,
                                             double radialAcc) override;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(const Vector3d &position) override;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(double horizontal,
                                         double vertical,
                                         double radial) override;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(const Vector3d &velocity) override;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(double horizontalRate,
                                         double verticalRate,
                                         double radialRate) override;
};

}

}

#endif
