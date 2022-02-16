#include "acceleration_axis_type.h"
#include "angle_unit_type.h"
#include "cartesianMotionState.h"
#include "euler_state_derivative_type.h"
#include "eulers.h"
#include "frameState.h"
#include "garbage_collector.h"
#include "kinematicState.h"
#include "position_axis_type.h"
#include "projectedFrameState.h"
#include "quat.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "referenceFrame.h"
#include "rotation_type.h"
#include "state_derivative_type.h"
#include "trigonometry.h"
#include "vector3d.h"
#include "velocity_axis_type.h"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <stack>
#include <string>
#include <typeinfo>
#include <vector>

// using namespace declarations
using namespace math::geometric::orientation;
using namespace math::linear_algebra::vector;
using namespace math::number_systems::complex;
using namespace math::trigonometric;
using namespace memory;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace utilities;

namespace physics
{

namespace kinematics
{

// file-scoped declarations
static GarbageCollector<ReferenceFrame> garbageCollector;

/**
 * Constructor
 */
ReferenceFrame::ReferenceFrame(void)
: m_frameStateType("ProjectedFrameState"),
  m_pParentFrame(nullptr)
{
    garbageCollector.addObject(this);
}

/**
 * Construct a ReferenceFrame defined with respect to the specified parent frame
 * @param pParentFrame a pointer to the parent frame
 */
ReferenceFrame::ReferenceFrame(ReferenceFrame *pParentFrame)
: m_pParentFrame(nullptr)
{
    garbageCollector.addObject(this);

//  if (pParentFrame != nullptr)
//      m_frameStateType = pParentFrame->getFrameStateType();

    setParent(pParentFrame);

    // always create the default frame state
    createFrameState(DEFAULT_FRAME_STATE);
}

/**
 * Construct a ReferenceFrame defined with respect to the specified parent frame
 * @param pParentFrame a pointer to the parent frame
 * @param state        the perturbation state of the frame state to be created
 */
ReferenceFrame::ReferenceFrame(ReferenceFrame *pParentFrame,
                               const std::string &state)
: m_pParentFrame(nullptr)
{
    garbageCollector.addObject(this);

//  if (pParentFrame != nullptr)
//      m_frameStateType = pParentFrame->getFrameStateType();

    setParent(pParentFrame);

    // always create the default frame state
    createFrameState(DEFAULT_FRAME_STATE);

    if (std::strcmp(state.c_str(), DEFAULT_FRAME_STATE) != 0)
    {
        // a frame state other than the default was specified
        createFrameState(state);
    }
}

/**
 * Construct a ReferenceFrame defined with respect to the specified parent frame
 * @param pParentFrame   a pointer to the parent frame
 * @param frameStateType the type of frame state to be instantiated
 * @param state          the perturbation state of the frame state to be created
 */
ReferenceFrame::ReferenceFrame(ReferenceFrame *pParentFrame,
                               FrameStateType &&frameStateType,
                               const std::string &state)
: m_frameStateType(frameStateType),
  m_pParentFrame(nullptr)
{
    garbageCollector.addObject(this);

    setParent(pParentFrame);

    // always create the default frame state
    createFrameState(DEFAULT_FRAME_STATE);

    if (std::strcmp(state.c_str(), DEFAULT_FRAME_STATE) != 0)
    {
        // a frame state other than the default was specified
        createFrameState(state);
    }
}

/**
 * Copy constructor
 */
ReferenceFrame::ReferenceFrame(const ReferenceFrame &frame)
: m_pParentFrame(nullptr)
{
    garbageCollector.addObject(this);

    operator = (frame);
}

/**
 * Move constructor
 */
ReferenceFrame::ReferenceFrame(ReferenceFrame &&frame)
: m_pParentFrame(nullptr)
{
    garbageCollector.releaseObject(&frame);
    garbageCollector.addObject(this);

    operator = (std::move(frame));
}

/**
 * Destructor
 */
ReferenceFrame::~ReferenceFrame(void)
{
    // all the motion states described within this frame of reference must be transformed to the parent frame
    // when the current object is deleted
    auto *pWorldFrame = ReferenceFrame::getWorldFrame();
    if (this != pWorldFrame)
    {
        auto motionStates = m_motionStates;
        auto &&itMotionState = motionStates.cbegin();
        while (itMotionState != motionStates.cend())
        {
            auto *pMotionState = m_motionStates.front();
            if (pMotionState != nullptr)
            {
                if (m_pParentFrame != nullptr)
                    pMotionState->transformToFrame(m_pParentFrame);
                else if (pMotionState->getFrame() != pWorldFrame)
                    pMotionState->setFrame(pWorldFrame);
            }

            ++itMotionState;
        }
    }

    // remove the frame states
    deleteFrameStates();

    // make all child frames point to the parent frame
    while (!m_children.empty())
    {
        auto *pChildFrame = m_children.front();
        if (pChildFrame != nullptr)
        {
            pChildFrame->setParent(m_pParentFrame);
        }
    }

    if (m_pParentFrame != nullptr)
        m_pParentFrame->removeChild(this);

    garbageCollector.releaseObject(this);
}

/**
 * Copy assignment operator
 */
ReferenceFrame &ReferenceFrame::operator = (const ReferenceFrame &frame)
{
    if (&frame != this)
    {
        Loggable<std::string, std::ostream>::operator = (frame);

        // don't copy motion states
//      m_motionStates = frame.m_motionStates;

        copyTree(&frame);
    }

    return *this;
}

/**
 * Move assignment operator
 */
ReferenceFrame &ReferenceFrame::operator = (ReferenceFrame &&frame)
{
    if (&frame != this)
    {
        frame.swap(*this);
    }

    return *this;
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const ReferenceFrame &frame)
{
    return frame.print(stream);
}

/**
 * Add a reference frame as being a child of this frame
 */
bool ReferenceFrame::addChild(ReferenceFrame *pChildFrame)
{
    bool bSuccess = (pChildFrame != nullptr);
    if (bSuccess)
    {
        auto &&itChild = std::find(m_children.cbegin(), m_children.cend(), pChildFrame);
        bSuccess = (itChild == m_children.cend());
        if (bSuccess)
        {
            auto *pParentFrame = pChildFrame->getParent();
            if (pParentFrame != nullptr)
                pParentFrame->removeChild(pChildFrame);

            m_children.push_back(pChildFrame);
            pChildFrame->m_pParentFrame = this;
        }
    }

    return bSuccess;
}

/**
 * Add a motion state to this frame of reference; returns true upon success
 */
bool ReferenceFrame::addMotionState(MotionState *pMotionState)
{
    bool bSuccess = (pMotionState != nullptr);
    if (bSuccess)
    {
        auto *pFrame = pMotionState->getFrame();
        if (pFrame != nullptr && pFrame != this)
        {
            pFrame->removeMotionState(pMotionState);
        }

        auto &&itMotionState = std::find(m_motionStates.cbegin(),
                                         m_motionStates.cend(),
                                         pMotionState);
        if (itMotionState == m_motionStates.cend())
            m_motionStates.push_back(pMotionState);
    }

    return bSuccess;
}

/**
 * Function to find a previously computed transformation from the motion state's transformation cache (if it
 * exists) and apply it to the motion state's current kinematic state
 */
bool ReferenceFrame::applyCachedTransformation(MotionState *pMotionState) const
{
    bool bCachedTransformationExists = false;
    if (pMotionState != nullptr)
    {
        auto *pMotionStateFrame = pMotionState->getFrame();
        if (pMotionState->cacheTransformationsEnabled() && pMotionStateFrame != nullptr)
        {
            auto &&cache = pMotionState->getTransformationCache();

            // find the source frame state/kinematic pair in the cache
            auto &&sourceFrameName = pMotionStateFrame->getName();
            auto &&itSourceFrameTransformPair = cache.find(sourceFrameName);
            if (itSourceFrameTransformPair == cache.end())
                return false;

            // find the destination frame state/kinematic pair in the cache
            auto &&destFrameName = m_name;
            auto &&itDestFrameTransformPair = cache.find(destFrameName);
            if (itDestFrameTransformPair == cache.end())
                return false;

            // find the motion state object's frame state in the destination object's map of frame states
            auto &&frameStateName = pMotionState->getFrameState();
            auto &&itDestFrameState = m_frameStates.find(frameStateName);
            if (itDestFrameState == m_frameStates.end())
                return false;

            // find the motion state object's frame state in the source object's map of frame states
            auto &&sourceFrameStates = pMotionStateFrame->getFrameStates();
            auto &&itSourceFrameState = sourceFrameStates.find(frameStateName);
            if (itSourceFrameState == sourceFrameStates.end())
                return false;

            // the motion state's current kinematic state must match that which is stored in the cache
            auto &&pSourceKinematicState = pMotionState->getKinematicState();
            auto &&pCachedSourceKinematicState = itSourceFrameTransformPair->second.second;
            if (!pSourceKinematicState->isSpatiallyEqual(*pCachedSourceKinematicState, 1.0e-8))
                return false;

            // this object's current frame state must match that which is stored in the cache
            auto *pDestFrameState = itDestFrameState->second;
            auto &&pCahcedDestFrameState = itDestFrameTransformPair->second.first;
            if (pDestFrameState == nullptr ||
                !pDestFrameState->isSpatiallyEqual(*pCahcedDestFrameState, 1.0e-8))
                return false;

            auto *pSourceFrameState = itSourceFrameState->second;
            auto &&pCachedSourceFrameState = itSourceFrameTransformPair->second.first;
            if (pSourceFrameState == nullptr ||
                !pSourceFrameState->isSpatiallyEqual(*pCachedSourceFrameState, 1.0e-8))
                return false;

            auto &&pDestKinematicState = itDestFrameTransformPair->second.second;
            pMotionState->setKinematicState(pDestKinematicState->clone());
            pMotionState->setFrame(const_cast<ReferenceFrame *>(this));
            bCachedTransformationExists = true;
        }
    }

    return bCachedTransformationExists;
}

/**
 * Calculate this object's instantaneous angular acceleration vector at the specified time with respect to the
 * parent frame given this object's orientation angles, rotational rates and rotational acceleration. The
 * resultant angular acceleration vector is described in the current object's frame of reference
 * @param t     the time to which the frames and motion states will be projected
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::calcAngularAcceleration(double t,
                                                 const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
    {
        auto &&orientation = pFrameState->getOrientation(t);
        auto &&rotationalAccelerations = pFrameState->getRotationalAccelerations();
        auto &&rotationalRates = pFrameState->getRotationalRates(t);

        return orientation.calcBodyAccelerations(rotationalRates, rotationalAccelerations);
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Calculate this object's instantaneous angular velocity vector at the specified time with respect to the
 * parent frame given this object's orientation angles and rotational rates. The resultant angular velocity
 * vector is described in the current object's frame of reference
 * @param t     the time to which the frames and motion states will be projected
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::calcAngularVelocity(double t,
                                             const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
    {
        auto &&orientation = pFrameState->getOrientation(t);
        auto &&rotationalRates = pFrameState->getRotationalRates(t);

        return orientation.calcBodyRates(rotationalRates);
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Function to clone a branch starting with the current object and ending with the specified input descendant
 * frame; in order for the clone to succeed, the current object must be an ancestor frame of the input
 * descendant frame (the function tests the descendant frame by name or pointer value to make the
 * determination). Upon success, the function returns a non-null pointer to the root of the newly cloned branch
 * @param pDescendantFrame the descendant frame which marks the end of the branch
 */
ReferenceFrame *ReferenceFrame::cloneBranch(ReferenceFrame *pDescendantFrame) const
{
    ReferenceFrame *pBranch = nullptr;
    pDescendantFrame = findBranch(pDescendantFrame);
    if (pDescendantFrame != nullptr && pDescendantFrame->isDescendant(this))
    {
        auto *pParentFrame = getParent();
        while (pDescendantFrame != pParentFrame)
        {
            auto *pBranchParent = pDescendantFrame->cloneFrame();
            if (pBranch != nullptr)
                pBranch->setParent(pBranchParent);

            pBranch = pBranchParent;
            pDescendantFrame = pDescendantFrame->getParent();
        }
    }

    return pBranch;
}

/**
 * Function to clone just the current frame (not the child or parent relationships)
 */
ReferenceFrame *ReferenceFrame::cloneFrame(void) const
{
    auto *pFrame = new ReferenceFrame();
    pFrame->copyFrame(this);

    return pFrame;
}

/**
 * Function to create a shared clone of the branch starting with the current object and ending with the
 * specified input descendant frame; in order for the clone to succeed, the current object must be an ancestor
 * frame of the input descendant frame (the function tests the descendant frame by name or pointer value to
 * make the determination). Upon success, the function returns a non-null pointer to the root of the newly
 * cloned branch
 * @param pDescendantFrame the descendant frame which marks the end of the branch
 */
std::shared_ptr<ReferenceFrame>
ReferenceFrame::cloneSharedBranch(ReferenceFrame *pDescendantFrame) const
{
    return std::shared_ptr<ReferenceFrame>(cloneBranch(pDescendantFrame));
}

/**
 * Function to created a shared clone of the current frame (not the child or parent relationships)
 */
std::shared_ptr<ReferenceFrame> ReferenceFrame::cloneSharedFrame(void) const
{
    return std::shared_ptr<ReferenceFrame>(cloneFrame());
}

/**
 * Function to create a shared clone of the current frame and its entire subtree
 */
std::shared_ptr<ReferenceFrame> ReferenceFrame::cloneSharedTree(void) const
{
    return std::shared_ptr<ReferenceFrame>(cloneTree());
}

/**
 * Function to clone the current frame and its entire subtree
 */
ReferenceFrame *ReferenceFrame::cloneTree(void) const
{
    ReferenceFrame *pClonedFrame = nullptr;
    ReferenceFrame *pClonedParent = nullptr;
    const auto *pFrame = this;
    const auto *pParentFrame = pFrame->getParent();
    do
    {
        if (pFrame == nullptr)
        {
            pClonedFrame = pClonedParent;
            pClonedParent = pClonedParent->getParent();
            pFrame = pParentFrame;
            pParentFrame = pParentFrame->getParent();
            if (pFrame == this || pFrame == nullptr)
                break;
            else
                pFrame = pFrame->getRightSibling();
        }
        else
        {
            pClonedFrame = pFrame->cloneFrame();
            pClonedFrame->setParent(pClonedParent);
            pClonedParent = pClonedFrame;
            pParentFrame = pFrame;
            pFrame = pFrame->getChild();
        }
    }
    while (pFrame != pParentFrame);

    return pClonedFrame;
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void ReferenceFrame::convertAngleUnits(const AngleUnitType &angleUnits)
{
    auto &&itFrameState = m_frameStates.cbegin();
    while (itFrameState != m_frameStates.cend())
    {
        itFrameState->second->convertAngleUnits(angleUnits);
        ++itFrameState;
    }
}

/**
 * Function to copy the input frame to the current object (not the child or parent relationships); returns true
 * upon success
 */
bool ReferenceFrame::copyFrame(const ReferenceFrame *pFrame)
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        copyFrameStates(pFrame);

//      m_frameStateType = pFrame->m_frameStateType;
        m_name = pFrame->m_name;
    }

