#ifndef UNARY_FUNCTOR_TYPE_H
#define UNARY_FUNCTOR_TYPE_H

#include "enumerable.h"
#include <regex>

namespace functional
{

namespace functors
{

namespace unary
{

/**
 * Encapsulated enumeration for unary functor types
 */
struct UnaryFunctorType final
: public attributes::abstract::Enumerable<UnaryFunctorType>
{
    /**
     * Enumerations
     */
    enum Enum { BitwiseNot, LogicalNot, Minus, Plus, Unknown };

    /**
     * Constructor
     */
    UnaryFunctorType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    UnaryFunctorType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    UnaryFunctorType(const UnaryFunctorType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    UnaryFunctorType(UnaryFunctorType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~UnaryFunctorType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    UnaryFunctorType &operator = (const UnaryFunctorType &type)
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
    UnaryFunctorType &operator = (UnaryFunctorType &&type)
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
    virtual UnaryFunctorType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "~", Enum::BitwiseNot },
          { "!", Enum::LogicalNot },
          { "-", Enum::Minus },
          { "+", Enum::Plus }
        };

        std::smatch match;
        static const std::regex criteria("\\s*(~|!|-|\\+).*");
        if (std::regex_match(type, match, criteria))
        {
            auto &&itType = typeMap.find(match[1]);
            m_type = (itType != typeMap.cend()) ? itType->second : Enum::Unknown;
        }

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
            case Enum::BitwiseNot: return "~";
            case Enum::LogicalNot: return "!";
            case Enum::Minus: return "-";
            case Enum::Plus: return "+";
            case Enum::Unknown:
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::BitwiseNot,
                 Enum::LogicalNot,
                 Enum::Minus,
                 Enum::Plus };
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
