#ifndef ROTATION_TYPE_H
#define ROTATION_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace math
{

namespace geometric
{

namespace orientation
{

/**
 * Encapsulated enumeration for classifying rotations as active or passive
 */
struct RotationType final
: public attributes::abstract::Enumerable<RotationType>
{
    /**
     * Enumerations
     */
    enum Enum { Active, Passive, Unknown };

    /**
     * Constructor
     */
    RotationType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    RotationType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    RotationType(const RotationType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    RotationType(RotationType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~RotationType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    RotationType &operator = (const RotationType &type)
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
    RotationType &operator = (RotationType &&type)
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
    virtual RotationType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "active", Enum::Active },
          { "passive", Enum::Passive },
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
            case Enum::Active: return "Active";
            case Enum::Passive: return "Passive";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Active, Enum::Passive };
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
