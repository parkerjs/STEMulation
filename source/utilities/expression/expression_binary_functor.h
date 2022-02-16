#ifndef EXPRESSION_BINARY_FUNCTOR_H
#define EXPRESSION_BINARY_FUNCTOR_H

#include "any.h"
#include "binary_functor.h"

namespace utilities
{

namespace expression
{

/**
 * Binary functor class specific to the expression evaluation classes
 */
template<typename T>
class ExpressionBinaryFunctor
: public functional::functors::binary::BinaryFunctor<functional::Any>
{
protected:

    /**
     * Constructor
     */
    ExpressionBinaryFunctor(void)
    : m_pBinaryFunctor(nullptr)
    {

    }

    /**
     * Constructor
     */
    ExpressionBinaryFunctor(functional::functors::binary::BinaryFunctor<T> *pBinaryFunctor)
    : m_pBinaryFunctor(pBinaryFunctor)
    {

    }

    /**
     * Copy constructor
     */
    ExpressionBinaryFunctor(const ExpressionBinaryFunctor<T> &expressionBinaryFunctor)
    : m_pBinaryFunctor(nullptr),
      m_result(expressionBinaryFunctor.m_result)
    {
        if (expressionBinaryFunctor.m_pBinaryFunctor != nullptr)
            m_pBinaryFunctor = expressionBinaryFunctor.m_pBinaryFunctor->clone();
    }

    /**
     * Move constructor
     */
    ExpressionBinaryFunctor(ExpressionBinaryFunctor<T> &&expressionBinaryFunctor)
    : m_pBinaryFunctor(std::move(expressionBinaryFunctor.m_pBinaryFunctor)),
      m_result(std::move(expressionBinaryFunctor.m_result))
    {
        expressionBinaryFunctor.m_pBinaryFunctor = nullptr;
    }

public:

    /**
     * Destructor
     */
    virtual ~ExpressionBinaryFunctor(void) override
    {
        if (m_pBinaryFunctor != nullptr)
            delete m_pBinaryFunctor;
    }

    /**
     * clone() function
     */
    inline virtual ExpressionBinaryFunctor<T> *clone(void) const override
    {
        return new ExpressionBinaryFunctor<T>(*this);
    }

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static ExpressionBinaryFunctor<T> *create(const functional::functors::binary::BinaryFunctorType &type)
    {
        ExpressionBinaryFunctor<T> *pExpressionBinaryFunctor = nullptr;
        auto *pBinaryFunctor = functional::functors::binary::BinaryFunctor<T>::create(type);
        if (pBinaryFunctor != nullptr)
            pExpressionBinaryFunctor = new ExpressionBinaryFunctor<T>(pBinaryFunctor);

        return pExpressionBinaryFunctor;
    }

    /**
     * Evaluation function
     */
    virtual functional::Any evaluate(const functional::Any &lhs, const functional::Any &rhs) const override
    {
        try
        {
            auto &&left = functional::any_cast<const std::vector<T> &>(lhs);
            try
            {
                auto &&right = functional::any_cast<const std::vector<T> &>(rhs);

                return evaluate(left, right);
            }
            catch (functional::bad_any_cast)
            {
                auto &&right = functional::any_cast<const T &>(rhs);

                return evaluate(left, right);
            }
        }
        catch (functional::bad_any_cast &)
        {
            auto &&left = functional::any_cast<const T &>(lhs);
            try
            {
                auto &&right = functional::any_cast<const std::vector<T> &>(rhs);

                return evaluate(left, right);
            }
            catch (functional::bad_any_cast)
            {
                auto &&right = functional::any_cast<const T &>(rhs);

                return evaluate(left, right);
            }
        }
    }

    /**
     * Evaluation function
     */
    virtual bool evaluate(functional::Any &result, const functional::Any &lhs,
                          const functional::Any &rhs) const final
    {
        bool bSuccess = true;
        try
        {
            auto &&left = functional::any_cast<const std::vector<T> &>(lhs);
            try
            {
                auto &&output = functional::any_cast<std::vector<T> &>(result);
                try
                {
                    auto &&right = functional::any_cast<const std::vector<T> &>(rhs);
                    bSuccess = evaluate(output, left, right);
                }
                catch (functional::bad_any_cast)
                {
                    auto &&right = functional::any_cast<const T &>(rhs);
                    bSuccess = evaluate(output, left, right);
                }
            }
            catch (functional::bad_any_cast)
            {
                bSuccess = false;
            }
        }
        catch (functional::bad_any_cast &)
        {
            auto &&left = functional::any_cast<const T &>(lhs);
            try
            {
                auto &&right = functional::any_cast<const std::vector<T> &>(rhs);
                try
                {
                    auto &&output = functional::any_cast<std::vector<T> &>(result);
                    bSuccess = evaluate(output, left, right);
                }
                catch (functional::bad_any_cast &)
                {
                    bSuccess = false;
                }
            }
            catch (functional::bad_any_cast &)
            {
                auto &&right = functional::any_cast<const T &>(rhs);
                try
                {
                    auto &&output = functional::any_cast<T &>(result);
                    bSuccess = evaluate(output, left, right);
                }
                catch (functional::bad_any_cast)
                {
                    bSuccess = false;
                }
            }
        }

        return bSuccess;
    }

protected:

    /**
     * Evaluation function
     */
    inline virtual functional::Any evaluate(const std::vector<T> &lhs, const std::vector<T> &rhs) const
    {
        evaluate(m_result, lhs, rhs);

        return std::ref(m_result);
    }

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(std::vector<T> &result, const std::vector<T> &lhs,
                                 const std::vector<T> &rhs) const
    {
        result.resize(lhs.size());
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), result.begin(),
                       [this] (const T &x, const T &y) { return m_pBinaryFunctor->evaluate(x, y); });

