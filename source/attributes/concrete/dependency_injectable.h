#ifndef DEPENDENCY_INJECTABLE_H
#define DEPENDENCY_INJECTABLE_H

#include "all_of.h"
#include "is_derived_from_template.h"
#include "is_specialization_of.h"
#include "lambda_helper.h"
#include "tuple_for_each.h"
#include "tuple_has_type.h"
#include <algorithm>
#include <boost/preprocessor/repetition.hpp>
#include <tuple>
#include <type_traits>

/**
 * Macro definitions
 */
#define DependencyInjectableVirtualBaseInitializer1(z, i, ...) __VA_ARGS__
#define DependencyInjectableVirtualBaseInitializer0(z, i, ...) \
BOOST_PP_COMMA_IF(i) \
BOOST_PP_REPEAT(BOOST_PP_SUB(BOOST_PP_TUPLE_ELEM(2, 0, __VA_ARGS__), BOOST_PP_ADD(i, 1)), \
                DependencyInjectableVirtualBaseInitializer1, tBase::) BOOST_PP_TUPLE_ELEM(2, 1, __VA_ARGS__)
#define DependencyInjectableVirtualBaseInitializer(i, ...) \
BOOST_PP_REPEAT(i, DependencyInjectableVirtualBaseInitializer0, (i, tDependencyInjectable(__VA_ARGS__)))

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that require dependency injection
 */
template<typename ... Dependencies>
class DependencyInjectable
{
public:

    /**
     * Typedef declarations
     */
    typedef DependencyInjectable<Dependencies ...> tDependencies;
    typedef tDependencies tDependencyInjectable;
    typedef std::tuple<Dependencies ...> tTuple;
    static const constexpr size_t Depth = 0;

private:

    /**
     * Friend declarations
     */
    template<typename ...> friend class DependencyInjectable;

    /**
     * Type traits test to determine if a tuple contains a derived type that is convertible to the specified
     * type
     */
    template<typename T, typename Tuple, bool = traits::tests::tuple_has_type<typename std::decay<T>::type,
             typename std::decay<Tuple>::type>::value>
    struct tuple_has_convertible_derived_type
    : public std::true_type
    {
        /**
         * Typedef declarations
         */
        typedef T type;
    };

    /**
     * Type traits test to determine if a tuple contains a derived type that is convertible to the specified
     * type
     */
    template<typename T, typename Arg, typename ... Args>
    struct tuple_has_convertible_derived_type<T, std::tuple<Arg, Args ...>, false>
    : public std::true_type
    {
        /**
         * Typedef declarations
         */
        typedef typename std::remove_pointer<typename std::decay<T>::type>::type DecayT;
        typedef typename std::remove_pointer<typename std::decay<Arg>::type>::type DecayArg;
        typedef typename std::conditional<std::is_base_of<DecayT, DecayArg>::value, Arg,
                typename tuple_has_convertible_derived_type<T, std::tuple<Args ...>, false>::type>::type type;
    };

    /**
     * Type traits test to determine if a tuple contains a derived type that is convertible to the specified
     * type
     */
    template<typename T>
    struct tuple_has_convertible_derived_type<T, std::tuple<>, false> : public std::false_type { };

    /**
     * Helper struct to assert that a tuple has a specified type
     */
    template<typename T, typename Tuple>
    struct assert_tuple_has_type
    : tuple_has_convertible_derived_type<T, Tuple>
    {
        static_assert(tuple_has_convertible_derived_type<T, Tuple>::value,
                      "A required dependency is missing!");
    };

