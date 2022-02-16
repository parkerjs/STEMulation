#include "motionStateContainer.h"
#include "referenceFrame.h"

// using namespace declarations
using namespace containers;

namespace physics
{

namespace kinematics
{

/**
 * Constructor
 */
MotionStateContainer::MotionStateContainer(void)
{

}

/**
 * Copy constructor
 */
MotionStateContainer::MotionStateContainer(const MotionStateContainer &container)
{
    operator = (container);
}

/**
 * Move constructor
 */
MotionStateContainer::MotionStateContainer(MotionStateContainer &&container)
{
    operator = (std::move(container));
}

/**
 * Destructor
 */
MotionStateContainer::~MotionStateContainer(void)
{

}

/**
 * Copy assignment operator
 */
MotionStateContainer &MotionStateContainer::operator = (const MotionStateContainer &container)
{
    if (&container != this)
    {
        m_motionStates = container.m_motionStates;
    }

    return *this;
}

/**
 * Move assignment operator
 */
MotionStateContainer &MotionStateContainer::operator = (MotionStateContainer &&container)
{
    if (&container != this)
    {
        container.swap(*this);
    }

    return *this;
}

/**
 * Add a motion state to this container
 */
void MotionStateContainer::addMotionState(const MotionState &motionState)
{
    m_motionStates.addEntry(motionState);
}

/**
 * Add a motion state to this container; returns true if the entry is successfully added and the current object
 * takes ownership of the input argument and becomes responsible for its destruction
 */
bool MotionStateContainer::addMotionState(MotionState *pMotionState)
{
    return m_motionStates.addEntry(pMotionState);
}

/**
 * Delete a motion state by value from this container; sets input argument to null upon success
 */
bool MotionStateContainer::deleteMotionState(MotionState *&pMotionState)
{
    return m_motionStates.deleteEntry(pMotionState);
}

/**
 * Delete all motion states described in the specified coordinate reference frame and coordinate system;
 * returns true upon success
 */
void MotionStateContainer::deleteMotionStates(const std::string &frame,
                                              const CoordinateType &coordinateType)
{
    m_motionStates.deleteEntries(frame + "_" + coordinateType.toString());
}

/**
 * Delete all motion states described in the specified coordinate reference frame and coordinate system;
 * returns true upon success
 */
void MotionStateContainer::deleteMotionStates(const ReferenceFrame *pFrame,
                                              const CoordinateType &coordinateType)
{
    if (pFrame != nullptr)
    {
        deleteMotionStates(pFrame->getName(), coordinateType);
    }
}

/**
 * Delete all motion states tagged between the provided starting and ending times
 */
void MotionStateContainer::deleteMotionStates(double startTime,
                                              double endTime)
{
    m_motionStates.deleteEntries(startTime, endTime);
}

/**
 * Delete all motion states described in the specified coordinate reference frame and coordinate system tagged
 * between the provided starting and ending times; returns an iterator to the element that follows the last
 * element erased by this function
 */
typename std::map<std::string, std::vector<MotionState *>>::iterator
MotionStateContainer::deleteMotionStates(double startTime,
                                         double endTime,
                                         const std::string &frame,
                                         const CoordinateType &coordinateType)
{
    return m_motionStates.deleteEntries(frame + "_" + coordinateType.toString(), startTime, endTime);
}

/**
 * Delete all motion states described in the specified coordinate reference frame and coordinate system tagged
 * between the provided starting and ending times; returns an iterator to the element that follows the last
 * element erased by this function
 */
typename std::map<std::string, std::vector<MotionState *>>::iterator
MotionStateContainer::deleteMotionStates(double startTime,
                                         double endTime,
                                         const ReferenceFrame *pFrame,
                                         const CoordinateType &coordinateType)
{
    if (pFrame != nullptr)
    {
        return deleteMotionStates(startTime, endTime, pFrame->getName(), coordinateType);
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Input reference frame object is null!");
}

/**
 * Delete motion states by value from this container
 */
bool MotionStateContainer::deleteMotionStates(std::vector<MotionState *> &motionStates)
{
    return m_motionStates.deleteEntries(motionStates);
}

/**
 * Delete all motion states from this container
 */
void MotionStateContainer::deleteMotionStates(void)
{
    m_motionStates.deleteEntries();
}

/**
 * Test for empty container
 */
bool MotionStateContainer::empty(void) const
{
    return m_motionStates.empty();
}

/**
 * Get the name of this class
 */
std::string MotionStateContainer::getClassName(void) const
{
    return "MotionStateContainer";
}

/**
 * Retrieve the latest motion state associated with the specified coordinate reference frame and coordinate
 * system; returns non-null upon success
 */
MotionState *MotionStateContainer::getLatestMotionState(const std::string &frame,
                                                        const CoordinateType &coordinateType) const
{
    return m_motionStates.getLatestEntry(frame + "_" + coordinateType.toString());
}

/**
 * Retrieve the latest motion state associated with the specified coordinate reference frame and coordinate
 * system; returns non-null upon success
 */
MotionState *MotionStateContainer::getLatestMotionState(const ReferenceFrame *pFrame,
                                                        const CoordinateType &coordinateType) const
{
    MotionState *pMotionState = nullptr;
    if (pFrame != nullptr)
    {
        pMotionState = getLatestMotionState(pFrame->getName(), coordinateType);
    }

    return pMotionState;
}

/**
 * Retrieve the latest motion states for all coordinate reference frames and coordinate systems; returns true
 * upon success and entries are appended to input argument
 */
void MotionStateContainer::getLatestMotionStates(std::vector<MotionState *> &motionStates) const
{
    m_motionStates.getLatestEntries(motionStates);
}

/**
 * Retrieve the most recently available motion state associated with the specified coordinate reference frame
 * and coordinate system; returns non-null upon success
 */
MotionState *
MotionStateContainer::getMostRecentAvailableMotionState(double time,
                                                        const std::string &frame,
                                                        const CoordinateType &coordinateType) const
{
    return m_motionStates.getMostRecentAvailableEntry(frame + "_" + coordinateType.toString(), time);
}

/**
 * Retrieve the most recently available motion state associated with the specified coordinate reference frame
 * and coordinate system; returns non-null upon success
 */
MotionState *
MotionStateContainer::getMostRecentAvailableMotionState(double time,
                                                        const ReferenceFrame *pFrame,
                                                        const CoordinateType &coordinateType) const
{
    MotionState *pMotionState = nullptr;
    if (pFrame != nullptr)
    {
        pMotionState = getMostRecentAvailableMotionState(time, pFrame->getName(), coordinateType);
    }

    return pMotionState;
}

/**
 * Retrieve the most recently available motion states described in the specified coordinate reference frame and
 * coordinate system; matching entries are appended to input argument
 */
void MotionStateContainer::getMostRecentAvailableMotionStates(double time,
                                                              std::vector<MotionState *> &motionStates) const
{
    m_motionStates.getMostRecentAvailableEntries(time, motionStates);
}

/**
 * Retrieve a motion state described in the specified coordinate reference frame and coordinate system tagged
 * at the provided time; returns non-null upon success
 */
MotionState *MotionStateContainer::getMotionState(double time,
                                                  const std::string &frame,
                                                  const CoordinateType &coordinateType) const
{
    return m_motionStates.getEntry(frame + "_" + coordinateType.toString(), time);
}

/**
 * Retrieve a motion state described in the specified coordinate reference frame and coordinate system tagged
 * at the provided time; returns non-null upon success
 */
MotionState *MotionStateContainer::getMotionState(double time,
                                                  const ReferenceFrame *pFrame,
                                                  const CoordinateType &coordinateType) const
{
    MotionState *pMotionState = nullptr;
    if (pFrame != nullptr)
    {
        pMotionState = getMotionState(time, pFrame->getName(), coordinateType);
    }

    return pMotionState;
}

/**
 * Get this object's motion state container
 */
IdentifierAndTimeSortedContainer<MotionState,
                                 std::string,
                                 double,
                                 &MotionState::getFrameAndCoordinateSystem,
                                 &MotionState::getTime> &
MotionStateContainer::getMotionStateContainer(void)
{
    return m_motionStates;
}

/**
 * Get this object's motion state container
 */
IdentifierAndTimeSortedContainer<MotionState,
                                 std::string,
                                 double,
                                 &MotionState::getFrameAndCoordinateSystem,
                                 &MotionState::getTime>
MotionStateContainer::getMotionStateContainer(void) const
{
    return m_motionStates;
}

/**
 * Retrieve all motion states described in the specified coordinate reference frame and coordinate system; upon
 * success, entries are appended to input argument
 */
void MotionStateContainer::getMotionStates(std::vector<MotionState *> &motionStates,
                                           const std::string &frame,
                                           const CoordinateType &coordinateType) const
{
    m_motionStates.getEntries(frame + "_" + coordinateType.toString(), motionStates);
}

/**
 * Retrieve all motion states described in the specified coordinate reference frame and coordinate system; upon
 * success, entries are appended to input argument
 */
void MotionStateContainer::getMotionStates(std::vector<MotionState *> &motionStates,
                                           const ReferenceFrame *pFrame,
                                           const CoordinateType &coordinateType) const
{
    if (pFrame != nullptr)
    {
        getMotionStates(motionStates, pFrame->getName(), coordinateType);
    }
}

/**
 * Retrieve all motion states tagged between the provided starting and ending times; upon success, entries are
 * appended to input argument
 */
void MotionStateContainer::getMotionStates(double startTime,
                                           double endTime,
                                           std::vector<MotionState *> &motionStates) const
{
    m_motionStates.getEntries(startTime, endTime, motionStates);
}

/**
 * Retrieve all motion states described in the specified coordinate reference frame and coordinate system
 * tagged between the provided starting and ending times; returns true upon success and entries are appended to
 * input argument
 */
bool MotionStateContainer::getMotionStates(std::vector<MotionState *> &motionStates,
                                           double startTime,
                                           double endTime,
                                           const std::string &frame,
                                           const CoordinateType &coordinateType) const
{
    return m_motionStates.getEntries(frame + "_" + coordinateType.toString(),
                                     startTime,
                                     endTime,
                                     motionStates);
}

/**
 * Retrieve all motion states described in the specified coordinate reference frame and coordinate system
 * tagged between the provided starting and ending times; returns true upon successs and entries are appended
 * to input argument
 */
bool MotionStateContainer::getMotionStates(std::vector<MotionState *> &motionStates,
                                           double startTime,
                                           double endTime,
                                           const ReferenceFrame *pFrame,
                                           const CoordinateType &coordinateType) const
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        bSuccess = getMotionStates(motionStates,
                                   startTime,
                                   endTime,
                                   pFrame->getName(),
                                   coordinateType);
    }

