#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H

#include "branch_counter.h"
#include "branch_printer.h"
#include "cloneable.h"
#include "node_counter.h"
#include "node_printer.h"
#include "output_streamable.h"
#include "tree_node.h"
#include <string>

namespace containers
{

namespace tree
{

/**
 * This class implements an iterative prefix tree. Insertion, retrieval, and deletion of branches are performed
 * via standard template library containers
 */
template<typename T,
         T (*pCloneFunction)(const T &) = nullptr,
         void (*pDeleteFunction)(T &) = nullptr>
class PrefixTree
: public attributes::interfaces::Cloneable<PrefixTree<T, pCloneFunction, pDeleteFunction>>,
  public attributes::concrete::OutputStreamable<PrefixTree<T, pCloneFunction, pDeleteFunction>>
{
public:

    /**
     * Typedef declarations
     */
    typedef PrefixTree<T, pCloneFunction, pDeleteFunction> tPrefixTree;
    typedef TreeNode<T, pCloneFunction, pDeleteFunction> tTreeNode;

    /**
     * Using declarations
     */
    using attributes::concrete::OutputStreamable<PrefixTree>::print;

    /**
     * Constructor
     */
    template<typename U = typename std::decay<T>::type,
             typename std::enable_if<std::is_default_constructible<U>::value, int>::type = 0>
    PrefixTree(void)
    : m_pRoot(nullptr)
    {
        m_pRoot = getRoot();
    }

    /**
     * Constructor
     */
    template<typename Arg, typename U = typename std::decay<T>::type,
             typename std::enable_if<!std::is_default_constructible<U>::value, int>::type = 0>
    PrefixTree(Arg &&arg)
    : m_pRoot(new tTreeNode(std::forward<Arg>(arg), nullptr, "Root"))
    {

    }

    /**
     * Construct a tree from a node and its children (subtree)
     */
    PrefixTree(const tTreeNode *pRoot)
    : m_pRoot(nullptr)
    {
        operator = (pRoot);
    }

    /**
     * Copy constructor
     */
    PrefixTree(const tPrefixTree &tree)
    : m_pRoot(nullptr)
    {
        operator = (tree);
    }

    /**
     * Move constructor
     */
    PrefixTree(tPrefixTree &&tree)
    : m_pRoot(nullptr)
    {
        operator = (std::move(tree));
    }

    /**
     * Destructor
     */
    virtual ~PrefixTree(void) override
    {
        destroy(m_pRoot);
    }

    /**
     * Copy assignment operator
     */
    tPrefixTree &operator = (const tPrefixTree &tree)
    {
        if (&tree != this)
        {
            destroy(m_pRoot);
            copy(tree.m_pRoot);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    tPrefixTree &operator = (tPrefixTree &&tree)
    {
        if (&tree != this)
        {
            m_pRoot = tree.m_pRoot;
            tree.m_pRoot = nullptr;
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    tPrefixTree &operator = (const tTreeNode *pTree)
    {
        if (pTree != m_pRoot)
        {
            destroy(m_pRoot);
            copy(pTree);
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual tPrefixTree *clone(void) const override
    {
        return new tPrefixTree(*this);
    }

    /**
     * clone() function
     */
    inline static tPrefixTree clone(tTreeNode *pRoot)
    {
        tPrefixTree tree;

        tree.copy(pRoot);

        return tree;
    }

    /**
     * Function to copy a node and all of its sub-branches
     */
    virtual void copy(const tTreeNode *pRoot) final
    {
        if (pRoot)
        {
            auto *pCopyParent = m_pRoot = pRoot->clone();
            const auto *pParent = pRoot, *pNode = pParent->getChild();
            auto *pCopyNode = pCopyParent->getChild();
            auto *pCopyRootParent = m_pRoot->getParent();

            do
            {
                if (pNode == nullptr)
                {
                    pCopyNode = pCopyParent;
                    pCopyParent = pCopyParent->getParent();
                    pNode = pParent;
                    pParent = pParent->getParent();
                    pNode = pNode->getRight();
                }
                else
                {
                    auto *pTemp = pNode->clone();
                    if (pCopyNode == nullptr)
                        pCopyNode = pTemp;
                    else
                    {
                        pCopyNode->setRight(pTemp);
                        pCopyNode = pCopyNode->getRight();
                    }

                    pCopyNode->setParent(pCopyParent);
                    pCopyParent = pCopyNode;
                    pCopyNode = pCopyNode->getChild();
                    pParent = pNode;
                    pNode = pNode->getChild();
                }
            }
            while (pCopyParent != pCopyRootParent);
        }
    }

    /**
     * Delete all branches from tree
     */
    inline virtual void destroy(void) final
    {
        destroy(m_pRoot);
    }

    /**
     * Delete all branches from the subtree denoted by pRoot
     */
    static void destroy(tTreeNode *&pRoot)
    {
        auto *pLeft = tTreeNode::getLeft(pRoot);
        auto *pParent = tTreeNode::getParent(pRoot);
        auto *pRight = tTreeNode::getRight(pRoot);
        for (auto *&pNode = pRoot; pNode != pParent;)
        {
            if (tTreeNode::getChild(pNode))
                pNode = pNode->getChild();
            else
            {
                auto *pTemp = pNode;
                if (pNode != nullptr && pNode->getParent() != pParent)
                    pNode->getParent()->setChild(pNode->getRight());
                else if (pParent != nullptr)
                {
                    auto *pChild = pParent->getChild();
                    if (pChild == pRoot)
                        pParent->setChild(pRight);
                }

                if (pNode != nullptr)
                    pNode = pNode->getParent();

                if (pTemp != nullptr)
                    delete pTemp;
            }
        }

        tTreeNode::setRight(pLeft, pRight);
        tTreeNode::setLeft(pRight, pLeft);
    }

    /**
     * Returns true if tree is empty
     */
    inline virtual bool empty(void) const final
    {
        return tTreeNode::getChild(m_pRoot) == nullptr;
    }

private:

    /**
     * Get Branch
     * @param[in]  branch     storage to hold resultant branch
     * @param      pRoot      points to the parent of the starting node
     * @param      pBranchEnd points to the ending node
     * @param[out] branch     the data from the nodes formed by tracing pBranchStart to pBranchEnd
     */
    template<class Container>
    inline static void getBranch(Container &branch, const tTreeNode *pRoot, const tTreeNode *pBranchEnd)
    {
        const auto *pNode = pBranchEnd;
        auto &&itBranch = branch.rbegin();
        while (pNode != pRoot && itBranch != branch.rend())
        {
            if (pNode == nullptr)
                break;
            else
            {
                *itBranch++ = pNode->getData();
                pNode = pNode->getParent();
            }
        }
    }

public:

    /**
     * Get the number of branches in the tree
     */
    inline virtual std::size_t getNumberOfBranches(void) const final
    {
        BranchCounter<T, pCloneFunction, pDeleteFunction> branchCounter;
        traverse(branchCounter, m_pRoot);

        return branchCounter.m_numBranches;
    }

    /**
     * Get the number of nodes in the tree
     */
    inline virtual std::size_t getNumberOfNodes(void) const final
    {
        NodeCounter<T, pCloneFunction, pDeleteFunction> nodeCounter;
        traverse(nodeCounter, m_pRoot);

        return nodeCounter.m_numNodes;
    }

    /**
     * Get pointer to the root node
     */
    template<typename U = typename std::decay<T>::type,
             typename std::enable_if<std::is_default_constructible<U>::value, int>::type = 0>
    inline tTreeNode *getRoot(void)
    {
        if (m_pRoot == nullptr)
        {
            m_pRoot = new tTreeNode;
            m_pRoot->setLabel("Root");
        }

        return m_pRoot;
    }

    /**
     * Get pointer to the root node
     */
    template<typename U = typename std::decay<T>::type,
             typename std::enable_if<!std::is_default_constructible<U>::value, int>::type = 0>
    inline tTreeNode *getRoot(void)
    {
        return m_pRoot;
    }

    /**
     * Insert a branch into the tree under the root node
     * @param  branch the desired data for which to insert
     * @return        true if branch was at least partially inserted as a branch in the tree
     */
    template<class Container>
    bool insert(const Container &branch)
    {
        auto *pNode = getRoot();
        auto &&itBranch = branch.cbegin();
        while (itBranch != branch.cend())
        {
            auto *pTemp = insert(pNode, *itBranch);
            if (!pTemp)
                break;

            pNode = pTemp;
            ++itBranch;
        }

        return itBranch != branch.cbegin();
    }

    /**
     * Insert a branch into a subtree
     * @param      branch the desired data for which to insert
     * @param[in]  pNode  a pointer to the subtree
     * @param[out] pNode  updated to contain a pointer to the end of the newly inserted branch
     * @return            true if branch was at least partially inserted as a branch in the tree
     */
    template<class Container>
    static bool insert(const Container &branch, tTreeNode *&pNode)
    {
        auto &&itBranch = branch.cbegin();
        while (itBranch != branch.cend())
        {
            auto *pTemp = insert(pNode, *itBranch);
            if (!pTemp)
                break;

            pNode = pTemp;
            ++itBranch;
        }

        return itBranch != branch.cbegin();
    }

    /**
     * Insert data as a child node of the input parent node
     * @param[in]  pParent a pointer to the parent node under which the input data will be inserted as a child
     *                     node
     * @param      data    the value to be inserted as a node into the tree
     * @param      label   an optional string that can be used to label the node
     * @param[out] pParent upon successful insertion of child, has been updated to reflect its new child
     * @return             a pointer to the newly inserted or already existing child node. Note that this
     *                     function overload will insert a node when it compares less or equal to a node
     *                     starting from the left-most child
     */
    inline static tTreeNode *insert(tTreeNode *pParent, const T &data, const std::string &label = "")
    {
        return insert(pParent, data, label, std::less_equal<T>());
    }

    /**
     * Insert data as a child node of the input parent node
     * @param[in]  pParent    a pointer to the parent node under which the input data will be inserted as a
     *                        child node
     * @param      data       the value to be inserted as a node into the tree
     * @param      label      an optional string that can be used to label the node
     * @param      comparison a binary function object that defines how to compare nodes for insertion
     * @param[out] pParent    upon successful insertion of child, has been updated to reflect its new child
     * @return                a pointer to the newly inserted or already existing child node
     */
    template<class CompareFunc>
    inline static tTreeNode *insert(tTreeNode *pParent, const T &data, const std::string &label,
                                    CompareFunc &&compareFunc)
    {
        auto *pRight = tTreeNode::getChild(pParent), *pLeft = tTreeNode::getLeft(pRight);
        retrieve(data, pRight, compareFunc, pLeft);

        if (!pRight || (pRight && pRight->getData() != data))
            pRight = insert(pLeft, pRight, new tTreeNode(data, pParent, label));

        return pRight;
    }

private:

    /**
     * Insert node as a child node of the input parent node
     * @param  pLeft  a pointer to the left node
     * @param  pRight a pointer to the right node
     * @param  pNode  a pointer to the node to be inserted
     * @return        a pointer to pNode
     */
    static tTreeNode *insert(tTreeNode *pLeft, tTreeNode *pRight, tTreeNode *pNode)
    {
        tTreeNode::setLeft(pRight, pNode);
        tTreeNode::setRight(pLeft, pNode);

        if (pLeft == nullptr)
        {
            auto *pParent = tTreeNode::getParent(pNode);
            tTreeNode::setChild(pParent, pNode);
        }

        return pNode;
    }

public:

    /**
     * Print all branches in the tree
     * @param stream a reference to an std::ostream object
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        return print(stream, "");
    }

    /**
     * Print all branches in the tree
     * @param stream a reference to an std::ostream object
     */
    inline virtual std::ostream &print(std::ostream &stream, const std::string &delimiter) const final
    {
        BranchPrinter<T, pCloneFunction, pDeleteFunction> branchPrinter(m_pRoot, stream, delimiter);
        traverse(branchPrinter, m_pRoot);

        return stream;
    }

    /**
     * Delete a branch from the tree (assuming an exact match is found)
     * @param  branch the desired branch to delete
     * @return        true if branch was found and deleted
     */
    template<class Container>
    inline bool remove(const Container &branch) const
    {
        return remove(const_cast<Container &>(branch), std::equal_to<T>());
    }

    /**
     * Delete a branch from the tree using specific comparison criteria
     * @param  branch      the desired branch to delete
     * @param  compareFunc a binary function object that defines how to compare nodes
     * @param  bRetrieve   flag indicating whether or not the input search branch should be modified to reflect
     *                     a branch in the tree, assuming a match satisfying the given search criteria exists
     * @return             true if branch was found and deleted
     */
    template<class Container, class CompareFunc>
    inline bool remove(Container &branch, CompareFunc &&compareFunc, bool bRetrieve = false) const
    {
        auto *pNode = m_pRoot;

        return remove(branch, compareFunc, pNode, bRetrieve);
    }

    /**
     * Delete a branch from the subtree that satisfies specific comparison criteria
     * @param  branch      the desired branch to delete
     * @param  compareFunc a binary function object that defines how to compare nodes
     * @param  pRoot       a pointer to the subtree to be searched
     * @param  bRetrieve   flag indicating whether or not the input search branch should be modified to reflect
     *                     a branch in the tree, assuming a match satisfying the given search criteria exists
     * @return             true if branch was at least partially deleted
     */
    template<class Container, class CompareFunc>
    static bool remove(Container &branch, CompareFunc &&compareFunc, tTreeNode *const pRoot,
                       bool bRetrieve = false)
    {
        tTreeNode *pBranchEnd = pRoot, *pParent;
        bool bFound = retrieve(branch, compareFunc, pBranchEnd, bRetrieve) && !tTreeNode::getChild(pBranchEnd);
        if (bFound && pRoot && pRoot != pBranchEnd)
        {
            for (auto *pNode = pBranchEnd; pRoot != pBranchEnd; pBranchEnd = pNode)
            {
                pParent = pNode->getParent();
                if (pNode->getChild() == nullptr)
                {
                    auto *pLeft = tTreeNode::getLeft(pNode);
                    auto *pRight = tTreeNode::getRight(pNode);
                    if (!tTreeNode::setRight(pLeft, pRight))
                        tTreeNode::setLeft(pRight, pLeft);

                    if (pParent->getChild() == pNode)
                        pParent->setChild(pNode->getRight());

                    if (pBranchEnd != nullptr)
                        delete pBranchEnd;
                }

                pNode = pParent;
            }
        }

        return bFound;
    }

    /**
     * Delete a node and its subtree
     */
    inline virtual void remove(tTreeNode *&pRoot) final
    {
        destroy(pRoot);
    }

    /**
     * Remove singleton nodes from the tree
     */
    virtual bool removeSingletons(void) final
    {
        bool bSuccess = (m_pRoot != nullptr);
        if (bSuccess)
        {
            tTreeNode *pNode = m_pRoot, *pParent = nullptr;
            do
            {
                if (pNode == nullptr)
                {
                    pNode = pParent;
                    pParent = pParent->getParent();
                    pNode = pNode->getRight();
                }
                else
                {
                    auto *pChild = pNode->getChild();
                    if (pNode->isSingleton() && pChild != nullptr && pParent != nullptr)
                    {
                        pParent->removeChild(pNode);
                        for (pNode = pNode->getChild(); pNode != nullptr; pNode = pNode->getRight())
                            pNode->setParent(pParent);

                        delete pNode;
                        pNode = pChild;
                    }

                    pParent = pNode;
                    pNode = pNode->getChild();
                }
            }
            while (pParent != nullptr);
        }

        return bSuccess;
    }

    /**
     * Search the tree for an exact match to branch
     * @param  branch the desired data for which to search the tree
     * @return        true if a branch matching the input is found
     */
    template<class Container>
    inline bool retrieve(const Container &branch) const
    {
        return retrieve(const_cast<Container &>(branch), std::equal_to<T>());
    }

    /**
     * Search the tree for a match that satisfies specific comparison criteria
     * @param  branch      the desired data for which to search the tree
     * @param  compareFunc a binary function object that defines how to compare nodes
     * @param  bRetrieve   flag indicating whether or not the input search branch should be modified to reflect
     *                     a branch in the tree, assuming a match satisfying the given search criteria exists
     * @return             true if a branch matching the input is found
     */
    template<class Container, class CompareFunc>
    inline bool retrieve(Container &branch, CompareFunc &&compareFunc, bool bRetrieve = false) const
    {
        auto *pNode = m_pRoot;

        return retrieve(branch, compareFunc, pNode, bRetrieve);
    }

    /**
     * Search a subtree for a branch that satisfies specific comparison criteria
     * @param      branch      the desired data for which to search the subtree pointed to by pNode
     * @param      compareFunc a binary function object that defines how to compare nodes
     * @param[in]  pNode       a pointer to the parent of the children to be searched
     * @param      bRetrieve   flag indicating whether or not the input search branch should be modified to
     *                         reflect a branch in the tree, assuming a match satisfying the given search
     *                         criteria exists
     * @param[out] pNode       will be updated to point to the last node in the subtree that matches the search
     *                         branch
     * @return                 true if a complete branch matched the criteria set forth by the comparison
     *                         criteria
     */
    template<class Container, class CompareFunc>
    static bool retrieve(Container &branch, CompareFunc &&compareFunc, tTreeNode *&pNode,
                         bool bRetrieve = false)
    {
        if (!branch.empty() && pNode != nullptr)
        {
            auto &&itBranch = branch.cbegin();
            auto *pParent = pNode, *pRoot = pParent;
            for (pNode = pParent->getChild();;)
            {
                if (retrieve(*itBranch, pNode, compareFunc))
                {
                    if (++itBranch == branch.cend())
                    {
                        if (bRetrieve)
                            getBranch(branch, pRoot, pNode);

                        return true;
                    }

                    pParent = pNode;
                    pNode = pNode->getChild();
                }
                else
                {
                    if (pNode == nullptr)
                    {
                        pNode = pParent;
                        pParent = pNode->getParent();
                        if (pNode == pRoot)
                            break;

                        --itBranch;
                    }

                    pNode = pNode->getRight();
                }
            }
        }

        return false;
    }

    /**
     * Search the children for a node that satisfies specific comparison criteria
     * @param      data        the value for which to search the children
     * @param[in]  pNode       a pointer to the first child
     * @param      compareFunc a binary function object which defines how to compare nodes
     * @param[out] pNode       a pointer to the node, if found, that matches the given criteria
     * @param[out] pLeft       a pointer to the node, if found, to the left of pNode
     * @return                 true if a node was found
     */
    template<class CompareFunc, typename std::enable_if<std::is_same<typename std::decay<CompareFunc>::type,
                                         std::less_equal<T>>::value, int>::type = 0>
    inline static bool retrieve(const T &data, tTreeNode *&pNode, CompareFunc &&compareFunc, tTreeNode *&pLeft)
    {
        std::vector<tTreeNode *> children;
        auto *pParent = tTreeNode::getParent(pNode);
        tTreeNode::getChildren(pParent, children);
        auto &&numChildren = children.size();
        int maxChild = int(numChildren - 1), minChild = 0;
        while (minChild <= maxChild)
        {
            int midChild = (maxChild + minChild) >> 1;
            pNode = children[midChild];

            auto &&nodeData = pNode->getData();
            if (nodeData < data)
                minChild = midChild + 1;
            else if (nodeData > data)
                maxChild = midChild - 1;
            else
                break;
        }

        if (pNode != nullptr)
        {
            auto &&nodeData = pNode->getData();
            if (data > nodeData)
            {
                pLeft = pNode;
                pNode = tTreeNode::getRight(pLeft);
            }
            else
                pLeft = tTreeNode::getLeft(pNode);

            return true;
        }
        else
            return false;
    }

    /**
     * Search the children for a node that satisfies specific comparison criteria
     * @param      data        the value for which to search the children
     * @param[in]  pNode       a pointer to the first child
     * @param      compareFunc a binary function object which defines how to compare nodes
     * @param[out] pNode       a pointer to the node, if found, that matches the given criteria
     * @param[out] pLeft       a pointer to the node, if found, to the left of pNode
     * @return                 true if a node was found
     */
    template<class CompareFunc, typename std::enable_if<!std::is_same<typename std::decay<CompareFunc>::type,
                                         std::less_equal<T>>::value, int>::type = 0>
    inline static bool retrieve(const T &data, tTreeNode *&pNode, CompareFunc &&compareFunc, tTreeNode *&pLeft)
    {
        for (; pNode != nullptr; pLeft = pNode, pNode = tTreeNode::getRight(pNode))
            if (pNode && compareFunc(pNode->getData(), data))
                return true;

        return false;
    }

    /**
     * Search the children for the specified data
     * @param      data        the value for which to search the children
     * @param[in]  pNode       a pointer to the first child
     * @param      compareFunc a binary function object which defines how to compare nodes
     * @param[out] pNode       a pointer to the node, if found, that matches the given criteria
     * @return                 true if a node was found
     */
    template<class CompareFunc>
    inline static bool retrieve(const T &data, tTreeNode *&pNode, CompareFunc &&compareFunc)
    {
        tTreeNode *pLeft = nullptr;

        return retrieve(data, pNode, compareFunc, pLeft);
    }

    /**
     * Prefix tree traversal function
     * @param nodeFunctor a function object used to process each node
     * @param pNode       a pointer to the first child
     */
    template<typename Functor>
    bool traverse(Functor &&nodeFunctor)
    {
        return traverse(std::forward<Functor>(nodeFunctor), m_pRoot);
    }

    /**
     * Prefix tree traversal function
     * @param nodeFunctor a function object used to process each node
     * @param pNode       a pointer to the first child
     */
    template<class Functor>
    static bool traverse(Functor &&nodeFunctor, tTreeNode *pNode)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
        {
            auto *pRoot = pNode->getParent(), *pParent = pRoot;
            do
            {
                bSuccess = std::forward<Functor>(nodeFunctor)(pNode);
                if (!bSuccess)
                    break;

                if (pNode == nullptr)
                {
                    pNode = pParent;
                    pParent = pParent->getParent();
                    pNode = pNode->getRight();
                }
                else
                {
                    pParent = pNode;
                    pNode = pNode->getChild();
                }
            }
            while (pParent != pRoot);
        }

        return bSuccess;
    }

private:

    /**
     * a pointer to the root of the tree
     */
    tTreeNode *m_pRoot;
};

}

}

#endif
