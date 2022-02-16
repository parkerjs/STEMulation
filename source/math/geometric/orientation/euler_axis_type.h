#ifndef EULER_AXIS_TYPE_H
#define EULER_AXIS_TYPE_H

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
 * Encapsulated enumeration for representing the roll, pitch, or yaw Euler axis
 */
struct EulerAxisType final
: public attributes::abstract::Enumerable<EulerAxisType>
{
    /**
     * Enumerations
     */
    enum Enum { Roll, Pitch, Yaw, Unknown };

    /**
     * Constructor
     */
    EulerAxisType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    EulerAxisType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    EulerAxisType(const EulerAxisType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    EulerAxisType(EulerAxisType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~EulerAxisType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    EulerAxisType &operator = (const EulerAxisType &type)
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
    EulerAxisType &operator = (EulerAxisType &&type)
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
    virtual EulerAxisType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "pitch", Enum::Pitch },
          { "roll", Enum::Roll },
          { "yaw", Enum::Yaw },
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
            case Enum::Pitch: return "Pitch";
            case Enum::Roll: return "Roll";
            case Enum::Yaw: return "Yaw";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Pitch, Enum::Roll, Enum::Yaw };
    }

    /**
     * Named constructor for Pitch EulerAxisType
     */
    inline static EulerAxisType pitch(void)
    {
        return Enum::Pitch;
    }

    /**
     * Named constructor for Roll EulerAxisType
     */
    inline static EulerAxisType roll(void)
    {
        return Enum::Roll;
    }

    /**
     * Named constructor for Yaw EulerAxisType
     */
    inline static EulerAxisType yaw(void)
    {
        return Enum::Yaw;
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
