#ifndef BINARY_FUNCTOR_TYPE_H
#define BINARY_FUNCTOR_TYPE_H

#include "enumerable.h"
#include <regex>

namespace functional
{

namespace functors
{

namespace binary
{

/**
 * Encapsulated enumeration for binary functor types
 */
struct BinaryFunctorType final
: public attributes::abstract::Enumerable<BinaryFunctorType>
{
    /**
     * Enumerations
     */
    enum Enum { Addition = 1, BitwiseAnd = 2, BitwiseLeftShift = 4, BitwiseOr = 8, BitwiseRightShift = 16,
                BitwiseXor = 32, Division = 64, Equal = 128, Greater = 256, GreaterEqual = 512,
                Less = 1024, LessEqual = 2048, LogicalAnd = 4096, LogicalOr = 8192, Modulo = 16384,
                Multiplication = 32768, NotEqual = 65536, Subtraction = 131072, Unknown = 262144 };

    /**
     * Constructor
     */
    BinaryFunctorType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    BinaryFunctorType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    BinaryFunctorType(const BinaryFunctorType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    BinaryFunctorType(BinaryFunctorType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~BinaryFunctorType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    BinaryFunctorType &operator = (const BinaryFunctorType &type)
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
    BinaryFunctorType &operator = (BinaryFunctorType &&type)
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
    virtual BinaryFunctorType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "+", Enum::Addition },
          { "&", Enum::BitwiseAnd },
          { "<<", Enum::BitwiseLeftShift },
          { "|", Enum::BitwiseOr },
          { ">>", Enum::BitwiseRightShift },
          { "^", Enum::BitwiseXor },
          { "/", Enum::Division },
          { "==", Enum::Equal },
          { ">", Enum::Greater },
          { ">=", Enum::GreaterEqual },
          { "<", Enum::Less },
          { "<=", Enum::LessEqual },
          { "&&", Enum::LogicalAnd },
          { "||", Enum::LogicalOr },
          { "%", Enum::Modulo },
          { "*", Enum::Multiplication },
          { "!=", Enum::NotEqual },
          { "-", Enum::Subtraction }
        };

        std::smatch match;
        static const std::regex criteria("\\s*(\\+|&{1,2}|<(=|<){0,1}|\\|{1,2}|>(=|>){0,1}|\\^|/|==|%|\\*|!=|-).*");
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
            case Enum::Addition: return "+";
            case Enum::BitwiseAnd: return "&";
            case Enum::BitwiseLeftShift: return "<<";
            case Enum::BitwiseOr: return "|";
            case Enum::BitwiseRightShift: return ">>";
            case Enum::BitwiseXor: return "^";
            case Enum::Division: return "/";
            case Enum::Equal: return "==";
            case Enum::Greater: return ">";
            case Enum::GreaterEqual: return ">=";
            case Enum::Less: return "<";
            case Enum::LessEqual: return "<=";
            case Enum::LogicalAnd: return "&&";
            case Enum::LogicalOr: return "||";
            case Enum::Modulo: return "%";
            case Enum::Multiplication: return "*";
            case Enum::NotEqual: return "!=";
            case Enum::Subtraction: return "-";
            case Enum::Unknown:
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Addition,
                 Enum::BitwiseAnd,
                 Enum::BitwiseLeftShift,
                 Enum::BitwiseOr,
                 Enum::BitwiseRightShift,
                 Enum::BitwiseXor,
                 Enum::Division,
                 Enum::Equal,
                 Enum::Greater,
                 Enum::GreaterEqual,
                 Enum::Less,
                 Enum::LessEqual,
                 Enum::LogicalAnd,
                 Enum::LogicalOr,
                 Enum::Modulo,
                 Enum::Multiplication,
                 Enum::NotEqual,
                 Enum::Subtraction };
    }

    /**
     * Function to test whether or not the member type is arithmetic
     */
    virtual bool isArithmetic(void) const final
    {
        switch (m_type)
        {
            case Enum::Addition:
            case Enum::Division:
            case Enum::Modulo:
            case Enum::Multiplication:
            case Enum::Subtraction:
            return true;
            default:
            return false;
        }
    }

    /**
     * Function to test whether or not the member type is bitwise
     */
    virtual bool isBitwise(void) const final
    {
        switch (m_type)
        {
            case Enum::BitwiseAnd:
            case Enum::BitwiseLeftShift:
            case Enum::BitwiseOr:
            case Enum::BitwiseRightShift:
            case Enum::BitwiseXor:
            return true;
            default:
            return false;
        }
    }

    /**
     * Function to test whether or not the member type is comparable
     */
    virtual bool isComparison(void) const final
    {
        switch (m_type)
        {
            case Enum::Equal:
            case Enum::Greater:
            case Enum::GreaterEqual:
            case Enum::Less:
            case Enum::LessEqual:
            case Enum::NotEqual:
            return true;
            default:
            return false;
        }
    }

    /**
     * Function to test whether or not the member type is logical
     */
    virtual bool isLogical(void) const final
    {
        switch (m_type)
        {
            case Enum::LogicalAnd:
            case Enum::LogicalOr:
            return true;
            default:
            return false;
        }
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