    return bSuccess;
}

/**
 * Function to copy the map of frame states from the input reference frame object to current object; returns
 * true upon success
 */
bool ReferenceFrame::copyFrameStates(const ReferenceFrame *pFrame)
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        copyFrameStates(pFrame->m_frameStates);
    }

    return bSuccess;
}

/**
 * Function to copy the input map of frame states to current object; returns true upon success
 */
void ReferenceFrame::copyFrameStates(const tFrameStates &frameStates)
{
    // eliminate frame states in the current object that are not present in the input object
    auto &&itFrameState = m_frameStates.begin();
    while (itFrameState != m_frameStates.end())
    {
        auto &&state = itFrameState->first;
        auto &&itSourceFrameState = frameStates.find(state);
        if (itSourceFrameState == frameStates.end())
        {
            // this existing destination frame state is not present in the source, eliminate it
            auto *pFrameState = itFrameState->second;
            itFrameState = m_frameStates.erase(itFrameState);
            if (pFrameState != nullptr)
                delete pFrameState;
        }
        else
            ++itFrameState;
    }

    // now copy the source frame state contents to the destination
    auto &&itSourceFrameState = frameStates.begin();
    while (itSourceFrameState != frameStates.end())
    {
        auto &&state = itSourceFrameState->first;
        auto *pSourceFrameState = itSourceFrameState->second;
        auto *pFrameState = createFrameState(state);
        if (pFrameState != nullptr && pSourceFrameState != nullptr)
            pFrameState->assign(pSourceFrameState);

        ++itSourceFrameState;
    }
}

/**
 * Function to copy the input frame and its subtree to the current object; returns true upon success
 */
bool ReferenceFrame::copyTree(const ReferenceFrame *pFrame)
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        auto *pDestFrame = this;
        auto *pParentFrame = pFrame->getParent();
        const auto *pChildFrame = pFrame->getChild();
        do
        {
            if (pChildFrame == nullptr)
            {
                pDestFrame->copyFrame(pFrame);

                auto numChildren = pFrame->m_children.size();
                auto &&destChildren = pDestFrame->m_children;
                while (destChildren.size() > numChildren)
                {
                    auto index = destChildren.size() - 1;
                    auto *pDestChild = pDestFrame->getChild(index);
                    pDestFrame->deleteChild(pDestChild);
                }

                pDestFrame = pDestFrame->getParent();
                pChildFrame = pFrame;
                pFrame = pFrame->getParent();
                pChildFrame = pChildFrame->getRightSibling();
            }
            else
            {
                auto index = pChildFrame->getChildIndex();
                auto *pDestFrameChild = pDestFrame->getChild(index);
                if (pDestFrameChild == nullptr)
                    pDestFrame = pDestFrame->createChild(pFrame->m_name);
                else
                    pDestFrame = pDestFrameChild;

                pFrame = pChildFrame;
                pChildFrame = pChildFrame->getChild();
            }
        }
        while (pFrame != pParentFrame);
    }

    return bSuccess;
}

/**
 * Create a root "World" reference frame
 * @param name  the name of the newly created frame
 * @param state the perturbation state of the frame state to be created
 */
ReferenceFrame *ReferenceFrame::create(const std::string &name,
                                       const std::string &state)
{
    auto *pFrame = new ReferenceFrame(nullptr, state);
    if (pFrame != nullptr && !name.empty())
        pFrame->setName(name);

    return pFrame;
}

/**
 * Create a root "World" reference frame
 * @param frameStateType the type of frame state to be instantiated
 * @param name           the name of the newly created frame
 * @param state          the desired perturbation state of the reference frame
 */
ReferenceFrame *ReferenceFrame::create(FrameStateType &&frameStateType,
                                       const std::string &name,
                                       const std::string &state)
{
    auto *pFrame = new ReferenceFrame(nullptr,
                                      std::move(frameStateType),
                                      state);
    if (pFrame != nullptr && !name.empty())
        pFrame->setName(name);

    return pFrame;
}

/**
 * Create a child frame with the current object's frame as its parent
 * @param name  the name of the newly created frame
 * @param state the perturbation state of the frame state to be created
 */
ReferenceFrame *ReferenceFrame::createChild(const std::string &name,
                                            const std::string &state)
{
    ReferenceFrame *pFrame = nullptr;
    if (!name.empty())
    {
        pFrame = findChild(name);
        if (pFrame == nullptr)
        {
            pFrame = new ReferenceFrame(this, state);
            pFrame->setName(name);
        }

        if (pFrame != nullptr)
        {
            auto *pFrameState = pFrame->createFrameState(state);
            auto *pThisFrameState = getFrameState(state);
            if (pFrameState != nullptr && pThisFrameState != nullptr)
                pFrameState->setTime(pThisFrameState->getTime());
        }
    }
    else
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Child frame must have a non-empty name.\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return pFrame;
}

/**
 * Create a frame state (in addition to default frame state); if the frame state already exists for the given
 * name, the function will return a pointer to that state
 * @param state the perturbation state of the frame state to be created
 */
FrameState *const ReferenceFrame::createFrameState(const std::string &state) const
{
    auto *pFrameState = m_frameStates[state];
    if (pFrameState == nullptr)
    {
        auto frameStateType = m_frameStateType;
        auto *pParentFrame = m_pParentFrame;
        while (pParentFrame != nullptr && frameStateType == FrameStateType::Unknown)
        {
            frameStateType = pParentFrame->getFrameStateType();
            pParentFrame = pParentFrame->getParent();
        }

        if (frameStateType != FrameStateType::Enum::Unknown)
        {
            pFrameState = FrameState::create(frameStateType,
                                             state);
            if (pFrameState == nullptr)
            {
                StaticSynchronizable<MotionState>::lock(0);
                logMsg(std::cout, LoggingLevel::Enum::Warning,
                       "A frame state of type \"" + std::string(frameStateType) +
                       "\"; the default frame state type will be instantiated.\n",
                       getQualifiedMethodName(__func__));
                StaticSynchronizable<MotionState>::unlock(0);
            }
        }

        if (pFrameState == nullptr)
            pFrameState = ProjectedFrameState::create(state);

        if (pFrameState != nullptr)
            m_frameStates[state] = pFrameState;
    }

    return pFrameState;
}
#ifdef RAPID_XML
/**
 * Create a ReferenceFrame tree from the specified XML element; upon success, function will return non-null
 */
ReferenceFrame *ReferenceFrame::createFromXML(xml_node<> *pNode)
{
    auto *pFrame = ReferenceFrame::create("");
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
        bSuccess = pFrame->readFromXML(pNode);

    if (!bSuccess)
        deleteFrame(pFrame);

    return pFrame;
}
#endif
/**
 * Function to create a new reference frame by merging both reference frame input objects with all frames along
 * the path between them. The resultant frame is a child of the least common ancestor of both frames and all
 * frames merged during the process will be projected to the time at which the common ancestor is defined. For
 * example, the tree diagram below shows the relationship before and after frames B and G are merged, where the
 * transformations imposed by frames B, E, and G have been combined into the newly created frame BG. Singleton
 * frames left behind as a result of the merge, as demonstrated by frame (G) in the diagram below, are removed
 * contingent upon the flag bRemoveSingletons being true. If singletons are removed from the resultant tree,
 * care must be taken to update external references to the frames that no longer exist. In order for the merge
 * to succeed the common ancestor frame must be non-null, both frames must reside within the same branch, and
 * most importantly all frames to be merged must be non-rotating. Upon success, the function returns a non-null
 * reference frame.
 * @param pStartFrame       the start of the chain
 * @param pEndFrame         the end of the chain
 * @param name              the name of the newly merged frame
 * @param bRemoveSingletons a flag that indicates whether or not singletons frames resulting from the merge
 *                          will be deleted
   \verbatim
        Before:               After:
          A                     A -------
         / \                   / \       \
        B   C    -------->    B   C      BG
       / \   \               / \   \     /\
      D   E   F             D   E   F   I  J
         / \                   / \
        G   H                (G)  H
       / \
      I   J
   \endverbatim
 */
ReferenceFrame *ReferenceFrame::createMergedFrame(ReferenceFrame *pStartFrame,
                                                  ReferenceFrame *pEndFrame,
                                                  const std::string &name,
                                                  bool bRemoveSingletons)
{
    ReferenceFrame *pMergedFrame = nullptr;
    auto *pCommonAncestorFrame = pStartFrame->determineCommonAncestor(pEndFrame);
    bool bSuccess = (pStartFrame != nullptr && pEndFrame != nullptr && pCommonAncestorFrame != nullptr);
    if (bSuccess)
    {
        bool bStartFrameIsAncestor = pEndFrame->isAncestor(pStartFrame);
        bool bStartFrameIsDescendant = pEndFrame->isDescendant(pStartFrame);
        bSuccess = (bStartFrameIsAncestor != bStartFrameIsDescendant); // exclusive "or"
        if (bSuccess)
        {
            // need to verify that all frame states in the chain are non-rotating
            std::set<std::string> states;
            auto *pDescendantFrame = (bStartFrameIsDescendant ? pEndFrame : pStartFrame);
            auto *pFrame = pDescendantFrame;
            while (bSuccess && pFrame != pCommonAncestorFrame)
            {
                auto &&itFrameState = pFrame->m_frameStates.cbegin();
                while (bSuccess && itFrameState != pFrame->m_frameStates.cend())
                {
                    auto &&state = itFrameState->first;
                    bSuccess = !pFrame->isRotating(state);
                    if (bSuccess)
                        states.insert(state);

                    ++itFrameState;
                }

                pFrame = pFrame->getParent();
            }

            if (!bSuccess)
            {
                StaticSynchronizable<MotionState>::lock(0);
                logMsg(std::cout, LoggingLevel::Enum::Warning,
                       "Merge failed, rotating frame \"" + pFrame->getName() + "\" cannot be combined.\n",
                       getQualifiedStaticMethodName(__func__, ReferenceFrame));
                StaticSynchronizable<MotionState>::unlock(0);
            }
            else
            {
                for (auto &&state : states)
                {
                    auto *pFrameState = pCommonAncestorFrame->getFrameState(state);
                    auto t = (pFrameState != nullptr) ? pFrameState->getTime() : 0.0;
                    auto *pMotionState = pDescendantFrame->createMotionState(CoordinateType::Cartesian, state);
                    if (pMotionState != nullptr)
                    {
                        pMotionState->transformToFrame(pCommonAncestorFrame, t);
                        if (pMergedFrame == nullptr)
                            pMergedFrame = pMotionState->toReferenceFrame(name);
                        else
                        {
                            pFrameState = pMergedFrame->createFrameState(state);
                            if (pFrameState != nullptr)
                            {
                                pFrameState->setAcceleration(pMotionState->getAcceleration());
                                pFrameState->setAngleUnits(pMotionState->getAngleUnits());
                                pFrameState->setOrientation(pMotionState->getEulers());
                                pFrameState->setOrigin(pMotionState->getPosition());
                                pFrameState->setRotationalAccelerations(pMotionState->getEulerAccelerations());
                                pFrameState->setRotationalRates(pMotionState->getEulerRates());
                                pFrameState->setVelocity(pMotionState->getVelocity());
                                pFrameState->setTime(pMotionState->getTime());
                            }
                        }

                        delete pMotionState;
                    }
                }

                // move the children of the descendant frame to the merged frame
                while (pDescendantFrame->hasChildren())
                {
                    auto *pChildFrame = pDescendantFrame->getChild();
                    pChildFrame->setParent(pMergedFrame);
                }

                if (bRemoveSingletons)
                {
                    while (pDescendantFrame != nullptr && pDescendantFrame != pCommonAncestorFrame)
                    {
                        auto *pParentFrame = pDescendantFrame->getParent();
                        if (!pDescendantFrame->hasChildren())
                            delete pDescendantFrame;
                        else
                            break;

                        pDescendantFrame = pParentFrame;
                    }
                }
            }
        }
    }

    if (!bSuccess)
    {
        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "A merged reference frame cannot be created.\n",
               getQualifiedStaticMethodName(__func__, ReferenceFrame));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return pMergedFrame;
}

/**
 * Create a motion state within this frame of reference
 * @param coordType the coordinate system in which the motion state will be described
 * @param state     the desired perturbation state of the motion state to be created
 */
MotionState *ReferenceFrame::createMotionState(const CoordinateType &coordType,
                                               const std::string &state) const
{
    return MotionState::create(const_cast<ReferenceFrame *>(this), coordType, state);
}

/**
 * Create a motion state within this frame of reference that is projected to the specified time
 * @param coordType the coordinate system in which the motion state will be described
 * @param t         the time to which the frames will be projected in order to calculate the motion state at
 *                  the desired time
 * @param state     the desired perturbation state of the motion state to be created
 */
MotionState *ReferenceFrame::createMotionState(const CoordinateType &coordType,
                                               double t,
                                               const std::string &state) const
{
    auto *pMotionState = ReferenceFrame::createMotionState(coordType, state);
    if (pMotionState != nullptr)
        pMotionState->update(t);

    return pMotionState;
}

/**
 * Create a shared root "World" reference frame
 * @param name  the name of the newly created frame
 * @param state the desired perturbation state of the reference frame
 */
std::shared_ptr<ReferenceFrame> ReferenceFrame::createShared(const std::string &name,
                                                             const std::string &state)
{
    return std::shared_ptr<ReferenceFrame>(ReferenceFrame::create(name, state));
}

/**
 * Create a child frame with the current object's frame as its parent
 * @param name  the name of the newly created frame
 * @param state the desired perturbation state of the reference frame
 */
std::shared_ptr<ReferenceFrame> ReferenceFrame::createSharedChild(const std::string &name,
                                                                  const std::string &state)
{
    return std::shared_ptr<ReferenceFrame>(ReferenceFrame::createChild(name, state));
}

/**
 * Function to create a new shared reference frame by merging both reference frame input objects with all
 * frames along the path between them. The resultant frame is a child of the least common ancestor of both
 * frames and all frames merged during the process will be projected to the time at which the common ancestor
 * is defined. For example, the tree diagram below shows the relationship before and after frames B and G are
 * merged, where the transformations imposed by frames B, E, and G have been combined into the newly created
 * frame BG. Singleton frames left behind as a result of the merge, as demonstrated by frame (G) in the diagram
 * below, are removed contingent upon the flag bRemoveSingletons being true. If singletons are removed from the
 * resultant tree, care must be taken to update external references to the frames that no longer exist. In
 * order for the merge to succeed the common ancestor frame must be non-null, both frames must reside within
 * the same branch, and most importantly all frames to be merged must be non-rotating. Upon success, the
 * function returns a non-null reference frame.
 * @param pStartFrame       the start of the chain
 * @param pEndFrame         the end of the chain
 * @param name              the name of the newly merged frame
 * @param bRemoveSingletons a flag that indicates whether or not singletons frames resulting from the merge
 *                          will be deleted
   \verbatim
        Before:               After:
          A                     A -------
         / \                   / \       \
        B   C    -------->    B   C      BG
       / \   \               / \   \     /\
      D   E   F             D   E   F   I  J
         / \                   / \
        G   H                (G)  H
       / \
      I   J
   \endverbatim
 */
