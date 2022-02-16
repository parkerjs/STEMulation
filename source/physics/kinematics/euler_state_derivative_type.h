#ifndef EULER_STATE_DERIVATIVE_TYPE_H
#define EULER_STATE_DERIVATIVE_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent Euler angles, rates, or accelerations
 */
struct EulerStateDerivativeType final
: public attributes::abstract::Enumerable<EulerStateDerivativeType>
{
    /**
     * Enumerations
     */
    enum Enum { Accelerations = 8, Eulers = 16, Rates = 32, Unknown };

    /**
     * Constructor
     */
    EulerStateDerivativeType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    EulerStateDerivativeType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    EulerStateDerivativeType(const EulerStateDerivativeType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    EulerStateDerivativeType(EulerStateDerivativeType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~EulerStateDerivativeType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    EulerStateDerivativeType &operator = (const EulerStateDerivativeType &type)
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
    EulerStateDerivativeType &operator = (EulerStateDerivativeType &&type)
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
    virtual EulerStateDerivativeType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "accelerations", Enum::Accelerations },
          { "eulers", Enum::Eulers },
          { "rates", Enum::Rates },
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
            case Enum::Accelerations: return "Accelerations";
            case Enum::Eulers: return "Eulers";
            case Enum::Rates: return "Rates";
            default: return "Unknown";
        }
    }

    /**
     * Named constructor for accelerations EulerStateDerivativeType
     */
    inline static EulerStateDerivativeType accelerations(void)
    {
        return Enum::Accelerations;
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Accelerations, Enum::Eulers, Enum::Rates };
    }

    /**
     * Named constructor for eulers EulerStateDerivativeType
     */
    inline static EulerStateDerivativeType eulers(void)
    {
        return Enum::Eulers;
    }

    /**
     * Named constructor for rates EulerStateDerivativeType
     */
    inline static EulerStateDerivativeType rates(void)
    {
        return Enum::Rates;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
