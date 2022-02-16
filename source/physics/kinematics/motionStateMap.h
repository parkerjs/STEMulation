#ifndef MOTION_STATE_MAP_H
#define MOTION_STATE_MAP_H

#include "cloneable.h"
#include "coordinate_type.h"
#include "export_library.h"
#include "initializable.h"
#include "loggable.h"
#include "output_streamable.h"
#include "reflective.h"
#include "serializable.h"
#include "swappable.h"
#include "updateable.h"

namespace physics
{

namespace kinematics
{

// forward declarations
class MotionState;
class ReferenceFrame;

/**
 * This class provides objects with a mapped container of motion states, along with a concrete interface to
 * store, retrieve and manipulate them
 */
class MotionStateMap
: public attributes::interfaces::Cloneable<MotionStateMap>,
  public attributes::interfaces::Initializable,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  public attributes::concrete::OutputStreamable<MotionStateMap>,
  public attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
  public attributes::interfaces::Swappable<MotionStateMap>,
  public attributes::interfaces::Updateable
{
public:

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

    /**
     * Constructor
     */
    EXPORT_STEM MotionStateMap(void);

    /**
     * Constructor
     * @param name         the name associated with the motion state
     * @param pMotionState an r-value reference pointer to a dynamically-allocated motion state object; upon
     *                     success, this object takes ownership of the motion state and is responsible for its
     *                     destruction
     */
    EXPORT_STEM MotionStateMap(const std::string &name,
                               MotionState *&&pMotionState);

    /**
     * Copy constructor
     */
    EXPORT_STEM MotionStateMap(const MotionStateMap &motionStateMap);

    /**
     * Move constructor
     */
    EXPORT_STEM MotionStateMap(MotionStateMap &&motionStateMap);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~MotionStateMap(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM MotionStateMap &operator = (const MotionStateMap &motionStateMap);

    /**
     * Move assignment operator
     */
    EXPORT_STEM MotionStateMap &operator = (MotionStateMap &&motionStateMap);

    /**
     * Assign a motion state to this object and associate it with the given name; returns true upon success
     * @param name         the name associated with the motion state
     * @param pMotionState a pointer to the source motion state
     */
    EXPORT_STEM virtual bool assignMotionState(const std::string &name,
                                               MotionState *pMotionState);

    /**
     * clone() function
     */
    EXPORT_STEM virtual MotionStateMap *clone(void) const override;

    /**
     * Create a motion state in the given frame of reference and associate it with the specified name
     * @param pFrame         the frame of reference in which the motion state will be created
     * @param name           the name associated with the motion state
     * @param coordinateType the coordinate system in which the motion state will be described
     */
    EXPORT_STEM virtual MotionState *createMotionState(ReferenceFrame *pFrame,
                                                       const std::string &name = "truth",
                                                       const CoordinateType &coordinateType =
                                                       CoordinateType::Cartesian);

    /**
     * Delete a motion state by name; returns true upon success
     * @param name the name associated with the motion state
     */
    EXPORT_STEM virtual bool deleteMotionState(const std::string &name) final;

    /**
     * Delete a motion state by value; if found, function sets input argument to null
     * @param pMotionState a pointer to the motion state to be deleted from this object
     */
    EXPORT_STEM virtual bool deleteMotionState(MotionState *&pMotionState) final;

    /**
     * Delete all motion states from this object
     */
    EXPORT_STEM virtual void deleteMotionStates(void) final;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Retrieve a motion state by name; upon success, function returns non-null
     * @param name the name associated with the motion state
     */
    EXPORT_STEM virtual MotionState *getMotionState(const std::string &name);

    /**
     * Get this object's map of motion states
     */
    EXPORT_STEM virtual std::map<std::string, MotionState *> &getMotionStates(void) final;

    /**
     * Determine if this object contains a motion state associated with the indicated name
     * @param name the name associated with the motion state
     */
    EXPORT_STEM virtual bool hasMotionState(const std::string &name) const final;

    /**
     * Determine if this object contains a key for the specified motion state
     * @param name the name associated with the motion state
     */
    EXPORT_STEM virtual bool hasMotionState(const MotionState *pMotionState) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Move a motion state into this object and associate it with the given name; upon success the current
     * object takes ownership of the input argument and becomes responsible for its eventual destruction;
     * hence, the function only accepts an r-value (temporary) reference to a dynamically-allocated motion
     * state; function returns true under the following conditions: 1) The motion state object argument is
     * non-null 2) The motion state object does not already exist in this object's map of motion states 3) The
     * name associated with the motion state is non-empty and is not already present in this object's map of
     * motion states
     * @param name         the name associated with the motion state
     * @param pMotionState an r-value reference pointer to a dynamically-allocated motion state object; upon
     *                     success, this object takes ownership of the motion state and is responsible for its
     *                     destruction
     */
    EXPORT_STEM virtual bool moveMotionState(const std::string &name,
                                             MotionState *&&pMotionState);

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * Purge null motion states from this object
     */
    EXPORT_STEM virtual void purgeNullMotionStates(void) final;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(MotionStateMap &motionStateMap) override final;

    /**
     * Update function
     */
    EXPORT_STEM virtual bool update(double time) override;

private:

    /**
     * a map of motion states associated with this object
     */
    std::map<std::string, MotionState *> m_motionStates;
};

}

}

#endif