std::shared_ptr<ReferenceFrame> ReferenceFrame::createSharedMergedFrame(ReferenceFrame *pStartFrame,
                                                                        ReferenceFrame *pEndFrame,
                                                                        const std::string &name,
                                                                        bool bRemoveSingletons)
{
    return std::shared_ptr<ReferenceFrame>(ReferenceFrame::createMergedFrame(pStartFrame,
                                                                             pEndFrame,
                                                                             name,
                                                                             bRemoveSingletons));
}

/**
 * Create a shared motion state within this frame of reference
 * @param coordType the coordinate system in which the motion state will be described
 * @param state     the desired perturbation state of the motion state to be created
 */
std::shared_ptr<MotionState> ReferenceFrame::createSharedMotionState(const CoordinateType &coordType,
                                                                     const std::string &state) const
{
    return std::shared_ptr<MotionState>(createMotionState(coordType, state));
}

/**
 * Create a motion state within this frame of reference that is projected to the specified time
 * @param coordType the coordinate system in which the motion state will be described
 * @param t         the time to which the frames will be projected in order to calculate the motion state at
 *                  the desired time
 * @param state     the desired perturbation state of the motion state to be created
 */
std::shared_ptr<MotionState> ReferenceFrame::createSharedMotionState(const CoordinateType &coordType,
                                                                     double t,
                                                                     const std::string &state) const
{
    return std::shared_ptr<MotionState>(createMotionState(coordType, t, state));
}

/**
 * Create a shared sibling frame with the parent of the current object's frame as its parent; function succeeds
 * if the current object has a non-null parent
 * @param name  the name of the newly created frame
 * @param state the desired perturbation state of the reference frame
 */
std::shared_ptr<ReferenceFrame> ReferenceFrame::createSharedSibling(const std::string &name,
                                                                    const std::string &state)
{
    return std::shared_ptr<ReferenceFrame>(createSibling(name, state));
}

/**
 * Create a sibling frame with the parent of the current object's frame as its parent; function succeeds if the
 * current object has a non-null parent
 * @param name  the name of the newly created frame
 * @param state the desired perturbation state of the reference frame
 */
ReferenceFrame *ReferenceFrame::createSibling(const std::string &name,
                                              const std::string &state)
{
    ReferenceFrame *pFrame = nullptr;
    auto *pParentFrame = getParent();
    if (pParentFrame != nullptr)
    {
        pFrame = pParentFrame->createChild(name, state);
    }

    return pFrame;
}

/**
 * Function to delete and remove a reference frame as being a child of this frame
 */
bool ReferenceFrame::deleteChild(ReferenceFrame *pChildFrame)
{
    auto &&itChild = std::find(m_children.cbegin(), m_children.cend(), pChildFrame);
    bool bSuccess = (itChild != m_children.cend());
    if (bSuccess)
        delete pChildFrame;

    return bSuccess;
}

/**
 * Function to delete and remove all children from this object's frame
 */
void ReferenceFrame::deleteChildren(void)
{
    while (true)
    {
        auto &&itChild = m_children.cbegin();
        if (itChild != m_children.cend())
        {
            auto *pChildFrame = *itChild;
            pChildFrame->destroy();
        }
        else
            break;
    }
}

/**
 * Function to delete a frame and its subtree; upon success, deletes input argument and sets it to null
 */
void ReferenceFrame::deleteFrame(ReferenceFrame *&pFrame)
{
    if (pFrame != nullptr)
    {
        delete pFrame;
        pFrame = nullptr;
    }
}

/**
 * Delete a frame state by name
 */
bool ReferenceFrame::deleteFrameState(const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        deleteFrameState(pFrameState);

    return bSuccess;
}

/**
 * Delete a frame state by pointer
 */
bool ReferenceFrame::deleteFrameState(FrameState *&pFrameState)
{
    bool bSuccess = false;
    auto &&itFrameState = m_frameStates.cbegin();
    while (itFrameState != m_frameStates.cend())
    {
        if (pFrameState == itFrameState->second)
        {
            itFrameState = m_frameStates.erase(itFrameState);
            if (pFrameState != nullptr)
            {
                delete pFrameState;
                pFrameState = nullptr;
            }

            bSuccess = true;
        }
        else
            ++itFrameState;
    }

    return bSuccess;
}

/**
 * Delete all frame states
 */
void ReferenceFrame::deleteFrameStates(void)
{
    auto &&itFrameState = m_frameStates.cbegin();
    while (itFrameState != m_frameStates.cend())
    {
        auto *pFrameState = itFrameState->second;
        if (pFrameState != nullptr)
            delete pFrameState;

        ++itFrameState;
    }

    m_frameStates.clear();
}

/**
 * Function to delete a frame and its subtree; upon success, deletes input argument and sets it to null
 */
void ReferenceFrame::deleteTree(ReferenceFrame *&pFrame)
{
    if (pFrame != nullptr)
    {
        pFrame->destroy();
        delete pFrame;
        pFrame = nullptr;
    }
}

/**
 * Function to deserialize this object's data
 */
std::istream &ReferenceFrame::deserialize(std::istream &stream)
{
    // NOTE: this function does NOT modify this object's current parent/children relationships

    if (stream)
    {
        // remove this object's current frame states.
        deleteFrameStates();

        // deserialize the frame states
        std::size_t numFrameStates = 0;
        stream.read((char *)&numFrameStates, sizeof(std::size_t));
        for (std::size_t i = 0; i < numFrameStates; ++i)
        {
            FrameState *pFrameState = nullptr;
            if (m_frameStateType != FrameStateType::Unknown)
            {
                pFrameState = FrameState::create(m_frameStateType,
                                                 std::string(DEFAULT_FRAME_STATE));
                if (pFrameState == nullptr)
                {
                    StaticSynchronizable<MotionState>::lock(0);
                    logMsg(std::cout, LoggingLevel::Enum::Warning,
                           "A frame state of type \"" + std::string(m_frameStateType) +
                           "\"; the default frame state type will be instantiated.\n",
                           getQualifiedMethodName(__func__));
                    StaticSynchronizable<MotionState>::unlock(0);
                }
            }

            if (pFrameState == nullptr)
                pFrameState = ProjectedFrameState::create(DEFAULT_FRAME_STATE);

            if (pFrameState != nullptr)
            {
                pFrameState->deserialize(stream);

                auto &&state = pFrameState->getName();
                m_frameStates[state] = pFrameState;
            }
        }

        std::getline(stream, m_name, '\0');
    }

    return stream;
}

/**
 * Function to destroy the ReferenceFrame and its subtree
 */
void ReferenceFrame::destroy(void)
{
    auto *pThisParent = getParent();
    auto *pFrame = this;
    do
    {
        if (pFrame != nullptr)
        {
            auto *pChildFrame = pFrame->getChild();
            if (pChildFrame == nullptr)
            {
                auto *pParentFrame = pFrame->getParent();
                if (pParentFrame != nullptr)
                    pParentFrame->removeChild(pFrame);

                if (pFrame != this)
                {
                    delete pFrame;
                }
                else
                    break;

                pFrame = pParentFrame;
            }
            else
                pFrame = pChildFrame;
        }
    }
    while (pFrame != nullptr && pFrame != pThisParent);
}

/**
 * Function to determine the common ancestor of both the current object's reference frame and the input
 * reference frame. If no common ancestor exists, the function returns null.
 */
ReferenceFrame *ReferenceFrame::determineCommonAncestor(ReferenceFrame *pFrame) const
{
    // loop until we find a common ancestor frame
    ReferenceFrame *pCommonAncestor = nullptr;
    if (isFamily(pFrame))
    {
        while (true)
        {
            pFrame = pFrame->getParent();
            if (pFrame != nullptr && !pFrame->isAncestor(this))
                continue;
            else
                pCommonAncestor = pFrame;

            break;
        }
    }

    return pCommonAncestor;
}

/**
 * Determine if the current object forms a branch with the specified descendant frame; the function tests each
 * frame by name or pointer value to make the determination; if found, the function returns a non-null pointer
 * to the descendant frame that marks the end of the matching branch
 * @param pDescendantFrame the descendant frame which marks the end of the branch
 */
ReferenceFrame *ReferenceFrame::findBranch(ReferenceFrame *pDescendantFrame) const
{
    if (pDescendantFrame != nullptr)
    {
        // is the current object an ancestor of the input descendant frame?
        if (isAncestor(pDescendantFrame))
        {
            return pDescendantFrame;
        }

        // the input descendant frame is not within the current object's subtree based upon the prior test;
        // does the current object have a descendant frame similarly named to that of the input descendant
        // frame?
        if (isAncestor(pDescendantFrame->getName()))
        {
            // the current object does have a descendant that is similarly named to that of the input
            // descendant, but need to verify that a branch in the current object's subtree matches the
            // branch formed by successively examining ancestor nodes of the input descendant frame
            auto *pFrame = pDescendantFrame;
            auto *pThisDescendantFrame = findFrame(pDescendantFrame->getName());
            auto *pThisFrame = pThisDescendantFrame;
            while (pFrame != nullptr && pThisFrame != nullptr)
            {
                if (pFrame->getName() != pThisFrame->getName())
                    return nullptr;

                if (pThisFrame == this)
                    return pThisDescendantFrame;

                pFrame = pFrame->getParent();
                pThisFrame = pThisFrame->getParent();
            }
        }
    }

    return nullptr;
}

/**
 * Find a child frame by name; returns non-null if a child frame by the given name currently exists
 */
ReferenceFrame *ReferenceFrame::findChild(const std::string &name) const
{
    if (!name.empty())
    {
        for (auto &&itChild = m_children.cbegin(); itChild != m_children.cend(); ++itChild)
        {
            auto *pChildFrame = *itChild;
            if (pChildFrame != nullptr && pChildFrame->m_name.compare(name) == 0)
                return pChildFrame;
        }
    }

    return nullptr;
}

/**
 * Function to find a common ancestor of both the current object's reference frame and the input reference
 * frame. If both frames reside in different trees, then the function tests on frame name in order to find a
 * common ancestor. In this case, the function returns a pointer to the current object's common ancestor that
 * resides in the current object's tree (if found). The input argument will be modified to reflect the common
 * frame found in the tree that contains the input argument. If both the current object's frame and the input
 * argument frame reside in the same tree, the return value and input argument will be the same. If no common
 * ancestor is found, the return value is null and the input argument is unmodified.
 * @param[in]  pFrame the input reference frame
 * @param[out] pFrame a pointer to the common ancestor found in the tree that contains the input argument
 */
ReferenceFrame *ReferenceFrame::findCommonAncestorFrame(ReferenceFrame *&pFrame)
{
    ReferenceFrame *pThisCommonAncestor = nullptr;
    if (pFrame != nullptr)
    {
        pThisCommonAncestor = determineCommonAncestor(pFrame);
        if (pThisCommonAncestor != nullptr)
            pFrame = pThisCommonAncestor; // they're in the same tree
        else
        {
            // the frames are likely in different trees; determine the common ancestor of the two frames
            auto *pThisFrame = this;
            while (pThisFrame != nullptr)
            {
                if (pFrame->isDescendant(pThisFrame->getName()) ||
                    pFrame->getName().compare(pThisFrame->getName()) == 0)
                {
                    // an ancestor frame has been found in the tree containing the current object; now try to
                    // find an ancestor frame in the input object
                    auto *pTempFrame = pFrame;
                    while (pTempFrame != nullptr)
                    {
                        if (pTempFrame->getName().compare(pThisFrame->getName()) == 0)
                            break;
                        else
                            pTempFrame = pTempFrame->getParent();
                    }

                    if (pTempFrame == nullptr)
                        return nullptr;
                    else
                        pFrame = pTempFrame;

                    pThisCommonAncestor = pThisFrame;

                    break;
                }
                else
                {
                    pThisFrame = pThisFrame->getParent();
                }
            }
        }
    }

    return pThisCommonAncestor;
}

/**
 * Find a frame within the specified frame's tree by name. If found, the function returns a non-null reference
 * frame.
 */
ReferenceFrame *ReferenceFrame::findFrame(ReferenceFrame *pFrame,
                                          const std::string &name)
{
    if (pFrame != nullptr && !name.empty())
    {
        // try to find a frame within the tree with the given name
        auto *pThisParent = pFrame->getParent();
        do
        {
            if (pFrame != nullptr && pFrame->m_name == name)
                return pFrame;
            else
            {
                auto *pChildFrame = pFrame->getChild();
                if (pChildFrame == nullptr)
                {
                    do
                    {
                        pChildFrame = pFrame->getRightSibling();
                        pFrame = pFrame->getParent();
                    }
                    while (pChildFrame == nullptr && pFrame != pThisParent);
                }

                pFrame = pChildFrame;
            }
        }
        while (pFrame != nullptr && pFrame != pThisParent);
    }

    return nullptr;
}

/**
 * Find a frame within the this object's tree by name. If found, the function returns a non-null reference
 * frame.
 */
ReferenceFrame *ReferenceFrame::findFrame(const std::string &name) const
{
    return findFrame(const_cast<ReferenceFrame *>(this), name);
}

/**
 * Find a frame within the current object's tree that matches the name of the input frame. If found, the
 * function returns a non-null reference frame.
 */
ReferenceFrame *ReferenceFrame::findFrame(ReferenceFrame *pFrame)
{
    return findFrame(this, pFrame != nullptr ? pFrame->getName() : "");
}

/**
 * Find a frame within the current object's tree that matches the name of the input frame. If found, the
 * function returns a non-null reference frame.
 */
ReferenceFrame *ReferenceFrame::findFrame(ReferenceFrame *pFrame) const
{
    return findFrame(const_cast<ReferenceFrame *>(this), pFrame != nullptr ? pFrame->getName() : "");
}

/**
 * Find a frame within the current object's tree that matches the frame in which the input motion state is
 * defined; if found, the function returns a non-null object
 */
ReferenceFrame *ReferenceFrame::findFrame(MotionState *pMotionState) const
{
    return findFrame(const_cast<ReferenceFrame *>(this), pMotionState);
}

/**
 * Find a frame within the input reference frame object's tree that matches the frame in which the input motion
 * state is defined; if found, the function returns a non-null object
 * @param pFrame       the root of a tree of reference frames in which the search will be performed
 * @param pMotionState the motion state defined in the target frame of reference
 */
