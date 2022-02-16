#ifndef FINITE_DIFFERENCE_SCHEME_H
#define FINITE_DIFFERENCE_SCHEME_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace math
{

namespace differentiators
{

namespace sequence
{

/**
 * Encapsulated enumeration to represent finite difference schemes
 */
struct FiniteDifferenceScheme final
: public attributes::abstract::Enumerable<FiniteDifferenceScheme>
{
    /**
     * Enumerations
     */
    enum Enum { Arbitrary, Backward, Central, Forward, Unknown };

    /**
     * Constructor
     */
    FiniteDifferenceScheme(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    FiniteDifferenceScheme(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    FiniteDifferenceScheme(const FiniteDifferenceScheme &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    FiniteDifferenceScheme(FiniteDifferenceScheme &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~FiniteDifferenceScheme(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    FiniteDifferenceScheme &operator = (const FiniteDifferenceScheme &type)
    {
        if (&type != this)
        {
            m_type = type.m_type;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    FiniteDifferenceScheme &operator = (FiniteDifferenceScheme &&type)
    {
        if (&type != this)
        {
            m_type = std::move(type.m_type);
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    virtual FiniteDifferenceScheme &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "Arbitrary", Enum::Arbitrary },
          { "Backward", Enum::Backward },
          { "Central", Enum::Central },
          { "Forward", Enum::Forward },
          { "unknown", Enum::Unknown }
        };

        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        auto &&itType = typeMap.find(type);
        m_type = (itType != typeMap.cend()) ? itType->second : Enum::Unknown;

        return *this;
    }

    /**
     * Conversion to enumeration operator
     */
    inline virtual operator Enum (void) const final
    {
        return m_type;
    }

    /**
     * Conversion to std::string operator
     */
    virtual operator std::string (void) const override
    {
        switch (m_type)
        {
            case Enum::Arbitrary: return "Arbitrary";
            case Enum::Backward: return "Backward";
            case Enum::Central: return "Central";
            case Enum::Forward: return "Forward";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Arbitrary,
                 Enum::Backward,
                 Enum::Central,
                 Enum::Forward };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

}

#endif