        return true;
    }

    /**
     * Evaluation function
     */
    inline virtual functional::Any evaluate(const std::vector<T> &lhs, const T &rhs) const
    {
        evaluate(m_result, lhs, rhs);

        return std::ref(m_result);
    }

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(std::vector<T> &result, const std::vector<T> &lhs, const T &rhs) const
    {
        result.resize(lhs.size());
        std::transform(lhs.cbegin(), lhs.cend(), result.begin(),
                       [this, &rhs] (const T &x) { return m_pBinaryFunctor->evaluate(x, rhs); });

        return true;
    }

    /**
     * Evaluation function
     */
    inline virtual functional::Any evaluate(const T &lhs, const std::vector<T> &rhs) const
    {
        evaluate(m_result, lhs, rhs);

        return std::ref(m_result);
    }

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(std::vector<T> &result, const T &lhs, const std::vector<T> &rhs) const
    {
        result.resize(rhs.size());
        std::transform(rhs.cbegin(), rhs.cend(), result.begin(),
                       [this, &lhs] (const T &x) { return m_pBinaryFunctor->evaluate(lhs, x); });

        return true;
    }

    /**
     * Evaluation function
     */
    inline virtual functional::Any evaluate(const T &lhs, const T &rhs) const
    {
        m_result.resize(1);
        m_pBinaryFunctor->evaluate(m_result[0], lhs, rhs);

        return std::ref(m_result[0]);
    }

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(T &result, const T &lhs, const T &rhs) const
    {
        return m_pBinaryFunctor->evaluate(result, lhs, rhs);
    }

public:

    /**
     * Function to get the binary functor type
     */
    inline virtual functional::functors::binary::BinaryFunctorType getType(void) const override
    {
        return m_pBinaryFunctor->getType();
    }

    /**
     * Function to determine whether or not the current object is an arithmetic operation
     */
    inline virtual bool isArithmetic(void) const override
    {
        return m_pBinaryFunctor->isArithmetic();
    }

    /**
     * Function to determine whether or not the current object is a bitwise operation
     */
    inline virtual bool isBitwise(void) const override
    {
        return m_pBinaryFunctor->isBitwise();
    }

    /**
     * Function to determine whether or not the current object is a comparison operation
     */
    inline virtual bool isComparison(void) const override
    {
        return m_pBinaryFunctor->isComparison();
    }

    /**
     * Function to determine whether or not the current object is a logical operation
     */
    inline virtual bool isLogical(void) const override
    {
        return m_pBinaryFunctor->isLogical();
    }

protected:

    /**
     * pointer to binary functor
     */
    functional::functors::binary::BinaryFunctor<T> *m_pBinaryFunctor;

    /**
     * workspace result vector
     */
    mutable std::vector<T> m_result;
};

}

}

#endif
