#ifndef IS_SPECIALIZATION_OF_H
#define IS_SPECIALIZATION_OF_H

#include <string>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine if T is a specialization of Template
 */
template<typename T, template<typename ...> class Template,
         bool = std::is_class<typename std::decay<T>::type>::value>
struct is_specialization_of : std::false_type { };

/**
 * Type traits struct to determine if T is a specialization of Template (std::true_type)
 */
template<template<typename ...> class Template, typename ... Args>
struct is_specialization_of<Template<Args ...>, Template, true> : std::true_type { };

}

}

#endif
