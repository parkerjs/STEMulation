#ifndef PLACE_VALUE_TYPE_H
#define PLACE_VALUE_TYPE_H

#include "decimal_place_value_type.h"
#include "whole_place_value_type.h"

namespace math
{

namespace measurement
{

/**
 * Encapsulated enumeration for representing commonly used place values in powers of ten (thousandths,
 * hundredths, tenths, ones, tens, hundreds, thousands, etc.)
 */
class PlaceValueType final
: public attributes::abstract::Enumerable<PlaceValueType>
{
public:

    /**
     * Constructor
     */
    PlaceValueType(const std::string &type = "Unknown")
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    PlaceValueType(const DecimalPlaceValueType::Enum &type)
    : m_decimalPlaceValueType(type)
    {

    }

    /**
     * Constructor
     */
    PlaceValueType(const WholePlaceValueType::Enum &type)
    : m_wholePlaceValueType(type)
    {

    }

    /**
     * Copy constructor
     */
    PlaceValueType(const PlaceValueType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    PlaceValueType(PlaceValueType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~PlaceValueType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    PlaceValueType &operator = (const PlaceValueType &type)
    {
        if (&type != this)
        {
            m_decimalPlaceValueType = type.m_decimalPlaceValueType;
            m_wholePlaceValueType = type.m_wholePlaceValueType;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    PlaceValueType &operator = (PlaceValueType &&type)
    {
        if (&type != this)
        {
            m_decimalPlaceValueType = std::move(type.m_decimalPlaceValueType);
            m_wholePlaceValueType = std::move(type.m_wholePlaceValueType);
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    virtual PlaceValueType &operator = (std::string type) override
    {
        m_decimalPlaceValueType = type;
        m_wholePlaceValueType = type;

        return *this;
    }

    /**
     * Conversion to enumeration operator
     */
    inline operator DecimalPlaceValueType::Enum (void) const
    {
        return m_decimalPlaceValueType;
    }

    /**
     * Conversion to enumeration operator
     */
    inline operator WholePlaceValueType::Enum (void) const
    {
        return m_wholePlaceValueType;
    }

    /**
     * Conversion from PlaceValueType to integral types (returns 0 for unknown types)
     */
    template<typename T,
             typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value, int>::type = 0>
    inline operator T (void) const
    {
        return m_wholePlaceValueType;
    }

    /**
     * Conversion from PlaceValueType to floating point types (returns 0 for unknown types)
     */
    template<typename T,
             typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value, int>::type = 0>
    inline operator T (void) const
    {
        return m_decimalPlaceValueType;
    }

    /**
     * Conversion to std::string operator
     */
    virtual operator std::string (void) const override
    {
        std::string decimalValue(m_decimalPlaceValueType);
        if (decimalValue != "Unknown")
            return decimalValue;
        else
            return static_cast<std::string>(m_wholePlaceValueType);
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    template<typename T,
             typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value, int>::type = 0>
    inline static auto enumerations(void)
    {
        return WholePlaceValueType::enumerations();
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    template<typename T,
             typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value, int>::type = 0>
    inline static auto enumerations(void)
    {
        return DecimalPlaceValueType::enumerations();
    }

    /**
     * an instance of DecimalPlaceValueType
     */
    DecimalPlaceValueType m_decimalPlaceValueType;

    /**
     * an instance of WholePlaceValueType
     */
    WholePlaceValueType m_wholePlaceValueType;
};

}

}

#endif