ReferenceFrame *ReferenceFrame::findFrame(ReferenceFrame *pFrame,
                                          MotionState *pMotionState)
{
    if (pFrame != nullptr && pMotionState != nullptr)
    {
        auto *pMotionStateFrame = pMotionState->getFrame();
        if (pMotionStateFrame != nullptr)
        {
            auto &&name = pMotionStateFrame->getName();

            return pFrame->findFrame(name);
        }
    }

    return nullptr;
}

/**
 * Function to find the specified frame state by name; if a frame state is not associated with the specified
 * name, the function returns null
 */
FrameState *ReferenceFrame::findFrameState(const std::string &state) const
{
    FrameState *pFrameState = nullptr;
    auto &&itFrameState = m_frameStates.find(state);
    if (itFrameState != m_frameStates.cend())
        pFrameState = itFrameState->second;

    return pFrameState;
}

/**
 * Function to find the least common root of a subtree that contains both the current object's frame of
 * reference as well as the input reference frame. If both frames reside in different trees, then the function
 * tests on frame name in order to find a common root. In this case, the function returns a pointer to the
 * current object's common root that resides in the current object's tree (if found). The input argument will
 * be modified to reflect the common frame found in the tree that contains the input argument. If both the
 * current object's frame and the input argument frame reside in the same tree, the return value and input
 * argument will be the same. If no common root is found, the return value is null and the input argument is
 * unmodified.
 * @param[in]  pFrame the input reference frame
 * @param[out] pFrame a pointer to the common ancestor found in the tree that contains the input argument
 */
ReferenceFrame *ReferenceFrame::findLeastCommonRootFrame(ReferenceFrame *&pFrame)
{
    ReferenceFrame *pThisCommonRoot = nullptr;
    if (pFrame != nullptr)
    {
        if (this == pFrame || isDescendant(pFrame))
            pThisCommonRoot = pFrame;
        else if (pFrame->isDescendant(this))
        {
            pThisCommonRoot = this;
            pFrame = pThisCommonRoot;
        }
        else // frames do not lie within the same branch...
            pThisCommonRoot = findCommonAncestorFrame(pFrame);
    }

    return pThisCommonRoot;
}

/**
 * Find a sibling frame by name; returns non-null upon success
 */
