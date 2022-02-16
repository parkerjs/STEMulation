#ifndef TRANSPORT_LAYER_PROTOCOL_H
#define TRANSPORT_LAYER_PROTOCOL_H

#include "enumerable.h"

namespace networking
{

/**
 * Encapsulated enumeration to represent transport layer protocols
 */
struct TransportLayerProtocol final
: public attributes::abstract::Enumerable<TransportLayerProtocol>
{
    /**
     * Enumerations
     */
    enum class Enum { DCCP, // Datagram Congestion Control Protocol
                      SCTP, // Stream Control Transmission Protocol
                      TCP,  // Transmission Control Protocol
                      UDP,  // User Datagram Protocol
                      Unknown
    };

    /**
     * Constructor
     */
    TransportLayerProtocol(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    TransportLayerProtocol(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    TransportLayerProtocol(const TransportLayerProtocol &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    TransportLayerProtocol(TransportLayerProtocol &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~TransportLayerProtocol(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    TransportLayerProtocol &operator = (const TransportLayerProtocol &type)
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
    TransportLayerProtocol &operator = (TransportLayerProtocol &&type)
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
    virtual TransportLayerProtocol &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "DCCP", Enum::DCCP },
          { "SCTP", Enum::SCTP },
          { "TCP", Enum::TCP },
          { "UDP", Enum::UDP },
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
            case Enum::DCCP: return "DCCP";
            case Enum::SCTP: return "SCTP";
            case Enum::TCP: return "TCP";
            case Enum::UDP: return "UDP";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::DCCP, Enum::SCTP, Enum::TCP, Enum::UDP };
    }

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

#endif
