#ifndef MOTION_STATE_CONTAINER_H
#define MOTION_STATE_CONTAINER_H

#include "export_library.h"
#include "identifier_and_time_sorted_container.h"
#include "motionState.h"

namespace physics
{

namespace kinematics
{

// forward declarations
class ReferenceFrame;

/**
 * This class implements a container for motion states and stores and retrieves them first by coordinate
 * reference frame and system, and then by time
 */
class MotionStateContainer
: public attributes::interfaces::Initializable,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<MotionStateContainer>
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM MotionStateContainer(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM MotionStateContainer(const MotionStateContainer &container);

    /**
     * Move constructor
     */
    EXPORT_STEM MotionStateContainer(MotionStateContainer &&container);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~MotionStateContainer(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM MotionStateContainer &operator = (const MotionStateContainer &container);

    /**
     * Move assignment operator
     */
    EXPORT_STEM MotionStateContainer &operator = (MotionStateContainer &&container);

    /**
     * Add a motion state to this container
     */
    EXPORT_STEM virtual void addMotionState(const MotionState &motionState) final;

    /**
     * Add a motion state to this container; returns true if the entry is successfully added and the current
     * object takes ownership of the input argument and becomes responsible for its destruction
     */
    EXPORT_STEM virtual bool addMotionState(MotionState *pMotionState) final;

    /**
     * Delete a motion state by value from this container; sets input argument to null upon success
     */
    EXPORT_STEM virtual bool deleteMotionState(MotionState *&pMotionState) final;

    /**
     * Delete all motion states described in the specified coordinate reference frame and coordinate system;
     * returns true upon success
     */
    EXPORT_STEM virtual void deleteMotionStates(const std::string &frame,
                                                const CoordinateType &coordinateType =
                                                CoordinateType::Cartesian) final;

    /**
     * Delete all motion states described in the specified coordinate reference frame and coordinate system;
     * returns true upon success
     */
    EXPORT_STEM virtual void deleteMotionStates(const ReferenceFrame *pFrame,
                                                const CoordinateType &coordinateType =
                                                CoordinateType::Cartesian) final;

    /**
     * Delete all motion states tagged between the provided starting and ending times
     */
    EXPORT_STEM virtual void deleteMotionStates(double startTime,
                                                double endTime) final;

    /**
     * Delete all motion states described in the specified coordinate reference frame and coordinate system
     * tagged between the provided starting and ending times; returns an iterator to the element that follows
     * the last element erased by this function
     */
    EXPORT_STEM virtual typename std::map<std::string, std::vector<MotionState *>>::iterator
    deleteMotionStates(double startTime,
                       double endTime,
                       const std::string &frame,
                       const CoordinateType &coordinateType = CoordinateType::Cartesian) final;

    /**
     * Delete all motion states described in the specified coordinate reference frame and coordinate system
     * tagged between the provided starting and ending times; returns an iterator to the element that follows
     * the last element erased by this function
     */
    EXPORT_STEM virtual typename std::map<std::string, std::vector<MotionState *>>::iterator
    deleteMotionStates(double startTime,
                       double endTime,
                       const ReferenceFrame *pFrame,
                       const CoordinateType &coordinateType = CoordinateType::Cartesian) final;

    /**
     * Delete motion states by value from this container
     */
    EXPORT_STEM virtual bool deleteMotionStates(std::vector<MotionState *> &motionStates) final;

    /**
     * Delete all motion states from this container
     */
    EXPORT_STEM virtual void deleteMotionStates(void) final;

    /**
     * Test for empty container
     */
    EXPORT_STEM virtual bool empty(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Retrieve the latest motion state associated with the specified coordinate reference frame and coordinate
     * system; returns non-null upon success
     */
    EXPORT_STEM virtual MotionState *getLatestMotionState(const std::string &frame,
                                                          const CoordinateType &coordinateType =
                                                          CoordinateType::Cartesian) const final;

    /**
     * Retrieve the latest motion state associated with the specified coordinate reference frame and coordinate
     * system; returns non-null upon success
     */
    EXPORT_STEM virtual MotionState *getLatestMotionState(const ReferenceFrame *pFrame,
                                                          const CoordinateType &coordinateType =
                                                          CoordinateType::Cartesian) const final;

    /**
     * Retrieve the latest motion states for all coordinate reference frames and coordinate systems; returns
     * true upon success and entries are appended to input argument
     */
    EXPORT_STEM virtual void getLatestMotionStates(std::vector<MotionState *> &motionStates) const final;

    /**
     * Retrieve the most recently available motion state associated with the specified coordinate reference
     * frame and coordinate system; returns non-null upon success
     */
    EXPORT_STEM virtual MotionState *getMostRecentAvailableMotionState(double time,
                                                                       const std::string &frame,
                                                                       const CoordinateType &coordinateType =
                                                                       CoordinateType::Cartesian) const final;

    /**
     * Retrieve the most recently available motion state associated with the specified coordinate reference
     * frame and coordinate system; returns non-null upon success
     */
    EXPORT_STEM virtual MotionState *getMostRecentAvailableMotionState(double time,
                                                                       const ReferenceFrame *pFrame,
                                                                       const CoordinateType &coordinateType =
                                                                       CoordinateType::Cartesian) const final;

    /**
     * Retrieve the most recently available motion states described in the specified coordinate reference frame
     * and coordinate system; matching entries are appended to input argument
     */
    EXPORT_STEM virtual void getMostRecentAvailableMotionStates(double time,
                                                                std::vector<MotionState *> &
                                                                motionStates) const final;

    /**
     * Retrieve a motion state described in the specified coordinate reference frame and coordinate system
     * tagged at the provided time; returns non-null upon success
     */
    EXPORT_STEM virtual MotionState *getMotionState(double time,
                                                    const std::string &frame,
                                                    const CoordinateType &coordinateType =
                                                    CoordinateType::Cartesian) const final;

    /**
     * Retrieve a motion state described in the specified coordinate reference frame and coordinate system
     * tagged at the provided time; returns non-null upon success
     */
    EXPORT_STEM virtual MotionState *getMotionState(double time,
                                                    const ReferenceFrame *pFrame,
                                                    const CoordinateType &coordinateType =
                                                    CoordinateType::Cartesian) const final;

    /**
     * Get this object's motion state container
     */
    EXPORT_STEM virtual containers::IdentifierAndTimeSortedContainer<MotionState,
                                                                     std::string,
                                                                     double,
                                                                     &MotionState::getFrameAndCoordinateSystem,
                                                                     &MotionState::getTime> &
    getMotionStateContainer(void) final;

    /**
     * Get this object's motion state container
     */
    EXPORT_STEM virtual containers::IdentifierAndTimeSortedContainer<MotionState,
                                                                     std::string,
                                                                     double,
                                                                     &MotionState::getFrameAndCoordinateSystem,
                                                                     &MotionState::getTime>
    getMotionStateContainer(void) const final;

    /**
     * Retrieve all motion states described in the specified coordinate reference frame and coordinate system;
     * upon success, entries are appended to input argument
     */
    EXPORT_STEM virtual void getMotionStates(std::vector<MotionState *> &motionStates,
                                             const std::string &frame,
                                             const CoordinateType &coordinateType =
                                             CoordinateType::Cartesian) const final;

    /**
     * Retrieve all motion states described in the specified coordinate reference frame and coordinate system;
     * upon success, entries are appended to input argument
     */
    EXPORT_STEM virtual void getMotionStates(std::vector<MotionState *> &motionStates,
                                             const ReferenceFrame *pFrame,
                                             const CoordinateType &coordinateType =
                                             CoordinateType::Cartesian) const final;

    /**
     * Retrieve all motion states tagged between the provided starting and ending times; upon success, entries
     * are appended to input argument
     */
    EXPORT_STEM virtual void getMotionStates(double startTime,
                                             double endTime,
                                             std::vector<MotionState *> &motionStates) const final;

    /**
     * Retrieve all motion states described in the specified coordinate reference frame and coordinate system
     * tagged between the provided starting and ending times; returns true upon success and entries are
     * appended to input argument
     */
    EXPORT_STEM virtual bool getMotionStates(std::vector<MotionState *> &motionStates,
                                             double startTime,
                                             double endTime,
                                             const std::string &frame,
                                             const CoordinateType &coordinateType =
                                             CoordinateType::Cartesian) const final;

    /**
     * Retrieve all motion states described in the specified coordinate reference frame and coordinate system
     * tagged between the provided starting and ending times; returns true upon successs and entries are
     * appended to input argument
     */
    EXPORT_STEM virtual bool getMotionStates(std::vector<MotionState *> &motionStates,
                                             double startTime,
                                             double endTime,
                                             const ReferenceFrame *pFrame,
                                             const CoordinateType &coordinateType =
                                             CoordinateType::Cartesian) const final;

    /**
     * Initialization function
     */
    virtual bool initialize(void) override;

    /**
     * Remove a motion state by value from this container
     */
    EXPORT_STEM virtual bool removeMotionState(MotionState *pMotionState) final;

    /**
     * Remove all motion states described in the specified coordinate reference frame and coordinate system
     */
    EXPORT_STEM virtual void removeMotionStates(const std::string &frame,
                                                const CoordinateType &coordinateType =
                                                CoordinateType::Cartesian) final;

    /**
     * Remove all motion states described in the specified coordinate reference frame and coordinate system
     */
    EXPORT_STEM virtual void removeMotionStates(const ReferenceFrame *pFrame,
                                                const CoordinateType &coordinateType =
                                                CoordinateType::Cartesian) final;

    /**
     * Remove all motion states tagged between the provided starting and ending times
     */
    EXPORT_STEM virtual void removeMotionStates(double startTime,
                                                double endTime) final;

    /**
     * Remove all motion states described in the specified coordinate reference frame and coordinate system
     * tagged between the provided starting and ending times; returns an iterator to the element that follows
     * the last element erased by this function
     */
    EXPORT_STEM virtual typename std::map<std::string, std::vector<MotionState *>>::iterator
    removeMotionStates(double startTime,
                       double endTime,
                       const std::string &frame,
                       const CoordinateType &coordinateType = CoordinateType::Cartesian) final;

    /**
     * Remove all motion states described in the specified coordinate reference frame and coordinate system
     * tagged between the provided starting and ending times; returns an iterator to the element that follows
     * the last element erased by this function
     */
    EXPORT_STEM virtual typename std::map<std::string, std::vector<MotionState *>>::iterator
    removeMotionStates(double startTime,
                       double endTime,
                       const ReferenceFrame *pFrame,
                       const CoordinateType &coordinateType = CoordinateType::Cartesian) final;

    /**
     * Remove all motion states from this container
     */
    EXPORT_STEM virtual void removeMotionStates(void) final;

    /**
     * Return the number of motion states contained within this object
     */
    EXPORT_STEM virtual std::size_t size(void) const final;

    /**
     * Return the number of motion states described in the specified coordinate reference frame and coordinate
     * system
     */
    EXPORT_STEM virtual std::size_t size(const std::string &frame,
                                         const CoordinateType &coordinateType =
                                         CoordinateType::Cartesian) const final;

    /**
     * Return the number of motion states described in the specified coordinate reference frame and coordinate
     * system
     */
    EXPORT_STEM virtual std::size_t size(const ReferenceFrame *pFrame,
                                         const CoordinateType &coordinateType =
                                         CoordinateType::Cartesian) const final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(MotionStateContainer &container) override final;

private:

    /**
     * this object's container of motion states
     */
    containers::IdentifierAndTimeSortedContainer<MotionState,
                                                 std::string,
                                                 double,
                                                 &MotionState::getFrameAndCoordinateSystem,
                                                 &MotionState::getTime> m_motionStates;
};

}

}

#endif
