#ifndef COMPARISON_BINARY_FUNCTOR_H
#define COMPARISON_BINARY_FUNCTOR_H

#include "binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace comparison
{

// forward declarations
template<typename, typename> class CompareEqualFunctor;
template<typename, typename> class CompareGreaterFunctor;
template<typename, typename> class CompareGreaterEqualFunctor;
template<typename, typename> class CompareLessFunctor;
template<typename, typename> class CompareLessEqualFunctor;
template<typename, typename> class CompareNotEqualFunctor;

/**
 * Base class for binary comparison functors
 */
template<typename T, typename Result = bool>
class ComparisonBinaryFunctor
: public BinaryFunctor<T, Result>
{
public:

    /**
     * Destructor
     */
    virtual ~ComparisonBinaryFunctor(void) override
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual ComparisonBinaryFunctor<T, Result> *clone(void) const override = 0;

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static ComparisonBinaryFunctor<T, Result> *create(const BinaryFunctorType &type)
    {
        typedef BinaryFunctorType::Enum BinaryOp;
        typedef ComparisonBinaryFunctor<T, Result> * (*tFactory)(void);
        static const std::map<BinaryFunctorType, tFactory> factoryMap =
        { { BinaryOp::Equal, reinterpret_cast<tFactory>(&CompareEqualFunctor<T, Result>::create) },
          { BinaryOp::Greater, reinterpret_cast<tFactory>(&CompareGreaterFunctor<T, Result>::create) },
          { BinaryOp::GreaterEqual, reinterpret_cast<tFactory>(&CompareGreaterEqualFunctor<T, Result>::create) },
          { BinaryOp::Less, reinterpret_cast<tFactory>(&CompareLessFunctor<T, Result>::create) },
          { BinaryOp::LessEqual, reinterpret_cast<tFactory>(&CompareLessEqualFunctor<T, Result>::create) },
          { BinaryOp::NotEqual, reinterpret_cast<tFactory>(&CompareNotEqualFunctor<T, Result>::create) } };

        auto &&itTypeFactoryPair = factoryMap.find(type);
        if (itTypeFactoryPair != factoryMap.cend())
            return (*itTypeFactoryPair->second)();
        else
            return nullptr;
    }

    /**
     * Function to determine whether or not the current object is a comparison operation
     */
    inline virtual bool isComparison(void) const override
    {
        return true;
    }
};

}

}

}

}

#endif
