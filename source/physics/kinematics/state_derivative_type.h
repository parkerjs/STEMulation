#ifndef STATE_DERIVATIVE_TYPE_H
#define STATE_DERIVATIVE_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent Cartesian position, velocity, or acceleration
 */
struct StateDerivativeType final
: public attributes::abstract::Enumerable<StateDerivativeType>
{
    /**
     * Enumerations
     */
    enum Enum { Acceleration = 1, Position = 2, Velocity = 4, Unknown };

    /**
     * Constructor
     */
    StateDerivativeType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    StateDerivativeType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    StateDerivativeType(const StateDerivativeType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    StateDerivativeType(StateDerivativeType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~StateDerivativeType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    StateDerivativeType &operator = (const StateDerivativeType &type)
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
    StateDerivativeType &operator = (StateDerivativeType &&type)
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
    virtual StateDerivativeType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "acceleration", Enum::Acceleration },
          { "position", Enum::Position },
          { "velocity", Enum::Velocity },
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
            case Enum::Acceleration: return "Acceleration";
            case Enum::Position: return "Position";
            case Enum::Velocity: return "Velocity";
            default: return "Unknown";
        }
    }

    /**
     * Named constructor for acceleration StateDerivativeType
     */
    inline static StateDerivativeType acceleration(void)
    {
        return Enum::Acceleration;
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Acceleration, Enum::Position, Enum::Velocity };
    }

    /**
     * Named constructor for position StateDerivativeType
     */
    inline static StateDerivativeType position(void)
    {
        return Enum::Position;
    }

    /**
     * Named constructor for velocity StateDerivativeType
     */
    inline static StateDerivativeType velocity(void)
    {
        return Enum::Velocity;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
