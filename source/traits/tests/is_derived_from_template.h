#ifndef IS_DERIVED_FROM_TEMPLATE_H
#define IS_DERIVED_FROM_TEMPLATE_H

#include <type_traits>

namespace traits
{

namespace tests
{

namespace detail
{

/**
 * Type traits struct to determine whether or not T is derived from a class template
 */
template<template<typename ...> class Base, typename Derived>
struct is_derived_from_template
{
    /**
     * Typedef declarations
     */
    typedef typename std::remove_cv<typename std::remove_reference<Derived>::type>::type T;

    /**
     * Static constants
     */
    template<typename ... Args> static auto test(Base<Args ...> *) ->
    typename std::integral_constant<bool, !std::is_same<T, Base<Args ...>>::value>;
    static std::false_type test(void *);

    /**
     * Type alias declarations
     */
    using type = decltype(test(std::declval<T *>()));
};

}

/**
 * Type alias declarations
 */
template<template<typename ...> class Base, typename Derived>
using is_derived_from_template = typename detail::is_derived_from_template<Base, Derived>::type;

}

}

#endif
