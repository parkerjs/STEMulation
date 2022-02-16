#ifndef BRANCH_PRINTER_H
#define BRANCH_PRINTER_H

#include <iostream>

namespace containers
{

namespace tree
{

/**
 * Functor to print the branches of the tree
 */
template<typename T, T (*pCloneFunction)(const T &) = nullptr, void (*pDeleteFunction)(T &) = nullptr>
struct BranchPrinter final
{
    /**
     * Typedef declarations
     */
    typedef BranchPrinter<T, pCloneFunction, pDeleteFunction> tBranchPrinter;
    typedef TreeNode<T, pCloneFunction, pDeleteFunction> tTreeNode;

    /**
     * Constructor
     */
    BranchPrinter(const tTreeNode *pRoot, std::ostream &stream, const std::string &delimiter = "")
    : m_delimiter(delimiter),
      m_pRoot(pRoot),
      m_stream(stream)
    {

    }

    /**
     * Copy constructor
     */
    BranchPrinter(const tBranchPrinter &branchPrinter)
    : m_delimiter(branchPrinter.m_delimiter),
      m_pRoot(branchPrinter.m_pRoot),
      m_stream(branchPrinter.m_stream)
    {

    }

    /**
     * Move constructor
     */
    BranchPrinter(tBranchPrinter &&branchPrinter)
    : m_delimiter(std::move(branchPrinter.m_delimiter)),
      m_pRoot(branchPrinter.m_pRoot),
      m_stream(branchPrinter.m_stream)
    {
        branchPrinter.m_pRoot = nullptr;
    }

    /**
     * Destructor
     */
    ~BranchPrinter(void)
    {

    }

    /**
     * Function call operator
     */
    inline bool operator () (const tTreeNode *pNode) const
    {
        bool bSuccess = true; // assume success
        if (pNode != nullptr && pNode->getChild() == nullptr)
        {
            bSuccess = print(pNode);
            m_stream << std::endl;
        }

        return bSuccess;
    }

    /**
     * Print function
     */
    template<typename U = T> inline typename
    std::enable_if<!std::is_pointer<typename std::remove_reference<U>::type>::value, bool>::type
    print(const tTreeNode *pNode) const
    {
        bool bSuccess = true; // assume success
        if (pNode != m_pRoot && pNode != nullptr)
        {
            bSuccess = print(pNode->getParent());
            if (bSuccess)
                m_stream << pNode->getData() << m_delimiter;
        }

        return bSuccess;
    }

    /**
     * Print function
     */
    template<typename U = T> inline typename
    std::enable_if<std::is_pointer<typename std::remove_reference<U>::type>::value, bool>::type
    print(const tTreeNode *pNode) const
    {
        bool bSuccess = true; // assume success
        if (pNode != m_pRoot && pNode != nullptr)
        {
            bSuccess = print(pNode->getParent());
            if (bSuccess)
            {
                auto *pData = pNode->getData();
                if (pData != nullptr)
                    m_stream << *pData << m_delimiter;
            }
        }

        return bSuccess;
    }

    /**
     * delimiter printed between branches
     */
    std::string m_delimiter;

    /**
     * pointer to the root of the tree
     */
    const tTreeNode *m_pRoot;

    /**
     * reference to an std::ostream object
     */
    std::ostream &m_stream;
};

}

}

#endif
