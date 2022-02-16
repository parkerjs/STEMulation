#ifndef PIVOT_TYPE_H
#define PIVOT_TYPE_H

#include "enumerable.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * Encapsulated enumeration for the type of pivoting used in matrix decomposition
 */
struct PivotType final
: public attributes::abstract::Enumerable<PivotType>
{
    /**
     * Enumerations
     */
    enum class Enum { Column, // column pivoting
                      Row,    // row pivoting
                      Unknown };

    /**
     * Constructor
     */
    PivotType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    PivotType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    PivotType(const PivotType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    PivotType(PivotType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~PivotType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    PivotType &operator = (const PivotType &type)
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
    PivotType &operator = (PivotType &&type)
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
    virtual PivotType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "column", Enum::Column },
          { "row", Enum::Row },
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
            case Enum::Column: return "Column";
            case Enum::Row: return "Row";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Column, Enum::Row };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

}

}

#endif
