#include "motionState.h"
#include "motionStateMap.h"
#include "referenceFrame.h"

// using namespace declarations
using namespace attributes::concrete;
using namespace utilities;

namespace physics
{

namespace kinematics
{

/**
 * Constructor
 */
MotionStateMap::MotionStateMap(void)
{

}

/**
 * Constructor
 * @param name         the name associated with the motion state
 * @param pMotionState an r-value reference pointer to a dynamically-allocated motion state object; upon
 *                     success, this object takes ownership of the motion state and is responsible for its
 *                     destruction
 */
MotionStateMap::MotionStateMap(const std::string &name,
                               MotionState *&&pMotionState)
{
    moveMotionState(name, std::move(pMotionState));
}

/**
 * Copy constructor
 */
MotionStateMap::MotionStateMap(const MotionStateMap &motionStateMap)
{
    operator = (motionStateMap);
}

/**
 * Move constructor
 */
MotionStateMap::MotionStateMap(MotionStateMap &&motionStateMap)
{
    operator = (std::move(motionStateMap));
}

/**
 * Destructor
 */
MotionStateMap::~MotionStateMap(void)
{
    // delete this object's associated motion states
    deleteMotionStates();
}

/**
 * Copy assignment operator
 */
MotionStateMap &MotionStateMap::operator = (const MotionStateMap &motionStateMap)
{
    if (&motionStateMap != this)
    {
        Loggable<std::string, std::ostream>::operator = (motionStateMap);

        // delete the current object's motion states
        deleteMotionStates();

        // clone the input argument's motion states and store in this object
        auto &&itMotionState = motionStateMap.m_motionStates.cbegin();
        while (itMotionState != motionStateMap.m_motionStates.cend())
        {
            auto *pMotionState = itMotionState->second;
            if (pMotionState != nullptr)
            {
                auto &&name = itMotionState->first;
                m_motionStates[name] = pMotionState->clone();
            }

            ++itMotionState;
        }
    }

    return *this;
}

/**
 * Move assignment operator
 */
MotionStateMap &MotionStateMap::operator = (MotionStateMap &&motionStateMap)
{
    if (&motionStateMap != this)
    {
        motionStateMap.swap(*this);
    }

    return *this;
}

/**
 * Assign a motion state to this object and associate it with the given name; returns true upon success
 * @param name         the name associated with the motion state
 * @param pMotionState a pointer to the source motion state
 */
bool MotionStateMap::assignMotionState(const std::string &name,
                                       MotionState *pMotionState)
{
    auto *pThisMotionState = getMotionState(name);
    bool bSuccess = (pMotionState != nullptr &&
                     pThisMotionState != nullptr);
    if (bSuccess)
    {

        bSuccess = pThisMotionState->assign(pMotionState);
    }

    return bSuccess;
}

/**
 * clone() function
 */
MotionStateMap *MotionStateMap::clone(void) const
{
    return new MotionStateMap(*this);
}

/**
 * Create a motion state in the given frame of reference and associate it with the specified name
 * @param pFrame         the frame of reference in which the motion state will be created
 * @param name           the name associated with the motion state
 * @param coordinateType the coordinate system in which the motion state will be described
 */
MotionState *MotionStateMap::createMotionState(ReferenceFrame *pFrame,
                                               const std::string &name,
                                               const CoordinateType &coordinateType)
{
    auto *pMotionState = getMotionState(name);
    if (pMotionState == nullptr || pMotionState->getCoordinateType() != coordinateType)
    {
        if (pMotionState != nullptr)
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "A motion state is already associated with the name \"" + name + "\", "
                   "but it is currently not described in the target coordinate system; "
                   "the current motion state will be deleted and a new one will be created.\n",
                   getQualifiedMethodName(__func__));

            // it's not described in the desired coordinate system
            deleteMotionState(pMotionState);
        }

        if (pFrame != nullptr)
        {
            pMotionState = pFrame->createMotionState(coordinateType);
            m_motionStates[name] = pMotionState;
        }
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "A motion state is already associated with the name \"" + name + "\".\n",
               getQualifiedMethodName(__func__));
    }

    return pMotionState;
}

/**
 * Delete a motion state by name; returns true upon success
 * @param name the name associated with the motion state
 */
