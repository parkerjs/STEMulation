#ifndef UNARY_FUNCTOR_H
#define UNARY_FUNCTOR_H

#include "functor.h"
#include "unary_functor_type.h"

namespace functional
{

namespace functors
{

namespace unary
{

// forward declarations
template<typename, typename> class BitwiseNotFunctor;
template<typename, typename> class LogicalNotFunctor;
template<typename, typename> class UnaryMinusFunctor;
template<typename, typename> class UnaryPlusFunctor;

/**
 * Base class for unary functors
 */
template<typename T, typename Result = T>
class UnaryFunctor
: public Functor<T, Result>,
  public attributes::concrete::OutputStreamable<UnaryFunctor<T, Result>>
{
public:

    /**
     * Destructor
     */
    virtual ~UnaryFunctor(void) override
    {

    }

    /**
     * Function call operator
     */
    inline virtual Result operator () (const T &rhs)
    {
        return evaluate(rhs);
    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual UnaryFunctor<T, Result> *clone(void) const override = 0;

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static UnaryFunctor<T, Result> *create(const UnaryFunctorType &type)
    {
        typedef UnaryFunctorType::Enum UnaryOp;
        typedef UnaryFunctor<T, Result> * (*tFactory)(void);
        static const std::map<UnaryFunctorType, tFactory> factoryMap =
        { { UnaryOp::BitwiseNot, reinterpret_cast<tFactory>(&BitwiseNotFunctor<T, Result>::create) },
          { UnaryOp::LogicalNot, reinterpret_cast<tFactory>(&LogicalNotFunctor<T, Result>::create) },
          { UnaryOp::Minus, reinterpret_cast<tFactory>(&UnaryMinusFunctor<T, Result>::create) },
          { UnaryOp::Plus, reinterpret_cast<tFactory>(&UnaryPlusFunctor<T, Result>::create) } };

        auto &&itTypeFactoryPair = factoryMap.find(type);
        if (itTypeFactoryPair != factoryMap.cend())
            return (*itTypeFactoryPair->second)();
        else
            return nullptr;
    }

    /**
     * Evaluation function
     */
    EXPORT_STEM virtual Result evaluate(const T &rhs) const = 0;

    /**
     * Evaluation function
     */
    inline virtual bool evaluate(Result &result, const T &rhs) const
    {
        result = evaluate(rhs);

        return true;
    }

    /**
     * Function to get the unary functor type
     */
    EXPORT_STEM virtual UnaryFunctorType getType(void) const = 0;

    /**
     * Function to determine whether or not the current object is a logical operation
     */
    inline virtual bool isLogical(void) const
    {
        return false;
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        return stream << std::string(getType());
    }
};

}

}

}

#endif
