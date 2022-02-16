#ifndef NODE_COUNTER_H
#define NODE_COUNTER_H

namespace containers
{

namespace tree
{

/**
 * Functor to count the number of nodes in the tree
 */
template<typename T, T (*pCloneFunction)(const T &) = nullptr, void (*pDeleteFunction)(T &) = nullptr>
struct NodeCounter final
{
    /**
     * Typedef declarations
     */
    typedef NodeCounter<T, pCloneFunction, pDeleteFunction> tNodeCounter;
    typedef TreeNode<T, pCloneFunction, pDeleteFunction> tTreeNode;

    /**
     * Constructor
     */
    NodeCounter(void)
    : m_numNodes(0)
    {

    }

    /**
     * Copy constructor
     */
    NodeCounter(const tNodeCounter &nodeCounter)
    : m_numNodes(nodeCounter.m_numNodes)
    {

    }

    /**
     * Move constructor
     */
    NodeCounter(tNodeCounter &&nodeCounter)
    : m_numNodes(std::move(nodeCounter.m_numNodes))
    {

    }

    /**
     * Destructor
     */
    ~NodeCounter(void)
    {

    }

    /**
     * Function call operator
     */
    inline bool operator () (const tTreeNode *pNode) const
    {
        if (pNode != nullptr)
            ++m_numNodes;

        return true;
    }

    /**
     * count of the number of nodes in the tree
     */
    mutable std::size_t m_numNodes;
};

}

}

#endif
