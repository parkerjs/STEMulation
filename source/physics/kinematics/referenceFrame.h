#ifndef REFERENCE_FRAME_H
#define REFERENCE_FRAME_H

#include "coordinate_type.h"
#include "export_library.h"
#include "frame_state_type.h"
#include "loggable.h"
#include "nameable.h"
#include "output_streamable.h"
#include "reflective.h"
#include "serializable.h"
#include "static_initializable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "swappable.h"
#include "time_reference_type.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#include "xml_writable.h"
#endif
#include <map>
#include <memory>
#include <vector>

// if a default frame state is not specified in preprocessor configuration, then define the following
#ifndef DEFAULT_FRAME_STATE
#define DEFAULT_FRAME_STATE "default"
#endif

// forward declarations
namespace math { namespace geometric { namespace orientation { class Eulers; } }
                 namespace linear_algebra { namespace vector { class Vector3d; } }
                 namespace trigonometric { struct AngleUnitType; } }
namespace memory { template<typename> class GarbageCollector; }
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace physics
{

namespace kinematics
{

// forward declarations
class CartesianMotionState;
class FrameState;
class MotionState;

/**
 * This class is used for performing transformations of physical rigid bodies between frames of reference(s).
 * Each frame of reference is defined with an origin, velocity, acceleration and orientation with respect to
 * its parent frame. In addition, each frame of reference can rotate with respect to its parent frame with
 * given angular rates and/or angular accelerations about its local axis of rotation.
 */
class ReferenceFrame
: public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual attributes::interfaces::Nameable,
  public attributes::concrete::OutputStreamable<ReferenceFrame>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
  public attributes::concrete::StaticInitializable<ReferenceFrame>,
  public attributes::concrete::StaticMutexMappable<MotionState, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<MotionState>,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<ReferenceFrame>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<ReferenceFrame>
#endif
{
public:

    /**
     * Friend class declarations
     */
    template<typename> friend class memory::GarbageCollector;
    friend class MotionState;

    /**
     * Type alias declarations
     */
    using AngleUnitType = math::trigonometric::AngleUnitType;
    using Eulers = math::geometric::orientation::Eulers;
    using Vector3d = math::linear_algebra::vector::Vector3d;

    /**
     * Typedef declarations
     */
    typedef std::vector<ReferenceFrame *>::iterator iterator;
    typedef std::map<std::string, FrameState *> tFrameStates;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

private:

    /**
     * Constructor
     */
    EXPORT_STEM ReferenceFrame(void);

    /**
     * Construct a ReferenceFrame defined with respect to the specified parent frame
     * @param pParentFrame a pointer to the parent frame
     */
    EXPORT_STEM ReferenceFrame(ReferenceFrame *pParentFrame);

    /**
     * Construct a ReferenceFrame defined with respect to the specified parent frame
     * @param pParentFrame a pointer to the parent frame
     * @param state        the perturbation state of the frame state to be created
     */
    EXPORT_STEM ReferenceFrame(ReferenceFrame *pParentFrame,
                               const std::string &state);

    /**
     * Construct a ReferenceFrame defined with respect to the specified parent frame
     * @param pParentFrame   a pointer to the parent frame
     * @param frameStateType the type of frame state to be instantiated
     * @param state          the perturbation state of the frame state to be created
     */
    EXPORT_STEM ReferenceFrame(ReferenceFrame *pParentFrame,
                               FrameStateType &&frameStateType,
                               const std::string &state);

    /**
     * Copy constructor
     */
    EXPORT_STEM ReferenceFrame(const ReferenceFrame &frame);

    /**
     * Move constructor
     */
    EXPORT_STEM ReferenceFrame(ReferenceFrame &&frame);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~ReferenceFrame(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM ReferenceFrame &operator = (const ReferenceFrame &frame);

    /**
     * Move assignment operator
     */
    EXPORT_STEM ReferenceFrame &operator = (ReferenceFrame &&frame);

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const ReferenceFrame &frame);

private:

    /**
     * Add a reference frame as being a child of this frame
     */
    EXPORT_STEM virtual bool addChild(ReferenceFrame *pChildFrame) final;

    /**
     * Add a motion state to this frame of reference; returns true upon success
     */
    EXPORT_STEM virtual bool addMotionState(MotionState *pMotionState) final;

    /**
     * Function to find a previously computed transformation from the motion state's transformation cache (if
     * it exists) and apply it to the motion state's current kinematic state
     */
    EXPORT_STEM virtual bool applyCachedTransformation(MotionState *pMotionState) const final;

    /**
     * Calculate this object's instantaneous angular acceleration vector at the specified time with respect to
     * the parent frame given this object's orientation angles, rotational rates and rotational acceleration.
     * The resultant angular acceleration vector is described in the current object's frame of reference
     * @param t     the time to which the frames and motion states will be projected
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d calcAngularAcceleration(double t,
                                                         const std::string &state =
                                                         DEFAULT_FRAME_STATE) const final;

    /**
     * Calculate this object's instantaneous angular velocity vector at the specified time with respect to the
     * parent frame given this object's orientation angles and rotational rates. The resultant angular velocity
     * vector is described in the current object's frame of reference
     * @param t     the time to which the frames and motion states will be projected
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d calcAngularVelocity(double t,
                                                     const std::string &state =
                                                     DEFAULT_FRAME_STATE) const final;

public:

    /**
     * Function to clone a branch starting with the current object and ending with the specified input
     * descendant frame; in order for the clone to succeed, the current object must be an ancestor frame of the
     * input descendant frame (the function tests the descendant frame by name or pointer value to make the
     * determination). Upon success, the function returns a non-null pointer to the root of the newly cloned
     * branch
     * @param pDescendantFrame the descendant frame which marks the end of the branch
     */
    EXPORT_STEM virtual ReferenceFrame *cloneBranch(ReferenceFrame *pDescendantFrame) const final;

    /**
     * Function to clone the current frame (not the child or parent relationships)
     */
    EXPORT_STEM virtual ReferenceFrame *cloneFrame(void) const final;

    /**
     * Function to create a shared clone of the branch starting with the current object and ending with the
     * specified input descendant frame; in order for the clone to succeed, the current object must be an
     * ancestor frame of the input descendant frame (the function tests the descendant frame by name or pointer
     * value to make the determination). Upon success, the function returns a non-null pointer to the root of
     * the newly cloned branch
     * @param pDescendantFrame the descendant frame which marks the end of the branch
     */
    EXPORT_STEM virtual std::shared_ptr<ReferenceFrame>
    cloneSharedBranch(ReferenceFrame *pDescendantFrame) const final;

    /**
     * Function to created a shared clone of the current frame (not the child or parent relationships)
     */
    EXPORT_STEM virtual std::shared_ptr<ReferenceFrame> cloneSharedFrame(void) const final;

    /**
     * Function to create a shared clone of the current frame and its entire subtree
     */
    EXPORT_STEM virtual std::shared_ptr<ReferenceFrame> cloneSharedTree(void) const final;

    /**
     * Function to clone the current frame and its entire subtree
     */
    EXPORT_STEM virtual ReferenceFrame *cloneTree(void) const final;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) final;

    /**
     * Function to copy the input frame to the current object (not the child or parent relationships); returns
     * true upon success
     */
    EXPORT_STEM virtual bool copyFrame(const ReferenceFrame *pFrame) final;

    /**
     * Function to copy the map of frame states from the input reference frame object to current object;
     * returns true upon success
     */
    EXPORT_STEM virtual bool copyFrameStates(const ReferenceFrame *pFrame) final;

    /**
     * Function to copy the input map of frame states to current object; returns true upon success
     */
    EXPORT_STEM virtual void copyFrameStates(const tFrameStates &frameStates) final;

    /**
     * Function to copy the input frame and its subtree to the current object; returns true upon success
     */
    EXPORT_STEM virtual bool copyTree(const ReferenceFrame *pFrame) final;

    /**
     * Create a root "World" reference frame
     * @param name  the name of the newly created frame
     * @param state the desired perturbation state of the reference frame
     */
    static EXPORT_STEM ReferenceFrame *create(const std::string &name = "World Frame",
                                              const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Create a root "World" reference frame
     * @param frameStateType the type of frame state to be instantiated
     * @param name           the name of the newly created frame
     * @param state          the desired perturbation state of the reference frame
     */
    static EXPORT_STEM ReferenceFrame *create(FrameStateType &&frameStateType,
                                              const std::string &name = "World Frame",
                                              const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Create a child frame with the current object's frame as its parent
     * @param name  the name of the newly created frame
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual ReferenceFrame *createChild(const std::string &name,
                                                    const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Create a frame state (in addition to default frame state); if the frame state already exists for the
     * given name, the function will return a pointer to that state
     * @param state the perturbation state of the frame state to be created
     */
    EXPORT_STEM virtual FrameState *const createFrameState(const std::string &state =
                                                           DEFAULT_FRAME_STATE) const final;
#ifdef RAPID_XML
    /**
     * Create a ReferenceFrame tree from the specified XML element; upon success, function will return non-null
     */
    static EXPORT_STEM ReferenceFrame *createFromXML(rapidxml::xml_node<char> *pNode);
#endif
    /**
     * Function to create a new reference frame by merging both reference frame input objects with all frames
     * along the path between them. The resultant frame is a child of the least common ancestor of both frames
     * and all frames merged during the process will be projected to the time at which the common ancestor is
     * defined. For example, the tree diagram below shows the relationship before and after frames B and G are
     * merged, where the transformations imposed by frames B, E, and G have been combined into the newly
     * created frame BG. Singleton frames left behind as a result of the merge, as demonstrated by frame (G) in
     * the diagram below, are removed contingent upon the flag bRemoveSingletons being true. If singletons are
     * removed from the resultant tree, care must be taken to update external references to the frames that no
     * longer exist. In order for the merge to succeed the common ancestor frame must be non-null, both frames
     * must reside within the same branch, and most importantly all frames to be merged must be non-rotating.
     * Upon success, the function returns a non-null reference frame.
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
     *
     */
    static EXPORT_STEM ReferenceFrame *createMergedFrame(ReferenceFrame *pStartFrame,
                                                         ReferenceFrame *pEndFrame,
                                                         const std::string &name,
                                                         bool bRemoveSingletons = false);

    /**
     * Create a motion state within this frame of reference
     * @param coordType the coordinate system in which the motion state will be described
     * @param state     the desired perturbation state of the motion state to be created
     */
    EXPORT_STEM virtual MotionState *createMotionState(const CoordinateType &coordType =
                                                       CoordinateType::Cartesian,
                                                       const std::string &state =
                                                       DEFAULT_FRAME_STATE) const final;

    /**
     * Create a shared motion state within this frame of reference that is projected to the specified time
     * @param coordType the coordinate system in which the motion state will be described
     * @param t         the time to which the frames will be projected in order to calculate the motion state
     *                  at the desired time
     * @param state     the desired perturbation state of the motion state to be created
     */
    EXPORT_STEM virtual MotionState *createMotionState(const CoordinateType &coordType,
                                                       double t,
                                                       const std::string &state =
                                                       DEFAULT_FRAME_STATE) const final;

    /**
     * Create a shared root "World" reference frame
     * @param name  the name of the newly created frame
     * @param state the desired perturbation state of the reference frame
     */
    static EXPORT_STEM std::shared_ptr<ReferenceFrame>
    createShared(const std::string &name = "World Frame",
                 const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Create a child frame with the current object's frame as its parent
     * @param name  the name of the newly created frame
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual std::shared_ptr<ReferenceFrame>
    createSharedChild(const std::string &name,
                      const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Function to create a new shared reference frame by merging both reference frame input objects with all
     * frames along the path between them. The resultant frame is a child of the least common ancestor of both
     * frames and all frames merged during the process will be projected to the time at which the common
     * ancestor is defined. For example, the tree diagram below shows the relationship before and after frames
     * B and G are merged, where the transformations imposed by frames B, E, and G have been combined into the
     * newly created frame BG. Singleton frames left behind as a result of the merge, as demonstrated by frame
     * (G) in the diagram below, are removed contingent upon the flag bRemoveSingletons being true. If
     * singletons are removed from the resultant tree, care must be taken to update external references to the
     * frames that no longer exist. In order for the merge to succeed the common ancestor frame must be
     * non-null, both frames must reside within the same branch, and most importantly all frames to be merged
     * must be non-rotating. Upon success, the function returns a non-null reference frame.
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
     *
     */
    static EXPORT_STEM std::shared_ptr<ReferenceFrame> createSharedMergedFrame(ReferenceFrame *pStartFrame,
                                                                               ReferenceFrame *pEndFrame,
                                                                               const std::string &name,
                                                                               bool bRemoveSingletons = false);

    /**
     * Create a shared motion state within this frame of reference
     * @param coordType the coordinate system in which the motion state will be described
     * @param state     the desired perturbation state of the motion state to be created
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState> createSharedMotionState(const CoordinateType &coordType =
                                                                             CoordinateType::Cartesian,
                                                                             const std::string &state =
                                                                             DEFAULT_FRAME_STATE) const final;

    /**
     * Create a motion state within this frame of reference that is projected to the specified time
     * @param coordType the coordinate system in which the motion state will be described
     * @param t         the time to which the frames will be projected in order to calculate the motion state
     *                  at the desired time
     * @param state     the desired perturbation state of the motion state to be created
     */
    EXPORT_STEM virtual std::shared_ptr<MotionState> createSharedMotionState(const CoordinateType &coordType,
                                                                             double t,
                                                                             const std::string &state =
                                                                             DEFAULT_FRAME_STATE) const final;

    /**
     * Create a shared sibling frame with the parent of the current object's frame as its parent; function
     * succeeds if the current object has a non-null parent
     * @param name  the name of the newly created frame
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual std::shared_ptr<ReferenceFrame>
    createSharedSibling(const std::string &name,
                        const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Create a sibling frame with the parent of the current object's frame as its parent; function succeeds if
     * the current object has a non-null parent
     * @param name  the name of the newly created frame
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual ReferenceFrame *createSibling(const std::string &name,
                                                      const std::string &state = DEFAULT_FRAME_STATE) final;

private:

    /**
     * Function to delete and remove a reference frame as being a child of this frame
     */
    EXPORT_STEM virtual bool deleteChild(ReferenceFrame *pChildFrame) final;

public:

    /**
     * Function to delete and remove all children from this object's frame
     */
    EXPORT_STEM virtual void deleteChildren(void) final;

    /**
     * Function to delete a frame; upon success, deletes input argument and sets it to null
     */
    static EXPORT_STEM void deleteFrame(ReferenceFrame *&pFrame);

    /**
     * Delete a frame state by name
     */
    EXPORT_STEM virtual bool deleteFrameState(const std::string &state) final;

    /**
     * Delete a frame state by pointer
     */
    EXPORT_STEM virtual bool deleteFrameState(FrameState *&pFrameState) final;

    /**
     * Delete all frame states
     */
    EXPORT_STEM virtual void deleteFrameStates(void) final;

    /**
     * Function to delete a frame and its subtree; upon success, deletes input argument and sets it to null
     */
    static EXPORT_STEM void deleteTree(ReferenceFrame *&pFrame);

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

private:

    /**
     * Function to destroy the ReferenceFrame and its subtree
     */
    EXPORT_STEM virtual void destroy(void) final;

    /**
     * Function to determine the common ancestor of both the current object's reference frame and the input
     * reference frame. If no common ancestor exists, the function returns null.
     */
    EXPORT_STEM virtual ReferenceFrame *determineCommonAncestor(ReferenceFrame *pFrame) const final;

public:

    /**
     * Determine if the current object forms a branch with the specified descendant frame; the function tests
     * each frame by name or pointer value to make the determination; if found, the function returns a non-null
     * pointer to the descendant frame that marks the end of the matching branch
     * @param pDescendantFrame the descendant frame which marks the end of the branch
     */
    EXPORT_STEM virtual ReferenceFrame *findBranch(ReferenceFrame *pDescendantFrame) const final;

    /**
     * Find a child frame by name; returns non-null if a child frame by the given name currently exists
     */
    EXPORT_STEM virtual ReferenceFrame *findChild(const std::string &name) const final;

    /**
     * Function to find a common ancestor of both the current object's reference frame and the input reference
     * frame. If both frames reside in different trees, then the function tests on frame name in order to find
     * a common ancestor. In this case, the function returns a pointer to the current object's common ancestor
     * that resides in the current object's tree (if found). The input argument will be modified to reflect the
     * common frame found in the tree that contains the input argument. If both the current object's frame and
     * the input argument frame reside in the same tree, the return value and input argument will be the same.
     * If no common ancestor is found, the return value is null and the input argument is unmodified.
     * @param[in]  pFrame the input reference frame
     * @param[out] pFrame a pointer to the common ancestor found in the tree that contains the input argument
     */
    EXPORT_STEM virtual ReferenceFrame *findCommonAncestorFrame(ReferenceFrame *&pFrame) final;

    /**
     * Find a frame within the specified frame's tree by name. If found, the function returns a non-null
     * reference frame.
     */
    static EXPORT_STEM ReferenceFrame *findFrame(ReferenceFrame *pFrame,
                                                 const std::string &name);

    /**
     * Find a frame within the this object's tree by name. If found, the function returns a non-null reference
     * frame.
     */
    EXPORT_STEM virtual ReferenceFrame *findFrame(const std::string &name) const final;

    /**
     * Find a frame within the current object's tree that matches the name of the input frame. If found, the
     * function returns a non-null reference frame.
     */
    EXPORT_STEM virtual ReferenceFrame *findFrame(ReferenceFrame *pFrame) final;

    /**
     * Find a frame within the current object's tree that matches the name of the input frame. If found, the
     * function returns a non-null reference frame.
     */
    EXPORT_STEM virtual ReferenceFrame *findFrame(ReferenceFrame *pFrame) const final;

    /**
     * Find a frame within the current object's tree that matches the frame in which the input motion state is
     * defined; if found, the function returns a non-null object
     */
    EXPORT_STEM virtual ReferenceFrame *findFrame(MotionState *pMotionState) const final;

    /**
     * Find a frame within the input reference frame object's tree that matches the frame in which the input
     * motion state is defined; if found, the function returns a non-null object
     * @param pFrame       the root of a tree of reference frames in which the search will be performed
     * @param pMotionState the motion state defined in the target frame of reference
     */
    static EXPORT_STEM ReferenceFrame *findFrame(ReferenceFrame *pFrame,
                                                 MotionState *pMotionState);

    /**
     * Function to find the specified frame state by name; if a frame state is not associated with the
     * specified name, the function returns null
     */
    EXPORT_STEM virtual FrameState *findFrameState(const std::string &state) const final;

    /**
     * Function to find the least common root of a subtree that contains both the current object's frame of
     * reference as well as the input reference frame. If both frames reside in different trees, then the
     * function tests on frame name in order to find a common root. In this case, the function returns a
     * pointer to the current object's common root that resides in the current object's tree (if found). The
     * input argument will be modified to reflect the common frame found in the tree that contains the input
     * argument. If both the current object's frame and the input argument frame reside in the same tree, the
     * return value and input argument will be the same. If no common root is found, the return value is null
     * and the input argument is unmodified.
     * @param[in]  pFrame the input reference frame
     * @param[out] pFrame a pointer to the common ancestor found in the tree that contains the input argument
     */
    EXPORT_STEM virtual ReferenceFrame *findLeastCommonRootFrame(ReferenceFrame *&pFrame) final;

    /**
     * Find a sibling frame by name; returns non-null upon success
     */
    EXPORT_STEM virtual ReferenceFrame *findSibling(const std::string &name) const final;

    /**
     * Apply the given function object to the current object and its subtree
     */
    template<typename UnaryFunction>
    void forEach(UnaryFunction &&function) const
    {
        const auto *pFrame = this;
        const auto *pParentFrame = pFrame->getParent();
        do
        {
            if (pFrame == nullptr)
            {
                pFrame = pParentFrame;
                pParentFrame = pParentFrame->getParent();
                pFrame = pFrame->getRightSibling();
            }
            else
            {
                std::forward<UnaryFunction>(function)(pFrame);

                pParentFrame = pFrame;
                pFrame = pFrame->getChild();
            }
        }
        while (pFrame != pParentFrame);
    }

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d getAcceleration(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getAcceleration(double acceleration[3],
                                             const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get angle units (Degrees or Radians)
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual AngleUnitType getAngleUnits(const std::string &state =
                                                    DEFAULT_FRAME_STATE) const final;

private:

    /**
     * Get the first child frame
     */
    EXPORT_STEM virtual ReferenceFrame *getChild(void) final;

    /**
     * Get the first child frame
     */
    EXPORT_STEM virtual ReferenceFrame *getChild(void) const final;

public:

    /**
     * Get a child frame by name; create a child frame by the given name if one does not currently exist
     * @param name  the name of the child frame
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual ReferenceFrame *getChild(const std::string &name,
                                                 const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Get child by index; returns non-null upon success
     */
    EXPORT_STEM virtual ReferenceFrame *getChild(std::size_t index) final;

    /**
     * Get the index of this child object
     */
    EXPORT_STEM virtual size_t getChildIndex(void) const final;

private:

    /**
     * Get the index of a child; if the input object is a child of the current object, function returns an
     * index in the range 0 to N - 1, where N is the number of children of the current object; otherwise,
     * function returns the current object's number of child frames
     */
    static EXPORT_STEM size_t getChildIndex(ReferenceFrame *pChildFrame);

public:

    /**
     * Get the children of the current frame
     */
    EXPORT_STEM virtual std::vector<ReferenceFrame *> getChildren(void) final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Function to return the depth of this frame relative to the root of the tree, where the root of the tree
     * is at a depth of one.
     */
    EXPORT_STEM virtual size_t getDepth(void) const final;

    /**
     * Function to get the specified frame state by name; if a frame state is not associated with the specified
     * name, the function will return the default state
     */
    EXPORT_STEM virtual FrameState *const getFrameState(const std::string &state =
                                                        DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's map of frame states
     */
    EXPORT_STEM virtual tFrameStates &getFrameStates(void) final;

    /**
     * Get this object's frame state type
     */
    EXPORT_STEM virtual FrameStateType getFrameStateType(void) const final;

private:

    /**
     * Get this frame's left sibling
     */
    EXPORT_STEM virtual ReferenceFrame *getLeftSibling(void) final;

    /**
     * Get this frame's left sibling
     */
    EXPORT_STEM virtual ReferenceFrame *getLeftSibling(void) const final;

public:

    /**
     * Get the motion states defined within this frame of reference
     */
    EXPORT_STEM virtual std::vector<MotionState *> &getMotionStates(void) final;

    /**
     * Get the name of this reference frame
     */
    EXPORT_STEM virtual std::string getName(void) const override;

    /**
     * Get the orientation angles of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Eulers getOrientation(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the orientation angles of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getOrientation(double orientation[3],
                                            const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the orientation angles of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Eulers getOrientation(double t,
                                              const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the orientation angles of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getOrientation(double orientation[3],
                                            double t,
                                            const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the position of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d getOrigin(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the position of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getOrigin(double origin[3],
                                       const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the position of the origin of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d getOrigin(double t,
                                           const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the position of the origin of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getOrigin(double origin[3],
                                       double t,
                                       const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the parent of this frame
     */
    EXPORT_STEM virtual ReferenceFrame *getParent(void) final;

    /**
     * Get the parent of this frame
     */
    EXPORT_STEM virtual ReferenceFrame *getParent(void) const final;

    /**
     * Get this object's pitch angle
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitch(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch angle
     * @param t     the time at which the return value will be computed
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitch(double t,
                                        const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitch(const AngleUnitType &angleUnits,
                                        const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch angle
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitch(double t,
                                        const AngleUnitType &angleUnits,
                                        const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch acceleration
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitchAcceleration(const std::string &state =
                                                    DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitchAcceleration(const AngleUnitType &angleUnits,
                                                    const std::string &state =
                                                    DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch rate
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitchRate(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch rate
     * @param t     the time at which the return value will be computed
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitchRate(double t,
                                            const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitchRate(const AngleUnitType &angleUnits,
                                            const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's pitch rate
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getPitchRate(double t,
                                            const AngleUnitType &angleUnits,
                                            const std::string &state = DEFAULT_FRAME_STATE) const final;

private:

    /**
     * Get this frame's right sibling
     */
    EXPORT_STEM virtual ReferenceFrame *getRightSibling(void) final;

    /**
     * Get this frame's right sibling
     */
    EXPORT_STEM virtual ReferenceFrame *getRightSibling(void) const final;

public:

    /**
     * Get this object's roll angle
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRoll(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's roll angle
     * @param t     the time at which the return value will be computed
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM double getRoll(double t,
                               const std::string &state = DEFAULT_FRAME_STATE) const;

    /**
     * Get this object's roll angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM double getRoll(const AngleUnitType &angleUnits,
                               const std::string &state = DEFAULT_FRAME_STATE) const;

    /**
     * Get this object's roll angle
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM double getRoll(double t,
                               const AngleUnitType &angleUnits,
                               const std::string &state = DEFAULT_FRAME_STATE) const;

    /**
     * Get this object's roll acceleration
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRollAcceleration(const std::string &state =
                                                   DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's roll acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRollAcceleration(const AngleUnitType &angleUnits,
                                                   const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's roll rate
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRollRate(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's roll rate
     * @param t     the time at which the return value will be computed
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRollRate(double t,
                                           const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's roll rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRollRate(const AngleUnitType &angleUnits,
                                           const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's roll rate
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getRollRate(double t,
                                           const AngleUnitType &angleUnits,
                                           const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the root reference frame
     */
    EXPORT_STEM virtual ReferenceFrame *getRoot(void) final;

    /**
     * Get the root reference frame
     */
    EXPORT_STEM virtual ReferenceFrame *getRoot(void) const final;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Eulers getRotationalAccelerations(const std::string &state =
                                                          DEFAULT_FRAME_STATE) const final;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getRotationalAccelerations(double rotationalAccelerations[3],
                                                        const std::string &state =
                                                        DEFAULT_FRAME_STATE) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Eulers getRotationalRates(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getRotationalRates(double rotationalRates[3],
                                                const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Eulers getRotationalRates(double t,
                                                  const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getRotationalRates(double rotationalRates[3],
                                                double t,
                                                const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get a sibling frame by name; creates a sibling frame by the given name if one does not currently exist
     * @param name  the name of the sibling frame
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual ReferenceFrame *getSibling(const std::string &name,
                                                   const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the time at which this frame is currently defined
     */
    EXPORT_STEM virtual double getTime(void) const final;

    /**
     * Get the time at which this frame is currently defined
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual double getTime(const std::string &state) const final;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d getVelocity(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getVelocity(double velocity[3],
                                         const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the velocity of the origin of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual Vector3d getVelocity(double t,
                                             const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get the velocity of the origin of this frame with respect to its parent at the specified time
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool getVelocity(double velocity[3],
                                         double t,
                                         const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get a pointer to world reference frame
     */
    static EXPORT_STEM ReferenceFrame *getWorldFrame(void);

    /**
     * Get this object's yaw angle
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYaw(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw angle
     * @param t     the time at which the return value will be computed
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYaw(double t,
                                      const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYaw(const AngleUnitType &angleUnits,
                                      const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw angle
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYaw(double t,
                                      const AngleUnitType &angleUnits,
                                      const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw acceleration
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYawAcceleration(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYawAcceleration(const AngleUnitType &angleUnits,
                                                  const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw rate
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYawRate(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw rate
     * @param t     the time at which the return value will be computed
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYawRate(double t,
                                          const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYawRate(const AngleUnitType &angleUnits,
                                          const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Get this object's yaw rate
     * @param t          the time at which the return value will be computed
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual double getYawRate(double t,
                                          const AngleUnitType &angleUnits,
                                          const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Return whether or not this frame has children
     */
    EXPORT_STEM virtual bool hasChildren(void) const final;

    /**
     * Initialization function to set origin, orientation, and their time-derivatives to zero
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void initialize(const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Initialization function to reset time-derivatives to zero
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void initializeTimeDerivatives(const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Function to insert a copy of a branch into the current object; in order for the insertion to succeed,
     * the input ancestor and descendant frames must form a valid branch. The function will attempt to find an
     * exact or partial matching branch at the level of the current object and traversing down the current
     * object's subtree; if one isn't found, the branch will be copied and inserted as a sibling of the current
     * object so long as it is has a non-null parent frame and is therefore not the root of a tree; otherwise,
     * the insertion fails and the function returns false
     * @param pAncestorFrame   the ancestor frame which marks the start of the branch
     * @param pDescendantFrame the descendant frame which marks the end of the branch
     */
    EXPORT_STEM virtual bool insertBranch(ReferenceFrame *pAncestorFrame,
                                          ReferenceFrame *pDescendantFrame) final;

    /**
     * Function to insert a copy of a branch into the destination reference frame object's tree; in order for
     * the insertion to succeed, the destination reference frame object must be non-null and the input ancestor
     * and descendant frames must form a valid branch. The function will attempt to find an exact or partial
     * matching branch at the level of the current object and traversing down the destination reference frame
     * object's subtree; if one isn't found, the branch will be copied and inserted as a sibling so long as it
     * is has a non-null parent frame and is therefore not the root of a tree; otherwise, the insertion fails
     * and the function returns false; if the destination reference frame tree is null, the branch, if valid,
     * is cloned and the input argument to the destination reference frame tree is modified to point to the
     * root ancestor of the cloned branch; function returns true upon success
     * @param pFrame           the destination reference frame object
     * @param pAncestorFrame   the ancestor frame which marks the start of the branch
     * @param pDescendantFrame the descendant frame which marks the end of the branch
     */
    static EXPORT_STEM bool insertBranch(ReferenceFrame *&pFrame,
                                         ReferenceFrame *pAncestorFrame,
                                         ReferenceFrame *pDescendantFrame);

    /**
     * Determines if the current object's reference frame is an ancestor frame of the input reference frame
     */
    EXPORT_STEM virtual bool isAncestor(const ReferenceFrame *pFrame) const final;

    /**
     * Determines if the current object is an ancestor of the frame associated with the given name
     */
    EXPORT_STEM virtual bool isAncestor(const std::string &name) const final;

    /**
     * Determines if the current object's reference frame is a descendant frame of the input reference frame
     */
    EXPORT_STEM virtual bool isDescendant(const ReferenceFrame *pFrame) const final;

    /**
     * Determines if the current object is a descendant of the frame associated with the given name
     */
    EXPORT_STEM virtual bool isDescendant(const std::string &name) const final;

    /**
     * Determines if the input object's reference frame resides in the same tree as the current object's
     * reference frame
     */
    EXPORT_STEM virtual bool isFamily(const ReferenceFrame *pFrame) const final;

    /**
     * Function to determine if this frame is inertial
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool isInertial(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Determines if the frame in question is a leaf node
     */
    EXPORT_STEM virtual bool isLeaf(void) const final;

    /**
     * Function to determine if this frame is non-inertial
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool isNonInertial(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Determines if the frame in question is the root of a tree
     */
    EXPORT_STEM virtual bool isRoot(void) const final;

    /**
     * Function to determine if this frame is rotating
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool isRotating(const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Function to print the contents of reference frame and its subtree
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * Function to print the contents of each reference frame along a chain starting at the current object's
     * reference frame and ending at the input reference frame object
     */
    EXPORT_STEM virtual std::ostream &printChain(const ReferenceFrame *pFrame,
                                                 std::ostream &stream) const final;

    /**
     * Function to print the contents of reference frame and its subtree
     */
    EXPORT_STEM virtual std::ostream &printTree(std::ostream &stream) const final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * Remove a reference frame as being a child of this frame (doesn't delete the child)
     */
    EXPORT_STEM virtual void removeChild(ReferenceFrame *pChildFrame) final;

    /**
     * Remove a frame state by name
     */
    EXPORT_STEM virtual bool removeFrameState(const std::string &state) final;

    /**
     * Remove a frame state by pointer
     */
    EXPORT_STEM virtual bool removeFrameState(FrameState *pFrameState) final;

    /**
     * Remove all frame states
     */
    EXPORT_STEM virtual void removeFrameStates(void) final;

    /**
     * Remove a motion state this frame of reference; returns true upon success
     */
    EXPORT_STEM virtual bool removeMotionState(const MotionState *pMotionState) final;

public:

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set the acceleration of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setAcceleration(const Vector3d &acceleration,
                                             const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the acceleration of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration,
                                             const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set angle units (Degrees or Radians)
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits,
                                           const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's frame state type
     */
    EXPORT_STEM virtual void setFrameStateType(const FrameStateType &frameStateType) final;

    /**
     * Set the name of this reference frame
     */
    EXPORT_STEM virtual void setName(const std::string &name) override final;

    /**
     * Set the orientation angles of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setOrientation(const Eulers &orientation,
                                            const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the orientation angles of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setOrientation(double roll,
                                            double pitch,
                                            double yaw,
                                            const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the position of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setOrigin(const Vector3d &origin,
                                       const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the position of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setOrigin(double xPosition,
                                       double yPosition,
                                       double zPosition,
                                       const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the parent of this frame
     */
    EXPORT_STEM virtual void setParent(ReferenceFrame *pParentFrame) final;

    /**
     * Set this object's pitch angle
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setPitch(double pitch,
                                      const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's pitch angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setPitch(double pitch,
                                      const AngleUnitType &angleUnits,
                                      const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's pitch acceleration
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration,
                                                  const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's pitch acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration,
                                                  const AngleUnitType &angleUnits,
                                                  const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's pitch rate
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate,
                                          const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's pitch rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate,
                                          const AngleUnitType &angleUnits,
                                          const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's roll angle
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setRoll(double roll,
                                     const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's roll angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setRoll(double roll,
                                     const AngleUnitType &angleUnits,
                                     const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's roll acceleration
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration,
                                                 const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's roll acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration,
                                                 const AngleUnitType &angleUnits,
                                                 const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's roll rate
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setRollRate(double rollRate,
                                         const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's roll rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setRollRate(double rollRate,
                                         const AngleUnitType &angleUnits,
                                         const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the rotational accelerations of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setRotationalAccelerations(const Eulers &rotationalAccelerations,
                                                        const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the rotational accelerations of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setRotationalAccelerations(double rollAcceleration,
                                                        double pitchAcceleration,
                                                        double yawAcceleration,
                                                        const std::string &state = DEFAULT_FRAME_STATE);

    /**
     * Set the rotational rates of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setRotationalRates(const Eulers &rotationalRates,
                                                const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the rotational rates of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setRotationalRates(double rollRate,
                                                double pitchRate,
                                                double yawRate,
                                                const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the time at which this frame is currently defined
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setTime(double t,
                                     const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the velocity of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setVelocity(const Vector3d &velocity,
                                         const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set the velocity of the origin of this frame with respect to its parent
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void setVelocity(double xVelocity,
                                         double yVelocity,
                                         double zVelocity,
                                         const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's yaw angle
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setYaw(double yaw,
                                    const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's yaw angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setYaw(double yaw,
                                    const AngleUnitType &angleUnits,
                                    const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's yaw acceleration
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration,
                                                const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's yaw acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration,
                                                const AngleUnitType &angleUnits,
                                                const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's yaw rate
     * @param state the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setYawRate(double yawRate,
                                        const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Set this object's yaw rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     * @param state      the desired perturbation status of this reference frame
     */
    EXPORT_STEM virtual void setYawRate(double yawRate,
                                        const AngleUnitType &angleUnits,
                                        const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Return whether or not the current object and input frame share a common branch
     */
    EXPORT_STEM virtual bool sharesCommonBranch(ReferenceFrame *pFrame) final;

    /**
     * Static initialization function
     */
    static EXPORT_STEM bool staticInitialize(void);

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(ReferenceFrame &frame) override final;

private:

    /**
     * Transform the input motion state from the parent frame to a child frame of reference
     * @param pCartesianMotionState the motion state to be transformed
     * @param t                     the time to which the frame will be projected
     * @param state                 the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool transformToChild(CartesianMotionState *pCartesianMotionState,
                                              double t,
                                              const std::string &state) const final;

public:

    /**
     * Spatially (and optionally temporally) transform the input motion state defined in the given input
     * reference frame to this object's frame of reference (by default, the transformation is spatial only)
     * @param pMotionState the motion state to be transformed
     * @param bTemporal    flag indicating whether or not a temporal transformation will take place. If false,
     *                     a spatial-only transformation will occur (time at which frames and motion state are
     *                     defined are not considered). If true, the motion state and all frames through which
     *                     it is transformed will be projected to the time at which the target frame is
     *                     defined.
     * @param state        the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool transformToFrame(MotionState *pMotionState,
                                              bool bTemporal = false,
                                              const std::string &state = DEFAULT_FRAME_STATE) const final;

    /**
     * Spatially (and optionally temporally) transform the input motion state defined in the given input
     * reference frame to this object's frame of reference (by default, the transformation is spatial only)
     * @param pMotionState the motion state to be transformed
     * @param state        the desired perturbation state of the reference frame
     * @param bTemporal    flag indicating whether or not a temporal transformation will take place. If false,
     *                     a spatial-only transformation will occur (time at which frames and motion state are
     *                     defined are not considered). If true, the motion state and all frames through which
     *                     it is transformed will be projected to the time at which the target frame is
     *                     defined.
     */
    EXPORT_STEM virtual bool transformToFrame(MotionState *pMotionState,
                                              const std::string &state = DEFAULT_FRAME_STATE,
                                              bool bTemporal = false) const final;

    /**
     * Spatially and temporally transform the input motion state defined in the given input reference frame to
     * this object's frame of reference. Since this transformation is temporal, the motion state and all frames
     * through which it is transformed will be projected to the specified time
     * @param pMotionState the motion state to be transformed
     * @param t            the time to which the frames and motion states will be projected
     * @param state        the desired perturbation state of the reference frame
     */
    EXPORT_STEM bool transformToFrame(MotionState *pMotionState,
                                      double t,
                                      const std::string &state = DEFAULT_FRAME_STATE) const;

private:

    /**
     * Spatially (and optionally temporally) transform the input motion state defined in the given input
     * reference frame to this object's frame of reference
     * @param pMotionState the motion state to be transformed
     * @param t            the time to which the frames and motion states will be projected if performing a
     *                     temporal transformation
     * @param bTemporal    flag indicating whether or not a temporal transformation will take place. If false,
     *                     a spatial-only transformation will occur (time at which frames and motion state are
     *                     defined is not considered). If true, the motion state and all frames through which
     *                     it is transformed will be projected to the specified time
     * @param state        the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool transformToFrame(MotionState *pMotionState,
                                              double t,
                                              bool bTemporal,
                                              const std::string &state) const final;

    /**
     * Transform the input motion state from the child frame to the parent frame of reference
     * @param pCartesianMotionState the motion state to be transformed
     * @param t                     the time to which the frame will be projected
     * @param state                 the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool transformToParent(CartesianMotionState *pCartesianMotionState,
                                               double t,
                                               const std::string &state) const final;

public:

    /**
     * Update this reference frame to the specified time. This function will update all quantities within the
     * frame that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param state         the desired perturbation state of the reference frame
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void update(double t,
                                    const std::string &state = DEFAULT_FRAME_STATE,
                                    const TimeReferenceType &timeReference = TimeReferenceType::Absolute) final;

    /**
     * Update this reference frame to the specified time. This function will update all quantities within the
     * frame that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     * @param state         the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void update(double t,
                                    const TimeReferenceType &timeReference,
                                    const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Update all reference frames in the current frame's subtree to the specified time. This function will
     * update all quantities within each frame that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param state         the desired perturbation state of the reference frame
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void updateAll(double t,
                                       const std::string &state = DEFAULT_FRAME_STATE,
                                       const TimeReferenceType &timeReference =
                                       TimeReferenceType::Delta) final;

    /**
     * Update all reference frames in the current frame's subtree to the specified time. This function will
     * update all quantities within each frame that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     * @param state         the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual void updateAll(double t,
                                       const TimeReferenceType &timeReference,
                                       const std::string &state = DEFAULT_FRAME_STATE) final;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Zero this reference frame's rotational orientation and associated derivatives
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool zeroRotation(const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Zero this reference frame's rotational derivatives
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool zeroRotationalDerivatives(const std::string &state = DEFAULT_FRAME_STATE) final;

    /**
     * Zero this reference frame's translational components and associated derivatives
     * @param state the desired perturbation state of the reference frame
     */
    EXPORT_STEM virtual bool zeroTranslation(const std::string &state = DEFAULT_FRAME_STATE) final;

private:

    /**
     * a set of child reference frames
     */
    std::vector<ReferenceFrame *> m_children;

    /**
     * reference frame states stored in this frame
     */
    mutable tFrameStates m_frameStates;

    /**
     * frame state type
     */
    FrameStateType m_frameStateType;

    /**
     * the motion states defined within this frame of reference
     */
    std::vector<MotionState *> m_motionStates;

    /**
     * name of this reference frame
     */
    std::string m_name;

    /**
     * pointer to the parent frame
     */
    ReferenceFrame *m_pParentFrame;
};

}

}

#endif