ReferenceFrame *ReferenceFrame::findSibling(const std::string &name) const
{
    ReferenceFrame *pSiblingFrame = nullptr;
    if (m_pParentFrame != nullptr) // if this frame has no parent, it's the root of a tree and has no siblings
        pSiblingFrame = m_pParentFrame->findChild(name);

    return pSiblingFrame;
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::getAcceleration(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getAcceleration();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getAcceleration(double acceleration[3],
                                     const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getAcceleration(acceleration);

    return bSuccess;
}

/**
 * Get angle units (Degrees or Radians)
 * @param state the desired perturbation state of the reference frame
 */
AngleUnitType ReferenceFrame::getAngleUnits(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getAngleUnits();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the first child frame
 */
ReferenceFrame *ReferenceFrame::getChild(void)
{
    auto &&itChild = m_children.cbegin();
    if (itChild != m_children.cend())
        return *itChild;

    return nullptr;
}

/**
 * Get the first child frame
 */
ReferenceFrame *ReferenceFrame::getChild(void) const
{
    return const_cast<ReferenceFrame *>(this)->getChild();
}

/**
 * Get a child frame by name; create a child frame by the given name if one does not currently exist
 * @param name  the name of the child frame
 * @param state the desired perturbation state of the reference frame
 */
ReferenceFrame *ReferenceFrame::getChild(const std::string &name,
                                         const std::string &state)
{
    ReferenceFrame *pFrame = nullptr;
    if (!name.empty())
    {
        pFrame = findChild(name);
        if (pFrame == nullptr)
        {
            pFrame = createChild(name, state);
        }
    }

    return pFrame;
}

/**
 * Get child by index; returns non-null upon success
 */
ReferenceFrame *ReferenceFrame::getChild(std::size_t index)
{
    ReferenceFrame *pChildFrame = nullptr;
    if (index < m_children.size())
    {
        auto &&itChild = m_children.cbegin();
        std::advance(itChild, index);
        pChildFrame = *itChild;
    }

    return pChildFrame;
}

/**
 * Get the index of this child object
 */
size_t ReferenceFrame::getChildIndex(void) const
{
    auto *pFrame = const_cast<ReferenceFrame *>(this);

    return getChildIndex(pFrame);
}

/**
 * Get the index of a child; if the input object is a child of the current object, function returns an index in
 * the range 0 to N - 1, where N is the number of children of the current object; otherwise, function returns
 * the current object's number of child frames
 */
size_t ReferenceFrame::getChildIndex(ReferenceFrame *pChildFrame)
{
    auto *pParentFrame = pChildFrame->getParent();
    if (pParentFrame == nullptr)
        return 0;
    else
    {
        auto &&itChild = std::find(pParentFrame->m_children.cbegin(),
                                   pParentFrame->m_children.cend(),
                                   pChildFrame);
        if (itChild != pParentFrame->m_children.cend())
            return std::distance(pParentFrame->m_children.cbegin(), itChild);
        else
        {
            // this should never happen...
            return pParentFrame->m_children.size();
        }
    }
}

/**
 * Get the children of the current frame
 */
std::vector<ReferenceFrame *> ReferenceFrame::getChildren(void)
{
    return m_children;
}

/**
 * Get the name of this class
 */
std::string ReferenceFrame::getClassName(void) const
{
    return "ReferenceFrame";
}

/**
 * Function to return the depth of this frame relative to the root of the tree, where the root of the tree is
 * at a depth of one.
 */
size_t ReferenceFrame::getDepth(void) const
{
    size_t depth = 0;
    auto *pFrame = this;
    while (pFrame != nullptr)
    {
        pFrame = pFrame->getParent();
        ++depth;
    }

    return depth;
}

/**
 * Function to get the specified frame state by name; if a frame state is not associated with the specified
 * name, the function will return the default state
 */
FrameState *const ReferenceFrame::getFrameState(const std::string &state) const
{
    FrameState *pFrameState = nullptr;

    auto &&itFrameState = m_frameStates.find(state);
    if (itFrameState != m_frameStates.end())
        pFrameState = itFrameState->second;

    if (pFrameState == nullptr)
        pFrameState = createFrameState(DEFAULT_FRAME_STATE);

    return pFrameState;
}

/**
 * Get this object's map of frame states
 */
ReferenceFrame::tFrameStates &ReferenceFrame::getFrameStates(void)
{
    return m_frameStates;
}

/**
 * Get this object's frame state type
 */
FrameStateType ReferenceFrame::getFrameStateType(void) const
{
    return m_frameStateType;
}

/**
 * Get this frame's left sibling
 */
ReferenceFrame *ReferenceFrame::getLeftSibling(void)
{
    auto *pParentFrame = getParent();
    if (pParentFrame == nullptr)
        return nullptr;
    else
    {
        auto &&itChild = std::find(pParentFrame->m_children.cbegin(), pParentFrame->m_children.cend(), this);
        if (itChild == pParentFrame->m_children.cbegin())
            return nullptr;

        return *--itChild;
    }
}

/**
 * Get this frame's left sibling
 */
ReferenceFrame *ReferenceFrame::getLeftSibling(void) const
{
    return const_cast<ReferenceFrame *>(this)->getLeftSibling();
}

/**
 * Get the motion states defined within this frame of reference
 */
std::vector<MotionState *> &ReferenceFrame::getMotionStates(void)
{
    return m_motionStates;
}

/**
 * Get the name of this reference frame
 */
std::string ReferenceFrame::getName(void) const
{
    return m_name;
}

/**
 * Get the orientation angles of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
Eulers ReferenceFrame::getOrientation(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getOrientation();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the orientation angles of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getOrientation(double orientation[3],
                                    const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getOrientation(orientation);

    return bSuccess;
}

/**
 * Get the orientation angles of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
Eulers ReferenceFrame::getOrientation(double t,
                                      const std::string &state) const
{
    Eulers orientation(getAngleUnits(state));

    getOrientation(orientation, t, state);

    return orientation;
}

/**
 * Get the orientation angles of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getOrientation(double orientation[3],
                                    double t,
                                    const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getOrientation(orientation, t);

    return bSuccess;
}

/**
 * Get the position of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::getOrigin(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getOrigin();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the position of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getOrigin(double origin[3],
                               const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getOrigin(origin);

    return bSuccess;
}

/**
 * Get the position of the origin of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::getOrigin(double t,
                                   const std::string &state) const
{
    Vector3d origin;

    getOrigin(origin, t, state);

    return origin;
}

/**
 * Get the position of the origin of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getOrigin(double origin[3],
                               double t,
                               const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getOrigin(origin, t);

    return bSuccess;
}

/**
 * Get the parent of this frame
 */
ReferenceFrame *ReferenceFrame::getParent(void)
{
    return m_pParentFrame;
}

/**
 * Get the parent of this frame
 */
ReferenceFrame *ReferenceFrame::getParent(void) const
{
    return m_pParentFrame;
}

/**
 * Get this object's pitch angle
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitch(const std::string &state) const
{
    double t = getTime(state);

    return getPitch(t, state);
}

/**
 * Get this object's pitch angle
 * @param t     the time at which the return value will be computed
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitch(double t,
                                const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getPitch(t, angleUnits, state);
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitch(const AngleUnitType &angleUnits,
                                const std::string &state) const
{
    double t = getTime(state);

    return getPitch(t, angleUnits, state);
}

/**
 * Get this object's pitch angle
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitch(double t,
                                const AngleUnitType &angleUnits,
                                const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getPitch(angleUnits, t);

    throw std::runtime_error("ReferenceFrame::getPitch(): Invalid/non-existent frame state given!");
}

/**
 * Get this object's pitch acceleration
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitchAcceleration(const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getPitchAcceleration(angleUnits, state);
}

/**
 * Get this object's pitch acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitchAcceleration(const AngleUnitType &angleUnits,
                                            const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getPitchAcceleration(angleUnits);

    throw std::runtime_error("ReferenceFrame::getPitchAcceleration(): Invalid/non-existent frame state given!");
}

/**
 * Get this object's pitch rate
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitchRate(const std::string &state) const
{
    double t = getTime(state);

    return getPitchRate(t, state);
}

/**
 * Get this object's pitch rate
 * @param t     the time at which the return value will be computed
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitchRate(double t,
                                    const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getPitchRate(t, angleUnits, state);
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitchRate(const AngleUnitType &angleUnits,
                                    const std::string &state) const
{
    double t = getTime(state);

    return getPitchRate(t, angleUnits, state);
}

/**
 * Get this object's pitch rate
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getPitchRate(double t,
                                    const AngleUnitType &angleUnits,
                                    const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getPitchRate(angleUnits, t);

    throw std::runtime_error("ReferenceFrame::getPitchRate(): Invalid/non-existent frame state given!");
}

/**
 * Get this frame's right sibling
 */
ReferenceFrame *ReferenceFrame::getRightSibling(void)
{
    auto *pParentFrame = getParent();
    if (pParentFrame == nullptr)
        return nullptr;
    else
    {
        auto &&itChild = std::find(pParentFrame->m_children.cbegin(), pParentFrame->m_children.cend(), this);
        if (++itChild == pParentFrame->m_children.cend())
            return nullptr;

        return *itChild;
    }
}

/**
 * Get this frame's right sibling
 */
ReferenceFrame *ReferenceFrame::getRightSibling(void) const
{
    return const_cast<ReferenceFrame *>(this)->getRightSibling();
}

/**
 * Get this object's roll angle
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRoll(const std::string &state) const
{
    double t = getTime(state);

    return getRoll(t, state);
}

/**
 * Get this object's roll angle
 * @param t     the time at which the return value will be computed
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRoll(double t,
                               const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getRoll(t, angleUnits, state);
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRoll(const AngleUnitType &angleUnits,
                               const std::string &state) const
{
    double t = getTime(state);

    return getRoll(t, angleUnits, state);
}

/**
 * Get this object's roll angle
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRoll(double t,
                               const AngleUnitType &angleUnits,
                               const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getRoll(angleUnits, t);

    throw std::runtime_error("ReferenceFrame::getRoll(): Invalid/non-existent frame state given!");
}

/**
 * Get this object's roll acceleration
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRollAcceleration(const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getRollAcceleration(angleUnits, state);
}

/**
 * Get this object's roll acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRollAcceleration(const AngleUnitType &angleUnits,
                                           const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getRollAcceleration(angleUnits);

    throw std::runtime_error("ReferenceFrame::getRollAcceleration(): Invalid/non-existent frame state given!");
}

/**
 * Get this object's roll rate
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRollRate(const std::string &state) const
{
    double t = getTime(state);

    return getRollRate(t, state);
}

/**
 * Get this object's roll rate
 * @param t     the time at which the return value will be computed
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRollRate(double t,
                                   const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getRollRate(t, angleUnits, state);
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRollRate(const AngleUnitType &angleUnits,
                                   const std::string &state) const
{
    double t = getTime(state);

    return getRollRate(t, angleUnits, state);
}

/**
 * Get this object's roll rate
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getRollRate(double t,
                                   const AngleUnitType &angleUnits,
                                   const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getRollRate(angleUnits, t);

    throw std::runtime_error("ReferenceFrame::getRollRate(): Invalid/non-existent frame state given!");
}

/**
 * Get the root reference frame
 */
ReferenceFrame *ReferenceFrame::getRoot(void)
{
    auto *pRootFrame = this;
    while (true)
    {
        auto *pParentFrame = pRootFrame->getParent();
        if (pParentFrame == nullptr)
            break;
        else
            pRootFrame = pParentFrame;
    }

    return pRootFrame;
}

/**
 * Get the root reference frame
 */
ReferenceFrame *ReferenceFrame::getRoot(void) const
{
    return const_cast<ReferenceFrame *>(this)->getRoot();
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
Eulers ReferenceFrame::getRotationalAccelerations(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getRotationalAccelerations();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getRotationalAccelerations(double rotationalAccelerations[3],
                                                const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getRotationalAccelerations(rotationalAccelerations);

    return bSuccess;
}

/**
 * Get the rotational rates of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
Eulers ReferenceFrame::getRotationalRates(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getRotationalRates();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the rotational rates of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getRotationalRates(double rotationalRates[3],
                                        const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getRotationalRates(rotationalRates);

    return bSuccess;
}

/**
 * Get the rotational rates of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
Eulers ReferenceFrame::getRotationalRates(double t,
                                          const std::string &state) const
{
    Eulers rotationalRates(getAngleUnits(state));

    getRotationalRates(rotationalRates, t, state);

    return rotationalRates;
}

/**
 * Get the rotational rates of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getRotationalRates(double rotationalRates[3],
                                        double t,
                                        const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getRotationalRates(rotationalRates, t);

    return bSuccess;
}

/**
 * @param name  the name of the sibling frame
 * @param state the desired perturbation state of the reference frame
 */
ReferenceFrame *ReferenceFrame::getSibling(const std::string &name,
                                           const std::string &state) const
{
    ReferenceFrame *pSiblingFrame = nullptr;
    if (m_pParentFrame != nullptr) // if this frame has no parent, it's the root of a tree and has no siblings
        pSiblingFrame = m_pParentFrame->getChild(name, state);

    return pSiblingFrame;
}

/**
 * Get the time at which this frame is currently defined
 */
double ReferenceFrame::getTime(void) const
{
    return getTime(DEFAULT_FRAME_STATE);
}

/**
 * Get the time at which this frame is currently defined
 * @param state the desired perturbation state of the reference frame
 */
double ReferenceFrame::getTime(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getTime();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the velocity of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::getVelocity(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getVelocity();

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "FrameState \"" + state + "\" does not exist.");
}

/**
 * Get the velocity of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getVelocity(double velocity[3],
                                 const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getVelocity(velocity);

    return bSuccess;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
Vector3d ReferenceFrame::getVelocity(double t,
                                     const std::string &state) const
{
    Vector3d velocity;

    getVelocity(velocity, t, state);

    return velocity;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent at the specified time
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::getVelocity(double velocity[3],
                                 double t,
                                 const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
        pFrameState->getVelocity(velocity, t);

    return bSuccess;
}

/**
 * Get a pointer to world reference frame
 */
ReferenceFrame *ReferenceFrame::getWorldFrame(void)
{
    static bool bInitialized = false;
    static std::unique_ptr<ReferenceFrame> pWorldFrame(ReferenceFrame::create("world_frame",
                                                                              DEFAULT_FRAME_STATE));
    if (!bInitialized)
    {
        // REMOVE the static world frame from the garbage collector
        garbageCollector.releaseObject(pWorldFrame.get());

        bInitialized = true;
    }

    return pWorldFrame.get();
}

/**
 * Get this object's yaw angle
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYaw(const std::string &state) const
{
    double t = getTime(state);

    return getYaw(t, state);
}

/**
 * Get this object's yaw angle
 * @param t     the time at which the return value will be computed
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYaw(double t,
                              const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getYaw(t, angleUnits, state);
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYaw(const AngleUnitType &angleUnits,
                              const std::string &state) const
{
    double t = getTime(state);

    return getYaw(t, angleUnits, state);
}

/**
 * Get this object's yaw angle
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYaw(double t,
                              const AngleUnitType &angleUnits,
                              const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getYaw(angleUnits, t);

    throw std::runtime_error("ReferenceFrame::getYaw(): Invalid/non-existent frame state given!");
}

/**
 * Get this object's yaw acceleration
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYawAcceleration(const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getYawAcceleration(angleUnits, state);
}

/**
 * Get this object's yaw acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYawAcceleration(const AngleUnitType &angleUnits,
                                          const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getYawAcceleration(angleUnits);

    throw std::runtime_error("ReferenceFrame::getYawAcceleration(): Invalid/non-existent frame state given!");
}

/**
 * Get this object's yaw rate
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYawRate(const std::string &state) const
{
    double t = getTime(state);

    return getYawRate(t, state);
}

/**
 * Get this object's yaw rate
 * @param t     the time at which the return value will be computed
 * @param state the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYawRate(double t,
                                  const std::string &state) const
{
    auto &&angleUnits = getAngleUnits(state);

    return getYawRate(t, angleUnits, state);
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYawRate(const AngleUnitType &angleUnits,
                                  const std::string &state) const
{
    double t = getTime(state);

    return getYawRate(t, angleUnits, state);
}

/**
 * Get this object's yaw rate
 * @param t          the time at which the return value will be computed
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
double ReferenceFrame::getYawRate(double t,
                                  const AngleUnitType &angleUnits,
                                  const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return pFrameState->getYawRate(angleUnits, t);

    throw std::runtime_error("ReferenceFrame::getYawRate(): Invalid/non-existent frame state given!");
}

/**
 * Return whether or not this frame has children
 */
bool ReferenceFrame::hasChildren(void) const
{
    return !m_children.empty();
}

/**
 * Initialization function to set origin, orientation, and their time-derivatives to zero
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::initialize(const std::string &state)
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        pFrameState->initialize();
}

/**
 * Initialization function to reset time-derivatives to zero
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::initializeTimeDerivatives(const std::string &state)
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        pFrameState->initializeTimeDerivatives();
}

/**
 * Function to insert a copy of a branch into the current object; in order for the insertion to succeed, the
 * input ancestor and descendant frames must form a valid branch. The function will attempt to find an exact or
 * partial matching branch at the level of the current object and traversing down the current object's subtree;
 * if one isn't found, the branch will be copied and inserted as a sibling of the current object so long as it
 * is has a non-null parent frame and is therefore not the root of a tree; otherwise, the insertion fails and
 * the function returns false
 * @param pAncestorFrame   the ancestor frame which marks the start of the branch
 * @param pDescendantFrame the descendant frame which marks the end of the branch
 */
bool ReferenceFrame::insertBranch(ReferenceFrame *pAncestorFrame,
                                  ReferenceFrame *pDescendantFrame)
{
    bool bSuccess = (pAncestorFrame != nullptr && pDescendantFrame != nullptr);
    if (bSuccess)
    {
        bSuccess = pDescendantFrame->isDescendant(pAncestorFrame);
        if (bSuccess)
        {
            // input ancestor and descendant frames form a valid branch; does this branch already exist
            // within the current object's tree?
            bSuccess = (m_name == pAncestorFrame->getName());
            if (bSuccess)
            {
                // this branch currently exists; verify that the input branch does not belong to the
                // current object
                if (pAncestorFrame != this)
                {
                    // try to find a matching branch within this object's tree
                    auto *pFrame = findBranch(pDescendantFrame);
                    if (pFrame != nullptr)
                    {
                        // a matching branch starting at the current object was found; copy the input branch to
                        // this object's branch
                        auto *pParentFrame = getParent();
                        while (pFrame != pParentFrame)
                        {
                            pFrame->copyFrame(pDescendantFrame);
                            pDescendantFrame = pDescendantFrame->getParent();
                            pFrame = pFrame->getParent();
                        }
                    }
                    else
                    {
                        // there's at least a partial match, so add the missing frames and copy the input
                        // branch
                        auto *pDestFrame = this;
                        auto *pDestParentFrame = pDestFrame->getParent();
                        std::function<void (ReferenceFrame *)> traverseBranchAndCopy = [&] (auto *pSourceFrame)
                        {
                            if (pSourceFrame != nullptr && pSourceFrame != pAncestorFrame)
                                traverseBranchAndCopy(pSourceFrame->getParent());

                            if (bSuccess && pSourceFrame != nullptr)
                            {
                                auto &&name = pSourceFrame->getName();
                                if (pDestParentFrame != nullptr)
                                    pDestFrame = pDestParentFrame->findChild(name);

                                if (pDestFrame != nullptr)
                                {
                                    // frame exists, copy the ancestor frame to the current frame
                                    pDestFrame->copyFrame(pSourceFrame);
                                }
                                else if (pDestParentFrame != nullptr)
                                {
                                    // frame doesn't exist, need to clone the source frame and add it as a
                                    // sibling of the current frame
                                    pDestFrame = pSourceFrame->cloneFrame();
                                    pDestParentFrame->addChild(pDestFrame);
                                }
                                else
                                    bSuccess = false;
                            }

                            pDestParentFrame = pDestFrame;
                        };

                        traverseBranchAndCopy(pDescendantFrame);
                    }
                }
            }

            if (!bSuccess)
            {
                // the branch doesn't exist, so it'll need to be created
                bSuccess = (m_pParentFrame != nullptr);
                if (bSuccess)
                {
                    auto *pBranch = pAncestorFrame->cloneBranch(pDescendantFrame);
                    m_pParentFrame->addChild(pBranch);
                }
            }
        }
    }

    return bSuccess;
}

/**
 * Function to insert a copy of a branch into the destination reference frame object's tree; in order for the
 * insertion to succeed, the destination reference frame object must be non-null and the input ancestor and
 * descendant frames must form a valid branch. The function will attempt to find an exact or partial matching
 * branch at the level of the current object and traversing down the destination reference frame object's
 * subtree; if one isn't found, the branch will be copied and inserted as a sibling so long as it is has a
 * non-null parent frame and is therefore not the root of a tree; otherwise, the insertion fails and the
 * function returns false; if the destination reference frame tree is null, the branch, if valid, is cloned and
 * the input argument to the destination reference frame tree is modified to point to the root ancestor of the
 * cloned branch; function returns true upon success
 */
bool ReferenceFrame::insertBranch(ReferenceFrame *&pFrame,
                                  ReferenceFrame *pAncestorFrame,
                                  ReferenceFrame *pDescendantFrame)
{
    bool bSuccess = (pAncestorFrame != nullptr && pDescendantFrame != nullptr);
    if (bSuccess)
    {
        if (pFrame == nullptr)
        {
            pFrame = pAncestorFrame->cloneBranch(pDescendantFrame);
            bSuccess = (pFrame != nullptr);
        }
        else
            bSuccess = pFrame->insertBranch(pAncestorFrame, pDescendantFrame);
    }

    return bSuccess;
}

/**
 * Determines if the current object's reference frame is an ancestor frame of the input reference frame
 */
bool ReferenceFrame::isAncestor(const ReferenceFrame *pFrame) const
{
    bool bSuccess = isFamily(pFrame);
    if (bSuccess)
        bSuccess = pFrame->isDescendant(this);

    return bSuccess;
}

/**
 * Determines if the current object is an ancestor of the frame associated with the given name
 */
bool ReferenceFrame::isAncestor(const std::string &name) const
{
    auto *pFrame = getRoot()->findFrame(name);
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
        bSuccess = isAncestor(pFrame);

    return bSuccess;
}

/**
 * Determines if the current object's reference frame is a descendant frame of the input reference frame
 */
bool ReferenceFrame::isDescendant(const ReferenceFrame *pFrame) const
{
    if (this != pFrame)
    {
        auto *pCurrent = this;
        do
        {
            if (pCurrent == pFrame)
                return true;
            else
                pCurrent = pCurrent->getParent();
        }
        while (pCurrent != nullptr);
    }

    return false;
}

/**
 * Determines if the current object is a descendant of the frame associated with the given name
 */
bool ReferenceFrame::isDescendant(const std::string &name) const
{
    auto *pFrame = getRoot()->findFrame(name);
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
        bSuccess = isDescendant(pFrame);

    return bSuccess;
}

/**
 * Determines if the input object's reference frame resides in the same tree as the current object's reference
 * frame
 */
bool ReferenceFrame::isFamily(const ReferenceFrame *pFrame) const
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        auto *pThisRoot = const_cast<ReferenceFrame *>(this)->getRoot();
        auto *pFrameRoot = const_cast<ReferenceFrame *>(pFrame)->getRoot();

        bSuccess = (pThisRoot == pFrameRoot);
    }

    return bSuccess;
}

/**
 * Function to determine if this frame is inertial
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::isInertial(const std::string &state) const
{
    return !isNonInertial(state);
}

/**
 * Determines if the frame in question is a leaf node
 */
bool ReferenceFrame::isLeaf(void) const
{
    return m_children.empty();
}

/**
 * Function to determine if this frame is non-inertial
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::isNonInertial(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return (isRotating(state) || pFrameState->getAcceleration() != 0.0);

    return false;
}

/**
 * Determines if the frame in question is the root of a tree
 */
bool ReferenceFrame::isRoot(void) const
{
    return m_pParentFrame == nullptr;
}

/**
 * Function to determine if this frame is rotating
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::isRotating(const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return (pFrameState->getRotationalRates() != 0.0 || pFrameState->getRotationalAccelerations() != 0.0);

    return false;
}

/**
 * Function to print the contents of reference frame and its subtree
 */
std::ostream &ReferenceFrame::print(std::ostream &stream) const
{
    std::string dashes(getDepth() << 1, '-');
    stream << " +--" << dashes << " Frame name: { " << m_name << " }" << std::endl;
    stream << " |--" << dashes << " Frame parent: { "
           << (m_pParentFrame != nullptr ? m_pParentFrame->m_name : "null")
           << " }" << std::endl;

    auto &&itFrameState = m_frameStates.begin();
    for (; itFrameState != m_frameStates.end(); ++itFrameState)
    {
        auto &&state = itFrameState->first;
        auto *pFrameState = itFrameState->second;
        auto &&acceleration = pFrameState->getAcceleration();
        auto &&orientation = pFrameState->getOrientation();
        auto &&origin = pFrameState->getOrigin();
        auto &&rotationalAccelerations = pFrameState->getRotationalAccelerations();
        auto &&rotationalRates = pFrameState->getRotationalRates();
        auto &&velocity = pFrameState->getVelocity();

        stream << " |-" << dashes << "> Frame state: { " << state << " }"
               << std::endl
               << " |--" << dashes << " Frame time (s): { " << pFrameState->getTime() << " }"
               << std::endl
               << " |--" << dashes << " Angle type: { " << std::string(pFrameState->getAngleUnits())
               << " }" << std::endl
               << " |--" << dashes << " Frame origin (x, y, z):" << origin
               << std::endl
               << " |--" << dashes << " Frame velocity (xd, yd, zd):" << velocity
               << std::endl
               << " |--" << dashes << " Frame acceleration (xdd, ydd, zdd):" << acceleration
               << std::endl
               << " |--" << dashes << " Frame orientation (roll, pitch, yaw):" << orientation
               << std::endl
               << " |--" << dashes << " Frame rotational rates (roll, pitch, yaw):" << rotationalRates
               << std::endl
               << " |--" << dashes << " Frame rotational accelerations (roll, pitch, yaw):"
               << rotationalAccelerations << std::endl;
    }

    return stream;
}

/**
 * Function to print the contents of each reference frame along a chain starting at the current object's
 * reference frame and ending at the input reference frame object
 */
std::ostream &ReferenceFrame::printChain(const ReferenceFrame *pFrame,
                                         std::ostream &stream) const
{
    if (pFrame != nullptr)
    {
        auto *pCommonAncestor = const_cast<ReferenceFrame *>(pFrame);
        auto *pThisCommonAncestor = const_cast<ReferenceFrame *>(this)->
                                    findCommonAncestorFrame(pCommonAncestor);
        if (pCommonAncestor != nullptr && pThisCommonAncestor != nullptr)
        {
            auto *pTempFrame = this;
            while (true)
            {
                pTempFrame->print(stream);
                if (pTempFrame == pThisCommonAncestor)
                    break;

                pTempFrame = pTempFrame->getParent();
            }

            pTempFrame = pCommonAncestor;
            while (pTempFrame != pFrame)
            {
                // determine which child frame is an ancestor of the current reference frame
                for (auto *pChild : pTempFrame->m_children)
                {
                    if ((pChild != nullptr && pChild->isAncestor(pFrame)) || pChild == pFrame)
                    {
                        pTempFrame = pChild;
                        break;
                    }
                }

                pTempFrame->print(stream);
            }
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
               "Input reference frame object is null!\n",
               getQualifiedMethodName(__func__));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return stream;
}

/**
 * Function to print the contents of reference frame and its subtree
 */
std::ostream &ReferenceFrame::printTree(std::ostream &stream) const
{
    const auto *pFrame = this;
    const auto *pParentFrame = pFrame->getParent();
    while (true)
    {
        if (pFrame == nullptr)
        {
            pFrame = pParentFrame;
            pParentFrame = pParentFrame->getParent();
            if (pFrame == this || pFrame == nullptr)
                break;
            else
                pFrame = pFrame->getRightSibling();
        }
        else
        {
            pFrame->print(stream);
            pParentFrame = pFrame;
            pFrame = pFrame->getChild();
        }
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool ReferenceFrame::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "referenceFrame") == 0);
    if (bSuccess)
    {
        auto *pRootFrame = ReferenceFrame::create("");
        std::stack<ReferenceFrame *> frames;
        std::stack<xml_node<> *> nodes;
        nodes.push(pNode);
        frames.push(pRootFrame);
        while (bSuccess && !nodes.empty())
        {
            auto *pFrame = frames.top();
            frames.pop();
            pNode = nodes.top();
            nodes.pop();

            auto *pChildReferenceFramesNode = pNode->first_node("childFrames");
            if (pChildReferenceFramesNode != nullptr)
            {
                auto *pChildReferenceFrameNode = pChildReferenceFramesNode->first_node();
                while (pChildReferenceFrameNode != nullptr)
                {
                    nodes.push(pChildReferenceFrameNode);

                    // the name of the child frame should be overwritten once data is read from XML
                    std::string name = "Child Frame";
                    auto *pChildFrame = pFrame->createChild(name);
                    auto &&index = pChildFrame->getChildIndex();
                    pFrame->setName(name + " " + std::to_string(index));
                    frames.push(pChildFrame);

                    pChildReferenceFrameNode = pChildReferenceFrameNode->next_sibling();
                }
            }

            auto *pFrameNameNode = pNode->first_node("name");
            bSuccess = (pFrameNameNode != nullptr);
            if (bSuccess)
                pFrame->m_name = pFrameNameNode->value();
            else
                break;

            auto *pParentFrameNode = pNode->first_node("parentFrame");
            bSuccess = (pParentFrameNode != nullptr);
            if (bSuccess)
            {
                auto *pParentFrame = pFrame->getParent();
                if (pParentFrame != nullptr)
                {
                    bSuccess = (std::strcmp(pParentFrame->m_name.c_str(), pParentFrameNode->value()) == 0);
                    if (!bSuccess)
                    {
                        StaticSynchronizable<MotionState>::lock(0);
                        logMsg(std::cout, LoggingLevel::Enum::Warning,
                               "\"" + std::string(pParentFrameNode->value()) +
                               "\" specified as parent of \"" +
                               pFrame->getName() + "\", but its actual parent is \"" +
                               pParentFrame->getName() + "\".",
                               getQualifiedMethodName(__func__));
                        StaticSynchronizable<MotionState>::unlock(0);
                    }
                }
            }

            if (!bSuccess)
                break;

            auto *pFrameStateTypeNode = pNode->first_node("frameStateType");
            bSuccess = (pFrameStateTypeNode != nullptr);
            if (bSuccess)
                pFrame->setFrameStateType(FrameStateType(pFrameStateTypeNode->value()));

            auto *pFrameStatesNode = pNode->first_node("frameStates");
            bSuccess = (pFrameStatesNode != nullptr);
            if (bSuccess)
            {
                std::size_t index = 0;
                tFrameStates frameStates;
                pFrame->deleteFrameStates();

                auto *pFrameStateNode = pFrameStatesNode->first_node();
                while (pFrameStateNode != nullptr)
                {
                    FrameState *pFrameState = nullptr;
                    auto frameStateType = pFrame->getFrameStateType();
                    if (frameStateType != FrameStateType::Enum::Unknown)
                    {
                        // the name of the frame state should be overwritten once data is read from XML
                        pFrameState = FrameState::create(std::string(frameStateType),
                                                         std::string(DEFAULT_FRAME_STATE) + "_" +
                                                         std::to_string(index));
                        if (pFrameState == nullptr)
                        {
                            StaticSynchronizable<MotionState>::lock(0);
                            logMsg(std::cout, LoggingLevel::Enum::Warning,
                                   "A frame state of type \"" + std::string(frameStateType) +
                                   "\"; the default frame state type will be instantiated.\n",
                                   getQualifiedMethodName(__func__));
                            StaticSynchronizable<MotionState>::unlock(0);
                        }
                    }

                    if (pFrameState == nullptr)
                        pFrameState = ProjectedFrameState::create(std::string(DEFAULT_FRAME_STATE) + "_" +
                                                                  std::to_string(index));

                    bSuccess = (pFrameState != nullptr);
                    if (bSuccess)
                    {
                        bSuccess = pFrameState->readFromXML(pFrameStateNode);
                        auto &&frameStateName = pFrameState->getName();
                        auto &&itFrameState = pFrame->m_frameStates.find(frameStateName);
                        if (itFrameState != pFrame->m_frameStates.end())
                        {
                            if (itFrameState->second != nullptr)
                                delete itFrameState->second;

                            itFrameState->second = pFrameState;
                        }
                        else
                            pFrame->m_frameStates.emplace(std::make_pair(frameStateName, pFrameState));
                    }

                    ++index;
                    pFrameStateNode = pFrameStateNode->next_sibling();
                }
            }
            else
                break;
        }

        if (bSuccess)
            copyTree(pRootFrame);

        ReferenceFrame::deleteTree(pRootFrame);
    }

    return bSuccess;
}
#endif
/**
 * Remove a reference frame as being a child of this frame (doesn't delete the child)
 */
void ReferenceFrame::removeChild(ReferenceFrame *pChildFrame)
{
    auto &&itChild = std::find(m_children.cbegin(), m_children.cend(), pChildFrame);
    if (itChild != m_children.cend())
    {
        pChildFrame->m_pParentFrame = nullptr;
        m_children.erase(itChild);
    }
}

/**
 * Remove a frame state by name
 */
bool ReferenceFrame::removeFrameState(const std::string &state)
{
    auto &&itFrameState = m_frameStates.find(state);
    bool bSuccess = (itFrameState != m_frameStates.cend());
    if (bSuccess)
        m_frameStates.erase(itFrameState);

    return bSuccess;
}

/**
 * Remove a frame state by pointer
 */
bool ReferenceFrame::removeFrameState(FrameState *pFrameState)
{
    bool bSuccess = false;
    auto &&itFrameState = m_frameStates.cbegin();
    while (itFrameState != m_frameStates.cend())
    {
        if (pFrameState == itFrameState->second)
        {
            itFrameState = m_frameStates.erase(itFrameState);
            bSuccess = true;
        }
        else
            ++itFrameState;
    }

    return bSuccess;
}

/**
 * Remove all frame states
 */
void ReferenceFrame::removeFrameStates(void)
{
    m_frameStates.clear();
}

/**
 * Remove a motion state this frame of reference; returns true upon success
 */
bool ReferenceFrame::removeMotionState(const MotionState *pMotionState)
{
    auto &&itMotionState = std::find(m_motionStates.cbegin(),
                                     m_motionStates.cend(),
                                     pMotionState);
    bool bSuccess = (itMotionState != m_motionStates.cend());
    if (bSuccess)
        m_motionStates.erase(itMotionState);

    return bSuccess;
}

/**
 * Function to serialize this object's data
 */
std::ostream &ReferenceFrame::serialize(std::ostream &stream) const
{
    // NOTE: this function does NOT serialize this object's parent/children relationships

    if (stream)
    {
        auto numFrameStates = m_frameStates.size();
        stream.write((const char *)&numFrameStates, sizeof(std::size_t));
        auto &&itFrameState = m_frameStates.cbegin();
        while (itFrameState != m_frameStates.cend())
        {
            auto *pFrameState = itFrameState->second;
            if (pFrameState != nullptr)
                pFrameState->serialize(stream);

            ++itFrameState;
        }

        stream << m_name << '\0';
    }

    return stream;
}

/**
 * Set the acceleration of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setAcceleration(const Vector3d &acceleration,
                                     const std::string &state)
{
    setAcceleration(acceleration[AccelerationAxisType::X],
                    acceleration[AccelerationAxisType::Y],
                    acceleration[AccelerationAxisType::Z],
                    state);
}

/**
 * Set the acceleration of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setAcceleration(double xAcceleration,
                                     double yAcceleration,
                                     double zAcceleration,
                                     const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setAcceleration(xAcceleration,
                                     yAcceleration,
                                     zAcceleration);
}

/**
 * Set angle units (Degrees or Radians)
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setAngleUnits(const AngleUnitType &angleUnits,
                                   const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setAngleUnits(angleUnits);
}

/**
 * Set this object's frame state type
 */
void ReferenceFrame::setFrameStateType(const FrameStateType &frameStateType)
{
    m_frameStateType = frameStateType;
}

/**
 * Set the name of this reference frame
 */
void ReferenceFrame::setName(const std::string &name)
{
    m_name = name;
}

/**
 * Set the orientation angles of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setOrientation(const Eulers &orientation,
                                    const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setOrientation(orientation);
}

/**
 * Set the orientation angles of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setOrientation(double roll,
                                    double pitch,
                                    double yaw,
                                    const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setOrientation(roll,
                                    pitch,
                                    yaw);
}

/**
 * Set the position of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setOrigin(const Vector3d &origin,
                               const std::string &state)
{
    setOrigin(origin[PositionAxisType::X],
              origin[PositionAxisType::Y],
              origin[PositionAxisType::Z],
              state);
}

/**
 * Set the position of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setOrigin(double xPosition,
                               double yPosition,
                               double zPosition,
                               const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setOrigin(xPosition,
                               yPosition,
                               zPosition);
}

/**
 * Set the parent of this frame
 */
void ReferenceFrame::setParent(ReferenceFrame *pParentFrame)
{
    if (pParentFrame != m_pParentFrame)
    {
        if (m_pParentFrame != nullptr)
            m_pParentFrame->removeChild(this);

        if (pParentFrame != nullptr)
            pParentFrame->addChild(this);
    }
}

/**
 * Set this object's pitch angle
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setPitch(double pitch,
                              const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setPitch(pitch, angleUnits, state);
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setPitch(double pitch,
                              const AngleUnitType &angleUnits,
                              const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setPitch(pitch, angleUnits);
}

/**
 * Set this object's pitch acceleration
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setPitchAcceleration(double pitchAcceleration,
                                          const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setPitchAcceleration(pitchAcceleration, angleUnits, state);
}

/**
 * Set this object's pitch acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setPitchAcceleration(double pitchAcceleration,
                                          const AngleUnitType &angleUnits,
                                          const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setPitchAcceleration(pitchAcceleration, angleUnits);
}

/**
 * Set this object's pitch rate
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setPitchRate(double pitchRate,
                                  const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setPitchRate(pitchRate, angleUnits, state);
}

/**
 * Set this object's pitch rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setPitchRate(double pitchRate,
                                  const AngleUnitType &angleUnits,
                                  const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setPitchRate(pitchRate, angleUnits);
}

/**
 * Set this object's roll angle
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setRoll(double roll,
                             const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setRoll(roll, angleUnits, state);
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setRoll(double roll,
                             const AngleUnitType &angleUnits,
                             const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRoll(roll, angleUnits);
}

/**
 * Set this object's roll acceleration
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setRollAcceleration(double rollAcceleration,
                                         const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setRollAcceleration(rollAcceleration, angleUnits, state);
}

/**
 * Set this object's roll acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setRollAcceleration(double rollAcceleration,
                                         const AngleUnitType &angleUnits,
                                         const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRollAcceleration(rollAcceleration, angleUnits);
}

/**
 * Set this object's roll rate
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setRollRate(double rollRate,
                                 const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setRollRate(rollRate, angleUnits, state);
}

/**
 * Set this object's roll rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setRollRate(double rollRate,
                                 const AngleUnitType &angleUnits,
                                 const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRollRate(rollRate, angleUnits);
}

/**
 * Set the rotational accelerations of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setRotationalAccelerations(const Eulers &rotationalAccelerations,
                                                const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRotationalAccelerations(rotationalAccelerations);
}

/**
 * Set the rotational accelerations of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setRotationalAccelerations(double rollAcceleration,
                                                double pitchAcceleration,
                                                double yawAcceleration,
                                                const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRotationalAccelerations(rollAcceleration,
                                                pitchAcceleration,
                                                yawAcceleration);
}

/**
 * Set the rotational rates of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setRotationalRates(const Eulers &rotationalRates,
                                        const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRotationalRates(rotationalRates);
}

/**
 * Set the rotational rates of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setRotationalRates(double rollRate,
                                        double pitchRate,
                                        double yawRate,
                                        const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setRotationalRates(rollRate,
                                        pitchRate,
                                        yawRate);
}

/**
 * Set the time at which this frame is currently defined
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setTime(double t,
                             const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setTime(t);
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setVelocity(const Vector3d &velocity,
                                 const std::string &state)
{
    setVelocity(velocity[VelocityAxisType::X],
                velocity[VelocityAxisType::Y],
                velocity[VelocityAxisType::Z],
                state);
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 * @param state the desired perturbation state of the reference frame
 */
void ReferenceFrame::setVelocity(double xVelocity,
                                 double yVelocity,
                                 double zVelocity,
                                 const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setVelocity(xVelocity,
                                 yVelocity,
                                 zVelocity);
}

/**
 * Set this object's yaw angle
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setYaw(double yaw,
                            const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setYaw(yaw, angleUnits, state);
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setYaw(double yaw,
                            const AngleUnitType &angleUnits,
                            const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setYaw(yaw, angleUnits);
}

/**
 * Set this object's yaw acceleration
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setYawAcceleration(double yawAcceleration,
                                        const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setYawAcceleration(yawAcceleration, angleUnits, state);
}

/**
 * Set this object's yaw acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setYawAcceleration(double yawAcceleration,
                                        const AngleUnitType &angleUnits,
                                        const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setYawAcceleration(yawAcceleration, angleUnits);
}

/**
 * Set this object's yaw rate
 * @param state the desired perturbation status of this reference frame
 */
void ReferenceFrame::setYawRate(double yawRate,
                                const std::string &state)
{
    auto &&angleUnits = getAngleUnits(state);

    setYawRate(yawRate, angleUnits, state);
}

/**
 * Set this object's yaw rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 * @param state      the desired perturbation status of this reference frame
 */
void ReferenceFrame::setYawRate(double yawRate,
                                const AngleUnitType &angleUnits,
                                const std::string &state)
{
    auto *pFrameState = findFrameState(state);
    if (pFrameState == nullptr)
        pFrameState = createFrameState(state);

    if (pFrameState != nullptr)
        pFrameState->setYawRate(yawRate, angleUnits);
}

/**
 * Return whether or not the current object and input frame share a common branch
 */
bool ReferenceFrame::sharesCommonBranch(ReferenceFrame *pFrame)
{
    bool bSuccess = (pFrame != nullptr);
    if (bSuccess)
    {
        bSuccess = isAncestor(pFrame);
        if (!bSuccess)
            bSuccess = isDescendant(pFrame);
    }

    return bSuccess;
}

/**
 * Static initialization function
 */
bool ReferenceFrame::staticInitialize(void)
{
    auto &&referenceFrames = garbageCollector.getObjects();
    if (!referenceFrames.empty())
    {
        std::string listOfFrames;
        auto &&itReferenceFrame = referenceFrames.cbegin();
        while (itReferenceFrame != referenceFrames.cend())
        {
            auto *pReferenceFrame = *itReferenceFrame;
            if (pReferenceFrame != nullptr)
            {
                listOfFrames += pReferenceFrame->getName();
                listOfFrames += " @ t = " + std::to_string(pReferenceFrame->getTime());
                listOfFrames += "\n";
            }

            ++itReferenceFrame;
        }

        StaticSynchronizable<MotionState>::lock(0);
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "The following frames will be deleted:\n" + listOfFrames,
               getQualifiedStaticMethodName(__func__, ReferenceFrame));
        StaticSynchronizable<MotionState>::unlock(0);
    }

    return true;
}

/**
 * Swap function
 */
void ReferenceFrame::swap(ReferenceFrame &frame)
{
    Loggable<std::string, std::ostream>::swap(frame);

    m_children.swap(frame.m_children);
    m_frameStates.swap(frame.m_frameStates);
    std::swap(m_frameStateType, frame.m_frameStateType);
    m_motionStates.swap(frame.m_motionStates);
    m_name.swap(frame.m_name);
    std::swap(m_pParentFrame, frame.m_pParentFrame);
}

/**
 * Transform the input motion state from the parent frame to a child frame of reference
 * @param pCartesianMotionState the motion state to be transformed
 * @param t                     the time to which the frame will be projected
 * @param state                 the desired perturbation state of the reference frame
 */
bool ReferenceFrame::transformToChild(CartesianMotionState *pCartesianMotionState,
                                      double t,
                                      const std::string &state) const
{
    bool bSuccess = (pCartesianMotionState != nullptr);
    if (bSuccess)
    {
        Quat frameOrientationQuat(1.0);
        auto &&orientation = getOrientation(t, state);
        bool bFrameHasOrientation = (orientation != 0.0);
        if (bFrameHasOrientation)
            frameOrientationQuat = orientation.calcQuaternion(RotationType::Passive);

        // Note that the position, velocity and acceleration are intentionally accessed in this way so that
        // we can get a reference to each 3d vector
        auto &acceleration = (*pCartesianMotionState)[StateDerivativeType::Acceleration];
        auto &position = (*pCartesianMotionState)[StateDerivativeType::Position];
        auto &velocity = (*pCartesianMotionState)[StateDerivativeType::Velocity];

        // compute frame angular acceleration and velocity
        auto &&frameAngularAcceleration = calcAngularAcceleration(t, state);
        auto &&frameAngularVelocity = calcAngularVelocity(t, state);

        // flags to indicate whether or not transformations involving
        // angular acceleration and velocity will need to be performed
        bool bFrameHasAngularAcceleration = (frameAngularAcceleration != 0.0);
        bool bFrameHasAngularVelocity = (frameAngularVelocity != 0.0);

        // shift position, velocity, acceleration by frame origin, velocity and acceleration
        acceleration -= getAcceleration(state);
        position -= getOrigin(t, state);
        velocity -= getVelocity(t, state);

        // apply rotation to acceleration, position, and velocity if the frame has orientation
        if (bFrameHasOrientation)
        {
            if (acceleration != 0.0)
                acceleration.rotate(frameOrientationQuat);

            if (position != 0.0)
                position.rotate(frameOrientationQuat);

            if (velocity != 0.0)
                velocity.rotate(frameOrientationQuat);
        }

        if (bFrameHasAngularVelocity)
        {
            if (position != 0.0)
            {
                // compute tangential velocity
                auto &&tangentialVelocity = frameAngularVelocity.calcCross(position);

                // apply tangential velocity
                velocity += tangentialVelocity;

                // compute centrifugal acceleration
                auto &&centrifugalAcceleration = -frameAngularVelocity.calcCross(tangentialVelocity);

                // apply centrifugal acceleration
                acceleration += centrifugalAcceleration;
            }

            if (velocity != 0.0)
            {
                // compute Coriolis acceleration
                auto &&coriolisAcceleration = 2 * frameAngularVelocity.calcCross(velocity);

                // apply Coriolis acceleration
                acceleration += coriolisAcceleration;
            }
        }

        if (bFrameHasAngularAcceleration && position != 0.0)
        {
            // compute Euler acceleration
            auto &&eulerAcceleration = frameAngularAcceleration.calcCross(position);

            // apply Euler acceleration
            acceleration += eulerAcceleration;
        }

        // get references to the motion state's Eulers, Euler rates, and Euler accelerations
        // prior to transformation
        auto &bodyEulers = (*pCartesianMotionState)[EulerStateDerivativeType::Eulers];
        auto &bodyEulerRates = (*pCartesianMotionState)[EulerStateDerivativeType::Rates];
        auto &bodyEulerAccelerations = (*pCartesianMotionState)[EulerStateDerivativeType::Accelerations];

        Quat bodyOrientQuat(1.0);
        auto &&angleUnits = pCartesianMotionState->getAngleUnits();
        bool bBodyHasOrientation = (bodyEulers != 0.0);
        if (bBodyHasOrientation)
            bodyOrientQuat = bodyEulers.calcQuaternion(RotationType::Active);

        // calculate body angular velocity
        Vector3d bodyAngularVelocity;
        bool bBodyHasAngularVelocity = (bodyEulerRates != 0.0);
        if (bBodyHasAngularVelocity)
            bodyAngularVelocity = bodyEulers.calcBodyRates(bodyEulerRates);

        // calculate body angular acceleration
        Vector3d bodyAngularAcceleration;
        bool bBodyHasAngularAcceleration = (bodyEulerAccelerations != 0.0);
        if (bBodyHasAngularVelocity || bBodyHasAngularAcceleration)
            bodyAngularAcceleration = bodyEulers.calcBodyAccelerations(bodyEulerRates,
                                                                       bodyEulerAccelerations);

        // calculate motion state body Eulers in the child frame
        if (bFrameHasOrientation)
        {
            if (bBodyHasOrientation)
                bodyOrientQuat = frameOrientationQuat * bodyOrientQuat;
            else
                bodyOrientQuat = frameOrientationQuat;

            bodyEulers = bodyOrientQuat.calcEulers(angleUnits,
                                                   RotationType::Active);
        }

        // rotate the frame angular velocity and acceleration
        bBodyHasOrientation = (bodyEulers != 0.0);
        if (bBodyHasOrientation)
        {
            if (bFrameHasAngularVelocity)
                frameAngularVelocity.rotate(bodyOrientQuat);

            if (bFrameHasAngularAcceleration)
                frameAngularAcceleration.rotate(bodyOrientQuat);
        }

        // calculate motion state body angular acceleration in child frame
        bodyAngularAcceleration += frameAngularAcceleration;
//      bFrameHasAngularAcceleration = (frameAngularAcceleration != 0.0);
        if (bFrameHasAngularVelocity && bBodyHasAngularVelocity)
            bodyAngularAcceleration += frameAngularVelocity.calcCross(bodyAngularVelocity);

        // calculate motion state body angular velocity in child frame
        bodyAngularVelocity += frameAngularVelocity;

        // calculate motion state body Euler rates in the child frame
        bBodyHasAngularVelocity = (bodyAngularVelocity != 0.0);
        if (bBodyHasAngularVelocity)
        {
            bodyEulerRates = bodyEulers.calcEulerRates(bodyAngularVelocity,
                                                       angleUnits);
        }

        // calculator motion state body Euler accelerations in the child frame
        if (bBodyHasAngularVelocity || bodyAngularAcceleration != 0.0)
            bodyEulerAccelerations = bodyEulers.calcEulerAccelerations(bodyAngularVelocity,
                                                                       bodyAngularAcceleration,
                                                                       angleUnits);

        // update the motion state's frame of reference
        auto *pFrame = const_cast<ReferenceFrame *>(this);
        pCartesianMotionState->setFrame(pFrame);
    }

    return bSuccess;
}

/**
 * Spatially (and optionally temporally) transform the input motion state defined in the given input reference
 * frame to this object's frame of reference (by default, the transformation is spatial only)
 * @param pMotionState the motion state to be transformed
 * @param bTemporal    flag indicating whether or not a temporal transformation will take place. If false, a
 *                     spatial-only transformation will occur (time at which frames and motion state are
 *                     defined are not considered). If true, the motion state and all frames through which it
 *                     is transformed will be projected to the time at which the target frame is defined.
 * @param state        the desired perturbation state of the reference frame
 */
bool ReferenceFrame::transformToFrame(MotionState *pMotionState,
                                      bool bTemporal,
                                      const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return transformToFrame(pMotionState,
                                pFrameState->getTime(),
                                bTemporal,
                                state);

    return false;
}

/**
 * Spatially (and optionally temporally) transform the input motion state defined in the given input reference
 * frame to this object's frame of reference (by default, the transformation is spatial only)
 * @param pMotionState the motion state to be transformed
 * @param state        the desired perturbation state of the reference frame
 * @param bTemporal    flag indicating whether or not a temporal transformation will take place. If false, a
 *                     spatial-only transformation will occur (time at which frames and motion state are
 *                     defined are not considered). If true, the motion state and all frames through which it
 *                     is transformed will be projected to the time at which the target frame is defined.
 */
bool ReferenceFrame::transformToFrame(MotionState *pMotionState,
                                      const std::string &state,
                                      bool bTemporal) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return transformToFrame(pMotionState,
                                pFrameState->getTime(),
                                bTemporal,
                                state);

    return false;
}

/**
 * Spatially and temporally transform the input motion state defined in the given input reference frame to this
 * object's frame of reference. Since this transformation is temporal, the motion state and all frames through
 * which it is transformed will be projected to the specified time
 * @param pMotionState the motion state to be transformed
 * @param t            the time to which the frames and motion states will be projected
 * @param state        the desired perturbation state of the reference frame
 */
bool ReferenceFrame::transformToFrame(MotionState *pMotionState,
                                      double t,
                                      const std::string &state) const
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        return transformToFrame(pMotionState,
                                t,
                                true,
                                state);

    return false;
}

