#ifndef ARITHMETIC_MATRIX_OPERATIONS_H
#define ARITHMETIC_MATRIX_OPERATIONS_H

#include "forward_matrix.h"
#include <algorithm>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Addition assignment operator
 */
template<typename L, typename R>
auto operator += (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<L>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<R>::type>::value,
                        decltype(std::forward<L>(lhs).operator += (std::forward<L>(lhs)))>::type
{
    decltype(std::forward<L>(lhs).operator += (std::forward<L>(lhs))) result(lhs);

    auto &&operation = [&rhs] (auto &&lhs) { return lhs + std::forward<R>(rhs); };
    std::transform(result.cbegin(), result.cend(), result.begin(), operation);

    return result;
}

/**
 * Addition assignment operator
 */
template<typename L, typename R>
inline auto operator += (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<R>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<L>::type>::value,
                        decltype(std::forward<R>(rhs).operator += (std::forward<R>(rhs)))>::type
{
    return std::forward<R>(rhs) += std::forward<L>(lhs);
}

/**
 * Subtraction assignment operator
 */
template<typename L, typename R>
auto operator -= (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<L>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<R>::type>::value,
                        decltype(std::forward<L>(lhs).operator -= (std::forward<L>(lhs)))>::type
{
    decltype(std::forward<L>(lhs).operator -= (std::forward<L>(lhs))) result(lhs);

    auto &&operation = [&rhs] (auto &&lhs) { return lhs - std::forward<R>(rhs); };
    std::transform(result.cbegin(), result.cend(), result.begin(), operation);

    return result;
}

/**
 * Subtraction assignment operator
 */
template<typename L, typename R>
inline auto operator -= (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<R>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<L>::type>::value,
                        decltype((-std::forward<R>(rhs)).operator += (std::forward<R>(rhs)))>::type
{
    return (-std::forward<R>(rhs)) -= std::forward<L>(lhs);
}

/**
 * Multiplication assignment operator
 */
template<typename L, typename R>
auto operator *= (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<L>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<R>::type>::value,
                        decltype(std::forward<L>(lhs).operator *= (std::forward<L>(lhs)))>::type
{
    decltype(std::forward<L>(lhs).operator *= (std::forward<L>(lhs))) result(lhs);

    auto &&operation = [&rhs] (auto &&lhs) { return lhs * std::forward<R>(rhs); };
    std::transform(result.cbegin(), result.cend(), result.begin(), operation);

    return result;
}

/**
 * Multiplication assignment operator
 */
template<typename L, typename R>
inline auto operator *= (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<R>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<L>::type>::value,
                        decltype(std::forward<R>(rhs).operator *= (std::forward<R>(rhs)))>::type
{
    return std::forward<R>(rhs) *= std::forward<L>(lhs);
}

/**
 * Division assignment operator
 */
template<typename L, typename R>
auto operator /= (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<L>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<R>::type>::value,
                        decltype(std::forward<L>(lhs).operator /= (std::forward<L>(lhs)))>::type
{
    decltype(std::forward<L>(lhs).operator /= (std::forward<L>(lhs))) result(lhs);

    auto &&operation = [&rhs] (auto &&lhs) { return lhs / std::forward<R>(rhs); };
    std::transform(result.cbegin(), result.cend(), result.begin(), operation);

    return result;
}

/**
 * Division assignment operator
 */
template<typename L, typename R>
auto operator /= (L &&lhs, R &&rhs) ->
typename std::enable_if<std::is_base_of<NumericMatrix, typename std::decay<R>::type>::value &&
                       !std::is_base_of<GeneralMatrix, typename std::decay<L>::type>::value,
                        decltype(std::forward<R>(rhs).operator /= (std::forward<R>(rhs)))>::type
{
    decltype(std::forward<R>(rhs).operator /= (std::forward<R>(rhs))) result(rhs);

    auto &&operation = [&lhs] (auto &&rhs) { return std::forward<L>(lhs) / rhs; };
    std::transform(result.cbegin(), result.cend(), result.begin(), operation);

    return result;
}

}

}

}

#endif