    /**
     * Helper structure to assign dependencies
     */
    template<int index, typename = void>
    struct assign_dependencies
    {
        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<index, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<(std::is_convertible<DecayTupleOne, tDependencyInjectable>::value ||
                                   traits::tests::is_specialization_of<DecayTupleOne, std::tuple>::value) &&
                                   traits::tests::is_specialization_of<DecayTupleTwo, std::tuple>::value &&
                                   traits::tests::tuple_has_type<typename std::add_lvalue_reference<Type>::type,
                                   DecayTupleTwo>::value>
        {
            std::get<Type>(tupleOne) = std::get<typename std::add_lvalue_reference<Type>::type>(tupleTwo);
            assign_dependencies<index - 1> { } (std::forward<TupleOne>(tupleOne),
                                                std::forward<TupleTwo>(tupleTwo));
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<index, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<(std::is_convertible<DecayTupleOne, tDependencyInjectable>::value ||
                                   traits::tests::is_specialization_of<DecayTupleOne, std::tuple>::value) &&
                                   traits::tests::is_specialization_of<DecayTupleTwo, std::tuple>::value &&
                                   traits::tests::tuple_has_type<typename std::add_rvalue_reference<Type>::type,
                                   DecayTupleTwo>::value>
        {
            std::get<Type>(tupleOne) = std::get<typename std::add_rvalue_reference<Type>::type>(tupleTwo);
            assign_dependencies<index - 1> { } (std::forward<TupleOne>(tupleOne),
                                                std::forward<TupleTwo>(tupleTwo));
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<index, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<(std::is_convertible<DecayTupleOne, tDependencyInjectable>::value ||
                                   traits::tests::is_specialization_of<DecayTupleOne, std::tuple>::value) &&
                                   traits::tests::is_specialization_of<DecayTupleTwo, std::tuple>::value &&
                                   traits::tests::tuple_has_type<Type, DecayTupleTwo>::value>
        {
            std::get<Type>(tupleOne) = std::get<Type>(tupleTwo);
            assign_dependencies<index - 1> { } (std::forward<TupleOne>(tupleOne),
                                                std::forward<TupleTwo>(tupleTwo));
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<index, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<!traits::tests::tuple_has_type<Type, DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_lvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_rvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  !tuple_has_convertible_derived_type<Type, DecayTupleTwo>::value>
        {
            // do nothing
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<index, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<!traits::tests::tuple_has_type<Type, DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_lvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_rvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  tuple_has_convertible_derived_type<Type, DecayTupleTwo>::value>
        {
            typedef typename tuple_has_convertible_derived_type<Type, DecayTupleTwo>::type type;
            std::get<Type>(tupleOne) = std::get<type>(tupleTwo);
            assign_dependencies<index - 1> { } (std::forward<TupleOne>(tupleOne),
                                                std::forward<TupleTwo>(tupleTwo));
        }
    };

    /**
     * Helper structure to assign dependencies
     */
    template<typename T>
    struct assign_dependencies<0, T>
    {
        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<0, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<(std::is_convertible<DecayTupleOne, tDependencyInjectable>::value ||
                                   traits::tests::is_specialization_of<DecayTupleOne, std::tuple>::value) &&
                                   traits::tests::is_specialization_of<DecayTupleTwo, std::tuple>::value &&
                                   traits::tests::tuple_has_type<typename std::add_lvalue_reference<Type>::type,
                                   DecayTupleTwo>::value>
        {
            std::get<Type>(tupleOne) = std::get<typename std::add_lvalue_reference<Type>::type>(tupleTwo);
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<0, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<(std::is_convertible<DecayTupleOne, tDependencyInjectable>::value ||
                                   traits::tests::is_specialization_of<DecayTupleOne, std::tuple>::value) &&
                                   traits::tests::is_specialization_of<DecayTupleTwo, std::tuple>::value &&
                                   traits::tests::tuple_has_type<typename std::add_rvalue_reference<Type>::type,
                                   DecayTupleTwo>::value>
        {
            std::get<Type>(tupleOne) = std::get<typename std::add_rvalue_reference<Type>::type>(tupleTwo);
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<0, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<(std::is_convertible<DecayTupleOne, tDependencyInjectable>::value ||
                                   traits::tests::is_specialization_of<DecayTupleOne, std::tuple>::value) &&
                                   traits::tests::is_specialization_of<DecayTupleTwo, std::tuple>::value &&
                                   traits::tests::tuple_has_type<Type, DecayTupleTwo>::value>
        {
            std::get<Type>(tupleOne) = std::get<Type>(tupleTwo);
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<0, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<!traits::tests::tuple_has_type<Type, DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_lvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_rvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  !tuple_has_convertible_derived_type<Type, DecayTupleTwo>::value>
        {
            // do nothing
        }

        /**
         * Function call operator
         */
        template<typename TupleOne, typename TupleTwo,
                 typename DecayTupleOne = typename std::decay<TupleOne>::type,
                 typename DecayTupleTwo = typename std::decay<TupleTwo>::type,
                 typename Type = typename std::decay<typename std::tuple_element<0, DecayTupleOne>::type>::type>
        inline auto operator () (TupleOne &&tupleOne, TupleTwo &&tupleTwo) ->
        typename std::enable_if_t<!traits::tests::tuple_has_type<Type, DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_lvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  !traits::tests::tuple_has_type<typename std::add_rvalue_reference<Type>::type,
                                  DecayTupleTwo>::value &&
                                  tuple_has_convertible_derived_type<Type, DecayTupleTwo>::value>
        {
            typedef typename tuple_has_convertible_derived_type<Type, DecayTupleTwo>::type type;
            std::get<Type>(tupleOne) = std::get<type>(tupleTwo);
        }
    };

private:

    /**
     * Constructor
     */
    DependencyInjectable(void) = delete;

public:

    /**
     * Constructor
     */
    template<typename Tuple, typename DecayTuple = typename std::decay<Tuple>::type,
             std::enable_if_t<traits::tests::is_specialization_of<DecayTuple,
                              attributes::concrete::DependencyInjectable>::value> * = nullptr>
    DependencyInjectable(Tuple &&dependencies)
    : DependencyInjectable(dependencies.getDependencies())
    {

    }

    /**
     * Constructor
     */
    template<typename Tuple, typename DecayTuple = typename std::decay<Tuple>::type,
             std::enable_if_t<traits::tests::is_specialization_of<DecayTuple, std::tuple>::value> * = nullptr>
    DependencyInjectable(Tuple &&dependencies)
    {
        if (traits::helpers::all_of<assert_tuple_has_type<Dependencies, DecayTuple>::value ...>::value)
            setDependencies(std::forward<Tuple>(dependencies));
    }

    /**
     * Constructor
     */
    template<typename Arg, typename DecayArg = typename std::decay<Arg>::type, typename = std::enable_if_t<
             !std::is_convertible<DecayArg, tDependencies>::value &&
             !std::is_same<tDependencies, DecayArg>::value &&
             !std::is_base_of<tDependencies, DecayArg>::value &&
             !traits::tests::is_specialization_of<DecayArg, std::tuple>::value &&
             !traits::tests::is_specialization_of<DecayArg, attributes::concrete::DependencyInjectable>::value>>
    DependencyInjectable(Arg &&dependency)
    {
        static_assert(assert_tuple_has_type<DecayArg, std::tuple<
                      Dependencies ...>>::value, "A required dependency is missing!");

        setDependencies(std::forward_as_tuple(std::forward<Arg>(dependency)));
    }
#if __GNUC__
private:
    /**
     * Constructor
     */
    explicit DependencyInjectable(bool)
    {
        // add this to work around gcc template deduction bug
    }

public:
#endif
    /**
     * Constructor
     */
    template<typename ... Args,
             typename = std::enable_if_t<(sizeof ... (Args) > 1 && sizeof ... (Dependencies) > 0) &&
             traits::helpers::all_of<
             !std::is_base_of<tDependencies, typename std::decay<Args>::type>::value &&
             !traits::tests::is_specialization_of<typename std::decay<Args>::type, std::tuple>::value &&
             !traits::tests::is_specialization_of<typename std::decay<Args>::type,
             attributes::concrete::DependencyInjectable>::value ...>::value>>
    DependencyInjectable(Args && ... dependencies)
    {
        if (traits::helpers::all_of<assert_tuple_has_type<Dependencies,
                                    std::tuple<Dependencies ...>>::value ...>::value)
            setDependencies(std::forward<Args>(dependencies) ...);
    }

    /**
     * Copy constructor
     */
    DependencyInjectable(const tDependencies &injectable)
    {
        operator = (injectable);
    }

    /**
     * Move constructor
     */
    DependencyInjectable(tDependencies &&injectable)
    {
        operator = (std::move(injectable));
    }

private:

    /**
     * Copy assignment operator
     */
    tDependencies &operator = (const tDependencies &injectable)
    {
        if (&injectable != this)
        {
            m_dependencies = injectable.m_dependencies;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    tDependencies &operator = (tDependencies &&injectable)
    {
        if (&injectable != this)
        {
            m_dependencies = std::move(injectable.m_dependencies);
        }

        return *this;
    }

public:

    /**
     * Destructor
     */
    virtual ~DependencyInjectable(void)
    {

    }

    /**
     * Assign this object's dependencies to the types contained within the input dependency tuple
     */
    template<typename Tuple>
    inline void assignDependencies(Tuple &&tuple)
    {
        auto &&assigner = [this] (auto *&pDependency)
        {
            pDependency = this->getDependency<typename std::decay<decltype(pDependency)>::type>();
        };

        functional::for_each(std::forward<Tuple>(tuple), assigner);
    }

    /**
     * Function to check whether all of this object's dependencies are initialized (if stored as pointers, they
     * are checked for being non-null)
     */
    inline bool dependenciesInitialized(void) const
    {
        return dependenciesInitialized(getDependencies());
    }

    /**
     * Function to check whether the tuple of input dependencies are initialized (if stored as pointers, they
     * are checked for being non-null)
     */
    template<typename Tuple, typename DecayTuple = typename std::decay<Tuple>::type> inline static typename
    std::enable_if<traits::tests::is_specialization_of<DecayTuple, std::tuple>::value ||
                   traits::tests::is_specialization_of<DecayTuple,
                   attributes::concrete::DependencyInjectable>::value, bool>::type
    dependenciesInitialized(Tuple &&tuple)
    {
        auto const is_valid = functional::make_recursive_overloaded_lambda
        (
            [] (auto &self, auto *pArg, auto && ... args) { return pArg != nullptr && self(args ...); },
            [] (auto &self, auto &&arg, auto && ... args) { return self(args ...); },
            [] (auto &self) { return true; }
        );

        return is_valid(tuple);
    }

    /**
     * Retrieve this object's dependencies
     */
    inline tTuple getDependencies(void) const
    {
        return m_dependencies;
    }

    /**
     * Retrieve this object's dependencies
     */
    inline tTuple &getDependencies(void)
    {
        return m_dependencies;
    }

    /**
     * Function to retrieve a dependency
     */
    template<typename T>
    inline auto getDependency(void) const ->
    typename std::enable_if<traits::tests::tuple_has_type<T, tTuple>::value, T>::type
    {
        return std::get<T>(m_dependencies);
    }

    /**
     * Function to retrieve a dependency
     */
    template<typename T, typename Tuple, typename DecayTuple = typename std::decay<Tuple>::type>
    inline static auto getDependency(Tuple &&dependencies) ->
    typename std::enable_if<traits::tests::tuple_has_type<T, tTuple>::value &&
                            traits::tests::is_specialization_of<DecayTuple,
                            attributes::concrete::DependencyInjectable>::value, T>::type
    {
        return std::get<T>(dependencies.getDependencies());
    }

    /**
     * Set this object's dependencies
     */
    template<typename ... Args, std::enable_if_t<(sizeof ... (Args) > 1)> * = nullptr>
    inline void setDependencies(Args && ... dependencies)
    {
        constexpr auto size = std::tuple_size<tTuple>::value;
        static_assert(size > 0, "List of dependencies must be specified.");
        assign_dependencies<size - 1> { } (m_dependencies,
                                           std::forward_as_tuple(std::forward<Args>(dependencies) ...));
    }

    /**
     * Set this object's dependencies
     */
    template<typename Tuple, typename DecayTuple = typename std::decay<Tuple>::type,
             std::enable_if<traits::tests::is_specialization_of<DecayTuple, std::tuple>::value> * = nullptr>
    inline void setDependencies(Tuple &&dependencies)
    {
        constexpr auto size = std::tuple_size<tTuple>::value;
        static_assert(size > 0, "List of dependencies must be specified.");
        assign_dependencies<size - 1> { } (m_dependencies, std::forward<Tuple>(dependencies));
    }

    /**
     * Function to set a dependency
     */
    template<typename T, typename U = typename std::decay<T>::type>
    inline auto setDependency(T &&dependency) ->
    typename std::enable_if<traits::tests::tuple_has_type<U, tTuple>::value, void>::type
    {
        std::get<U>(m_dependencies) = dependency;
    }

private:

    /**
     * this object's dependency tuple
     */
    std::tuple<Dependencies ...> m_dependencies;
};

/**
 * Helper class to convert a tuple to a DependencyInjectable type
 */
template<typename T> struct tuple_as_dependency_injectable_type { };
template<typename ... Args> struct tuple_as_dependency_injectable_type<std::tuple<Args ...>>
{ typedef DependencyInjectable<Args ...> type; };

/**
 * Helper class to allow an injection dependent class to inherit type T's injection dependencies
 */
template<typename T, bool = traits::tests::is_derived_from_template<DependencyInjectable, T>::value>
struct dependencies_of { typedef void type; };
template<typename T> struct dependencies_of<T, true> { typedef T type; };

/**
 * This specialization implements a base collector class and serves to disambiguate calls to functions in the
 * base DependencyInjectable class
 */
template<typename Dependency, typename ... Dependencies>
class DependencyInjectable<dependencies_of<Dependency>, Dependencies ...>
: virtual public Dependency::tDependencyInjectable,
  public tuple_as_dependency_injectable_type<decltype(std::tuple_cat(std::declval<
                                             typename Dependency::tDependencyInjectable::tTuple>(),
                                             std::declval<std::tuple<Dependencies ...>>()))>::type
{
public:

    /**
     * Helper class to build a tuple of virtual base classes
     */
    template<typename Arg, bool = Arg::Depth != 0, typename ... Args>
    struct get_virtual_bases
    {
        /**
         * Typedef declarations
         */
        typedef typename Arg::tBase T;
        typedef typename get_virtual_bases<T, T::Depth != 0,
                                           typename Arg::tDependencyInjectable, Args ...>::type type;
    };

    /**
     * Helper class to build a tuple of virtual base classes
     */
    template<typename Arg, typename ... Args>
    struct get_virtual_bases<Arg, false, Args ...>
    {
        /**
         * Typedef declarations
         */
        typedef std::tuple<typename Arg::tDependencyInjectable, Args ...> type;
    };

    /**
     * Typedef declarations
     */
    typedef decltype(std::tuple_cat(std::declval<typename Dependency::tDependencyInjectable::tTuple>(),
                     std::declval<std::tuple<Dependencies ...>>())) tTuple;
    typedef typename tuple_as_dependency_injectable_type<tTuple>::type tDependencies;
    typedef DependencyInjectable<dependencies_of<Dependency>, Dependencies ...> tDependencyInjectable;
    typedef Dependency tBase;
    static const constexpr auto Depth = std::tuple_size<typename get_virtual_bases<tBase>::type>::value;

private:

    /**
     * Constructor
     */
    DependencyInjectable(void) = delete;

public:

    /**
     * Forwarding constructor
     */
    template<typename Arg,
             typename ... Args,
             typename std::enable_if<!std::is_same<Arg,
                                                   decltype(std::make_index_sequence<Depth>{ })>::value,
                                                   int>::type = 0>
    DependencyInjectable(Arg &&arg,
                         Args && ... args)
    : DependencyInjectable(std::make_index_sequence<Depth>{ },
                           std::forward<Arg>(arg),
                           std::forward<Args>(args) ...)
    {

    }

private:

    /**
     * Forwarding constructor
     */
    template<typename ... Args,
             size_t ... Indices>
    DependencyInjectable(std::index_sequence<Indices ...>,
                         Args && ... args)
    : std::tuple_element<Indices,
                         typename get_virtual_bases<
                         typename Dependency::tDependencyInjectable>::type>::type{
                         std::forward<Args>(args) ...} ...,
      tDependencies(std::forward<Args>(args) ...)
    {

    }

public:

    /**
     * Forwarding assignment operator
     */
    template<typename Arg>
    auto &operator = (Arg &&arg)
    {
        return tDependencies::operator = (std::forward<Arg>(arg));
    }

    /**
     * Destructor
     */
    virtual ~DependencyInjectable(void) override
    {

    }

    /**
     * Using declarations
     */
    using tDependencies::dependenciesInitialized;
    using tDependencies::getDependencies;
    using tDependencies::getDependency;
    using tDependencies::setDependencies;
    using tDependencies::setDependency;
};

}

}

#endif
