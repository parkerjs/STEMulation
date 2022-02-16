#ifndef PREFIX_TREE_INTERPOLATOR_H
#define PREFIX_TREE_INTERPOLATOR_H

#include "reflective.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <iostream>

namespace math
{

namespace interpolators
{

/**
 * This class implements an algorithm to perform prefix tree node interpolation
 */
template<typename T, class functor>
class PrefixTreeInterpolator
: virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticMutexMappable<PrefixTreeInterpolator<T, functor>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<PrefixTreeInterpolator<T, functor>>
{
public:

    /**
     * Constructor
     * @param pRoot       a pointer to the root of the tree
     * @param nodeFunctor a reference to a function object that operates on or manipulates one or more nodes in
     *                    the tree
     */
    PrefixTreeInterpolator(const Containers::Tree::TreeNode<T> *pRoot, const functor &nodeFunctor)
    : m_bExtrapolate(false),
      m_nodeFunctor(nodeFunctor),
      m_pRoot(pRoot)
    {

    }

    /**
     * Copy constructor
     */
    PrefixTreeInterpolator(const PrefixTreeInterpolator<T, functor> &interpolator)
    {
        operator = (interpolator);
    }

    /**
     * Move constructor
     */
    PrefixTreeInterpolator(PrefixTreeInterpolator<T, functor> &&interpolator)
    {
        operator = (std::move(interpolator));
    }

    /**
     *  Destructor
     */
    virtual ~PrefixTreeInterpolator(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    PrefixTreeInterpolator<T, functor> &operator = (const PrefixTreeInterpolator<T, functor> &interpolator)
    {
        if (&interpolator != this)
        {
            m_bExtrapolate = interpolator.m_bExtrapolate;
            m_nodeFunctor = interpolator.m_nodeFunctor;
            m_pRoot = interpolator.m_pRoot;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    PrefixTreeInterpolator<T, functor> &operator = (PrefixTreeInterpolator<T, functor> &&number)
    {
        if (&interpolator != this)
        {
            m_bExtrapolate = std::move(interpolator.m_bExtrapolate);
            m_nodeFunctor = std::move(interpolator.m_nodeFunctor);
            m_pRoot = std::move(interpolator.m_pRoot);
            interpolator.m_pRoot = nullptr;
        }

        return *this;
    }

private:

    /**
     * Function to adjust the bounding nodes (for extrapolation and singletons)
     */
    virtual bool adjustBoundingNodes(Containers::Tree::TreeNode<T> *&pLeft,
                                     Containers::Tree::TreeNode<T> *&pRight) const final
    {
        bool bSingleton = false;
        if (pLeft && !pRight)
        {
            bSingleton = pLeft->isSingleton();
            if (m_bExtrapolate && !bSingleton)
            {
                pRight = pLeft;
                pLeft = Containers::Tree::TreeNode<T>::getLeft(pRight);
            }
            else
                pRight = pLeft;
        }
        else if (!pLeft && pRight)
        {
            bSingleton = pRight->isSingleton();
            if (m_bExtrapolate && !bSingleton)
            {
                pLeft = pRight;
                pRight = Containers::Tree::TreeNode<T>::getRight(pLeft);
            }
            else
                pLeft = pRight;
        }

        if (bSingleton && m_bExtrapolate)
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "Extrapolation failed because singleton node encountered in prefix tree."
                      << std::endl << std::endl;
            this->unlock(0);

            return false;
        }

        return true;
    }

public:

    /**
     * Set extrapolation status
     */
    inline virtual void extrapolate(bool bExtrapolate) final
    {
        m_bExtrapolate = bExtrapolate;
    }

    /**
     * Query extrapolation status
     */
    inline virtual bool extrapolate(void) const final
    {
        return m_bExtrapolate;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "PrefixTreeInterpolator";
    }

    /**
     * Interpolation function
     */
    template<class Container> bool interpolate(const Container &abscissas, T &y) const
    {
        return traverse(abscissas.begin(), abscissas.end(), m_pRoot, y);
    }

private:

    /**
     * Function to perform a recursive pre-order tree traversal, bound the current iterator between two child
     * nodes, and process the node bounds using the node functor
     */
    template<class iterator>
    bool traverse(iterator it, iterator itEnd, const Containers::Tree::TreeNode<T> *pNode, T &y) const
    {
        T x1, x2, y1, y2;
        Containers::Tree::TreeNode<T> *pLeft = nullptr;
        auto *pRight = Containers::Tree::TreeNode<T>::getChild(pNode);

        if (it != itEnd)
        {
            Containers::Tree::PrefixTree<T>::retrieve(*it, pRight, std::greater_equal<T>(), pLeft);
            adjustBoundingNodes(pLeft, pRight);
        }

        if (pLeft)
        {
            if (Containers::Tree::TreeNode<T>::getChild(pLeft) != nullptr && it != itEnd)
                traverse(it + 1, itEnd, pLeft, y1);
            else
            {
                y = m_nodeFunctor(pLeft->getData());

                return true;
            }

            x1 = pLeft->getData();
        }

        if (pRight)
        {
            if (Containers::Tree::TreeNode<T>::getChild(pRight) != nullptr && it != itEnd)
                traverse(it + 1, itEnd, pRight, y2);
            else
            {
                y = m_nodeFunctor(pRight->getData());

                return true;
            }

            x2 = pRight->getData();
        }

        if (pLeft || pRight)
        {
            y = m_nodeFunctor(*it, x1, x2, y1, y2);

            return true;
        }

        return false;
    }

    /**
     * extrapolation flag
     */
    bool m_bExtrapolate;

    /**
     * an instance of the node function object
     */
    functor m_nodeFunctor;

    // a pointer to the root of the tree
    const Containers::Tree::TreeNode<T> *m_pRoot;
};

}

}

#endif
