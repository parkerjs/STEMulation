#ifndef WHOLE_PLACE_VALUE_TYPE_H
#define WHOLE_PLACE_VALUE_TYPE_H

#include "enumerable.h"

namespace math
{

namespace measurement
{

/**
 * Encapsulated enumeration for representing commonly used whole number place values in powers of ten (tens,
 * hundreds, thousands, etc.)
 */
class WholePlaceValueType final
: public attributes::abstract::Enumerable<WholePlaceValueType>
{
public:

    /**
     * Enumerations
     */
    enum class Enum { Ones, Tens, Hundreds, Thousands, TenThousands, HundredThousands, Millions, TenMillions,
                      HundredMillions, Billions, TenBillions, HundredBillions, Trillions, TenTrillions,
                      HundredTrillions, Quadrillions, Unknown };

    /**
     * Constructor
     */
    WholePlaceValueType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    WholePlaceValueType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    WholePlaceValueType(const WholePlaceValueType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    WholePlaceValueType(WholePlaceValueType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~WholePlaceValueType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    WholePlaceValueType &operator = (const WholePlaceValueType &type)
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
    WholePlaceValueType &operator = (WholePlaceValueType &&type)
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
    virtual WholePlaceValueType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "ones", Enum::Ones },
          { "tens", Enum::Tens },
          { "hundreds", Enum::Hundreds },
          { "thousands", Enum::Thousands },
          { "tenthousands", Enum::TenThousands },
          { "hundredthousands", Enum::HundredThousands },
          { "millions", Enum::Millions },
          { "tenmillions", Enum::TenMillions },
          { "hundredmillions", Enum::HundredMillions },
          { "billions", Enum::Billions },
          { "tenbillions", Enum::TenBillions },
          { "hundredbillions", Enum::HundredBillions },
          { "trillions", Enum::Trillions },
          { "tentrillions", Enum::TenTrillions },
          { "hundredtrillions", Enum::HundredTrillions },
          { "quadrillions", Enum::Quadrillions }
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
     * Conversion from WholePlaceValueType to long (returns 0 for unknown types)
     */
    operator long long (void) const
    {
        switch (m_type)
        {
            case Enum::Ones:             return 1LL;
            case Enum::Tens:             return 10LL;
            case Enum::Hundreds:         return 100LL;
            case Enum::Thousands:        return 1000LL;
            case Enum::TenThousands:     return 10000LL;
            case Enum::HundredThousands: return 100000LL;
            case Enum::Millions:         return 1000000LL;
            case Enum::TenMillions:      return 10000000LL;
            case Enum::HundredMillions:  return 100000000LL;
            case Enum::Billions:         return 1000000000LL;
            case Enum::TenBillions:      return 10000000000LL;
            case Enum::HundredBillions:  return 100000000000LL;
            case Enum::Trillions:        return 1000000000000LL;
            case Enum::TenTrillions:     return 10000000000000LL;
            case Enum::HundredTrillions: return 100000000000000LL;
            case Enum::Quadrillions:     return 1000000000000000LL;
            default: return 0;
        }
    }

    /**
     * Conversion to std::string operator
     */
    virtual operator std::string (void) const override
    {
        switch (m_type)
        {
            case Enum::Ones: return "Ones";
            case Enum::Tens: return "Tens";
            case Enum::Hundreds: return "Hundreds";
            case Enum::Thousands: return "Thousands";
            case Enum::TenThousands: return "TenThousands";
            case Enum::HundredThousands: return "HundredThousands";
            case Enum::Millions: return "Millions";
            case Enum::TenMillions: return "TenMillions";
            case Enum::HundredMillions: return "HundredMillions";
            case Enum::Billions: return "Billions";
            case Enum::TenBillions: return "TenBillions";
            case Enum::HundredBillions: return "HundredBillions";
            case Enum::Trillions: return "Trillions";
            case Enum::TenTrillions: return "TenTrillions";
            case Enum::HundredTrillions: return "HundredTrillions";
            case Enum::Quadrillions: return "Quadrillions";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Ones,
                 Enum::Hundreds,
                 Enum::Thousands,
                 Enum::TenThousands,
                 Enum::HundredThousands,
                 Enum::Millions,
                 Enum::TenMillions,
                 Enum::HundredMillions,
                 Enum::Billions,
                 Enum::TenBillions,
                 Enum::HundredBillions,
                 Enum::Trillions,
                 Enum::TenTrillions,
                 Enum::HundredTrillions,
                 Enum::Quadrillions };
    }

private:

    /**
     * this object's type enumeration
     */
    Enum m_type;
};

}

}

#endif
