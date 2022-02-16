#ifndef SPHERICAL_CONVERSION_TYPE_H
#define SPHERICAL_CONVERSION_TYPE_H

#include "enumerable.h"
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent the conversion function used to take spherical zenith to spherical
 * elevation and vice versa
 */
struct SphericalConversionType final
: public attributes::abstract::Enumerable<SphericalConversionType>
{
    /**
     * Enumerations
     */
    enum Enum { ElevationToZenith,
                NegativeElevationToZenith,
                ZenithToElevation,
                ZenithToNegativeElevation,
                Unknown };

    /**
     * Constructor
     */
    SphericalConversionType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    SphericalConversionType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    SphericalConversionType(const SphericalConversionType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    SphericalConversionType(SphericalConversionType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~SphericalConversionType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    SphericalConversionType &operator = (const SphericalConversionType &type)
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
    SphericalConversionType &operator = (SphericalConversionType &&type)
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
    virtual SphericalConversionType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "elevationtozenith", Enum::ElevationToZenith },
          { "negativeelevationtozenith", Enum::NegativeElevationToZenith },
          { "zenithtoelevation", Enum::ZenithToElevation },
          { "zenithtonegativeelevation", Enum::ZenithToNegativeElevation }
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
            case Enum::ElevationToZenith: return "elevationToZenith";
            case Enum::NegativeElevationToZenith: return "negativeElevationToZenith";
            case Enum::ZenithToElevation: return "zenithToElevation";
            case Enum::ZenithToNegativeElevation: return "zenithToNegativeElevation";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::ElevationToZenith,
                 Enum::NegativeElevationToZenith,
                 Enum::ZenithToElevation,
                 Enum::ZenithToNegativeElevation };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
