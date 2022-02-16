#ifndef TRIANGULAR_MATRIX_TYPE_H
#define TRIANGULAR_MATRIX_TYPE_H

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
 * Encapsulated enumeration to classify a matrix as upper or lower triangular
 */
struct TriangularMatrixType final
: public attributes::abstract::Enumerable<TriangularMatrixType>
{
    /**
     * Enumerations
     */
    enum class Enum { Lower, // Lower triangular matrix
                      Upper, // Upper triangular matrix
                      Unknown };

    /**
     * Constructor
     */
    TriangularMatrixType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    TriangularMatrixType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    TriangularMatrixType(const TriangularMatrixType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    TriangularMatrixType(TriangularMatrixType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~TriangularMatrixType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    TriangularMatrixType &operator = (const TriangularMatrixType &type)
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
    TriangularMatrixType &operator = (TriangularMatrixType &&type)
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
    virtual TriangularMatrixType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "lower", Enum::Lower },
          { "upper", Enum::Upper },
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
            case Enum::Lower: return "Lower";
            case Enum::Upper: return "Upper";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Lower, Enum::Upper };
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
