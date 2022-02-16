#ifndef BINARY_FUNCTOR_H
#define BINARY_FUNCTOR_H

#include "binary_functor_type.h"
#include "export_library.h"
#include "functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

// forward declarations
namespace arithmetic { template<typename, typename> class ArithmeticBinaryFunctor; }
namespace bitwise { template<typename, typename> class BitwiseBinaryFunctor; }
namespace comparison { template<typename, typename> class ComparisonBinaryFunctor; }
namespace logical { template<typename, typename> class LogicalBinaryFunctor; }

/**
 * Base class for binary functors
 */
template<typename T, typename Result = T>
class BinaryFunctor
: public Functor<T, Result>,
  public attributes::concrete::OutputStreamable<BinaryFunctor<T, Result>>
{
public:

    /**
     * Destructor
     */
    virtual ~BinaryFunctor(void) override
    {

    }

    /**
     * Function call operator
     */
    inline virtual Result operator () (const T &lhs, const T &rhs)
    {
        return evaluate(lhs, rhs);
    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual BinaryFunctor<T, Result> *clone(void) const override = 0;

    /**
     * create() function
     * @param type the functor type to be instantiated
     */
    static BinaryFunctor<T, Result> *create(const BinaryFunctorType &type)
    {
        using namespace arithmetic;
        using namespace bitwise;
        using namespace comparison;
        using namespace logical;
        typedef BinaryFunctorType::Enum BinaryOp;
        typedef BinaryFunctor<T, Result> * (*tFactory)(const BinaryFunctorType &type);
        static const std::map<BinaryFunctorType, tFactory> factoryMap =
        { { BinaryOp::Addition, reinterpret_cast<tFactory>(&ArithmeticBinaryFunctor<T, Result>::create) },
          { BinaryOp::BitwiseAnd, reinterpret_cast<tFactory>(&BitwiseBinaryFunctor<T, Result>::create) },
          { BinaryOp::BitwiseLeftShift, reinterpret_cast<tFactory>(&BitwiseBinaryFunctor<T, Result>::create) },
          { BinaryOp::BitwiseOr, reinterpret_cast<tFactory>(&BitwiseBinaryFunctor<T, Result>::create) },
          { BinaryOp::BitwiseRightShift, reinterpret_cast<tFactory>(&BitwiseBinaryFunctor<T, Result>::create) },
          { BinaryOp::BitwiseXor, reinterpret_cast<tFactory>(&BitwiseBinaryFunctor<T, Result>::create) },
          { BinaryOp::Division, reinterpret_cast<tFactory>(&ArithmeticBinaryFunctor<T, Result>::create) },
          { BinaryOp::Equal, reinterpret_cast<tFactory>(&ComparisonBinaryFunctor<T, Result>::create) },
          { BinaryOp::Greater, reinterpret_cast<tFactory>(&ComparisonBinaryFunctor<T, Result>::create) },
          { BinaryOp::GreaterEqual, reinterpret_cast<tFactory>(&ComparisonBinaryFunctor<T, Result>::create) },
          { BinaryOp::Less, reinterpret_cast<tFactory>(&ComparisonBinaryFunctor<T, Result>::create) },
          { BinaryOp::LessEqual, reinterpret_cast<tFactory>(&ComparisonBinaryFunctor<T, Result>::create) },
          { BinaryOp::LogicalAnd, reinterpret_cast<tFactory>(&LogicalBinaryFunctor<T, Result>::create) },
          { BinaryOp::LogicalOr, reinterpret_cast<tFactory>(&LogicalBinaryFunctor<T, Result>::create) },
          { BinaryOp::Modulo, reinterpret_cast<tFactory>(&ArithmeticBinaryFunctor<T, Result>::create) },
          { BinaryOp::Multiplication, reinterpret_cast<tFactory>(&ArithmeticBinaryFunctor<T, Result>::create) },
          { BinaryOp::NotEqual, reinterpret_cast<tFactory>(&ComparisonBinaryFunctor<T, Result>::create) },
          { BinaryOp::Subtraction, reinterpret_cast<tFactory>(&ArithmeticBinaryFunctor<T, Result>::create) } };

        auto &&itTypeFactoryPair = factoryMap.find(type);
        if (itTypeFactoryPair != factoryMap.cend())
            return (*itTypeFactoryPair->second)(type);
        else
            return nullptr;
    }

    /**
     * Evaluation function
     */
    EXPORT_STEM virtual Result evaluate(const T &lhs, const T &rhs) const = 0;

    /**
     * Evaluation function
     */
    inline bool evaluate(Result &result, const T &lhs, const T &rhs) const
    {
        result = evaluate(lhs, rhs);

        return true;
    }

    /**
     * Function to get the binary functor type
     */
    EXPORT_STEM virtual BinaryFunctorType getType(void) const = 0;

    /**
     * Function to determine whether or not the current object is an arithmetic operation
     */
    inline virtual bool isArithmetic(void) const
    {
        return false;
    }

    /**
     * Function to determine whether or not the current object is a bitwise operation
     */
    inline virtual bool isBitwise(void) const
    {
        return false;
    }

    /**
     * Function to determine whether or not the current object is a comparison operation
     */
    inline virtual bool isComparison(void) const
    {
        return false;
    }

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
