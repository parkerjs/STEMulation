#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>
#include <set>

#include "reflective.h"

#if defined _MSC_VER && \
    defined _WIN32
  #pragma warning (disable : 4250) // disables the 'inherits via dominance' warning
#endif

namespace attributes
{

namespace abstract
{

// forward declarations
template<typename>
class Singleton;

/**
 * This class provides an abstract interface for types that implement the singleton pattern
 */
template<>
class Singleton<void>
{
public:

    /**
     * Constructor
     */
    Singleton(void)
    {
        getSingletons().insert(this);
    }

private:

    /**
     * Copy constructor
     */
    Singleton(const Singleton<void> &singleton) = delete;

    /**
     * Move constructor
     */
    Singleton(Singleton<void> &&singleton) = delete;

public:

    /**
     * Destructor
     */
    virtual ~Singleton(void)
    {
        getSingletons().erase(this);
    }

private:

    /**
     * Copy assignment operator
     */
    Singleton<void> &
    operator = (const Singleton<void> &singleton) = delete;

    /**
     * Move assignment operator
     */
    Singleton<void> &
    operator = (Singleton<void> &&singleton) = delete;

public:

    /**
     * Delete all singletons
     */
    inline static void
    deleteAll(void)
    {
        while (!getSingletons().empty())
        {
            auto *pSingleton = *getSingletons().begin();
            if (pSingleton != nullptr)
            {
                delete pSingleton;
            }
        }
    }

protected:

    /**
     * Get a set of instantiated singletons
     */
    inline static std::set<Singleton<void> *> &
    getSingletons(void)
    {
        static std::set<Singleton<void> *> singletons;

        return singletons;
    }
};

/**
 * This class provides an abstract interface for types that implement the singleton pattern
 */
template<typename T>
class Singleton
: virtual private abstract::Reflective,
  public Singleton<void>
{
protected:

    /**
     * Constructor
     */
    Singleton(void)
    {
        m_pInstance = dynamic_cast<T *>(this);
    }

    /**
     * Copy constructor
     */
    Singleton(const Singleton<T> &singleton) = delete;

    /**
     * Move constructor
     */
    Singleton(Singleton<T> &&singleton) = delete;

    /**
     * Destructor
     */
    virtual ~Singleton(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Singleton<T> &
    operator = (const Singleton<T> &singleton) = delete;

    /**
     * Move assignment operator
     */
    Singleton<T> &
    operator = (Singleton<T> &&singleton) = delete;

public:

    /**
     * Delete instance (concrete types)
     */
    inline static bool
    deleteInstance(void)
    {
        bool bSuccess = (m_pInstance != nullptr);
        if (bSuccess)
        {
            std::cout << "Warning from " + getQualifiedStaticMethodName(__func__,
                                                                        Singleton<T>)
                                         + ": "
                      << "Deleting instance of "
                      << m_pInstance->getName()
                      << "."
                      << std::endl;
            delete m_pInstance;
            m_pInstance = nullptr;
        }

        return bSuccess;
    }

    /**
     * Get an instance of this singleton (for abstract types)
     */
    template<typename U = T>
    inline static typename std::enable_if<std::is_abstract<U>::value,
                                          U *>::type

    getInstance(void)
    {
        return nullptr;
    }

    /**
     * Get the name assigned to this object
     */
    EXPORT_STEM virtual std::string
    getName(void) const = 0;

protected:

    /**
     * a static instance of type T
     */
    static T *m_pInstance;
};

/**
 * Initialization of static member variables
 */
template<typename T>
T *
Singleton<T>::m_pInstance = T::getInstance();

}

}

#endif
