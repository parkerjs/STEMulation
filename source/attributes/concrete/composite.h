#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <algorithm>
#include <stdexcept>
#include <typeinfo>
#include <vector>

#include "indexable.h"
#include "nameable.h"
#include "reflective.h"
#include "swappable.h"

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface from which derived types can implement the composite design pattern
 */
template<typename T>
class Composite
: public attributes::interfaces::Indexable<std::size_t>,
  public attributes::interfaces::Nameable,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<Composite<T>>
{
protected:

    /**
     * Constructor
     * @param name    the name of this composite
     * @param pParent a pointer to the parent composite of the current object
     */
    Composite(const std::string &name = "",
              Composite<T> *pParent = nullptr)
    : m_name(name),
      m_pParent(nullptr)
    {
        setParent(pParent);
    }

    /**
     * Constructor
     * @param pParent a pointer to the parent composite of the current object
     * @param name    the name of this composite
     */
    Composite(Composite<T> *pParent,
              const std::string &name = "")
    : Composite<T>(name, pParent)
    {

    }

    /**
     * Copy constructor
     */
    Composite(const Composite<T> &composite)
    : m_pParent(nullptr)
    {
        operator = (composite);
    }

    /**
     * Move constructor
     */
    Composite(Composite<T> &&composite)
    : m_pParent(nullptr)
    {
        composite.swap(*this);
    }

    /**
     * Destructor
     */
    virtual ~Composite(void) override
    {
        destroy();
    }

    /**
     * Copy assignment operator
     */
    Composite<T> &
    operator = (const Composite<T> &composite)
    {
        if (&composite != this)
        {
            copyTree(composite);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Composite<T> &
    operator = (Composite<T> &&composite)
    {
        if (&composite != this)
        {
            composite.swap(*this);
        }

        return *this;
    }

public:

    /**
     * Conversion operator
     */
    inline
    operator T & (void)
    {
        return dynamic_cast<T &>(*this);
    }

    /**
     * Add a child composite to the current object
     */
    template<typename U>
    bool
    addChild(U *pChild)
    {
        bool bSuccess = (pChild != nullptr &&
                         pChild != this);

        if (bSuccess)
        {
            auto &&itChild = std::find(m_children.begin(),
                                       m_children.end(),
                                       pChild);

            bSuccess = (itChild == m_children.end());
            if (bSuccess)
            {
                auto *pParent = pChild->getParent();
                if (pParent != nullptr)
                {
                    pParent->removeChild(pChild);
                }

                m_children.push_back(pChild);
                pChild->m_pParent = this;
            }
        }

        return bSuccess;
    }

    /**
     * Cast this object to the specified type
     */
    template<typename U,
             typename std::enable_if<!std::is_same<Composite<T>,
                                                   typename std::decay<U>::type>::value,
                                     int>::type = 0>
    inline U *
    cast(void) const
    {
        return dynamic_cast<U *>(const_cast<Composite<T> *>(this));
    }

    /**
     * Cast this object to the specified type
     */
    template<typename U,
             typename std::enable_if<std::is_same<Composite<T>,
                                                  typename std::decay<U>::type>::value,
                                     int>::type = 0>
    inline Composite<T> *
    cast(void) const
    {
        return const_cast<Composite<T> *>(this);
    }

    /**
     * Function to copy a composite, not including children, into the current object
     * @param composite a reference to the composite to be copied to the current object
     */
    inline virtual void
    copy(const Composite<T> &composite)
    {
        // this method should be overridden by derived classes
    }

    /**
     * Function to copy a composite, including children, into the current object
     * @param composite a reference to the composite to be copied to the current object
     */
    virtual void
    copyTree(const Composite<T> &composite) final
    {
        auto *pDestComposite = this;
        auto *pSourceComposite = &composite;
        auto *pParent = pSourceComposite->getParent();
        const auto *pChild = pSourceComposite->getChild();
        do
        {
            if (pChild == nullptr)
            {
                pDestComposite->copy(*pSourceComposite);
                pDestComposite->m_name = pSourceComposite->m_name;

                auto numChildren = pSourceComposite->m_children.size();
                auto &destChildren = pDestComposite->m_children;
                while (destChildren.size() > numChildren)
                {
                    auto index = destChildren.size() - 1;
                    auto *pDestChild = pDestComposite->getChild(index);
                    pDestComposite->deleteChild(pDestChild);
                }

                pDestComposite = pDestComposite->getParent();
                pChild = pSourceComposite;
                pSourceComposite = pSourceComposite->getParent();
                pChild = pChild->getRightSibling();
            }
            else
            {
                auto index = pChild->getIndex();
                auto *pDestCompositeChild = pDestComposite->getChild(index);
                if (pDestCompositeChild == nullptr)
                {
                    pDestComposite = pDestComposite->createChild(pSourceComposite->m_name);
                    if (pDestComposite == nullptr)
                    {
                        break;
                    }
                }
                else
                {
                    pDestComposite = pDestCompositeChild;
                }

                pSourceComposite = pChild;
                pChild = pChild->getChild();
            }
        }
        while (pSourceComposite != pParent);

        if (pParent != nullptr)
        {
            pParent->addChild(this);
        }
    }

    /**
     * Create a child composite with the current object as its parent
     */
    inline virtual T *
    createChild(const std::string &name = "")
    {
        return nullptr; // this method should be overridden by derived classes
    }

    /**
     * Delete a child by index; returns true upon success
     * @param index the linear index associated with the child composite
     */
    virtual bool
    deleteChild(std::size_t index) final
    {
        auto *pChild = getChild(index);
        bool bSuccess = (pChild != nullptr);
        if (bSuccess)
        {
            bSuccess = deleteChild(pChild);
        }

        return bSuccess;
    }

    /**
     * Delete a child by name; returns true upon success
     * @param name the name of the child composite
     */
    virtual bool
    deleteChild(const std::string &name) final
    {
        auto *pChild = getChild(name);
        bool bSuccess = (pChild != nullptr);
        if (bSuccess)
        {
            bSuccess = deleteChild(pChild);
        }

        return bSuccess;
    }

    /**
     * Delete a child of type U by value; if found, function sets input argument to null
     * @param pChild a pointer to the child composite to be deleted from the current object
     */
    template<typename U>
    bool
    deleteChild(U * &pChild)
    {
        bool bSuccess = removeChild(pChild);
        if (bSuccess)
        {
            Composite<T> *pComposite = pChild;
            if (pComposite != nullptr)
            {
                delete pComposite;
            }

            pChild = nullptr;
        }

        return bSuccess;
    }

    /**
     * Delete all child composites of type U from the current object
     */
    template<typename U>
    void
    deleteChildren(void)
    {
        std::vector<U *> children;
        getChildren(children);

        auto &&itChild = children.begin();
        while (itChild != children.end())
        {
            auto *pChild = *itChild;
            if (pChild != nullptr)
            {
                deleteChild(pChild);
            }

            ++itChild;
        }
    }

    /**
     * Delete all child composites from the current object
     */
    virtual void
    deleteChildren(void) final
    {
        while (true)
        {
            auto &&itChild = m_children.begin();
            if (itChild != m_children.end())
            {
                auto *pChild = *itChild;
                pChild->destroy();
            }
            else
            {
                break;
            }
        }
    }

    /**
     * Function to destroy the current object and all of its descendants
     */
    virtual void
    destroy(void)
    {
        auto *pThisParent = getParent();
        auto *pComposite = this;
        do
        {
            if (pComposite != nullptr)
            {
                auto *pChild = pComposite->getChild();
                if (pChild == nullptr)
                {
                    auto *pParent = pComposite->getParent();
                    if (pParent != nullptr)
                    {
                        pParent->removeChild(pComposite);
                    }

                    if (pComposite != this)
                    {
                        delete pComposite;
                    }
                    else
                    {
                        break;
                    }

                    pComposite = pParent;
                }
                else
                {
                    pComposite = pChild;
                }
            }
        }
        while (pComposite != nullptr &&
               pComposite != pThisParent);
    }

    /**
     * Function to determine the common ancestor of both the current object and the input composite
     * @param  pComposite the input composite
     * @return            a non-null pointer to the associated common ancestor, if found
     */
    template<typename U>
    U *
    determineCommonAncestor(U *pComposite) const
    {
        // loop until we find a common ancestor
        U *pCommonAncestor = nullptr;
        if (isFamily(pComposite))
        {
            while (true)
            {
                pComposite = pComposite->template getParent<U>();

                if (pComposite != nullptr &&
                    !pComposite->isAncestor(this))
                {
                    continue;
                }
                else
                {
                    pCommonAncestor = pComposite;
                }

                break;
            }
        }

        return pCommonAncestor;
    }

    /**
     * Find a composite by name within the current object and its subtree
     */
    inline virtual Composite<T> *
    find(const std::string &name) const final
    {
        return find<Composite<T>>(name);
    }

    /**
     * Find a composite by name within the current object and its subtree; if no name is supplied, function
     * returns the first composite it finds
     */
    template<typename U>
    U *
    find(const std::string &name = "") const
    {
        return find<U>([&name] (auto *pComposite)
                       {
                           return name.empty() ||
                                  pComposite->getName() == name;
                       });
    }

    /**
     * Function to find a composite within the current object and its subtree using the specified criteria
     */
    template<typename U,
             typename Criteria,
             typename std::enable_if<!std::is_convertible<typename std::decay<Criteria>::type,
                                                          std::string>::value,
                                     int>::type = 0>
    U *
    find(Criteria &&criteria) const
    {
        const auto *pComposite = this;
        const auto *pParentComposite = pComposite->getParent();
        do
        {
            if (pComposite == nullptr)
            {
                pComposite = pParentComposite;
                pParentComposite = pParentComposite->getParent();
                pComposite = pComposite->getRightSibling();
            }
            else
            {
                auto *pDynamicComposite = pComposite->template cast<U>();
                if (pDynamicComposite != nullptr &&
                    std::forward<Criteria>(criteria)(pDynamicComposite))
                {
                    return pDynamicComposite;
                }

                pParentComposite = pComposite;
                pComposite = pComposite->getChild();
            }
        }
        while (pComposite != pParentComposite);

        return nullptr;
    }

    /**
     * Function to find the common ancestor of the subtree that contains both the current object and the input
     * composite. If both composites reside in different trees, then the function tests on the names of the
     * composites in order to find a common ancestor. In this case, the function returns a pointer to the
     * current object's common ancestor that resides in the current object's tree (if found). The input
     * argument will be modified to reflect the common composite found in the tree that contains the input
     * argument. If both the current object and the input argument reside in the same tree, the return value
     * and input argument will be the same. If no common ancestor is found, the return value is null and the
     * input argument is unmodified.
     * @param[in]  pComposite a pointer to the composite for which the common ancestor will attempt to be found
     * @param[out] pComposite a non-null pointer to the common ancestor, if found
     * @return                a non-null pointer to the common ancestor, which if found, will be different from
     *                        the output argument if the current object and input composite reside in different
     *                        trees
     */
    template<typename U>
    U *
    findCommonAncestor(U * &pComposite) const
    {
        U *pThisCommonAncestor = nullptr;
        if (pComposite != nullptr)
        {
            pThisCommonAncestor = determineCommonAncestor<U>(pComposite);
            if (pThisCommonAncestor != nullptr)
            {
                pComposite = pThisCommonAncestor; // they're in the same tree
            }
            else
            {
                // the composites are likely in different trees; determine the common ancestor
                auto *pAncestorComposite = pComposite->template getGreatestAncestor<U>();
                auto *pTempComposite = this->cast<U>();
                while (pTempComposite != nullptr)
                {
                    auto *pSearchComposite = pAncestorComposite->template find<U>(pTempComposite->m_name);
                    if (pSearchComposite != nullptr)
                    {
                        pComposite = pSearchComposite;
                        pThisCommonAncestor = pTempComposite;

                        break;
                    }
                    else
                    {
                        pTempComposite = pTempComposite->template getParent<U>();
                    }
                }
            }
        }

        return pThisCommonAncestor;
    }

    /**
     * Function to find the least common ancestor of the subtree that contains both the current object and the
     * input composite. If both composites reside in different trees, then the function tests on the names of
     * the composites in order to find a common ancestor. In this case, the function returns a pointer to the
     * current object's common ancestor that resides in the current object's tree (if found). The input
     * argument will be modified to reflect the common composite found in the tree that contains the input
     * argument. If both the current object and the input argument reside in the same tree, the return value
     * and input argument will be the same. If no common ancestor is found, the return value is null and the
     * input argument is unmodified.
     * @param[in]  pComposite a pointer to the composite for which the least common ancestor will attempt to be
     *                        found
     * @param[out] pComposite a non-null pointer to the least common ancestor, if found
     * @return                a non-null pointer to the least common ancestor, which if found, will be
     *                        different from the output argument if the current object and input composite
     *                        reside in different trees
     */
    template<typename U>
    U *
    findLeastCommonAncestor(U * &pComposite) const
    {
        U *pThisCommonAncestor = nullptr;
        if (pComposite != nullptr)
        {
            if (this == pComposite ||
                isDescendant(pComposite))
            {
                pThisCommonAncestor = pComposite;
            }
            else if (pComposite->isDescendant(this))
            {
                pThisCommonAncestor = this->cast<U>();
                pComposite = pThisCommonAncestor;
            }
            else // composites do not lie within the same branch...
            {
                pThisCommonAncestor = findCommonAncestor(pComposite);
            }
        }

        return pThisCommonAncestor;
    }

    /**
     * Apply the given function object to composites of the specified type within the current object and its
     * subtree
     */
    template<typename U = Composite<T>,
             typename UnaryFunction>
    void
    forEach(UnaryFunction &&function) const
    {
        const auto *pComposite = this;
        const auto *pParentComposite = pComposite->getParent();
        do
        {
            if (pComposite == nullptr)
            {
                pComposite = pParentComposite;
                pParentComposite = pParentComposite->getParent();
                pComposite = pComposite->getRightSibling();
            }
            else
            {
                auto *pDynamicComposite = pComposite->template cast<U>();
                if (pDynamicComposite != nullptr)
                {
                    std::forward<UnaryFunction>(function)(pDynamicComposite);
                }

                pParentComposite = pComposite;
                pComposite = pComposite->getChild();
            }
        }
        while (pComposite != pParentComposite);
    }

    /**
     * Get the current object's first child composite
     */
    inline Composite<T> *
    getChild(void) const
    {
        return getChild<Composite<T>>();
    }

    /**
     * Get the first child composite of type T from the current object
     */
    template<typename U>
    U *
    getChild(void) const
    {
        auto &&itChild = m_children.cbegin();
        while (itChild != m_children.cend())
        {
            auto *pChild = dynamic_cast<U *>(*itChild);
            if (pChild != nullptr)
            {
                return pChild;
            }

            ++itChild;
        }

        return nullptr;
    }

    /**
     * Get a child composite by index; returns non-null upon success
     */
    inline virtual Composite<T> *
    getChild(std::size_t index) const final
    {
        return getChild<Composite<T>>(index);
    }

    /**
     * Get a child composite by index; returns non-null upon success
     */
    template<typename U>
    U *
    getChild(std::size_t index) const
    {
        U *pChild = nullptr;
        if (index < m_children.size())
        {
            auto &&itChild = m_children.begin();
            std::advance(itChild,
                         index);
            pChild = dynamic_cast<U *>(*itChild);
        }

        return pChild;
    }

    /**
     * Get a child composite by name; returns non-null upon success
     */
    inline virtual Composite<T> *
    getChild(const std::string &name) const final
    {
        return getChild<Composite<T>>(name);
    }

    /**
     * Get a child composite by name; returns non-null upon success
     */
    template<typename U>
    U *
    getChild(const std::string &name) const
    {
        if (!name.empty())
        {
            for (auto &&itChild = m_children.begin(); itChild != m_children.end(); ++itChild)
            {
                auto *pChild = *itChild;

                if (pChild != nullptr &&
                    pChild->getName().compare(name) == 0)
                {
                    return dynamic_cast<U *>(pChild);
                }
            }
        }

        return nullptr;
    }

    /**
     * Get the current object's child composites
     */
    inline virtual std::vector<Composite<T> *> &
    getChildren(void) final
    {
        return m_children;
    }

    /**
     * Get a vector of child composites of type U
     */
    template<typename U>
    inline void
    getChildren(std::vector<U *> &children) const
    {
        const_cast<Composite<T> *>(this)->getChildren(children);
    }

    /**
     * Get a vector of child composites of type U
     */
    template<typename U>
    void
    getChildren(std::vector<U *> &children)
    {
        // remove child composites that are neither of type U nor a child of this object from the input vector
        auto &&itChild = children.cbegin();
        while (itChild != children.cend())
        {
            auto *pChild = dynamic_cast<U *>(*itChild);
            if (pChild == nullptr ||
                !pChild->isChild(this))
            {
                itChild = children.erase(itChild);
            }
            else
            {
                ++itChild;
            }
        }

        // add child composites that are of type U from the current object to the input vector
        auto &&itThisChild = m_children.cbegin();
        while (itThisChild != m_children.cend())
        {
            auto *pChild = dynamic_cast<U *>(*itThisChild);
            if (pChild != nullptr)
            {
                children.push_back(pChild);
            }

            ++itThisChild;
        }
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string
    getClassName(void) const override
    {
        return "Composite<" + std::string(typeid (T).name())
                            + ">";
    }

    /**
     * Function to return the depth of the current object relative to the root of the tree, where the root of
     * the tree is at a depth of one
     */
    virtual size_t
    getDepth(void) const final
    {
        size_t depth = 0;
        auto *pComposite = this;
        while (pComposite != nullptr)
        {
            pComposite = pComposite->getParent();
            ++depth;
        }

        return depth;
    }

    /**
     * Get a vector of descendant composites of type U within the current object's subtree
     */
    template<typename U>
    void
    getDescendants(std::vector<U *> &descendants)
    {
        // remove composites that are neither of type U nor a descendant of this object from the input vector
        auto &&itDescendant = descendants.cbegin();
        while (itDescendant != descendants.cend())
        {
            auto *pDescendantComposite = dynamic_cast<U *>(*itDescendant);

            if (pDescendantComposite == nullptr ||
                !pDescendantComposite->isDescendant(this))
            {
                itDescendant = descendants.erase(itDescendant);
            }
            else
            {
                ++itDescendant;
            }
        }

        // traverse the subtree, find descendant platforms of type U
        auto *pComposite = getChild();
        while (pComposite != this &&
               pComposite != nullptr)
        {
            auto *pDescendantComposite = pComposite->template cast<U>();
            if (pDescendantComposite != nullptr)
            {
                descendants.push_back(pDescendantComposite);
            }

            auto *pChild = pComposite->getChild();
            while (pChild == nullptr &&
                   pComposite != this)
            {
                pChild = pComposite->getRightSibling();
                pComposite = pComposite->getParent();
            }

            pComposite = pChild;
        }
    }

    /**
     * Get the greatest ancestor composite by type; returns non-null upon success
     */
    template<typename U>
    U *
    getGreatestAncestor(void) const
    {
        U *pAncestorComposite = nullptr;
        auto *pComposite = this;
        while (pComposite != nullptr)
        {
            auto *pParent = pComposite->template getParent<U>();
            if (pParent != nullptr)
            {
                pAncestorComposite = pParent;
            }

            pComposite = pComposite->getParent();
        }

        return pAncestorComposite;
    }

    /**
     * Get this object's index
     */
    virtual std::size_t
    getIndex(void) const override final
    {
        if (m_pParent == nullptr)
        {
            return 0;
        }
        else
        {
            auto &&itChild = std::find(m_pParent->m_children.cbegin(),
                                       m_pParent->m_children.cend(),
                                       this);

            if (itChild != m_pParent->m_children.cend())
            {
                return std::distance(m_pParent->m_children.cbegin(),
                                     itChild);
            }
            else
            {
                // this should never happen...
                return m_pParent->m_children.size();
            }
        }
    }

    /**
     * Get the least ancestor composite by type; returns non-null upon success
     */
    template<typename U>
    U *
    getLeastAncestor(void) const
    {
        auto *pComposite = this;
        while (pComposite != nullptr)
        {
            auto *pAncestorComposite = pComposite->template getParent<U>();
            if (pAncestorComposite != nullptr)
            {
                return pAncestorComposite;
            }

            pComposite = pComposite->getParent();
        }

        return nullptr; // there is no ancestor of the specified type
    }

    /**
     * Get the current object's left sibling
     */
    inline virtual Composite<T> *
    getLeftSibling(void) const final
    {
        return getLeftSibling<Composite<T>>();
    }

    /**
     * Get the current object's left sibling
     */
    template<typename U>
    U *
    getLeftSibling(void) const
    {
        auto *pParent = getParent();
        if (pParent == nullptr)
        {
            return nullptr;
        }
        else
        {
            auto &&itChild = std::find(pParent->m_children.cbegin(),
                                       pParent->m_children.cend(),
                                       this);

            if (itChild == pParent->m_children.cbegin())
            {
                return nullptr;
            }

            return dynamic_cast<U *>(*--itChild);
        }
    }

    /**
     * Get the name assigned to this object
     */
    inline virtual std::string
    getName(void) const override final
    {
        return m_name;
    }

    /**
     * Get the parent composite of the current object; returns null if the current object is a root
     */
    inline virtual Composite<T> *
    getParent(void) const final
    {
        return m_pParent;
    }

    /**
     * Get the parent composite of the current object; returns null if the current object is a root
     */
    template<typename U>
    inline U *
    getParent(void) const
    {
        return dynamic_cast<U *>(m_pParent);
    }

    /**
     * Get the current object's right sibling
     */
    inline virtual Composite<T> *
    getRightSibling(void) const
    {
        return getRightSibling<Composite<T>>();
    }

    /**
     * Get the current object's right sibling
     */
    template<typename U>
    U *
    getRightSibling(void) const
    {
        auto *pParent = getParent();
        if (pParent == nullptr)
        {
            return nullptr;
        }
        else
        {
            auto &&itChild = std::find(pParent->m_children.cbegin(),
                                       pParent->m_children.cend(),
                                       this);

            if (++itChild == pParent->m_children.cend())
            {
                return nullptr;
            }

            return dynamic_cast<U *>(*itChild);
        }
    }

    /**
     * Get the root composite
     */
    virtual Composite<T> *
    getRoot(void) const
    {
        auto *pRoot = cast<Composite<T>>();
        while (true)
        {
            auto *pParent = pRoot->getParent();
            if (pParent == nullptr)
            {
                break;
            }
            else
            {
                pRoot = pParent;
            }
        }

        return pRoot;
    }

    /**
     * Return whether or not the current object has children
     */
    inline virtual bool
    hasChildren(void) const final
    {
        return !m_children.empty();
    }

    /**
     * Return whether or not the current object and input composite reside within a common branch
     */
    template<typename U>
    bool
    hasCommonBranch(U *pComposite) const
    {
        bool bSuccess = (pComposite != nullptr);
        if (bSuccess)
        {
            bSuccess = isAncestor(pComposite);
            if (!bSuccess)
            {
                bSuccess = isDescendant(pComposite);
            }
        }

        return bSuccess;
    }

    /**
     * Return whether or not the current object has a parent
     */
    inline virtual bool
    hasParent(void) const final
    {
        return m_pParent != nullptr;
    }

    /**
     * Determines if the current object is an ancestor of the input composite
     */
    template<typename U>
    bool
    isAncestor(U *pComposite) const
    {
        bool bSuccess = isFamily(pComposite);
        if (bSuccess)
        {
            bSuccess = pComposite->isDescendant(this);
        }

        return bSuccess;
    }

    /**
     * Determines if the current object is a child of the input composite
     */
    template<typename U>
    inline bool
    isChild(U *pComposite)
    {
        return pComposite != nullptr &&
               m_pParent == pComposite;
    }

    /**
     * Determines if the current object is a descendant of the input composite
     */
    template<typename U>
    bool
    isDescendant(U *pComposite) const
    {
        auto *pCurrent = this->cast<U>();
        if (pComposite != pCurrent)
        {
            do
            {
                if (pCurrent == pComposite)
                {
                    return true;
                }
                else
                {
                    pCurrent = pCurrent->template getParent<U>();
                }
            }
            while (pCurrent != nullptr);
        }

        return false;
    }

    /**
     * Determines if the input object resides in the same tree as the current object
     */
    template<typename U>
    bool
    isFamily(U *pComposite) const
    {
        bool bSuccess = (pComposite != nullptr);
        if (bSuccess)
        {
            auto *pThisRoot = getGreatestAncestor<U>();
            auto *pRoot = pComposite->template getGreatestAncestoretRoot<U>();

            bSuccess = (pThisRoot == pRoot);
        }

        return bSuccess;
    }

    /**
     * Determines if the current object is the root of a tree
     */
    inline virtual bool
    isRoot(void) const final
    {
        return m_pParent == nullptr;
    }

    /**
     * Remove a child composite from the current object (doesn't delete the child composite); returns true upon
     * success
     * @param pChild a pointer to the child composite to be removed from the current object
     */
    template<typename U>
    bool
    removeChild(U *pChild)
    {
        bool bSuccess = false;
        do
        {
            auto &&itChild = std::find(m_children.begin(),
                                       m_children.end(),
                                       pChild);

            bSuccess = (itChild != m_children.end());
            if (bSuccess)
            {
                pChild->m_pParent = nullptr;
                m_children.erase(itChild);
            }
        }
        while (bSuccess);

        return bSuccess;
    }

private:

    /**
     * Set this object's index
     */
    inline virtual void
    setIndex(std::size_t index) override final
    {
        // disable this as index is determined by child's position relative to parent's first child
    }

public:

    /**
     * Set the name assigned to this object
     */
    virtual void
    setName(const std::string &name) override
    {
        auto *pComposite = getRoot()->find(name);
        if (pComposite == nullptr)
        {
            m_name = name;
        }
        else
        {
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__)
                                                              + ": "
                                                              + "Composite name must be unique.");
        }
    }

    /**
     * Set the parent composite of the current object
     */
    template<typename U>
    void
    setParent(U *pParent)
    {
        if (m_pParent != nullptr)
        {
            m_pParent->removeChild(this);
        }

        if (pParent != nullptr &&
            pParent != m_pParent)
        {
            pParent->addChild(this);
        }
    }

    /**
     * Swap function
     */
    virtual void
    swap(Composite<T> &composite) override final
    {
        m_children.swap(composite.m_children);

        // child composites have been swapped, need to set the parent composites accordingly
        std::for_each(m_children.cbegin(),
                      m_children.cend(),
                      [this] (auto *pChild)
                      {
                          pChild->setParent(this);
                      });
        std::for_each(composite.m_children.cbegin(),
                      composite.m_children.cend(),
                      [&composite] (auto *pChild)
                      {
                          pChild->setParent(&composite);
                      });
        m_name.swap(composite.m_name);

        // swap the parent composites
        auto *pParent = composite.getParent();
        composite.setParent(m_pParent);
        setParent(pParent);
    }

protected:

    /**
     * vector of child composites
     */
    std::vector<Composite<T> *> m_children;

    /**
     * name of this composite
     */
    std::string m_name;

    /**
     * pointer to the parent composite of the current object
     */
    Composite<T> *m_pParent;
};

}

}

#endif
