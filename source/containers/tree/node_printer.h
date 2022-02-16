#ifndef NODE_PRINTER_H
#define NODE_PRINTER_H

namespace containers
{

namespace tree
{

/**
 * Functor to print the nodes of the tree
 */
template<typename T, T (*pCloneFunction)(const T &) = nullptr, void (*pDeleteFunction)(T &) = nullptr>
struct NodePrinter final
{
    /**
     * Typedef declarations
     */
    typedef NodePrinter<T, pCloneFunction, pDeleteFunction> tNodePrinter;
    typedef TreeNode<T, pCloneFunction, pDeleteFunction> tTreeNode;

    /**
     * Constructor
     */
    NodePrinter(std::ostream &stream, const std::string &delimiter = "\n")
    : m_delimiter(delimiter),
      m_stream(stream)
    {

    }

    /**
     * Copy constructor
     */
    NodePrinter(const tNodePrinter &nodePrinter)
    : m_delimiter(nodePrinter.m_delimiter),
      m_stream(nodePrinter.m_stream)
    {

    }

    /**
     * Move constructor
     */
    NodePrinter(tNodePrinter &&nodePrinter)
    : m_delimiter(std::move(nodePrinter.m_delimiter)),
      m_stream(nodePrinter.m_stream)
    {

    }

    /**
     * Destructor
     */
    ~NodePrinter(void)
    {

    }

    /**
     * Function call operator
     */
    inline void operator () (const tTreeNode *pNode) const
    {
        auto *pData = tTreeNode::getData(pNode);

        NodePrinter::operator () (pData);
    }

    /**
     * Function call operator
     */
    inline void operator () (T *pData) const
    {
        if (pData != nullptr)
            m_stream << *pData << m_delimiter;
    }

    /**
     * delimiter printed between nodes
     */
    std::string m_delimiter;

    /**
     * reference to an std::ostream object
     */
    std::ostream &m_stream;
};

}

}

#endif
