#ifndef COLUMN_PIVOT_STRATEGY_H
#define COLUMN_PIVOT_STRATEGY_H

#include "enumerable.h"
#include <algorithm>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * Encapsulated enumeration for the column pivot strategy used in matrix decomposition
 */
struct ColumnPivotStrategy final
: public attributes::abstract::Enumerable<ColumnPivotStrategy>
{
    /**
     * Enumerations
     */
    enum class Enum { MaxElement,     // column pivot strategy via maximum element
                      MaxNorm,        // column pivot strategy via maximum 2-norm
                      NonZeroElement, // column pivot strategy via non-zero element
                      Unknown };

    /**
     * Constructor
     */
    ColumnPivotStrategy(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    ColumnPivotStrategy(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    ColumnPivotStrategy(const ColumnPivotStrategy &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    ColumnPivotStrategy(ColumnPivotStrategy &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~ColumnPivotStrategy(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    ColumnPivotStrategy &operator = (const ColumnPivotStrategy &type)
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
    ColumnPivotStrategy &operator = (ColumnPivotStrategy &&type)
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
    virtual ColumnPivotStrategy &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "maxelement", Enum::MaxElement },
          { "maxnorm", Enum::MaxNorm },
          { "nonzeroelement", Enum::NonZeroElement },
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
            case Enum::MaxElement: return "MaxElement";
            case Enum::MaxNorm: return "MaxNorm";
            case Enum::NonZeroElement: return "NonZeroElement";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::MaxElement, Enum::MaxNorm, Enum::NonZeroElement };
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
