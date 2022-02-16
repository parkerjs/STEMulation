#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "expression.h"

namespace utilities
{

namespace expression
{

/**
 * This class evaluates arithmetic/boolean expressions
 */
template<typename T, typename Result = T>
class ExpressionEvaluator
: public Expression<T, Result>
{
public:

    /**
     * Type alias declarations
     */
    using BinaryFunctorType = typename Expression<T, Result>::BinaryFunctorType;
    using ErrorType = typename Expression<T, Result>::ErrorType;
    using UnaryFunctorType = typename Expression<T, Result>::UnaryFunctorType;

    /**
     * Using declarations
     */
    using Expression<T, Result>::evaluate;

    /**
     * Constructor
     */
    ExpressionEvaluator(void)
    {

    }

    /**
     * Copy constructor
     */
    ExpressionEvaluator(const ExpressionEvaluator<T, Result> &evaluator)
    : Expression<T, Result>(evaluator)
    {
        operator = (evaluator);
    }

    /**
     * Move constructor
     */
    ExpressionEvaluator(ExpressionEvaluator<T, Result> &&evaluator)
    : Expression<T, Result>(std::move(evaluator))
    {
        operator = (std::move(evaluator));
    }

    /**
     * Destructor
     */
    virtual ~ExpressionEvaluator(void) override
    {
        clear();
    }

    /**
     * Copy assignment operator
     */
    ExpressionEvaluator<T, Result> &operator = (const ExpressionEvaluator<T, Result> &evaluator)
    {
        if (&evaluator != this)
        {
            Expression<T, Result>::operator = (evaluator);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    ExpressionEvaluator<T, Result> &operator = (ExpressionEvaluator<T, Result> &&evaluator)
    {
        if (&evaluator != this)
        {
            Expression<T, Result>::operator = (std::move(evaluator));

            m_functors = std::move(evaluator.m_functors);
        }

        return *this;
    }

private:

    /**
     * Function to perform cleanup
     */
    inline virtual void clear() override
    {
        for (auto *pFunctor : m_functors)
            if (pFunctor != nullptr)
                delete pFunctor;

        m_functors.clear();
    }

public:

    /**
     * clone() function
     */
    inline virtual ExpressionEvaluator<T, Result> *clone(void) const override
    {
        return new ExpressionEvaluator<T, Result>(*this);
    }

    /**
     * Function to evaluate the expression
     * @param pExpression a character string containing the expression
     */
    inline virtual bool evaluate(const char *pExpression, Result &result) final
    {
        bool bSuccess = false;
        try
        {
            this->m_numericOperands.clear();
            functional::Any argument(std::ref(result));
            bSuccess = Expression<T, Result>::evaluate(pExpression, argument);
            result = functional::any_cast<Result &>(argument);
            clear();
        }
        catch (functional::bad_any_cast &)
        {
            bSuccess = false;
        }

        return bSuccess;
    }

private:

    /**
     * Function to process a binary operation
     */
    inline virtual bool processBinaryOperation(ExpressionBinaryFunctor<T> *&pBinaryFunctor,
                                               functional::Any &lhs,
                                               functional::Any &rhs) override
    {
        m_functors.push_back(pBinaryFunctor);
        lhs = pBinaryFunctor->evaluate(lhs, rhs);
        pBinaryFunctor = nullptr;

        return true;
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
            bSuccess = pFunction->configure(arguments);
            if (bSuccess)
            {
                try
                {
                    result = (*pFunction)(arguments);
                }
                catch (std::bad_function_call)
                {
                    this->m_errorType = ErrorType::BadFunctionCall;
                    bSuccess = false;
                }
            }
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
            result = this->m_numericOperands.back();
            pExpression = pEnd; // advance the pointer and store the result
        }

        return bSuccess;
    }

    /**
     * Function to process a unary operation
     */
    inline virtual bool processUnaryOperation(ExpressionUnaryFunctor<T> *&pUnaryFunctor,
                                              functional::Any &rhs) override
    {
        m_functors.push_back(pUnaryFunctor);
        rhs = pUnaryFunctor->evaluate(rhs);
        pUnaryFunctor = nullptr;

        return true;
    }

    /**
     * Function to process a variable
     */
    inline virtual bool processVariable(const std::string &name, functional::Any &result) override
    {
        auto &&itRegistryEntry = this->m_registry.findByName(name);
        bool bSuccess = (itRegistryEntry != this->m_registry.cend());
        if (bSuccess)
            result = itRegistryEntry->second.toAny();

        return bSuccess;
    }

    /**
     * a vector of pointers to function objects
     */
    std::vector<functional::functors::Functor<functional::Any> *> m_functors;
};

}

}

#endif
