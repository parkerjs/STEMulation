#ifndef BINARY_FUNCTOR_ADAPTOR_H
#define BINARY_FUNCTOR_ADAPTOR_H

#include "binary_functor.h"

namespace functional
{

namespace functors
{

// forward declarations
namespace unary { template<typename T, typename Result> class UnaryFunctor; }

namespace binary
{

/**
 * This class adapts a unary functor for use as a binary functor
 */
template<typename T, typename Result = T>
class BinaryFunctorAdaptor
: public BinaryFunctor<T, Result>
{
public:

    /**
     * Constructor
     */
    BinaryFunctorAdaptor(Unary::UnaryFunctor<T, Result> *pUnaryFunctor)
    : m_pUnaryFunctor(pUnaryFunctor != nullptr ? pUnaryFunctor->clone() : nullptr)
    {

    }

    /**
     * Destructor
     */
    virtual ~BinaryFunctorAdaptor(void) override
    {
        if (m_pUnaryFunctor != nullptr)
            delete m_pUnaryFunctor;
    }

    /**
     * Function call operator
     */
    inline virtual Result operator () (const T &lhs, const T &rhs) override
    {
        return evaluate(lhs, rhs);
    }

    /**
     * clone() function
     */
    inline virtual BinaryFunctorAdaptor<T, Result> *clone(void) const override
    {
        return new BinaryFunctorAdaptor<T, Result>(m_pUnaryFunctor);
    }

    /**
     * Evaluation function (ignore lhs operand, as unary operator is applied to its rhs operand)
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        if (m_pUnaryFunctor != nullptr)
            return m_pUnaryFunctor->evaluate(rhs);

        return rhs;
    }

    /**
     * Evaluation function (ignore lhs operand, as unary operator is applied to its rhs operand)
     */
    inline virtual bool evaluate(Result &result, const T &lhs, const T &rhs) const override
    {
        bool bSuccess = (m_pUnaryFunctor != nullptr);
        if (bSuccess)
            bSuccess = m_pUnaryFunctor->evaluate(result, rhs);

        return bSuccess;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType::Enum getType(void) const override
    {
        return BinaryFunctorType::Unknown;
    }

private:

    /**
     * a pointer to a unary function object
     */
    Unary::UnaryFunctor<T, Result> *m_pUnaryFunctor;
};

}

}

}

#endif