    return bSuccess;
}

/**
 * Initialization function
 */
bool MotionStateContainer::initialize(void)
{
    return m_motionStates.initialize();
}

/**
 * Remove a motion state by value from this container
 */
bool MotionStateContainer::removeMotionState(MotionState *pMotionState)
{
    return m_motionStates.removeEntry(pMotionState);
}

/**
 * Remove all motion states described in the specified coordinate reference frame and coordinate system
 */
void MotionStateContainer::removeMotionStates(const std::string &frame,
                                              const CoordinateType &coordinateType)
{
    m_motionStates.removeEntries(frame + "_" + coordinateType.toString());
}

/**
 * Remove all motion states described in the specified coordinate reference frame and coordinate system
 */
void MotionStateContainer::removeMotionStates(const ReferenceFrame *pFrame,
                                              const CoordinateType &coordinateType)
{
    if (pFrame != nullptr)
    {
        removeMotionStates(pFrame->getName(), coordinateType);
    }
}

/**
 * Remove all motion states tagged between the provided starting and ending times
 */
void MotionStateContainer::removeMotionStates(double startTime,
                                              double endTime)
{
    m_motionStates.removeEntries(startTime, endTime);
}

/**
 * Remove all motion states described in the specified coordinate reference frame and coordinate system tagged
 * between the provided starting and ending times; returns an iterator to the element that follows the last
 * element erased by this function
 */
typename std::map<std::string, std::vector<MotionState *>>::iterator
MotionStateContainer::removeMotionStates(double startTime,
                                         double endTime,
                                         const std::string &frame,
                                         const CoordinateType &coordinateType)
{
    return m_motionStates.removeEntries(frame + "_" + coordinateType.toString(), startTime, endTime);
}

/**
 * Remove all motion states described in the specified coordinate reference frame and coordinate system tagged
 * between the provided starting and ending times; returns an iterator to the element that follows the last
 * element erased by this function
 */
typename std::map<std::string, std::vector<MotionState *>>::iterator
MotionStateContainer::removeMotionStates(double startTime,
                                         double endTime,
                                         const ReferenceFrame *pFrame,
                                         const CoordinateType &coordinateType)
{
    if (pFrame != nullptr)
    {
        return removeMotionStates(startTime, endTime, pFrame->getName(), coordinateType);
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Input reference frame object is null!");
}

/**
 * Remove all motion states from this container
 */
void MotionStateContainer::removeMotionStates(void)
{
    m_motionStates.removeEntries();
}

/**
 * Return the number of motion states contained within this object
 */
std::size_t MotionStateContainer::size(void) const
{
    return m_motionStates.size();
}

/**
 * Return the number of motion states described in the specified coordinate reference frame and coordinate
 * system
 */
std::size_t MotionStateContainer::size(const std::string &frame,
                                       const CoordinateType &coordinateType) const
{
    return m_motionStates.size(frame + "_" + coordinateType.toString());
}

/**
 * Return the number of motion states described in the specified coordinate reference frame and coordinate
 * system
 */
std::size_t MotionStateContainer::size(const ReferenceFrame *pFrame,
                                       const CoordinateType &coordinateType) const
{
    if (pFrame != nullptr)
    {
        return size(pFrame->getName(), coordinateType);
    }

    return 0;
}

/**
 * Swap function
 */
void MotionStateContainer::swap(MotionStateContainer &container)
{
    m_motionStates.swap(container.m_motionStates);
}

}

}
