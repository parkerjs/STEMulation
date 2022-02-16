#ifndef OPERATOR_H
#define OPERATOR_H

#include <utility>

namespace attributes
{

namespace operators
{

/* *INDENT-OFF* */

/*
 * This macro defines a class template for binary operator overloads
 */
#define DefineBinaryOperatorOverloads(Operation, Symbol)                                                     \
template<typename T>                                                                                         \
class Operation                                                                                              \
: public Base##Operation                                                                                     \
{                                                                                                            \
public:                                                                                                      \
                                                                                                             \
    /*******************************************************************************************************/\
    /* Operator overload                                                                                   */\
    /*******************************************************************************************************/\
    virtual ~Operation(void)                                                                                 \
    {                                                                                                        \
                                                                                                             \
    }                                                                                                        \
                                                                                                             \
    /*******************************************************************************************************/\
    /* Operator overload                                                                                   */\
    /*******************************************************************************************************/\
    template<typename U, typename V>                                                                         \
    inline friend auto operator Symbol (U &&lhs, V &&rhs) -> typename                                        \
    std::enable_if<std::is_base_of<Base##Operation, typename std::remove_reference<U>::type>::value &&       \
                   std::is_same<typename std::decay<U>::type, T>::value &&                                   \
                   std::is_lvalue_reference<U>::value, typename std::remove_reference<decltype(typename std::\
                   remove_reference<U>::type(lhs).operator Symbol##= (std::forward<V>(rhs)))>::type>::type   \
    {                                                                                                        \
        typename std::remove_reference<U>::type u(lhs);                                                      \
        return std::move(u) Symbol std::forward<V>(rhs);                                                     \
    }                                                                                                        \
                                                                                                             \
    /*******************************************************************************************************/\
    /* Operator overload                                                                                   */\
    /*******************************************************************************************************/\
    template<typename U, typename V>                                                                         \
    inline friend auto operator Symbol (U &&lhs, V &&rhs) -> typename                                        \
    std::enable_if<std::is_base_of<Base##Operation, typename std::remove_reference<U>::type>::value &&       \
                   std::is_same<typename std::decay<U>::type, T>::value &&                                   \
                  !std::is_lvalue_reference<U>::value,                                                       \
                  typename std::remove_reference<decltype(lhs Symbol##= std::forward<V>(rhs))>::type>::type  \
    {                                                                                                        \
        return std::forward<U>(lhs).operator Symbol##= (std::forward<V>(rhs));                               \
    }                                                                                                        \
                                                                                                             \
    /*******************************************************************************************************/\
    /* Operator overload                                                                                   */\
    /*******************************************************************************************************/\
    template<typename U, typename V>                                                                         \
    inline friend auto operator Symbol (U &&lhs, V &&rhs) -> typename                                        \
    std::enable_if<std::is_base_of<Base##Operation, typename std::remove_reference<V>::type>::value &&       \
                  !std::is_base_of<Base##Operation, typename std::remove_reference<U>::type>::value &&       \
                  !std::is_same<typename std::decay<U>::type, T>::value &&                                   \
                   std::is_same<typename std::decay<V>::type, T>::value &&                                   \
                  !std::is_array<typename std::remove_reference<U>::type>::value &&                          \
                   std::is_constructible<typename std::remove_reference<U>::type>::value &&                  \
                   std::is_lvalue_reference<U>::value, typename std::remove_reference<decltype(typename      \
                   std::remove_reference<U>::type(lhs) Symbol##= std::forward<V>(rhs))>::type>::type         \
    {                                                                                                        \
        typename std::remove_reference<U>::type u(lhs);                                                      \
        return std::move(u) Symbol std::forward<V>(rhs);                                                     \
    }                                                                                                        \
                                                                                                             \
    /*******************************************************************************************************/\
    /* Operator overload                                                                                   */\
    /*******************************************************************************************************/\
    template<typename U, typename V>                                                                         \
    inline friend auto operator Symbol (U &&lhs, V &&rhs) -> typename                                        \
    std::enable_if<std::is_base_of<Base##Operation, typename std::remove_reference<V>::type>::value &&       \
                  !std::is_base_of<Base##Operation, typename std::remove_reference<U>::type>::value &&       \
                  !std::is_same<typename std::decay<U>::type, T>::value &&                                   \
                   std::is_same<typename std::decay<V>::type, T>::value &&                                   \
                   std::is_array<typename std::remove_reference<U>::type>::value &&                          \
                   std::is_lvalue_reference<U>::value, typename std::remove_reference<V>::type>::type        \
    {                                                                                                        \
        typename std::remove_reference<V>::type v(lhs);                                                      \
        return std::move(v) Symbol std::forward<V>(rhs);                                                     \
    }                                                                                                        \
                                                                                                             \
    /*******************************************************************************************************/\
    /* Operator overload                                                                                   */\
    /*******************************************************************************************************/\
    template<typename U, typename V>                                                                         \
    inline friend auto operator Symbol (U &&lhs, V &&rhs) -> typename                                        \
    std::enable_if<std::is_base_of<Base##Operation, typename std::remove_reference<V>::type>::value &&       \
                  !std::is_base_of<Base##Operation, typename std::remove_reference<U>::type>::value &&       \
                  !std::is_same<typename std::decay<U>::type, T>::value &&                                   \
                   std::is_same<typename std::decay<V>::type, T>::value &&                                   \
                  !std::is_lvalue_reference<U>::value, typename std::remove_reference<decltype(              \
                   std::forward<U>(lhs) Symbol##= std::forward<V>(rhs))>::type>::type                        \
    {                                                                                                        \
        return std::forward<U>(lhs) Symbol##= std::forward<V>(rhs);                                          \
    }                                                                                                        \
};

/* *INDENT-ON* */

}

}

#endif