bool MotionStateMap::deleteMotionState(const std::string &name)
{
    auto &&itNameMotionStatePair = m_motionStates.find(name);
    bool bSuccess = (itNameMotionStatePair != m_motionStates.end());
    if (bSuccess)
    {
        auto *pMotionState = itNameMotionStatePair->second;
        if (pMotionState != nullptr)
            delete pMotionState;

        m_motionStates.erase(itNameMotionStatePair);
    }

    return bSuccess;
}

/**
 * Delete a motion state by value; if found, function sets input argument to null
 * @param pMotionState a pointer to the motion state to be deleted from this object
 */
bool MotionStateMap::deleteMotionState(MotionState *&pMotionState)
{
    bool bFound = false;
    auto &&itMotionState = m_motionStates.begin();
    while (itMotionState != m_motionStates.end())
    {
        if (itMotionState->second == pMotionState)
        {
            m_motionStates.erase(itMotionState);
            bFound = true;
        }

        ++itMotionState;
    }

    if (bFound && pMotionState != nullptr)
    {
        delete pMotionState;
        pMotionState = nullptr;
    }

    return bFound;
}

/**
 * Delete all motion states from this object
 */
void MotionStateMap::deleteMotionStates(void)
{
    auto &&itNameMotionStatePair = m_motionStates.begin();
    while (itNameMotionStatePair != m_motionStates.end())
    {
        auto *pMotionState = itNameMotionStatePair->second;
        if (pMotionState != nullptr)
            delete pMotionState;

        ++itNameMotionStatePair;
    }

    m_motionStates.clear();
}

/**
 * Function to deserialize this object's data
 */
std::istream &MotionStateMap::deserialize(std::istream &stream)
{
    if (stream)
    {
        // read the size of the map
        std::size_t size = 0;
        stream.read((char *)&size, sizeof(std::size_t));

        // de-serialize the map of motion states from the stream
        bool bSuccess = true; // assume success
        for (std::size_t i = 0; bSuccess && i < size; ++i)
        {
            // read the name of the motion state
            std::string name;
            std::getline(stream, name, '\0');
            auto &&itNameMotionStatePair = m_motionStates.find(name);
            bSuccess = (itNameMotionStatePair != m_motionStates.end());
            if (bSuccess)
            {
                auto *pMotionState = itNameMotionStatePair->second;
                bSuccess = (pMotionState != nullptr);
                if (bSuccess)
                {
                    pMotionState->deserialize(stream);
                }
                else
                {
                    logMsg(std::cout, LoggingLevel::Enum::Error,
                           "De-serialization failed; "
                           "A null motion state was mapped to \"" + name + "\"\n",
                           getQualifiedMethodName(__func__));
                }
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "De-serialization failed; A motion state named \"" + name + "\" "
                       "does not exist in this object's map of motion states!\n",
                       getQualifiedMethodName(__func__));
            }

            if (!bSuccess)
                stream.setstate(std::istream::failbit);
        }
    }

    return stream;
}

/**
 * Get the name of this class
 */
std::string MotionStateMap::getClassName(void) const
{
    return "MotionStateMap";
}

/**
 * Retrieve a motion state by name; upon success, function returns non-null
 * @param name the name associated with the motion state
 */
MotionState *MotionStateMap::getMotionState(const std::string &name)
{
    MotionState *pMotionState = nullptr;
    auto &&itMotionState = m_motionStates.find(name);
    if (itMotionState != m_motionStates.end())
        pMotionState = itMotionState->second;

    return pMotionState;
}

/**
 * Get this object's map of motion states
 */
std::map<std::string, MotionState *> &MotionStateMap::getMotionStates(void)
{
    return m_motionStates;
}

/**
 * Determine if this object contains a motion state associated with the indicated name
 * @param name the name associated with the motion state
 */
bool MotionStateMap::hasMotionState(const std::string &name) const
{
    auto &&itMotionState = m_motionStates.find(name);

    return itMotionState != m_motionStates.cend() && itMotionState->second != nullptr;
}

/**
 * Determine if this object contains a key for the specified motion state
 * @param name the name associated with the motion state
 */
bool MotionStateMap::hasMotionState(const MotionState *pMotionState) const
{
    auto &&itMotionState = m_motionStates.cbegin();
    while (itMotionState != m_motionStates.cend())
    {
        if (itMotionState->second == pMotionState)
            return true;

        ++itMotionState;
    }

    return false;
}

