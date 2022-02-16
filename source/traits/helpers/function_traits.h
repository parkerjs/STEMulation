#ifndef FUNCTION_TRAITS_H
#define FUNCTION_TRAITS_H

#include <functional>
#include <tuple>

namespace traits
{

namespace helpers
{

/**
 * Helper class to infer function return type and signature (for generic types that are functors)
 */
template<typename Function>
struct function_traits
: public function_traits<decltype(&Function::operator())>
{

};

/**
 * Helper class to infer function return type and signature (member functions)
 */
template<typename Return, typename Object, typename ... Args>
struct function_traits<Return (Object::*)(Args ...)>
{
    /**
     * Typedef declarations
     */
    typedef Return return_type;
    typedef Object instance_type;
    typedef Object const &instance_reference;

    /**
     * Static constants
     */
    static constexpr std::size_t arity = sizeof ... (Args);

    /**
     * Helper class to return the argument type for the Nth argument
     */
    template<std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args ...>>::type;
    };
};

/**
 * Helper class to infer function return type and signature (const member functions)
 */
template<typename Return, typename Object, typename ... Args>
struct function_traits<Return (Object::*)(Args ...) const>
: public function_traits<Return (Object::*)(Args ...)>
{

};

/**
 * Helper class to infer function return type and signature (non-member functions)
 */
template<typename Return, typename ... Args>
struct function_traits<Return (*)(Args ...)>
{
    /**
     * Typedef declarations
     */
    typedef Return return_type;

    /**
     * Static constants
     */
    static constexpr std::size_t arity = sizeof ... (Args);

    /**
     * Helper class to return the argument type for the Nth argument
     */
    template<std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args ...>>::type;
    };
};

}

}

#endif
