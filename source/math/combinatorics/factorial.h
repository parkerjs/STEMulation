#ifndef FACTORIAL_H
#define FACTORIAL_H

#include "algorithm.h"

namespace math
{

namespace combinatorics
{

/**
 * Struct for calculating the Nth factorial using template meta-programming
 */
template<size_t N>
struct Factorial
{
    enum { value = N * Factorial<N - 1>::value };
};

/**
 * Specialization for computing the factorial of 0
 */
template<>
struct Factorial<0>
{
    enum { value = 1 };
};

/**
 * Type alias declarations
 */
template<size_t N> using FactorialTable = typename utilities::generate_array<long long, N + 1, Factorial>::result;

}

}

#endif
