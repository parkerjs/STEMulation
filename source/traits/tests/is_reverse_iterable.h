#ifndef IS_REVERSE_ITERABLE_H
#define IS_REVERSE_ITERABLE_H

namespace traits
{

namespace tests
{

/**
 * Type traits test to determine if an iterator of a given category and type can be reverse-iterated
 */
template<typename>
struct is_reverse_iterable final
{
    /**
     * Static constants
     */
    static constexpr bool value = false;
};

/**
 * Type traits test to determine if an iterator of a given category and type can be reverse-iterated
 */
template<typename T, bool reverse, template<typename, bool, typename ...> class Iterator,
         typename ... categories>
struct is_reverse_iterable<Iterator<T, reverse, categories ...>> final
{
    /**
     * Static constants
     */
    static constexpr bool value = true;
};

}

}

#endif