/**
 * Spatially (and optionally temporally) transform the input motion state defined in the given input reference
 * frame to this object's frame of reference
 * @param pMotionState the motion state to be transformed
 * @param t            the time to which the frames and motion states will be projected if performing a
 *                     temporal transformation
 * @param bTemporal    flag indicating whether or not a temporal transformation will take place. If false, a
 *                     spatial-only transformation will occur (time at which frames and motion state are
 *                     defined is not considered). If true, the motion state and all frames through which it is
 *                     transformed will be projected to the specified time
 * @param state        the desired perturbation state of the reference frame
 */
bool ReferenceFrame::transformToFrame(MotionState *pMotionState,
                                      double t,
                                      bool bTemporal,
                                      const std::string &state) const
{
    bool bSuccess = true;
    if (pMotionState != nullptr)
    {
        bool bTransformCachingEnabled = pMotionState->cacheTransformationsEnabled();
        auto *pMotionStateFrame = pMotionState->getFrame();
        if (bTransformCachingEnabled && pMotionStateFrame != nullptr)
            applyCachedTransformation(pMotionState);

        pMotionStateFrame = pMotionState->getFrame();
        bSuccess = isFamily(pMotionStateFrame);
        if (bSuccess)
        {
            // update the motion state to time t
            if (bTemporal && pMotionState->getTime() != t)
            {
                auto dt = t - pMotionState->getTime();
                pMotionState->update(dt);
            }

            // perform a quick check to see if we need to proceed...
            if (pMotionStateFrame == this)
                return bSuccess;

            if (bTransformCachingEnabled)
                pMotionState->cacheCurrentState();

            CartesianMotionState *pCartesianMotionState = nullptr;
            if (pMotionState->isCartesian())
                pCartesianMotionState = dynamic_cast<CartesianMotionState *>(pMotionState);
            else
                pCartesianMotionState = new CartesianMotionState(*pMotionState);

            if (pCartesianMotionState->debugTransformsEnabled())
            {
                // print the transformation chain through which the motion state will be transformed
                pMotionStateFrame->printChain(this, std::cout);

                // print the current contents of the motion state if transform debugging is enabled
                pCartesianMotionState->print(std::cout) << std::endl;
            }

            // loop until we are at the target frame
            do
            {
                if (pMotionStateFrame == this)
                    break;
                else if (pMotionStateFrame == nullptr)
                {
                    StaticSynchronizable<MotionState>::lock(0);
                    logMsg(std::cout, LoggingLevel::Enum::Warning,
                           "ReferenceFrame was not assigned to the current MotionState.\n",
                           getQualifiedMethodName(__func__));
                    StaticSynchronizable<MotionState>::unlock(0);

                    bSuccess = false; // don't return false here, we have to free
                                      // the motion state that may have been allocated
                    continue;
                }
                else
                {
                    bool bMotionStateFrameIsAncestor = pMotionStateFrame->isAncestor(this);
                    if (bMotionStateFrameIsAncestor)
                    {
                        // determine which child frame is an ancestor of the current object's reference frame
                        for (auto *pChild : pMotionStateFrame->m_children)
                        {
                            if ((pChild != nullptr && pChild->isAncestor(this)) || pChild == this)
                            {
                                pMotionStateFrame = pChild;
                                break;
                            }
                        }
                    }

                    if (!bTemporal)
                        t = pMotionStateFrame->getTime(state);

                    // transform into or out of the current frame
                    if (!bMotionStateFrameIsAncestor && pMotionStateFrame != this)
                        bSuccess = pMotionStateFrame->transformToParent(pCartesianMotionState, t, state);
                    else if (bMotionStateFrameIsAncestor)
                        bSuccess = pMotionStateFrame->transformToChild(pCartesianMotionState, t, state);

                    if (pCartesianMotionState->debugTransformsEnabled())
                    {
                        // print the current contents of the motion state if transform debugging is enabled
                        pCartesianMotionState->print(std::cout) << std::endl;
                    }
                }

                pMotionStateFrame = pCartesianMotionState->getFrame();
            }
            while (bSuccess);

            if (bTransformCachingEnabled)
                pMotionState->cacheCurrentState();

            if (pMotionState != pCartesianMotionState)
            {
                // need to copy the motion state, it was dynamically allocated...
                pMotionState->assign(pCartesianMotionState);

                delete pCartesianMotionState;
            }
        }
        else
        {
            StaticSynchronizable<MotionState>::lock(0);
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "The current object's frame of reference is not related to the input motion "
                   "state's frame of reference (they do not exist within the same tree); "
                   "the transformation could not be performed.\n",
                   getQualifiedMethodName(__func__));
            StaticSynchronizable<MotionState>::unlock(0);
        }
    }

    return bSuccess;
}

