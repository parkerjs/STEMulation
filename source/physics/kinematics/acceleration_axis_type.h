#ifndef ACCELERATION_AXIS_TYPE_H
#define ACCELERATION_AXIS_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent the x, y, or z acceleration axis in a Cartesian coordinate system
 */
struct AccelerationAxisType final
: public attributes::abstract::Enumerable<AccelerationAxisType>
{
    /**
     * Enumerations
     */
    enum Enum { X, Y, Z, Unknown };

    /**
     * Constructor
     */
    AccelerationAxisType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    AccelerationAxisType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    AccelerationAxisType(const AccelerationAxisType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    AccelerationAxisType(AccelerationAxisType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~AccelerationAxisType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    AccelerationAxisType &operator = (const AccelerationAxisType &type)
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
    AccelerationAxisType &operator = (AccelerationAxisType &&type)
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
    virtual AccelerationAxisType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "x", Enum::X },
          { "y", Enum::Y },
          { "z", Enum::Z },
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
            case Enum::X: return "x";
            case Enum::Y: return "y";
            case Enum::Z: return "z";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::X, Enum::Y, Enum::Z };
    }

    /**
     * Named constructor for X AccelerationAxisType
     */
    inline static AccelerationAxisType x(void)
    {
        return Enum::X;
    }

    /**
     * Named constructor for Y AccelerationAxisType
     */
    inline static AccelerationAxisType y(void)
    {
        return Enum::Y;
    }

    /**
     * Named constructor for Z AccelerationAxisType
     */
    inline static AccelerationAxisType z(void)
    {
        return Enum::Z;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