/**
 * Initialization function
 */
bool MotionStateMap::initialize(void)
{
    purgeNullMotionStates();

    bool bSuccess = true; // assume success
    auto &&itMotionState = m_motionStates.begin();
    while (bSuccess && itMotionState != m_motionStates.end())
    {
        auto *pMotionState = itMotionState->second;
        bSuccess = (pMotionState != nullptr);
        if (bSuccess)
        {
            bSuccess = pMotionState->initialize();
        }

        ++itMotionState;
    }

    return bSuccess;
}

/**
 * Move a motion state into this object and associate it with the given name; upon success the current object
 * takes ownership of the input argument and becomes responsible for its eventual destruction; hence, the
 * function only accepts an r-value (temporary) reference to a dynamically-allocated motion state; function
 * returns true under the following conditions: 1) The motion state object argument is non-null 2) The motion
 * state object does not already exist in this object's map of motion states 3) The name associated with the
 * motion state is non-empty and is not already present in this object's map of motion states
 * @param name         the name associated with the motion state
 * @param pMotionState an r-value reference pointer to a dynamically-allocated motion state object; upon
 *                     success, this object takes ownership of the motion state and is responsible for its
 *                     destruction
 */
bool MotionStateMap::moveMotionState(const std::string &name,
                                     MotionState *&&pMotionState)
{
    bool bSuccess = (pMotionState != nullptr);
    if (bSuccess)
    {
        bSuccess = !name.empty() && !hasMotionState(name) && !hasMotionState(pMotionState);
        if (bSuccess)
        {
            m_motionStates.emplace(name, std::move(pMotionState));
            pMotionState = nullptr;
        }
    }

    return bSuccess;
}

/**
 * Output stream print function
 * @param stream a reference to an std::ostream object
 */
std::ostream &MotionStateMap::print(std::ostream &stream) const
{
    if (stream)
    {
        auto &&itMotionState = m_motionStates.cbegin();
        while (itMotionState != m_motionStates.cend())
        {
            auto &&name = itMotionState->first;
            auto *pMotionState = itMotionState->second;
            if (pMotionState != nullptr)
            {
                stream << "Motion state: \"" << name << "\"" << std::endl;
                pMotionState->print(stream);
                stream << std::endl << std::endl;
            }

            ++itMotionState;
        }
    }

    return stream;
}

/**
 * Purge null motion states from this object
 */
void MotionStateMap::purgeNullMotionStates(void)
{
    auto &&itNameMotionStatePair = m_motionStates.begin();
    while (itNameMotionStatePair != m_motionStates.end())
    {
        if (itNameMotionStatePair->second == nullptr)
            itNameMotionStatePair = m_motionStates.erase(itNameMotionStatePair);
        else
            ++itNameMotionStatePair;
    }
}

/**
 * Function to serialize this object's data
 */
std::ostream &MotionStateMap::serialize(std::ostream &stream) const
{
    if (stream)
    {
        // write the size of the map
        auto size = m_motionStates.size();
        stream.write((const char *)&size, sizeof(std::size_t));

        // serialize this object's map of motion states
        auto &&itNameMotionStatePair = m_motionStates.begin();
        while (itNameMotionStatePair != m_motionStates.end())
        {
            auto &&name = itNameMotionStatePair->first;
            auto *pMotionState = itNameMotionStatePair->second;
            if (pMotionState != nullptr)
            {
                // write the name of the motion state
                stream << name << '\0';

                // serialize the motion state
                pMotionState->serialize(stream);
            }

            ++itNameMotionStatePair;
        }
    }

    return stream;
}

/**
 * Swap function
 */
void MotionStateMap::swap(MotionStateMap &motionStateMap)
{
    Loggable<std::string, std::ostream>::swap(motionStateMap);

    std::swap(m_motionStates, motionStateMap.m_motionStates);
}

/**
 * Update function
 */
bool MotionStateMap::update(double time)
{
    // update this object's motion states
    auto &&itMotionState = m_motionStates.begin();
    while (itMotionState != m_motionStates.end())
    {
        auto *pMotionState = itMotionState->second;
        if (pMotionState != nullptr)
        {
            pMotionState->update(time);
        }

        ++itMotionState;
    }

    return true;
}

}

}
