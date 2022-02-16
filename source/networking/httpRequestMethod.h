#ifndef HTTP_REQUEST_METHOD_H
#define HTTP_REQUEST_METHOD_H

#include "enumerable.h"

namespace networking
{

/**
 * Encapsulated enumeration to represent HTTP request methods
 */
struct HttpRequestMethod final
: public attributes::abstract::Enumerable<HttpRequestMethod>
{
    /**
     * Enumerations
     */
    enum class Enum { Connect, Delete, Get, Head, Options, Patch, Post, Put, Trace, Unknown };

    /**
     * Constructor
     */
    HttpRequestMethod(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    HttpRequestMethod(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    HttpRequestMethod(const HttpRequestMethod &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    HttpRequestMethod(HttpRequestMethod &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~HttpRequestMethod(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    HttpRequestMethod &operator = (const HttpRequestMethod &type)
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
    HttpRequestMethod &operator = (HttpRequestMethod &&type)
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
    virtual HttpRequestMethod &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "CONNECT", Enum::Connect },
          { "DELETE", Enum::Delete },
          { "GET", Enum::Get },
          { "HEAD", Enum::Head },
          { "OPTIONS", Enum::Options },
          { "PATCH", Enum::Patch },
          { "POST", Enum::Post },
          { "PUT", Enum::Put },
          { "TRACE", Enum::Trace },
          { "UNKNOWN", Enum::Unknown }
        };

        std::transform(type.cbegin(), type.cend(), type.begin(), ::toupper);
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
            case Enum::Connect: return "CONNECT";
            case Enum::Delete: return "DELETE";
            case Enum::Get: return "GET";
            case Enum::Head: return "HEAD";
            case Enum::Options: return "OPTIONS";
            case Enum::Patch: return "PATCH";
            case Enum::Post: return "POST";
            case Enum::Put: return "PUT";
            case Enum::Trace: return "TRACE";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Connect, Enum::Delete, Enum::Get, Enum::Head, Enum::Options, Enum::Post, Enum::Put,
                 Enum::Trace };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

#endif
