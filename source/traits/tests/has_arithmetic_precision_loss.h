#ifndef HAS_ARITHMETIC_PRECISION_LOSS_H
#define HAS_ARITHMETIC_PRECISION_LOSS_H

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not a conversion from RHS to LHS causes a loss of precision
 */
template<typename L, typename R>
struct has_arithmetic_precision_loss final
{
    typedef typename std::decay<L>::type Left;
    typedef typename std::decay<R>::type Right;

    /**
     * Static constants
     */
    static const bool value = std::is_fundamental<Left>::value && std::is_fundamental<Right>::value &&
                            ((std::is_integral<Left>::value && std::is_floating_point<Right>::value) ||
                            ((sizeof(Left) < sizeof(Right)) && ((std::is_floating_point<Left>::value &&
                              std::is_floating_point<Right>::value) || (std::is_integral<Left>::value &&
                              std::is_integral<Right>::value))) ||
                             (std::is_signed<Left>::value && !std::is_signed<Right>::value) ||
                            (!std::is_signed<Left>::value && std::is_signed<Right>::value) ||
                             (std::is_integral<Left>::value && std::is_floating_point<Right>::value));
};

}

}

#endif