/**
 * Transform the input motion state from the child frame to the parent frame of reference
 * @param pCartesianMotionState the motion state to be transformed
 * @param t                     the time to which the frame will be projected
 * @param state                 the desired perturbation state of the reference frame
 */
bool ReferenceFrame::transformToParent(CartesianMotionState *pCartesianMotionState,
                                       double t,
                                       const std::string &state) const
{
    bool bSuccess = (pCartesianMotionState != nullptr);
    if (bSuccess)
    {
        Quat conjFrameOrientationQuat(1.0);
        auto &&orientation = getOrientation(t, state);
        bool bFrameHasOrientation = (orientation != 0.0);
        if (bFrameHasOrientation)
            conjFrameOrientationQuat = orientation.calcQuaternion(RotationType::Active);

        // either the input motion state is defined in a frame of reference that is a descendant frame
        // of the current object's frame of reference, or we have to transform into input motion state's
        // parent frame and again test for ascendancy/descendancy. Note that the position, velocity and
        // acceleration are intentionally accessed in this way so that we can get a reference to each 3d vector
        auto &acceleration = (*pCartesianMotionState)[StateDerivativeType::Acceleration];
        auto &position = (*pCartesianMotionState)[StateDerivativeType::Position];
        auto &velocity = (*pCartesianMotionState)[StateDerivativeType::Velocity];

        // compute frame angular acceleration and velocity
        auto &&frameAngularAcceleration = calcAngularAcceleration(t, state);
        auto &&frameAngularVelocity = calcAngularVelocity(t, state);

        // flags to indicate whether or not transformations involving
        // angular acceleration and velocity will need to be performed
        bool bFrameHasAngularAcceleration = (frameAngularAcceleration != 0.0);
        bool bFrameHasAngularVelocity = (frameAngularVelocity != 0.0);

        // apply rotation to acceleration, angular acceleration, angular
        // velocity, position, and velocity if the frame has orientation
        if (bFrameHasOrientation)
        {
            if (acceleration != 0.0)
                acceleration.rotate(conjFrameOrientationQuat);

            if (position != 0.0)
                position.rotate(conjFrameOrientationQuat);

            if (velocity != 0.0)
                velocity.rotate(conjFrameOrientationQuat);
        }

        if (bFrameHasAngularVelocity)
        {
            auto rotatedFrameAngularVelocity = frameAngularVelocity;
            if (bFrameHasOrientation)
                rotatedFrameAngularVelocity.rotate(conjFrameOrientationQuat);

            bool bRotatedFrameHasAngularVelocity = (rotatedFrameAngularVelocity != 0.0);
            if (bRotatedFrameHasAngularVelocity)
            {
                if (position != 0.0)
                {
                    // compute tangential velocity
                    auto &&tangentialVelocity = rotatedFrameAngularVelocity.calcCross(position);

                    // apply tangential velocity
                    velocity -= tangentialVelocity;

                    // compute centrifugal acceleration
                    auto &&centrifugalAcceleration = -rotatedFrameAngularVelocity.calcCross(tangentialVelocity);

                    // apply centrifugal acceleration
                    acceleration += centrifugalAcceleration;
                }

                if (velocity != 0.0)
                {
                    // compute Coriolis acceleration
                    auto &&coriolisAcceleration = -2 * rotatedFrameAngularVelocity.calcCross(velocity);

                    // apply Coriolis acceleration
                    acceleration += coriolisAcceleration;
                }
            }
        }

        if (bFrameHasAngularAcceleration)
        {
            auto rotatedFrameAngularAcceleration = frameAngularAcceleration;
            if (bFrameHasOrientation)
                rotatedFrameAngularAcceleration.rotate(conjFrameOrientationQuat);

            if (position != 0.0 && rotatedFrameAngularAcceleration != 0.0)
            {
                // compute Euler acceleration
                auto &&eulerAcceleration = -rotatedFrameAngularAcceleration.calcCross(position);

                // apply Euler acceleration
                acceleration += eulerAcceleration;
            }
        }

        // shift position, velocity, acceleration by frame origin, velocity and acceleration
        acceleration += getAcceleration(state);
        position += getOrigin(t, state);
        velocity += getVelocity(t, state);

        // get references to the motion state's Eulers, Euler rates, and Euler accelerations
        // prior to transformation
        auto &bodyEulers = (*pCartesianMotionState)[EulerStateDerivativeType::Eulers];
        auto &bodyEulerRates = (*pCartesianMotionState)[EulerStateDerivativeType::Rates];
        auto &bodyEulerAccelerations = (*pCartesianMotionState)[EulerStateDerivativeType::Accelerations];

        Quat bodyOrientQuat(1.0);
        auto &&angleUnits = pCartesianMotionState->getAngleUnits();
        bool bBodyHasOrientation = (bodyEulers != 0.0);
        if (bBodyHasOrientation)
            bodyOrientQuat = bodyEulers.calcQuaternion(RotationType::Active);

        // calculate body angular velocity
        Vector3d bodyAngularVelocity;
        bool bBodyHasAngularVelocity = (bodyEulerRates != 0.0);
        if (bBodyHasAngularVelocity)
            bodyAngularVelocity = bodyEulers.calcBodyRates(bodyEulerRates);

        // calculate body angular acceleration
        Vector3d bodyAngularAcceleration;
        bool bBodyHasAngularAcceleration = (bodyEulerAccelerations != 0.0);
        if (bBodyHasAngularVelocity || bBodyHasAngularAcceleration)
            bodyAngularAcceleration = bodyEulers.calcBodyAccelerations(bodyEulerRates,
                                                                       bodyEulerAccelerations);

        // rotate the body angular velocity and acceleration
        if (bBodyHasOrientation)
        {
            if (bFrameHasAngularVelocity)
                frameAngularVelocity.rotate(bodyOrientQuat);

            if (bFrameHasAngularAcceleration)
                frameAngularAcceleration.rotate(bodyOrientQuat);
        }

        // calculate motion state body angular acceleration in parent frame
        bodyAngularAcceleration -= frameAngularAcceleration;
        if (bFrameHasAngularVelocity && bBodyHasAngularVelocity)
            bodyAngularAcceleration -= frameAngularVelocity.calcCross(bodyAngularVelocity);

        // calculate motion state body angular velocity in parent frame
        bodyAngularVelocity -= frameAngularVelocity;

        // calculate motion state body Eulers in the parent frame
        if (bFrameHasOrientation)
        {
            if (bBodyHasOrientation)
                bodyOrientQuat = conjFrameOrientationQuat * bodyOrientQuat;
            else
                bodyOrientQuat = conjFrameOrientationQuat;

            bodyEulers = bodyOrientQuat.calcEulers(angleUnits,
                                                   RotationType::Active);
        }
        else if (bBodyHasOrientation)
            bodyEulers -= orientation;

        // calculate motion state body Euler rates in the child frame
        if (true || bBodyHasAngularVelocity) // TODO: this test may not be correct
        {
            bodyEulerRates = bodyEulers.calcEulerRates(bodyAngularVelocity,
                                                       angleUnits);
        }
        else if (bFrameHasAngularVelocity)
            bodyEulerRates -= getRotationalRates(t, state);

        // calculator motion state body Euler accelerations in the child frame
        if (true || bBodyHasAngularVelocity || bBodyHasAngularAcceleration) // TODO: this test is not correct
            bodyEulerAccelerations = bodyEulers.calcEulerAccelerations(bodyAngularVelocity,
                                                                       bodyAngularAcceleration,
                                                                       angleUnits);
        else if (bFrameHasAngularAcceleration)
            bodyEulerAccelerations -= getRotationalAccelerations(state);

        // update the motion state's frame of reference
        pCartesianMotionState->setFrame(m_pParentFrame);
    }

    return bSuccess;
}

