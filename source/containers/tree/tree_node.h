#ifndef TREE_NODE_H
#define TREE_NODE_H

#include "cloneable.h"
#include "is_reference_wrapped.h"
#include <functional>
#include <string>
#include <vector>

#if defined _MSC_VER && defined _WIN32
#pragma warning (disable : 4520)
#endif

namespace containers
{

namespace tree
{

/**
 * This class implements a node data structure that can be used to build tree data structures
 */
template<typename T, T (*pCloneFunction)(const T &) = nullptr, void (*pDeleteFunction)(T &) = nullptr>
class TreeNode
: public attributes::interfaces::Cloneable<TreeNode<T, pCloneFunction, pDeleteFunction>>
{
public:

    /**
     * Typedef declarations
     */
    typedef TreeNode<T, pCloneFunction, pDeleteFunction> tTreeNode;

    /**
     * Constructor
     */
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::remove_reference<U>::type>::value ||
                                     std::is_pointer<typename std::decay<U>::type>::value, int>::type = 0>
    TreeNode(void)
    : m_data(nullptr),
      m_pChild(nullptr),
      m_pLeft(nullptr),
      m_pParent(nullptr),
      m_pRight(nullptr)
    {

    }

    /**
     * Constructor
     */
    template<typename U = T,
             typename std::enable_if<!traits::tests::is_reference_wrapped<typename std::decay<U>::type>::value &&
                                     !std::is_pointer<typename std::remove_reference<U>::type>::value &&
                                     !std::is_pointer<typename std::decay<U>::type>::value, int>::type = 0>
    TreeNode(void)
    : m_pChild(nullptr),
      m_pLeft(nullptr),
      m_pParent(nullptr),
      m_pRight(nullptr)
    {

    }

    /**
     * Constructor
     */
    template<typename U>
    TreeNode(U &&data, tTreeNode *pParent = nullptr, const std::string &label = "")
    : m_data(std::forward<U>(data)),
      m_label(label),
      m_pChild(nullptr),
      m_pLeft(nullptr),
      m_pParent(pParent),
      m_pRight(nullptr)
    {

    }

private:

    /**
     * Copy constructor
     */
    explicit TreeNode(const tTreeNode &node)
    : m_data(node.m_data),
      m_label(node.m_label),
      m_pChild(nullptr),
      m_pLeft(nullptr),
      m_pParent(nullptr),
      m_pRight(nullptr)
    {

    }

    /**
     * Move constructor
     */
    explicit TreeNode(tTreeNode &&node)
    : m_data(std::move(node.m_data)),
      m_label(std::move(node.m_label)),
      m_pChild(node.m_pChild),
      m_pLeft(node.m_pLeft),
      m_pParent(node.m_pParent),
      m_pRight(node.m_pRight)
    {
        node.m_pChild = nullptr;
        node.m_pData = nullptr;
        node.m_pLeft = nullptr;
        node.m_pParent = nullptr;
        node.m_pRight = nullptr;
    }

public:

    /**
     * Destructor
     */
    virtual ~TreeNode(void) override
    {
        if (pDeleteFunction != nullptr)
            (*pDeleteFunction)(m_data);
    }

    /**
     * Add a child to the current node
     */
    inline virtual void addChild(tTreeNode *pChild) final
    {
        tTreeNode *pLeft = nullptr, *pNode = m_pChild;
        for (; pNode != nullptr; pNode = getRight(pNode))
        {
            pLeft = pNode;
            if (pNode == pChild)
                return;
        }

        setParent(pChild, this);
        setRight(pLeft, pChild);
    }

    /**
     * Add a child to the specified node
     */
    inline static bool addChild(tTreeNode *pNode, tTreeNode *pChild)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->addChild(pChild);

        return bSuccess;
    }

    /**
     * clone() function
     */
    inline virtual tTreeNode *clone(void) const override
    {
        auto *pNode = new tTreeNode(*this);
        if (pCloneFunction != nullptr)
            pNode->m_data = (*pCloneFunction)(m_data);

        return pNode;
    }

    /**
     * Get pointer to the child of the current node
     */
    inline virtual tTreeNode *getChild(void) const final
    {
        return m_pChild;
    }

    /**
     * Get pointer to the child of the specified node
     */
    inline static tTreeNode *getChild(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pChild : nullptr;
    }

