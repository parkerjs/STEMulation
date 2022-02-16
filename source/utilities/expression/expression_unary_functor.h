#ifndef EXPRESSION_UNARY_FUNCTOR_H
#define EXPRESSION_UNARY_FUNCTOR_H

#include "any.h"
#include "unary_functor.h"

namespace utilities
{

namespace expression
{

/**
 * Unary functor class specific to the expression evaluation classes
 */
template<typename T>
class ExpressionUnaryFunctor
: public functional::functors::unary::UnaryFunctor<functional::Any>
{
protected:

    /**
     * Constructor
     */
    ExpressionUnaryFunctor(void)
    : m_pUnaryFunctor(nullptr)
    {

    }

    /**
     * Constructor
     */
    ExpressionUnaryFunctor(functional::functors::unary::UnaryFunctor<T> *pUnaryFunctor)
    : m_pUnaryFunctor(pUnaryFunctor)
    {

    }

    /**
     * Copy constructor
     */
    ExpressionUnaryFunctor(const ExpressionUnaryFunctor<T> &expressionUnaryFunctor)
    : m_pUnaryFunctor(nullptr),
      m_result(expressionUnaryFunctor.m_result)
    {
        if (expressionUnaryFunctor.m_pUnaryFunctor != nullptr)
            m_pUnaryFunctor = expressionUnaryFunctor.m_pUnaryFunctor->clone();
    }

    /**
     * Move constructor
     */
    ExpressionUnaryFunctor(ExpressionUnaryFunctor<T> &&expressionUnaryFunctor)
    : m_pUnaryFunctor(std::move(expressionUnaryFunctor.m_pUnaryFunctor)),
      m_result(std::move(expressionUnaryFunctor.m_result))
    {
        expressionUnaryFunctor.m_pUnaryFunctor = nullptr;
    }

public:

    /**
     * Destructor
     */
    virtual ~ExpressionUnaryFunctor(void) override
    {
        if (m_pUnaryFunctor != nullptr)
            delete m_pUnaryFunctor;
    }

    /**
     * clone() function
     */
    inline virtual ExpressionUnaryFunctor<T> *clone(void) const override
    {
        return new ExpressionUnaryFunctor<T>(*this);
    }

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static ExpressionUnaryFunctor<T> *create(const functional::functors::unary::UnaryFunctorType &type)
    {
        ExpressionUnaryFunctor<T> *pExpressionUnaryFunctor = nullptr;
        auto *pUnaryFunctor = functional::functors::unary::UnaryFunctor<T>::create(type);
        if (pUnaryFunctor != nullptr)
            pExpressionUnaryFunctor = new ExpressionUnaryFunctor<T>(pUnaryFunctor);

        return pExpressionUnaryFunctor;
    }

    /**
     * Evaluation function
     */
    virtual functional::Any evaluate(const functional::Any &rhs) const override
    {
        try
        {
            auto &&right = functional::any_cast<const std::vector<T> &>(rhs);

            return evaluate(right);
        }
        catch (functional::bad_any_cast &)
        {
            auto &&right = functional::any_cast<const T &>(rhs);

            return evaluate(right);
        }
    }

    /**
     * Evaluation function
     */
    virtual bool evaluate(functional::Any &result, const functional::Any &rhs) const override
    {
        bool bSuccess = true;
        try
        {
            auto &&output = functional::any_cast<std::vector<T> &>(result);
            auto &&right = functional::any_cast<const std::vector<T> &>(rhs);

            bSuccess = evaluate(output, right);
        }
        catch (functional::bad_any_cast &)
        {
            try
            {
                auto &&output = functional::any_cast<T &>(result);
                auto &&right = functional::any_cast<const T &>(rhs);

                bSuccess = evaluate(output, right);
            }
            catch (functional::bad_any_cast)
            {
                bSuccess = false;
            }
        }

        return bSuccess;
    }

protected:

    /**
     * Evaluation function
     */
    inline virtual functional::Any evaluate(const std::vector<T> &rhs) const
    {
        evaluate(m_result, rhs);

        return std::ref(m_result);
    }

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(std::vector<T> &result, const std::vector<T> &rhs) const
    {
        result.resize(rhs.size());
        std::transform(rhs.cbegin(), rhs.cend(), result.begin(),
                       [this] (const T &x) { return m_pUnaryFunctor->evaluate(x); });

        return true;
    }

    /**
     * Evaluation function
     */
    inline virtual functional::Any evaluate(const T &rhs) const
    {
        m_result.resize(1);
        m_pUnaryFunctor->evaluate(m_result[0], rhs);

        return std::ref(m_result[0]);
    }

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(T &result, const T &rhs) const
    {
        return m_pUnaryFunctor->evaluate(result, rhs);
    }

public:

    /**
     * Function to get the unary functor type
     */
    inline virtual functional::functors::unary::UnaryFunctorType getType(void) const override
    {
        return m_pUnaryFunctor->getType();
    }

    /**
     * Function to determine whether or not the current object is a logical operation
     */
    inline virtual bool isLogical(void) const override
    {
        return m_pUnaryFunctor->isLogical();
    }

protected:

    /**
     * a pointer to a unary function object
     */
    functional::functors::unary::UnaryFunctor<T> *m_pUnaryFunctor;

    /**
     * workspace result vector
     */
    mutable std::vector<T> m_result;
};

}

}

#endif
