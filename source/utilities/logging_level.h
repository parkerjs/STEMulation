#ifndef LOGGING_LEVEL_H
#define LOGGING_LEVEL_H

#include "enumerable.h"
#include <algorithm>

namespace utilities
{

/**
 * Encapsulated enumeration representing various logging levels
 */
struct LoggingLevel final
: public attributes::abstract::Enumerable<LoggingLevel>
{
    /**
     * Enumerations
     */
    enum class Enum { Debug, Error, Exception, Unknown, Usage, Warning };

    /**
     * Constructor
     */
    LoggingLevel(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    LoggingLevel(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    LoggingLevel(const LoggingLevel &level)
    {
        operator = (level);
    }

    /**
     * Move constructor
     */
    LoggingLevel(LoggingLevel &&level)
    {
        operator = (std::move(level));
    }

    /**
     * Destructor
     */
    virtual ~LoggingLevel(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    LoggingLevel &operator = (const LoggingLevel &level)
    {
        if (&level != this)
        {
            m_type = level.m_type;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    LoggingLevel &operator = (LoggingLevel &&level)
    {
        if (&level != this)
        {
            m_type = std::move(level.m_type);
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    virtual LoggingLevel &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "debug", Enum::Debug },
          { "error", Enum::Error },
          { "exception", Enum::Exception },
          { "usage", Enum::Usage },
          { "warning", Enum::Warning }
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
            case Enum::Debug: return "debug";
            case Enum::Error: return "error";
            case Enum::Exception: return "exception";
            case Enum::Usage: return "usage";
            case Enum::Warning: return "warning";
            default: return "unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Debug, Enum::Error, Enum::Exception, Enum::Usage, Enum::Warning };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

#endif