    /**
     * Retrieve a vector of the current node's children
     */
    inline virtual std::vector<tTreeNode *> getChildren(void) const final
    {
        std::vector<tTreeNode *> children;

        getChildren(children);

        return children;
    }

    /**
     * Retrieve a vector of the current node's children
     */
    inline virtual void getChildren(std::vector<tTreeNode *> &children) const final
    {
        for (auto *pChild = m_pChild; pChild != nullptr; pChild = pChild->getRight())
            children.push_back(pChild);
    }

    /**
     * Retrieve a vector of children from the specified node
     */
    inline static std::vector<tTreeNode *> getChildren(const tTreeNode *pNode)
    {
        std::vector<tTreeNode *> children;

        getChildren(pNode, children);

        return children;
    }

    /**
     * Retrieve a vector of children from the specified node
     */
    inline static void getChildren(const tTreeNode *pNode, std::vector<tTreeNode *> &children)
    {
        if (pNode != nullptr)
            pNode->getChildren(children);
    }

    /**
     * Get the data stored in the specified node
     */
    inline static bool getData(const tTreeNode *pNode, T &data)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            data = pNode->getData();

        return bSuccess;
    }

    /**
     * Get the data stored in the current node
     */
    inline virtual T getData(void) const final
    {
        return m_data;
    }

    /**
     * Get the data stored in the current node
     */
    inline virtual T &getData(void) final
    {
        return m_data;
    }

    /**
     * Get current node's label
     */
    inline virtual std::string getLabel(void) const final
    {
        return m_label;
    }

    /**
     * Get the specified node's label
     */
    inline static std::string getLabel(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_label : "";
    }

    /**
     * Get pointer to the left of the current node
     */
    inline virtual tTreeNode *getLeft(void) const final
    {
        return m_pLeft;
    }

    /**
     * Get pointer to the left of the specified node
     */
    inline static tTreeNode *getLeft(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pLeft : nullptr;
    }

    /**
     * Function to get the current node's number of children
     */
    inline virtual int getNumberOfChildren(void) const final
    {
        auto *pChild = m_pChild;
        int numChildren = 0;
        for (numChildren = 0; pChild != nullptr; ++numChildren)
            pChild = pChild->m_pRight;

        return numChildren;
    }

    /**
     * Function to get the specified node's number of children
     */
    inline static int getNumberOfChildren(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->getNumberOfChildren() : 0;
    }

    /**
     * Get pointer to the parent of the current node
     */
    inline virtual tTreeNode *getParent(void) const final
    {
        return m_pParent;
    }

    /**
     * Get pointer to the parent of the specified node
     */
    inline static tTreeNode *getParent(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pParent : nullptr;
    }

    /**
     * Get pointer to the right of the current node
     */
    inline virtual tTreeNode *getRight(void) const final
    {
        return m_pRight;
    }

    /**
     * Get pointer to the right of the specified node
     */
    inline static tTreeNode *getRight(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pRight : nullptr;
    }

    /**
     * Function to determine if the current node has a child node
     */
    inline virtual bool hasChild(void) const final
    {
        return m_pChild != nullptr;
    }

    /**
     * Function to determine if the specified node has a child node
     */
    inline static bool hasChild(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pChild != nullptr : false;
    }

    /**
     * Function to determine if the current node has a left node
     */
    inline virtual bool hasLeft(void) const final
    {
        return m_pLeft != nullptr;
    }

    /**
     * Function to determine if the specified node has a left node
     */
    inline static bool hasLeft(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pLeft != nullptr : false;
    }

    /**
     * Function to determine if the current node has a parent node
     */
    inline virtual bool hasParent(void) const final
    {
        return m_pParent != nullptr;
    }

    /**
     * Function to determine if the specified node has a parent node
     */
    inline static bool hasParent(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pParent != nullptr : false;
    }

    /**
     * Function to determine if the current node has a right node
     */
    inline virtual bool hasRight(void) const final
    {
        return m_pRight != nullptr;
    }

    /**
     * Function to determine if the specified node has a right node
     */
    inline static bool hasRight(const tTreeNode *pNode)
    {
        return pNode != nullptr ? pNode->m_pRight != nullptr : false;
    }

    /**
     * Function to check whether or not node is a singleton
     */
    inline virtual bool isSingleton(void) const final
    {
        return m_pLeft == nullptr && m_pRight == nullptr;
    }

