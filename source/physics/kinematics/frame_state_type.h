#ifndef FRAME_STATE_TYPE_H
#define FRAME_STATE_TYPE_H

#include "enumerable.h"
#include <algorithm>
#include <map>
#include <string>

namespace physics
{

namespace kinematics
{

/**
 * Encapsulated enumeration to represent the type of frame state (Interpolated, Projected)
 */
struct FrameStateType final
: public attributes::abstract::Enumerable<FrameStateType>
{
    /**
     * Enumerations
     */
    enum Enum { Interpolated, Projected, Unknown };

    /**
     * Constructor
     */
    FrameStateType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    FrameStateType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    FrameStateType(const FrameStateType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    FrameStateType(FrameStateType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    ~FrameStateType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    FrameStateType &operator = (const FrameStateType &type)
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
    FrameStateType &operator = (FrameStateType &&type)
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
    virtual FrameStateType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "interpolated", Enum::Interpolated },
          { "projected", Enum::Projected },
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
            case Enum::Interpolated: return "Interpolated";
            case Enum::Projected: return "Projected";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Interpolated, Enum::Projected };
    }

    /**
     * Named constructor for Interpolated FrameStateType
     */
    inline static FrameStateType interpolated(void)
    {
        return Enum::Interpolated;
    }

    /**
     * Named constructor for Projected FrameStateType
     */
    inline static FrameStateType projected(void)
    {
        return Enum::Projected;
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
