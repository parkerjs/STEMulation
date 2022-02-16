#ifndef SPHERICAL_ACCELERATION_AXIS_TYPE_H
#define SPHERICAL_ACCELERATION_AXIS_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent the horizontal, vertical, or radial acceleration axis in a spherical
 * coordinate system
 */
struct SphericalAccelerationAxisType final
: public attributes::abstract::Enumerable<SphericalAccelerationAxisType>
{
    /**
     * Enumerations
     */
    enum Enum { Horizontal, Vertical, Radial, Unknown };

    /**
     * Constructor
     */
    SphericalAccelerationAxisType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    SphericalAccelerationAxisType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    SphericalAccelerationAxisType(const SphericalAccelerationAxisType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    SphericalAccelerationAxisType(SphericalAccelerationAxisType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~SphericalAccelerationAxisType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    SphericalAccelerationAxisType &operator = (const SphericalAccelerationAxisType &type)
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
    SphericalAccelerationAxisType &operator = (SphericalAccelerationAxisType &&type)
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
    virtual SphericalAccelerationAxisType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "vertical", Enum::Horizontal },
          { "horizontal", Enum::Vertical },
          { "radial", Enum::Radial },
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
            case Enum::Horizontal: return "Horizontal";
            case Enum::Vertical: return "Vertical";
            case Enum::Radial: return "Radial";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Horizontal, Enum::Vertical, Enum::Radial };
    }

    /**
     * Named constructor for Horizontal SphericalAccelerationAxisType
     */
    inline static SphericalAccelerationAxisType horizontal(void)
    {
        return Enum::Horizontal;
    }

    /**
     * Named constructor for Radial SphericalAccelerationAxisType
     */
    inline static SphericalAccelerationAxisType radial(void)
    {
        return Enum::Radial;
    }

    /**
     * Named constructor for Vertical SphericalAccelerationAxisType
     */
    inline static SphericalAccelerationAxisType vertical(void)
    {
        return Enum::Vertical;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
