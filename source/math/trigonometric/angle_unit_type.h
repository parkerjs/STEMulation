#ifndef ANGLE_UNIT_TYPE_H
#define ANGLE_UNIT_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace math
{

namespace trigonometric
{

/**
 * Encapsulated enumeration for describing the units in which angles are represented
 */
struct AngleUnitType final
: public attributes::abstract::Enumerable<AngleUnitType>
{
    /**
     * Enumerations
     */
    enum Enum { Degrees, Radians, Unknown };

    /**
     * Constructor
     */
    AngleUnitType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    AngleUnitType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    AngleUnitType(const AngleUnitType &angleUnits)
    {
        operator = (angleUnits);
    }

    /**
     * Move constructor
     */
    AngleUnitType(AngleUnitType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~AngleUnitType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    AngleUnitType &operator = (const AngleUnitType &angleUnits)
    {
        if (&angleUnits != this)
        {
            m_type = angleUnits.m_type;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    AngleUnitType &operator = (AngleUnitType &&type)
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
    virtual AngleUnitType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "degrees", Enum::Degrees },
          { "radians", Enum::Radians },
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
            case Enum::Degrees: return "Degrees";
            case Enum::Radians: return "Radians";
            default: return "Unknown";
        }
    }

    /**
     * Named constructor for Degrees AngleUnitType
     */
    inline static AngleUnitType degrees(void)
    {
        return Enum::Degrees;
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Degrees, Enum::Radians };
    }

    /**
     * Named constructor for Radians AngleUnitType
     */
    inline static AngleUnitType radians(void)
    {
        return Enum::Radians;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