/**
 * Update this reference frame to the specified time. This function will update all quantities within the frame
 * that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param state         the desired perturbation state of the reference frame
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void ReferenceFrame::update(double t,
                            const std::string &state,
                            const TimeReferenceType &timeReference)
{
    update(t, timeReference, state);
}

/**
 * Update this reference frame to the specified time. This function will update all quantities within the frame
 * that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 * @param state         the desired perturbation state of the reference frame
 */
void ReferenceFrame::update(double t,
                            const TimeReferenceType &timeReference,
                            const std::string &state)
{
    auto *pFrameState = getFrameState(state);
    if (pFrameState != nullptr)
        pFrameState->update(t, timeReference);
}

/**
 * Update all reference frames in the current frame's subtree to the specified time. This function will update
 * all quantities within each frame that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param state         the desired perturbation state of the reference frame
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void ReferenceFrame::updateAll(double t,
                               const std::string &state,
                               const TimeReferenceType &timeReference)
{
    updateAll(t, timeReference, state);
}

/**
 * Update all reference frames in the current frame's subtree to the specified time. This function will update
 * all quantities within each frame that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 * @param state         the desired perturbation state of the reference frame
 */
void ReferenceFrame::updateAll(double t,
                               const TimeReferenceType &timeReference,
                               const std::string &state)
{
    auto *pThisParent = getParent();
    auto *pFrame = this;
    do
    {
        pFrame->update(t, timeReference, state);

        auto *pChildFrame = pFrame->getChild();
        if (pChildFrame == nullptr)
        {
            do
            {
                pChildFrame = pFrame->getRightSibling();
                pFrame = pFrame->getParent();
            }
            while (pChildFrame == nullptr && pFrame != pThisParent);
        }

        pFrame = pChildFrame;
        if (pFrame == pThisParent)
            break;
    }
    while (pFrame != nullptr);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool ReferenceFrame::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "referenceFrame") == 0);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            std::map<ReferenceFrame *, xml_node<> *> frameNodeMap;
            auto *pThisParent = getParent();
            auto *pFrame = this;
            do
            {
                xml_node<> *pFrameNode = nullptr;
                auto *pParentFrame = pFrame->getParent();
                if (pThisParent != pParentFrame)
                {
                    pFrameNode = pDocument->allocate_node(node_element, "referenceFrame");
                    pNode->append_node(pFrameNode);
                }
                else
                    pFrameNode = pNode;

                if (bSuccess)
                {
                    auto *pFrameNameNode = pDocument->allocate_node(node_element, "name");
                    auto *pDataNode = pDocument->allocate_node(node_data, pFrame->m_name.c_str());
                    pFrameNameNode->append_node(pDataNode);
                    pFrameNode->append_node(pFrameNameNode);
                }

                xml_node<> *pFrameParentNode = nullptr;
                if (bSuccess)
                {
                    pFrameParentNode = pDocument->allocate_node(node_element, "parentFrame");
                    auto &&parentName = (pParentFrame != nullptr) ? pParentFrame->m_name : std::string("null");
                    auto *pFrameParentString = pDocument->allocate_string(parentName.c_str());
                    auto *pDataNode = pDocument->allocate_node(node_data, pFrameParentString);
                    pFrameParentNode->append_node(pDataNode);
                    pFrameNode->append_node(pFrameParentNode);
                }

                if (bSuccess)
                {
                    auto *pFrameStatesNode = pDocument->allocate_node(node_element, "frameStates");
                    auto &&itFrameState = pFrame->m_frameStates.cbegin();
                    while (bSuccess && itFrameState != pFrame->m_frameStates.cend())
                    {
                        FrameState *pFrameState = itFrameState->second;
                        bSuccess = (pFrameState != nullptr);
                        if (bSuccess)
                        {
                            auto *pFrameStateNode = pDocument->allocate_node(node_element, "frameState");
                            bSuccess = pFrameState->writeToXML(pFrameStateNode);
                            pFrameStatesNode->append_node(pFrameStateNode);
                        }

                        ++itFrameState;
                    }

                    pFrameNode->append_node(pFrameStatesNode);
                }

                if (bSuccess)
                {
                    frameNodeMap.emplace(std::make_pair(pFrame, pFrameNode));
                    pFrameParentNode = frameNodeMap[pParentFrame];
                    if (pParentFrame != pThisParent)
                    {
                        auto *pChildReferenceFramesNode = pFrameParentNode->first_node("childFrames");
                        if (pChildReferenceFramesNode != nullptr)
                        {
                            pChildReferenceFramesNode = pDocument->allocate_node(node_element, "cildFrame");
                            pFrameParentNode->append_node(pChildReferenceFramesNode);
                        }

                        pChildReferenceFramesNode->append_node(pFrameNode);
                    }
                }

                if (bSuccess)
                {
                    auto *pChildFrame = pFrame->getChild();
                    if (pChildFrame == nullptr)
                    {
                        do
                        {
                            pChildFrame = pFrame->getRightSibling();
                            pFrame = pFrame->getParent();
                        }
                        while (pChildFrame == nullptr && pFrame != pThisParent);
                    }

                    pFrame = pChildFrame;
                    if (pFrame == pThisParent)
                        break;
                }
            }
            while (bSuccess && pFrame != nullptr);
        }
    }

    return bSuccess;
}
#endif
/**
 * Zero this reference frame's rotational orientation and associated derivatives
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::zeroRotation(const std::string &state)
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
    {
        pFrameState->setOrientation(0.0, 0.0, 0.0);
        pFrameState->setRotationalAccelerations(0.0, 0.0, 0.0);
        pFrameState->setRotationalRates(0.0, 0.0, 0.0);
    }

    return bSuccess;
}

/**
 * Zero this reference frame's rotational orientation and associated derivatives
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::zeroRotationalDerivatives(const std::string &state)
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
    {
        pFrameState->setRotationalAccelerations(0.0, 0.0, 0.0);
        pFrameState->setRotationalRates(0.0, 0.0, 0.0);
    }

    return bSuccess;
}

/**
 * Zero this reference frame's translational components and associated derivatives
 * @param state the desired perturbation state of the reference frame
 */
bool ReferenceFrame::zeroTranslation(const std::string &state)
{
    auto *pFrameState = getFrameState(state);
    bool bSuccess = (pFrameState != nullptr);
    if (bSuccess)
    {
        pFrameState->setAcceleration(0.0, 0.0, 0.0);
        pFrameState->setOrigin(0.0, 0.0, 0.0);
        pFrameState->setVelocity(0.0, 0.0, 0.0);
    }

    return bSuccess;
}

}

}
