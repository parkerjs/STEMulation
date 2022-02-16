#ifndef COORDINATE_TYPE_H
#define COORDINATE_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent the type of coordinate system (Cartesian, Spherical, etc.)
 */
struct CoordinateType final
: public attributes::abstract::Enumerable<CoordinateType>
{
    /**
     * Enumerations
     */
    enum Enum { Cartesian, Spherical, Unknown };

    /**
     * Constructor
     */
    CoordinateType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    CoordinateType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    CoordinateType(const CoordinateType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    CoordinateType(CoordinateType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~CoordinateType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    CoordinateType &operator = (const CoordinateType &type)
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
    CoordinateType &operator = (CoordinateType &&type)
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
    virtual CoordinateType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "cartesian", Enum::Cartesian },
          { "spherical", Enum::Spherical },
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
            case Enum::Cartesian: return "Cartesian";
            case Enum::Spherical: return "Spherical";
            default: return "Unknown";
        }
    }

    /**
     * Named constructor for Cartesian CoordinateType
     */
    inline static CoordinateType cartesian(void)
    {
        return Enum::Cartesian;
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Cartesian, Enum::Spherical };
    }

    /**
     * Named constructor for Spherical CoordinateType
     */
    inline static CoordinateType spherical(void)
    {
        return Enum::Spherical;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
