#ifndef EULER_ACCELERATION_AXIS_TYPE_H
#define EULER_ACCELERATION_AXIS_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration for representing the roll, pitch, or yaw Euler acceleration axis
 */
struct EulerAccelerationAxisType final
: public attributes::abstract::Enumerable<EulerAccelerationAxisType>
{
    /**
     * Enumerations
     */
    enum Enum { Roll, Pitch, Yaw, Unknown };

    /**
     * Constructor
     */
    EulerAccelerationAxisType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    EulerAccelerationAxisType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    EulerAccelerationAxisType(const EulerAccelerationAxisType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    EulerAccelerationAxisType(EulerAccelerationAxisType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~EulerAccelerationAxisType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    EulerAccelerationAxisType &operator = (const EulerAccelerationAxisType &type)
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
    EulerAccelerationAxisType &operator = (EulerAccelerationAxisType &&type)
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
    virtual EulerAccelerationAxisType &operator = (std::string type) override
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
     * Named constructor for Pitch EulerAccelerationAxisType
     */
    inline static EulerAccelerationAxisType pitch(void)
    {
        return Enum::Pitch;
    }

    /**
     * Named constructor for Roll EulerAccelerationAxisType
     */
    inline static EulerAccelerationAxisType roll(void)
    {
        return Enum::Roll;
    }

    /**
     * Named constructor for Yaw EulerAccelerationAxisType
     */
    inline static EulerAccelerationAxisType yaw(void)
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

#endif
