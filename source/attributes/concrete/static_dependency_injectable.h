#ifndef STATIC_DEPENDENCY_INJECTABLE_H
#define STATIC_DEPENDENCY_INJECTABLE_H

#include "dependency_injectable.h"
#include "tuple_for_each.h"

namespace attributes
{

namespace concrete
{

/**
 * StaticDependency : This class implements a container class to store a static dependency
 */
template<typename T>
class StaticDependency final
{
private:

    /**
     * Friend declarations
     */
    template<typename Tuple> friend void initializeStaticDependencies(Tuple &&);
    template<typename ...> friend class StaticDependencyInjectable;

    /**
     * Function to retrieve a dependency
     */
    inline static T &getDependency(void)
    {
        static T m_dependency;

        return m_dependency;
    }

    /**
     * Function to set a dependency
     */
    template<typename Dependency>
    inline static void setDependency(Dependency &&dependency)
    {
        getDependency() = std::forward<Dependency>(dependency);
    }
};

/**
 * Function to assign static dependencies to the types contained within the input dependency tuple
 */
template<typename Tuple>
inline static void initializeStaticDependencies(Tuple &&tuple)
{
    auto &&assigner = [] (auto &&dependency)
    {
        static StaticDependency<typename std::decay<decltype(dependency)>::type> staticDependency;

        staticDependency.setDependency(std::forward<decltype(dependency)>(dependency));
    };

    functional::for_each(tuple, assigner);
}

/**
 * This class provides a concrete, static interface for types that require dependency injection
 */
template<typename ... Dependencies>
class StaticDependencyInjectable
{
public:

    /**
     * Constructor
     */
    StaticDependencyInjectable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~StaticDependencyInjectable(void)
    {

    }

    /**
     * Retrieve this object's dependencies
     */
    inline static auto &getStaticDependencies(void)
    {
        return getStaticDependencyInjectable().getDependencies();
    }

    /**
     * Function to retrieve a dependency
     */
    template<typename Dependency>
    inline static auto getStaticDependency(void)
    {
        return getStaticDependencyInjectable().template getDependency<Dependency>();
    }

private:

    /**
     * Retrieve this object's dependencies
     */
    inline static auto &getStaticDependencyInjectable(void)
    {
        auto &&initializer = [] (void)
        {
            auto &&assigner = [] (auto &&dependency)
            {
                static StaticDependency<typename std::decay<decltype(dependency)>::type> staticDependency;

                dependency = staticDependency.getDependency();
            };

            std::tuple<Dependencies ...> tuple;
            functional::for_each(tuple, assigner);

            return tuple;
        };

        static DependencyInjectable<Dependencies ...> dependencies(initializer());

        return dependencies;
    }

public:

    /**
     * Set this object's dependencies
     */
    template<typename ... Args>
    inline static void setStaticDependencies(Args && ... args)
    {
        getStaticDependencyInjectable().setDependencies(std::forward<Args>(args) ...);
    }

    /**
     * Function to set a dependency
     */
    template<typename Dependency>
    inline static void setStaticDependency(Dependency &&dependency)
    {
        getStaticDependencyInjectable().setDependency(std::forward<Dependency>(dependency));
    }

    /**
     * Function to check whether all of this object's dependencies are initialized (if stored as pointers, they
     * are checked for being non-null)
     */
    inline static bool staticDependenciesInitialized(void)
    {
        return getStaticDependencyInjectable().dependenciesInitialized();
    }

    /**
     * Function to check whether the tuple of input dependencies are initialized (if stored as pointers, they
     * are checked for being non-null)
     */
    template<typename Tuple>
    inline static auto staticDependenciesInitialized(Tuple &&dependencies)
    {
        return getStaticDependencyInjectable().dependenciesInitialized(std::forward<Tuple>(dependencies));
    }
};

}

}

#endif