    /**
     * Make the current node a root node
     */
    inline virtual void makeRoot(void) final
    {
        if (m_pParent != nullptr)
        {
            setRight(m_pLeft, m_pRight);
            setLeft(m_pRight, m_pLeft);

            if (m_pParent->m_pChild == this)
                m_pParent->m_pChild = m_pRight;

            m_pLeft = nullptr;
            m_pRight = nullptr;
        }
    }

    /**
     * Make the specified node a root node
     */
    inline static bool makeRoot(tTreeNode *pNode, tTreeNode *pLeft)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->makeRoot();

        return bSuccess;
    }

    /**
     * Remove a child from the current node
     */
    inline virtual bool removeChild(tTreeNode *pChild) final
    {
        auto *pNode = m_pChild;
        for (; pNode != nullptr; pNode = getRight(pNode))
        {
            if (pNode == pChild)
            {
                if (m_pChild == pChild)
                    m_pChild = pChild->m_pRight;

                if (m_pChild == nullptr)
                    m_pChild = pChild->m_pChild;

                setRight(pChild->m_pLeft, pChild->m_pRight);
                setLeft(pChild->m_pRight, pChild->m_pLeft);

                pChild->m_pLeft = nullptr;
                pChild->m_pRight = nullptr;
                pChild->m_pParent = nullptr;

                break;
            }
        }

        return pNode == pChild;
    }

    /**
     * Remove a child from the current node
     */
    inline static bool removeChild(tTreeNode *pNode, tTreeNode *pChild)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->removeChild(pChild);

        return bSuccess;
    }

    /**
     * Set pointer to the child of the current node
     */
    inline virtual void setChild(tTreeNode *pChild) final
    {
        m_pChild = pChild;
        if (m_pChild != nullptr)
            m_pChild->m_pParent = this;
    }

    /**
     * Set pointer to the child of the specified node
     */
    inline static bool setChild(tTreeNode *pNode, tTreeNode *pChild)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->setChild(pChild);

        return bSuccess;
    }

    /**
     * Set the current node's data
     */
    inline virtual void setData(const T &data) final
    {
        m_data = data;
    }

    /**
     * Set the data stored in the specified node
     */
    inline static bool setData(const tTreeNode *pNode, const T &data)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->setData(data);

        return bSuccess;
    }

    /**
     * Set the current node's label
     */
    inline virtual void setLabel(const std::string &label) final
    {
        m_label = label;
    }

    /**
     * Set the specified node's label
     */
    inline static bool setLabel(tTreeNode *pNode, const std::string &label)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->m_label = label;

        return bSuccess;
    }

    /**
     * Set pointer to the left of the current node
     */
    inline virtual void setLeft(tTreeNode *pLeft) final
    {
        m_pLeft = pLeft;
        if (m_pLeft != nullptr)
            m_pLeft->m_pRight = this;
    }

    /**
     * Set pointer to the left of the specified node
     */
    inline static bool setLeft(tTreeNode *pNode, tTreeNode *pLeft)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->setLeft(pLeft);

        return bSuccess;
    }

    /**
     * Set pointer to the parent of the current node
     */
    inline virtual void setParent(tTreeNode *pParent) final
    {
        m_pParent = pParent;
        if (m_pParent != nullptr && m_pParent->m_pChild == nullptr)
            m_pParent->m_pChild = this;
    }

    /**
     * Set pointer to the parent of the specified node
     */
    inline static bool setParent(tTreeNode *pNode, tTreeNode *pParent)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->setParent(pParent);

        return bSuccess;
    }

    /**
     * Set pointer to the right of the current node
     */
    inline virtual void setRight(tTreeNode *pRight) final
    {
        m_pRight = pRight;
        if (m_pRight != nullptr)
            m_pRight->m_pLeft = this;
    }

    /**
     * Set pointer to the right of the specified node
     */
    inline static bool setRight(tTreeNode *pNode, tTreeNode *pRight)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
            pNode->setRight(pRight);

        return bSuccess;
    }

private:

    /**
     * the data
     */
    T m_data;

    /**
     * node label
     */
    std::string m_label;

    /**
     * pointer to first child
     */
    tTreeNode *m_pChild;

    /**
     * pointer to left sibling
     */
    tTreeNode *m_pLeft;

    /**
     * pointer to parent
     */
    tTreeNode *m_pParent;

    /**
     * pointer to right sibling
     */
    tTreeNode *m_pRight;
};

}

}

#ifdef _MSC_VER
#pragma warning (default : 4520)
#endif

#endif
