#ifndef CARTESIAN_MOTION_STATE_H
#define CARTESIAN_MOTION_STATE_H

#include "motionState.h"

namespace physics
{

namespace kinematics
{

// forward declarations
struct AccelerationAxisType;
struct PositionAxisType;
struct VelocityAxisType;

/**
 * This class stores and manipulates the Cartesian motion state of a spatially-
 *                              orientable physical object
 */
class CartesianMotionState
: public MotionState,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Using declarations
     */
    using MotionState::operator [];
    using MotionState::calcApproachTime;
    using MotionState::calcMinimumApproach;
    using MotionState::calcRange;
    using MotionState::calcRangeAcceleration;
    using MotionState::calcRangeRate;
    using MotionState::calcSpeed;
    using MotionState::calcSphericalState;
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
    EXPORT_STEM CartesianMotionState(const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct an empty motion state at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    explicit EXPORT_STEM CartesianMotionState(double t,
                                              const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM CartesianMotionState(double t,
                                     const Vector3d &position,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position and initial velocity at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM CartesianMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, and constant acceleration at the
     * specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM CartesianMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, constant acceleration and Euler
     * orientation at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM CartesianMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const Eulers &eulers,
                                     const AngleUnitType &angleUnits,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a motion state with the given position, initial velocity, constant acceleration, Euler
     * orientation and Euler rates at the specified time
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM CartesianMotionState(double t,
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
     * @param state the perturbation state of this object's reference frame
     */
    EXPORT_STEM CartesianMotionState(double t,
                                     const Vector3d &position,
                                     const Vector3d &velocity,
                                     const Vector3d &acceleration,
                                     const Eulers &eulers,
                                     const Eulers &eulerRates,
                                     const Eulers &eulerAccelerations,
                                     const AngleUnitType &angleUnits,
                                     const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Construct a Cartesian motion state from an abstract motion state constant reference
     */
    EXPORT_STEM CartesianMotionState(const MotionState &motionState);

    /**
     * Construct a Cartesian motion state from a spherical motion state
     */
    EXPORT_STEM CartesianMotionState(const SphericalMotionState &sphericalMotionState);

    /**
     * Copy constructor
     */
    EXPORT_STEM CartesianMotionState(const CartesianMotionState &motionState);

    /**
     * Move constructor
     */
    EXPORT_STEM CartesianMotionState(CartesianMotionState &&motionState);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~CartesianMotionState(void) override;

    /**
     * Assignment operator
     */
    EXPORT_STEM virtual CartesianMotionState &operator = (const MotionState &motionState) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM CartesianMotionState &operator = (const CartesianMotionState &motionState);

    /**
     * Move assignment operator
     */
    EXPORT_STEM CartesianMotionState &operator = (CartesianMotionState &&motionState);

    /**
     * Subscript operator
     * @param type an enumeration which takes on one of the following values:
     *             = AccelerationAxisType
     *             = PositionAxisType
     *             = VelocityAxisType
     */
    template<typename Enum>
    inline auto &operator [] (Enum &&type)
    {
        return get(std::forward<Enum>(type));
    }

    /**
     * Subscript operator
     * @param type an enumeration which takes on one of the following values:
     *             = AccelerationAxisType
     *             = PositionAxisType
     *             = VelocityAxisType
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
     * Calculate the time at which the current object's motion state approaches another motion state at the
     * desired range. The time returned represents the closest time, whether it be in the future or past, at
     * which the motion states approach one another at the given range. If the motion states do not approach
     * one another within the specified range, the subroutine returns infinity
     */
    EXPORT_STEM virtual double calcApproachTime(double range) const override;

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
     * Calculate the spherical motion state from the current object's Cartesian motion state at the specified
     * time
     * @param pSphericalMotionState the object in which the resultant spherical state will be stored
     * @param angleUnits            the desired angle output units, Degrees or Radians
     * @param t                     the time to which the resultant motion state will be projected
     */
    EXPORT_STEM virtual bool calcSphericalState(SphericalMotionState *pSphericalMotionState,
                                                const AngleUnitType &angleUnits,
                                                double t) const override;

    /**
     * clone() function
     */
    EXPORT_STEM virtual CartesianMotionState *clone(void) const override;

public:

    /**
     * Create a Cartesian motion state within the given frame of reference
     * @param pFrame the frame of reference in which the new motion state will be created
     * @param state  the perturbation state of this object's reference frame
     */
    static EXPORT_STEM CartesianMotionState *create(ReferenceFrame *pFrame,
                                                    const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Get Cartesian acceleration in x, y, or z
     */
    EXPORT_STEM double &get(const AccelerationAxisType &type);

    /**
     * Get Cartesian acceleration in x, y, or z (const overload)
     */
    EXPORT_STEM double get(const AccelerationAxisType &type) const;

    /**
     * Get Cartesian position in x, y, or z
     */
    EXPORT_STEM double &get(const PositionAxisType &type);

    /**
     * Get Cartesian position in x, y, or z (const overload)
     */
    EXPORT_STEM double get(const PositionAxisType &type) const;

    /**
     * Get Cartesian velocity in x, y, or z
     */
    EXPORT_STEM double &get(const VelocityAxisType &type);

    /**
     * Get Cartesian velocity in x, y, or z (const overload)
     */
    EXPORT_STEM double get(const VelocityAxisType &type) const;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double &xAcceleration,
                                             double &yAcceleration,
                                             double &zAcceleration) const override;

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
    EXPORT_STEM virtual void getPosition(double &xPosition,
                                         double &yPosition,
                                         double &zPosition) const override;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double &xPosition,
                                         double &yPosition,
                                         double &zPosition,
                                         double t) const override;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double position[3], double t) const override;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual void getVelocity(double &xVelocity,
                                         double &yVelocity,
                                         double &zVelocity) const override;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double &xVelocity,
                                         double &yVelocity,
                                         double &zVelocity,
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
    EXPORT_STEM CartesianMotionState project(double t) const;

    /**
     * Scale the acceleration in x, y, or z
     */
    EXPORT_STEM void scale(const AccelerationAxisType &type,
                           double value);

    /**
     * Scale position in x, y, or z
     */
    EXPORT_STEM void scale(const PositionAxisType &type,
                           double value);

    /**
     * Scale velocity in x, y, or z
     */
    EXPORT_STEM void scale(const VelocityAxisType &type,
                           double value);

    /**
     * Set Cartesian acceleration in x, y, or z
     */
    EXPORT_STEM void set(const AccelerationAxisType &type,
                         double acceleration);

    /**
     * Set Cartesian position in x, y, or z
     */
    EXPORT_STEM void set(const PositionAxisType &type,
                         double position);

    /**
     * Set Cartesian velocity in x, y, or z
     */
    EXPORT_STEM void set(const VelocityAxisType &type,
                         double velocity);

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(const Vector3d &acceleration) override;

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration) override;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(const Vector3d &position) override;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(double xPosition,
                                         double yPosition,
                                         double zPosition) override;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(const Vector3d &velocity) override;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(double xVelocity,
                                         double yVelocity,
                                         double zVelocity) override;
};

}

}

#endif
