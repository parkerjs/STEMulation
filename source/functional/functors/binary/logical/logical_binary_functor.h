#ifndef LOGICAL_BINARY_FUNCTOR_H
#define LOGICAL_BINARY_FUNCTOR_H

#include "binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace logical
{

// forward declarations
template<typename, typename> class LogicalAndFunctor;
template<typename, typename> class LogicalOrFunctor;

/**
 * Base class for binary logical functors
 */
template<typename T, typename Result = bool>
class LogicalBinaryFunctor
: public BinaryFunctor<T, Result>
{
public:

    /**
     * Destructor
     */
    virtual ~LogicalBinaryFunctor(void) override
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual LogicalBinaryFunctor<T, Result> *clone(void) const override = 0;

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static LogicalBinaryFunctor<T, Result> *create(const BinaryFunctorType &type)
    {
        typedef BinaryFunctorType::Enum BinaryOp;
        typedef LogicalBinaryFunctor<T, Result> * (*tFactory)(void);
        static const std::map<BinaryFunctorType, tFactory> factoryMap =
        { { BinaryOp::LogicalAnd, reinterpret_cast<tFactory>(&LogicalAndFunctor<T, Result>::create) },
          { BinaryOp::LogicalOr, reinterpret_cast<tFactory>(&LogicalOrFunctor<T, Result>::create) } };

        auto &&itTypeFactoryPair = factoryMap.find(type);
        if (itTypeFactoryPair != factoryMap.cend())
            return (*itTypeFactoryPair->second)();
        else
            return nullptr;
    }

    /**
     * Function to determine whether or not the current object is a logical operation
     */
    inline virtual bool isLogical(void) const override
    {
        return true;
    }
};

}

}

}

}

#endif
