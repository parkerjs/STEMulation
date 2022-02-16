#ifndef RADAR_MEASUREMENT_TYPE_H
#define RADAR_MEASUREMENT_TYPE_H

#include "enumerable.h"
#include <algorithm>

/**
 * Encapsulated enumeration representing commonly measured radar quantities
 */
struct RadarMeasurementType final
: public attributes::abstract::Enumerable<RadarMeasurementType>
{
    /**
     * Enumerations
     */
    enum class Enum { Azimuth,
                      Elevation,
                      Range,
                      RangeRate,
                      Zenith,
                      Unknown };

    /**
     * Constructor
     */
    RadarMeasurementType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    RadarMeasurementType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    RadarMeasurementType(const RadarMeasurementType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    RadarMeasurementType(RadarMeasurementType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~RadarMeasurementType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    RadarMeasurementType &operator = (const RadarMeasurementType &type)
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
    RadarMeasurementType &operator = (RadarMeasurementType &&type)
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
    virtual RadarMeasurementType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "azimuth", Enum::Azimuth },
          { "elevation", Enum::Elevation },
          { "range", Enum::Range },
          { "rangerate", Enum::RangeRate },
          { "zenith", Enum::Zenith }
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
            case Enum::Azimuth: return "Azimuth";
            case Enum::Elevation: return "Elevation";
            case Enum::Range: return "Range";
            case Enum::RangeRate: return "RangeRate";
            case Enum::Zenith: return "Zenith";
            default: return "unknown";
        }
    }

    /**
     * Named constructor for azimuth RadarMeasurementType
     */
    inline static RadarMeasurementType azimuth(void)
    {
        return Enum::Azimuth;
    }

    /**
     * Named constructor for elevation RadarMeasurementType
     */
    inline static RadarMeasurementType elevation(void)
    {
        return Enum::Elevation;
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Azimuth,
                 Enum::Elevation,
                 Enum::Range,
                 Enum::RangeRate,
                 Enum::Zenith };
    }

    /**
     * Named constructor for range RadarMeasurementType
     */
    inline static RadarMeasurementType range(void)
    {
        return Enum::Range;
    }

    /**
     * Named constructor for range rate RadarMeasurementType
     */
    inline static RadarMeasurementType rangeRate(void)
    {
        return Enum::RangeRate;
    }

    /**
     * Named constructor for zenith RadarMeasurementType
     */
    inline static RadarMeasurementType zenith(void)
    {
        return Enum::Zenith;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

#endif
