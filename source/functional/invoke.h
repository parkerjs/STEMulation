#ifndef INVOKE_H
#define INVOKE_H

#include <functional>
#include <type_traits>

namespace functional
{

/**
 * Function to invoke the callable object func with args
 */
template<typename Functor, typename ... Args>
inline typename std::enable_if<std::is_member_pointer<typename std::decay<Functor>::type>::value,
                               typename std::result_of<Functor && (Args && ...)>::type>::type
invoke(Functor &&func, Args && ... args)
{
    return std::mem_fn(func)(std::forward<Args>(args) ...);
}

/**
 * Function to invoke the callable object func with args
 */
template<typename Functor, typename ... Args>
inline typename std::enable_if<!std::is_member_pointer<typename std::decay<Functor>::type>::value,
                               typename std::result_of<Functor && (Args && ...)>::type>::type
invoke(Functor &&func, Args && ... args)
{
    return std::forward<Functor>(func)(std::forward<Args>(args)...);
}

/**
 * Function to invoke the callable object func with args
 */
template<typename Return, typename Functor, typename ... Args>
Return invoke(Functor &&func, Args && ... args)
{
    return invoke(std::forward<Functor>(func), std::forward<Args>(args) ...);
}

}

#endif
