#ifndef ARITHMETIC_BINARY_FUNCTOR_H
#define ARITHMETIC_BINARY_FUNCTOR_H

#include "binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace arithmetic
{

// forward declarations
template<typename, typename> class AdditionFunctor;
template<typename, typename> class DivisionFunctor;
template<typename, typename> class ModuloFunctor;
template<typename, typename> class MultiplicationFunctor;
template<typename, typename> class SubtractionFunctor;

/**
 * Base class for binary arithmetic functors
 */
template<typename T, typename Result = T>
class ArithmeticBinaryFunctor
: public BinaryFunctor<T, Result>
{
public:

    /**
     * Destructor
     */
    virtual ~ArithmeticBinaryFunctor(void) override
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual ArithmeticBinaryFunctor<T, Result> *clone(void) const override = 0;

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    inline static ArithmeticBinaryFunctor<T, Result> *create(const BinaryFunctorType &type)
    {
        typedef ArithmeticBinaryFunctor<T, Result> * (*tFactory)(void);
        typedef BinaryFunctorType::Enum BinaryOp;
        static const std::map<BinaryFunctorType, tFactory> factoryMap =
        { { BinaryOp::Addition, reinterpret_cast<tFactory>(&AdditionFunctor<T, Result>::create) },
          { BinaryOp::Division, reinterpret_cast<tFactory>(&DivisionFunctor<T, Result>::create) },
          { BinaryOp::Modulo, reinterpret_cast<tFactory>(&ModuloFunctor<T, Result>::create) },
          { BinaryOp::Multiplication, reinterpret_cast<tFactory>(&MultiplicationFunctor<T, Result>::create) },
          { BinaryOp::Subtraction, reinterpret_cast<tFactory>(&SubtractionFunctor<T, Result>::create) } };

        auto &&itTypeFactoryPair = factoryMap.find(type);
        if (itTypeFactoryPair != factoryMap.cend())
            return (*itTypeFactoryPair->second)();
        else
            return nullptr;
    }

    /**
     * Function to determine whether or not the current object is an arithmetic operation
     */
    inline virtual bool isArithmetic(void) const override
    {
        return true;
    }
};

}

}

}

}

#endif
