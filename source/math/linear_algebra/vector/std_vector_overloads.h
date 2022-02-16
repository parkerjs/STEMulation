#ifndef STD_VECTOR_OVERLOADS_H
#define STD_VECTOR_OVERLOADS_H

#include "has_addition.h"
#include "has_division.h"
#include "has_multiplication.h"
#include "has_subtraction.h"
#include "is_std_vector.h"

namespace math
{

namespace linear_algebra
{

namespace vector
{

/**
 * std::vector addition
 */
template<typename L, typename R,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_addition_operator<L, R>::value>::type * = nullptr>
auto operator + (const std::vector<L> &lhs, const std::vector<R> &rhs)
{
    decltype(lhs + rhs) result(lhs.size());
    auto &&function = [] (auto &&x, auto &&y) { return x + y; };
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar addition
 */
template<typename L, typename R, typename U = typename std::decay<R>::type,
         typename std::enable_if<traits::tests::operators::arithmetic::has_addition_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator + (const std::vector<L> &lhs, R &&rhs)
{
    std::vector<decltype(L() + std::forward<R>(rhs))> result(lhs.size());
    auto &&function = [&rhs] (auto &&x) { return x + std::forward<R>(rhs); };
    std::transform(lhs.cbegin(), lhs.cend(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar addition
 */
template<typename L, typename R, typename U = typename std::decay<L>::type,
         typename std::enable_if<traits::tests::operators::arithmetic::has_addition_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator + (L &&lhs, const std::vector<R> &rhs)
{
    return rhs + std::forward<L>(lhs);
}

/**
 * std::vector subtraction
 */
template<typename L, typename R,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_subtraction_operator<L, R>::value>::type * = nullptr>
auto operator - (const std::vector<L> &lhs, const std::vector<R> &rhs)
{
    decltype(lhs - rhs) result(lhs.size());
    auto &&function = [] (auto &&x, auto &&y) { return x - y; };
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar subtraction
 */
template<typename L, typename R, typename U = typename std::decay<R>::type,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_subtraction_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator - (const std::vector<L> &lhs, R &&rhs)
{
    std::vector<decltype(L() - std::forward<R>(rhs))> result(lhs.size());
    auto &&function = [&rhs] (auto &&x) { return x - std::forward<R>(rhs); };
    std::transform(lhs.cbegin(), lhs.cend(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar subtraction
 */
template<typename L, typename R, typename U = typename std::decay<L>::type,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_subtraction_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator - (L &&lhs, const std::vector<R> &rhs)
{
    std::vector<R> result(rhs.size());
    auto &&function = [&lhs] (auto &&x) { return std::forward<L>(lhs) - x; };
    std::transform(rhs.cbegin(), rhs.cend(), result.begin(), function);

    return result;
}

/**
 * std::vector multiplication
 */
template<typename L, typename R,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_multiplication_operator<L, R>::value>::type * = nullptr>
auto operator * (const std::vector<L> &lhs, const std::vector<R> &rhs)
{
    decltype(lhs * rhs) result(lhs.size());
    auto &&function = [] (auto &&x, auto &&y) { return x * y; };
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar multiplication
 */
template<typename L, typename R, typename U = typename std::decay<R>::type,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_multiplication_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator * (const std::vector<L> &lhs, R &&rhs)
{
    std::vector<decltype(L() * std::forward<R>(rhs))> result(lhs.size());
    auto &&function = [&rhs] (auto &&x) { return x * std::forward<R>(rhs); };
    std::transform(lhs.cbegin(), lhs.cend(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar multiplication
 */
template<typename L, typename R, typename U = typename std::decay<L>::type,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_multiplication_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator * (L &&lhs, const std::vector<R> &rhs)
{
    return rhs * std::forward<L>(lhs);
}

/**
 * std::vector division
 */
template<typename L, typename R,
         typename std::enable_if<traits::tests::operators::arithmetic
                                       ::has_division_operator<L, R>::value>::type * = nullptr>
auto operator / (const std::vector<L> &lhs, const std::vector<R> &rhs)
{
    decltype(lhs / rhs) result(lhs.size());
    auto &&function = [] (auto &&x, auto &&y) { return x / y; };
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar division
 */
template<typename L, typename R, typename U = typename std::decay<R>::type,
         typename std::enable_if<traits::tests::operators::arithmetic::has_division_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator / (const std::vector<L> &lhs, R &&rhs)
{
    std::vector<decltype(L() / std::forward<R>(rhs))> result(lhs.size());
    auto &&function = [&rhs] (auto &&x) { return x / std::forward<R>(rhs); };
    std::transform(lhs.cbegin(), lhs.cend(), result.begin(), function);

    return result;
}

/**
 * std::vector scalar division
 */
template<typename L, typename R, typename U = typename std::decay<L>::type,
         typename std::enable_if<traits::tests::operators::arithmetic::has_division_operator<L, R>::value &&
                                !traits::tests::is_std_vector<U>::value>::type * = nullptr>
auto operator / (L &&lhs, const std::vector<R> &rhs)
{
    std::vector<R> result(rhs.size());
    auto &&function = [&lhs] (auto &&x) { return std::forward<L>(lhs) / x; };
    std::transform(rhs.cbegin(), rhs.cend(), result.begin(), function);

    return result;
}

/**
 * std::vector stream extraction operator
 */
template<typename R>
auto &operator << (std::ostream &stream, const std::vector<R> &rhs)
{
    auto width = stream.width();
    auto &&function = [&stream, &width] (auto &&x) { std::cout << std::setw(width) << x << std::endl; };
    std::for_each(rhs.cbegin(), rhs.cend(), function);

    return stream;
}

}

}

}

#endif
