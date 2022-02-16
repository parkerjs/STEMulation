#ifndef FACTORY_CONSTRUCTIBLE_H
#define FACTORY_CONSTRUCTIBLE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "any.h"
#include "export_library.h"
#include "function_traits.h"

namespace attributes
{

namespace abstract
{

// forward declarations
EXPORT_STEM functional::Any &
addFactoryMap(const std::type_info &,
              functional::Any);
EXPORT_STEM functional::Any &
getFactoryMap(const std::type_info &);

/**
 * This class provides an abstract interface for types that can be constructed via factories
 */
template<typename T>
class FactoryConstructible
{
public:

    /**
     * Constructor
     */
    FactoryConstructible(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    FactoryConstructible(const FactoryConstructible &constructible) = delete;

    /**
     * Move constructor
     */
    FactoryConstructible(FactoryConstructible &&constructible) = delete;

public:

    /**
     * Destructor
     */
    virtual ~FactoryConstructible(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    FactoryConstructible &
    operator = (const FactoryConstructible &constructible) = delete;

    /**
     * Move assignment operator
     */
    FactoryConstructible &
    operator = (FactoryConstructible &&constructible) = delete;

public:

    /**
     * Base holder container class to store the factory
     */
    class BaseFactory
    {
    public:

        /**
         * Constructor
         */
        BaseFactory(void)
        {

        }

    private:

        /**
         * Copy constructor
         */
        BaseFactory(const BaseFactory &factory) = delete;

        /**
         * Move constructor
         */
        BaseFactory(BaseFactory &&factory) = delete;

    public:

        /**
         * Destructor
         */
        virtual ~BaseFactory(void)
        {

        }

    private:

        /**
         * Copy assignment operator
         */
        BaseFactory &
        operator = (const BaseFactory &factory) = delete;

        /**
         * Move assignment operator
         */
        BaseFactory &
        operator = (BaseFactory &&factory) = delete;

    public:

        /**
         * create() function
         */
        EXPORT_STEM virtual T *
        create(std::vector<functional::Any> &arguments) = 0;
    };

    /**
     * create() function
     * @param name the class to be instantiated
     * @param args a variadic list of arguments that are forwarded to the factory
     */
    template<typename ... Args>
    static T *
    create(const std::string &name,
           Args && ... args)
    {
        T *pInstance = nullptr;
        auto &&factoryMap = getFactoryMap();
        auto &&itNameFactoryPair = factoryMap.find(name);
        if (itNameFactoryPair != factoryMap.cend())
        {
            auto pFactory = itNameFactoryPair->second;
            if (pFactory != nullptr)
            {
                std::vector<functional::Any> arguments = { functional::Any(std::ref(args)) ... };

                pInstance = pFactory->create(arguments);
            }
        }

        return pInstance;
    }

    /**
     * Get a reference to the factory map
     */
    inline static std::map<std::string,
                           std::shared_ptr<BaseFactory>> &
    getFactoryMap(void)
    {
        typedef std::map<std::string,
                         std::shared_ptr<BaseFactory>> tFactoryMap;
        static auto &&factoryMap = addFactoryMap(typeid (T),
                                                 functional::Any(tFactoryMap()));

        return functional::any_cast<tFactoryMap &>(factoryMap);
    }

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string
    getFactoryName(void) const = 0;

    /**
     * Function to add a factory to the registry
     */
    template<typename Return,
             typename ... Messages>
    inline static void
    registerFactory(const std::string &name,
                    Return (*pFunction)(Messages ...))
    {
        auto &&factoryMap = getFactoryMap();
        auto &&itNameFactoryPair = factoryMap.find(name);
        if (itNameFactoryPair != factoryMap.cend())
        {
            removeFactory(name);
        }

        factoryMap.emplace(name,
                           new Factory<Return (*)(Messages ...)>(pFunction));
    }

    /**
     * Function to add a factory to the registry
     */
    template<typename Return,
             typename ... Messages>
    inline static void
    registerFactory(const std::string &name,
                    const std::function<Return(Messages ...)> &function)
    {
        auto &&factoryMap = getFactoryMap();
        auto &&itNameFactoryPair = factoryMap.find(name);
        if (itNameFactoryPair != factoryMap.cend())
        {
            removeFactory(name);
        }

        factoryMap.emplace(name,
                           new Factory<std::function<Return(Messages ...)>>(function));
    }

    /**
     * Function to remove a factory from the registry
     */
    inline static bool
    removeFactory(const std::string &name)
    {
        auto &&factoryMap = getFactoryMap();
        auto &&itNameFactoryPair = factoryMap.find(name);
        bool bSuccess = (itNameFactoryPair != factoryMap.cend());
        if (bSuccess)
        {
            factoryMap.erase(itNameFactoryPair);
        }

        return bSuccess;
    }

private:

    /**
     * Holder container class to store the factory
     */
    template<typename Function>
    class Factory
    : public BaseFactory
    {
    public:

        /**
         * Constructor
         */
        template<typename Return,
                 typename ... Messages>
        Factory(Return(*pFunction)(Messages ...))
        : m_factory(pFunction)
        {

        }

        /**
         * Constructor
         */
        template<typename Return,
                 typename ... Messages>
        Factory(const std::function<Return(Messages ...)> &function)
        : m_factory(function)
        {

        }

    private:

        /**
         * Copy constructor
         */
        Factory(const Factory &factory) = delete;

        /**
         * Move constructor
         */
        Factory(Factory &&factory) = delete;

    public:

        /**
         * Destructor
         */
        virtual ~Factory(void) override
        {

        }

    private:

        /**
         * Copy assignment operator
         */
        Factory &
        operator = (const Factory &factory) = delete;

        /**
         * Move assignment operator
         */
        Factory &
        operator = (Factory &&factory) = delete;

        /**
         * create() function
         */
        inline virtual T *
        create(std::vector<functional::Any> &arguments) override
        {
            constexpr auto arity = traits::helpers::function_traits<Function>::arity;

            return invoke<arity>(std::forward<Function>(m_factory),
                                 arguments);
        }

        /**
         * Invoke the receiving function and expand the vector of messages into an argument list
         */
        template<std::size_t ... indices>
        inline T *
        invoke(Function &&factory,
               std::vector<functional::Any> &arguments,
               std::index_sequence<indices ...>)
        {
            T *pInstance = nullptr;
            if (factory != nullptr)
            {
                pInstance = factory(arguments[indices] ...);
            }

            return pInstance;
        }

        /**
         * Invoke the receiving function and expand the vector of messages into an argument list
         */
        template<std::size_t Arity>
        inline T *
        invoke(Function &&function,
               std::vector<functional::Any> &arguments)
        {
            if (arguments.size() < Arity)
            {
                throw "Incorrect number of arguments...";
            }

            return invoke(std::forward<Function>(function),
                          arguments,
                          std::make_index_sequence<Arity> { });
        }

        /**
         * member instance of the factory
         */
        Function m_factory;
    };
};

/**
 * This class serves as a helper class to facilitate object factory registration
 */
template<typename ... T>
struct FactoryRegistrar
{
    /**
     * Constructor
     */
    template<typename Factory>
    FactoryRegistrar(const std::string &name,
                     Factory &&factory)
    {
        int dummy[] = { 0,
                        ((void) FactoryConstructible<T>::registerFactory(name,
                                                                         std::forward<Factory>(factory)),
                         0) ... };

        // silence unused variable warnings...
        (void) (dummy);
    }
};

}

}

#endif
