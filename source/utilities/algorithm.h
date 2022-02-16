#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <algorithm>
#include <array>

namespace utilities
{

/**
 * Structure to hold table of compile-time generated data
 */
template<typename T, T ... Args>
struct array_holder final
{
    static constexpr std::array<T, sizeof ... (Args)> getData() { return { { Args ... } }; }
};

/**
 * Helper structure to facilitate compile-time generation of data table
 */
template<typename T, size_t N, template<size_t>
class Function, T ... Args>
struct generate_array_impl final
{
    typedef typename generate_array_impl<T, N - 1, Function, Function<N>::value, Args ...>::result result;
};

/**
 * Specialization of generate_array_impl for index 0
 */
template<typename T, template<size_t>
class Function, T ... Args>
struct generate_array_impl<T, 0, Function, Args ...> final
{
    typedef array_holder<T, Function<0>::value, Args ...> result;
};

/**
 * This class can be used to generate compile-time static arrays of data
 */
template<typename T, size_t N, template<size_t>
class Function> struct generate_array final
{
    typedef typename generate_array_impl<T, N - 1, Function>::result result;
};

/**
 * This function returns an iterator to the greatest element in an ordered set that is less than or equal to
 * the specified value
 */
template<typename T, typename Container>
inline typename Container::const_iterator infimum(T &&value, const Container &container)
{
    return infimum(container.cbegin(), container.cend(), std::forward<T>(value));
}

/**
 * This function returns an iterator to the greatest element in an ordered set that is less than or equal to
 * the specified value
 */
template<typename T, class ForwardIterator>
inline ForwardIterator infimum(T &&value, ForwardIterator first, ForwardIterator last)
{
    auto it = std::upper_bound(first, last, std::forward<T>(value));
    if (it != first)
        --it;
    else
        it = last;

    return it;
}

/**
 * This function returns an iterator to the smallest element in an ordered set that is greater than or equal to
 * the specified value, or an iterator to the end of the vector when the value is greater than all elements in
 * the set.
 */
template<typename T, typename Container>
inline typename Container::const_iterator supremum(T &&value, const Container &container)
{
    return std::lower_bound(container.cbegin(), container.cend(), std::forward<T>(value));
}

/**
 * This function returns an iterator to the smallest element in an ordered set that is greater than or equal to
 * the specified value, or an iterator to the end of the vector when the value is greater than all elements in
 * the set.
 */
template<typename T, class ForwardIterator>
inline ForwardIterator supremum(T &&value, ForwardIterator first, ForwardIterator last)
{
    return std::lower_bound(first, last, std::forward<T>(value));
}

}

#endif
