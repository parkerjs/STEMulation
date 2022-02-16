#ifndef BRANCH_COUNTER_H
#define BRANCH_COUNTER_H

#include "tree_node.h"

namespace containers
{

namespace tree
{

/**
 * Functor to count the number of branches in the tree
 */
template<typename T, T (*pCloneFunction)(const T &) = nullptr, void (*pDeleteFunction)(T &) = nullptr>
struct BranchCounter final
{
    /**
     * Typedef declarations
     */
    typedef BranchCounter<T, pCloneFunction, pDeleteFunction> tBranchCounter;
    typedef TreeNode<T, pCloneFunction, pDeleteFunction> tTreeNode;

    /**
     * Constructor
     */
    BranchCounter(void)
    : m_numBranches(0)
    {

    }

    /**
     * Copy constructor
     */
    BranchCounter(const tBranchCounter &branchCounter)
    : m_numBranches(branchCounter.m_numBranches)
    {

    }

    /**
     * Move constructor
     */
    BranchCounter(tBranchCounter &&branchCounter)
    : m_numBranches(std::move(branchCounter.m_numBranches))
    {

    }

    /**
     * Destructor
     */
    ~BranchCounter(void)
    {

    }

    /**
     * Function call operator
     */
    inline bool operator () (const tTreeNode *pNode) const
    {
        if (pNode != nullptr && pNode->getChild() == nullptr)
            ++m_numBranches;

        return true;
    }

    /**
     * count of the number of branches in the tree
     */
    mutable std::size_t m_numBranches;
};

}

}

#endif
