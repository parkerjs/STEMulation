#ifndef TIME_REFERENCE_TYPE_H
#define TIME_REFERENCE_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent time with respect to zero or as some delta
 */
struct TimeReferenceType final
: public attributes::abstract::Enumerable<TimeReferenceType>
{
    /**
     * Enumerations
     */
    enum Enum { Absolute, Delta, Unknown };

    /**
     * Constructor
     */
    TimeReferenceType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    TimeReferenceType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    TimeReferenceType(const TimeReferenceType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    TimeReferenceType(TimeReferenceType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~TimeReferenceType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    TimeReferenceType &operator = (const TimeReferenceType &type)
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
    TimeReferenceType &operator = (TimeReferenceType &&type)
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
    virtual TimeReferenceType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "absolute", Enum::Absolute },
          { "delta", Enum::Delta },
          { "unknown", Enum::Unknown }
        };

        std::transform(type.cbegin(), type.cend(), type.begin(), ::tolower);
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
            case Enum::Absolute: return "Absolute";
            case Enum::Delta: return "Delta";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Absolute, Enum::Delta };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
