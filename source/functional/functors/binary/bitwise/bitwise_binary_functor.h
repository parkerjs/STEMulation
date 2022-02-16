#ifndef BITWISE_BINARY_FUNCTOR_H
#define BITWISE_BINARY_FUNCTOR_H

#include "binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace bitwise
{

// forward declarations
template<typename, typename> class BitwiseAndFunctor;
template<typename, typename> class BitwiseLeftShiftFunctor;
template<typename, typename> class BitwiseOrFunctor;
template<typename, typename> class BitwiseRightShiftFunctor;
template<typename, typename> class BitwiseXorFunctor;

/**
 * Base class for binary bitwise functors
 */
template<typename T, typename Result = T>
class BitwiseBinaryFunctor
: public BinaryFunctor<T, Result>
{
public:

    /**
     * Destructor
     */
    virtual ~BitwiseBinaryFunctor(void) override
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual BitwiseBinaryFunctor<T, Result> *clone(void) const override = 0;

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static BitwiseBinaryFunctor<T, Result> *create(const BinaryFunctorType &type)
    {
        typedef BinaryFunctorType::Enum BinaryOp;
        typedef BitwiseBinaryFunctor<T, Result> * (*tFactory)(void);
        static const std::map<BinaryFunctorType, tFactory> factoryMap =
        { { BinaryOp::BitwiseAnd, reinterpret_cast<tFactory>(&BitwiseAndFunctor<T, Result>::create) },
          { BinaryOp::BitwiseLeftShift, reinterpret_cast<tFactory>(&BitwiseLeftShiftFunctor<T, Result>::create) },
          { BinaryOp::BitwiseOr, reinterpret_cast<tFactory>(&BitwiseOrFunctor<T, Result>::create) },
          { BinaryOp::BitwiseRightShift, reinterpret_cast<tFactory>(&BitwiseRightShiftFunctor<T, Result>::create) },
          { BinaryOp::BitwiseXor, reinterpret_cast<tFactory>(&BitwiseXorFunctor<T, Result>::create) } };

        auto &&itTypeFactoryPair = factoryMap.find(type);
        if (itTypeFactoryPair != factoryMap.cend())
            return (*itTypeFactoryPair->second)();
        else
            return nullptr;
    }

    /**
     * Function to determine whether or not the current object is a bitwise operation
     */
    inline virtual bool isBitwise(void) const override
    {
        return true;
    }
};

}

}

}

}

#endif
