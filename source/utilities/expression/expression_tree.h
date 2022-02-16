#ifndef EXPRESSION_TREE_H
#define EXPRESSION_TREE_H

#include "any.h"
#include "expression.h"
#include "prefix_tree.h"
#include "output_streamable.h"
#include <iostream>
#include <list>
#include <set>

namespace utilities
{

namespace expression
{

/**
 * This class builds an expression tree from an arithmetic/boolean expression
 */
template<typename T, typename Result = T>
class ExpressionTree
: public Expression<T, Result>,
  public attributes::concrete::OutputStreamable<ExpressionTree<T, Result>>
{
public:

    /**
     * Type alias declarations
     */
    using BinaryFunctorType = typename Expression<T, Result>::BinaryFunctorType;
    using ErrorType = typename Expression<T, Result>::ErrorType;
    using PrefixTree = containers::tree::PrefixTree<functional::Any>;
    using TreeNode = containers::tree::TreeNode<functional::Any>;
    using UnaryFunctorType = typename Expression<T, Result>::UnaryFunctorType;

    /**
     * Using declarations
     */
    using Expression<T, Result>::evaluate;

    /**
     * Constructor
     */
    ExpressionTree(void)
    {

    }

    /**
     * Copy constructor
     */
    ExpressionTree(const ExpressionTree<T, Result> &tree)
    : Expression<T, Result>(tree)
    {
        operator = (tree);
    }

    /**
     * Move constructor
     */
    ExpressionTree(ExpressionTree<T, Result> &&tree)
    : Expression<T, Result>(std::move(tree))
    {
        operator = (std::move(tree));
    }

    /**
     * Destructor
     */
    virtual ~ExpressionTree(void) override
    {
        destroy();
    }

    /**
     * Copy assignment operator
     */
    ExpressionTree<T, Result> &operator = (const ExpressionTree<T, Result> &tree)
    {
        if (&tree != this)
        {
            Expression<T, Result>::operator = (tree);

            build(tree.m_expression.c_str()); // build function will release current tree and objects therein
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    ExpressionTree<T, Result> &operator = (ExpressionTree<T, Result> &&tree)
    {
        if (&tree != this)
        {
            Expression<T, Result>::operator = (std::move(tree));

            m_expression = std::move(tree.m_expression);
            m_tree = std::move(tree.m_tree);
        }

        return *this;
    }

    /**
     * Equality operator
     */
    inline virtual bool operator == (const ExpressionTree<T, Result> &tree) const final
    {
        return m_expression == tree.m_expression; // it should be sufficient to just test on the expression
                                                  // strings within the trees rather than testing for equality
                                                  // of each node
    }

    /**
     * Function to build a tree from the given expression
     * @param pExpression a character string containing the expression from which the tree will be built
     */
    inline virtual bool build(const char *pExpression)
    {
        m_expression.clear();
        this->m_numericOperands.clear();

        destroy(); // need to release current tree, if it exists...

        functional::Any result;
        bool bSuccess = Expression<T, Result>::evaluate(pExpression, result);
        if (bSuccess)
        {
            auto **pNode = functional::any_cast<TreeNode *>(&result);
            bSuccess = (pNode != nullptr);
            if (pNode != nullptr)
            {
                m_tree.getRoot()->addChild(*pNode);
                m_expression = pExpression;
            }
        }
        else
            clear();

        return bSuccess;
    }

    /**
     * Function to perform cleanup
     */
    inline virtual void clear(void) override
    {
        destroy();
    }

    /**
     * clone() function
     */
    inline virtual ExpressionTree<T, Result> *clone(void) const override
    {
        return new ExpressionTree<T, Result>(*this);
    }

    /**
     * Function to destroy the contents of the tree
     */
    virtual bool destroy(void) final
    {
        // lambda function to perform cleanup on the data stored in each node
        auto &&destructionFunctor = [] (TreeNode *pNode)
        {
            functional::Any any;
            if (TreeNode::getData(pNode, any))
            {
                auto *pFunction = functional::any_cast<ExpressionFunctor<T> *>(&any);
                if (pFunction != nullptr)
                    delete *pFunction;
                else
                {
                    auto *pBinaryFunctor = functional::any_cast<ExpressionBinaryFunctor<T> *>(&any);
                    if (pBinaryFunctor != nullptr)
                        delete *pBinaryFunctor;
                    else
                    {
                        auto *pUnaryFunctor = functional::any_cast<ExpressionUnaryFunctor<T> *>(&any);
                        if (pUnaryFunctor != nullptr)
                            delete *pUnaryFunctor;
                    }
                }
            }

            return true;
        };

        // need to traverse the tree and delete the pointers to objects that are contained within the Any's
        bool bSuccess = m_tree.traverse(destructionFunctor);

        // finally, delete all nodes within the tree
        m_tree.destroy();

        return bSuccess;
    }

    /**
     * Function to test if tree is empty
     */
    inline virtual bool empty(void) const final
    {
        return m_tree.empty();
    }

    /**
     * Function to evaluate the tree
     */
    inline virtual bool evaluate(Result &result)
    {
        bool bSuccess = !m_tree.empty();
        if (bSuccess)
        {
            auto *pNode = m_tree.getRoot();
            functional::Any any;
            bSuccess = evaluate(pNode, any);
            if (bSuccess)
            {
                try
                {
                    result = functional::any_cast<Result &>(any);
                }
                catch (functional::bad_any_cast &)
                {
                    bSuccess = false;
                }
            }
        }

        return bSuccess;
    }

protected:

    /**
     * Function to evaluate the tree
     * @param pNode a pointer to the current tree node
     */
    virtual bool evaluate(TreeNode *pNode, functional::Any &result) final
    {
        bool bSuccess = true; // assume success

        try
        {
            functional::Any any, lhs, rhs;
            if (TreeNode::getData(pNode, any))
            {
                auto *pTempFunctionPointer = functional::any_cast<ExpressionFunctor<T> *>(&any);
                if (pTempFunctionPointer != nullptr)
                {
                    auto *pFunction = *pTempFunctionPointer;
                    std::vector<functional::Any> arguments;
                    auto *pChild = TreeNode::getChild(pNode);
                    for (int i = 0; bSuccess && pChild != nullptr; ++i)
                    {
                        bSuccess &= evaluate(pChild, any);
                        if (bSuccess)
                        {
                            arguments.emplace_back(std::move(any));
                            pChild = TreeNode::getRight(pChild);
                        }
                    }

                    if (bSuccess)
                        result = (*pFunction)(arguments);
                }
                else
                {
                    auto *pLeftChild = TreeNode::getChild(pNode);
                    if (pLeftChild != nullptr)
                        bSuccess = evaluate(pLeftChild, lhs);

                    if (bSuccess)
                    {
                        auto *pRightChild = TreeNode::getRight(pLeftChild);
                        if (pRightChild != nullptr)
                            bSuccess = evaluate(pRightChild, rhs);

                        if (bSuccess)
                        {
                            if (!TreeNode::getData(pNode, any))
                                result = lhs;
                            else // is the any object a binary functor?
                            {
                                auto *pBinaryFunctor = functional::any_cast<ExpressionBinaryFunctor<T> *>(&any);
                                if (pBinaryFunctor != nullptr)
                                    result = (*pBinaryFunctor)->evaluate(lhs, rhs);
                                else // is the any object a unary functor?
                                {
                                    auto *pUnaryFunctor = functional::any_cast<ExpressionUnaryFunctor<T> *>(&any);
                                    if (pUnaryFunctor != nullptr)
                                        result = (*pUnaryFunctor)->evaluate(lhs);
                                    else
                                        result = (TreeNode::getParent(pNode) == nullptr) ? lhs : any;
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (std::bad_function_call)
        {
            this->m_errorType = ErrorType::BadFunctionCall;
            bSuccess = false;
        }

        return bSuccess;
    }

public:

    /**
     * Get the string containing the expression from which the tree was built
     */
    inline virtual std::string getExpression(void) const final
    {
        return m_expression;
    }

    /**
     * Get a reference to this object's tree
     */
    inline virtual PrefixTree &getTree(void) final
    {
        return m_tree;
    }

    /**
     * Initialization function
     */
    inline virtual bool initialize(void) override
    {
        bool bSuccess = Expression<T, Result>::initialize();
        if (bSuccess)
            destroy();

        return bSuccess;
    }

    /**
     * Function to print the contents (branches) within the expression tree
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        return m_tree.print(stream);
    }

protected:

    /**
     * Function to process a binary operation
     */
    inline virtual bool processBinaryOperation(ExpressionBinaryFunctor<T> *&pBinaryFunctor,
                                               functional::Any &lhs, functional::Any &rhs) override
    {
        auto *pLeft = functional::any_cast<TreeNode *>(&lhs);
        auto *pRight = functional::any_cast<TreeNode *>(&rhs);
        bool bSuccess = (pLeft != nullptr && pRight != nullptr);
        if (bSuccess)
        {
            auto *pParent = new TreeNode(pBinaryFunctor);
            pParent->setLabel(pBinaryFunctor->getType());
            pParent->addChild(*pLeft);
            pParent->addChild(*pRight);
            *pLeft = pParent;
            pBinaryFunctor = nullptr; // prevents the functor from being deleted outside this function
        }

        return bSuccess;
    }

    /**
     * Function to process a function call
     */
    virtual bool processFunction(ExpressionFunctor<T> *&pFunction, std::vector<functional::Any> &arguments,
                                 functional::Any &result) override
    {
        bool bSuccess = (pFunction != nullptr);
        if (bSuccess)
        {
            auto *pNode = new TreeNode(pFunction);
            pNode->setLabel(pFunction->getName());
            result = pNode;

            // store the arguments as children
            for (auto &&argument : arguments)
            {
                auto *pArgument = functional::any_cast<TreeNode *>(argument);
                argument = pArgument->getData(); // do this for the configure() function
                bSuccess &= (pArgument != nullptr);
                if (bSuccess)
                    pNode->addChild(pArgument);
                else
                    break;
            }

            bSuccess = pFunction->configure(arguments);
            pFunction = nullptr; // prevents the functor from being deleted outside this function
        }

        return bSuccess;
    }

    /**
     * Function to process a numeric operand
     */
    inline virtual bool processNumericOperand(const char *&pExpression, functional::Any &result) override
    {
        char *pEnd = nullptr;
        auto &&operand = strtod(pExpression, &pEnd);
        bool bSuccess = (pEnd != pExpression);
        if (bSuccess)
        {
            this->m_numericOperands.push_back(operand);
            auto *pNode = new TreeNode(this->m_numericOperands.back());
            pNode->setLabel(std::string((char *)pExpression, pEnd));
            pExpression = pEnd; // advance the pointer and store the result
            result = pNode;
        }

        return bSuccess;
    }

    /**
     * Function to process a unary operation
     */
    inline virtual bool processUnaryOperation(ExpressionUnaryFunctor<T> *&pUnaryFunctor,
                                              functional::Any &rhs) override
    {
        auto **pNode = functional::any_cast<TreeNode *>(&rhs);
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
        {
            auto *pParent = new TreeNode(pUnaryFunctor);
            pParent->setLabel(pUnaryFunctor->getType());
            pParent->addChild(*pNode);
            *pNode = pParent;
            pUnaryFunctor = nullptr; // prevents the functor from being deleted outside this function
        }

        return bSuccess;
    }

    /**
     * Function to process a variable
     */
    virtual bool processVariable(const std::string &name, functional::Any &result) override
    {
        auto &&itRegistryEntry = this->m_registry.findByName(name);
        bool bSuccess = (itRegistryEntry != this->m_registry.cend());
        if (bSuccess)
        {
            auto *pNode = new TreeNode(itRegistryEntry->second.toAny());
            pNode->setLabel(name);
            result = pNode;
        }

        return bSuccess;
    }

public:

    /**
     * Update this object's variable registry
     */
    inline virtual bool updateVariableRegistry(const utilities::VariableRegistry &registry) final
    {
        this->m_registry = registry;

        // lambda function to perform update on variable nodes
        auto &&updateFunctor = [&registry] (TreeNode *pNode)
        {
            if (pNode != nullptr)
            {
                auto &&variable = pNode->getLabel();
                auto &&itRegistryEntry = registry.findByName(variable);
                if (itRegistryEntry != registry.cend())
                    pNode->setData(itRegistryEntry->second.toAny());
            }

            return true;
        };

        // need to traverse the tree and update nodes associated with variable entries
        return m_tree.traverse(updateFunctor);
    }

protected:

    /**
     * string containing the expression from which the tree was built
     */
    std::string m_expression;

    /**
     * instance of a prefix tree to store the nodes
     */
    PrefixTree m_tree;
};

}

}

#endif
