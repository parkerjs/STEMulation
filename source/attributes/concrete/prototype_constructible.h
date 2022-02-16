#ifndef PROTOTYPE_CONSTRUCTIBLE_H
#define PROTOTYPE_CONSTRUCTIBLE_H

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that can be constructed via an existing instance of the
 * same type
 */
template<typename T>
class PrototypeConstructible
{
public:

    /**
     * Constructor
     */
    PrototypeConstructible(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    PrototypeConstructible(const PrototypeConstructible &constructible) = delete;

    /**
     * Move constructor
     */
    PrototypeConstructible(PrototypeConstructible &&constructible) = delete;

public:

    /**
     * Destructor
     */
    virtual ~PrototypeConstructible(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    PrototypeConstructible &operator = (const PrototypeConstructible &constructible) = delete;

    /**
     * Move assignment operator
     */
    PrototypeConstructible &operator = (PrototypeConstructible &&constructible) = delete;

public:

    /**
     * Clone a prototype
     * @param name the associated prototype
     */
    inline static T *clone(const std::string &name)
    {
        T *pClone = nullptr;
        auto &&prototypeMap = getPrototypeMap();
        auto &&itNamePrototypePair = prototypeMap.find(name);
        if (itNamePrototypePair != prototypeMap.cend())
        {
            auto *pPrototype = itNamePrototypePair->second;
            if (pPrototype != nullptr)
                pClone = pPrototype->clone();
        }

        return pClone;
    }

    /**
     * Check whether or not the registry contains the specified prototype
     * @param name the associated prototype
     */
    inline static bool containsPrototype(const std::string &name)
    {
        auto &&prototypeMap = getPrototypeMap();
        auto &&itNamePrototypePair = prototypeMap.find(name);

        return itNamePrototypePair != prototypeMap.cend();
    }

    /**
     * Get a reference to the map of prototypes
     */
    inline static std::map<std::string, T *> &getPrototypeMap(void)
    {
        static std::map<std::string, T *> prototypeMap; // map of prototypes

        return prototypeMap;
    }

    /**
     * Function to add a prototype to the registry
     * @param name       the associated prototype
     * @param pPrototype an r-value reference pointer to the prototype to be added
     */
    template<typename Prototype>
    inline static void registerPrototype(const std::string &name, Prototype *&&pPrototype)
    {
        auto &&prototypeMap = getPrototypeMap();
        auto &&itNamePrototypePair = prototypeMap.find(name);
        if (itNamePrototypePair != prototypeMap.cend())
            removePrototype(name);

        prototypeMap.emplace(name, pPrototype);
    }

    /**
     * Function to remove a prototype from the registry
     * @param name the associated prototype
     */
    inline static bool removePrototype(const std::string &name)
    {
        auto &&prototypeMap = getPrototypeMap();
        auto &&itNamePrototypePair = prototypeMap.find(name);
        bool bSuccess = (itNamePrototypePair != prototypeMap.cend());
        if (bSuccess)
        {
            auto *pPrototype = itNamePrototypePair->second;
            bSuccess = (pPrototype != nullptr);
            if (bSuccess)
                delete pPrototype;

            prototypeMap.erase(itNamePrototypePair);
        }

        return bSuccess;
    }
};

}

}

#endif
