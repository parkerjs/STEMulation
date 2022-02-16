#ifndef POINTER_UNWRAPPER_H
#define POINTER_UNWRAPPER_H

#include <type_traits>

namespace traits
{

namespace helpers
{

/**
 * Helper class to infer the underlying type to which a pointer points
 */
template<typename T>
struct pointer_unwrapper
{
    /**
     * Typedef declarations
     */
    typedef T type;
};

/**
 * Helper class to infer the underlying type to which a pointer points
 */
template<typename T>
struct pointer_unwrapper<T *>
{
    /**
     * Typedef declarations
     */
    typedef typename pointer_unwrapper<T>::type type;
};

/**
 * Helper class to infer the underlying type to which a pointer points
 */
template<typename T>
struct pointer_unwrapper<std::shared_ptr<T>>
{
    /**
     * Typedef declarations
     */
    typedef typename pointer_unwrapper<T>::type type; type;
};

/**
 * Helper class to infer the underlying type to which a pointer points
 */
template<typename T>
struct pointer_unwrapper<std::unique_ptr<T>>
{
    /**
     * Typedef declarations
     */
    typedef typename pointer_unwrapper<T>::type type; type;
};

/**
 * Helper class to infer the underlying type to which a pointer points
 */
template<typename T>
struct pointer_unwrapper<std::weak_ptr<T>>
{
    /**
     * Typedef declarations
     */
    typedef typename pointer_unwrapper<T>::type type; type;
};

}

}

#endif
