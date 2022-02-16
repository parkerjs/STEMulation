#ifndef MATRIX_DIMENSION_TYPE_H
#define MATRIX_DIMENSION_TYPE_H

#include "enumerable.h"
#include <algorithm>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Encapsulated enumeration to represent the row and/or column dimension of a two-dimensional matrix
 */
struct MatrixDimensionType final
: public attributes::abstract::Enumerable<MatrixDimensionType>
{
    /**
     * Enumerations
     */
    enum class Enum { Columns = 1, Rows = 0, RowsAndColumns = 2, Unknown = 3 };

    /**
     * Constructor
     */
    MatrixDimensionType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    MatrixDimensionType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    MatrixDimensionType(const MatrixDimensionType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    MatrixDimensionType(MatrixDimensionType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~MatrixDimensionType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    MatrixDimensionType &operator = (const MatrixDimensionType &type)
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
    MatrixDimensionType &operator = (MatrixDimensionType &&type)
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
    virtual MatrixDimensionType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "columns", Enum::Columns },
          { "rows", Enum::Rows },
          { "rowsandcolumns", Enum::RowsAndColumns },
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
            case Enum::Columns: return "Columns";
            case Enum::Rows: return "Rows";
            case Enum::RowsAndColumns: return "RowsAndColumns";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Columns, Enum::Rows, Enum::RowsAndColumns };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

}

#endif
