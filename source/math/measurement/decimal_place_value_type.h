#ifndef DECIMAL_PLACE_VALUE_TYPE_H
#define DECIMAL_PLACE_VALUE_TYPE_H

#include "enumerable.h"

namespace math
{

namespace measurement
{

/**
 * Encapsulated enumeration for representing commonly used decimal place values in powers of ten (tenths,
 * hundredths, thousandths, etc.)
 */
struct DecimalPlaceValueType final
: public attributes::abstract::Enumerable<DecimalPlaceValueType>
{
public:

    /**
     * Enumerations
     */
    enum class Enum { Tenth, Hundredth, Thousandth, TenThousandth, HundredThousandth, Millionth, TenMillionth,
                      HundredMillionth, Billionth, TenBillionth, HundredBillionth, Trillionth, TenTrillionth,
                      HundredTrillionth, Quadrillionth, Unknown };

    /**
     * Constructor
     */
    DecimalPlaceValueType(const std::string &type = "Unknown")
    : m_type(Enum::Unknown)
    {
        operator = (type);
    }

    /**
     * Constructor
     */
    DecimalPlaceValueType(const Enum &type)
    : m_type(type)
    {

    }

    /**
     * Copy constructor
     */
    DecimalPlaceValueType(const DecimalPlaceValueType &type)
    {
        operator = (type);
    }

    /**
     * Move constructor
     */
    DecimalPlaceValueType(DecimalPlaceValueType &&type)
    {
        operator = (std::move(type));
    }

    /**
     * Destructor
     */
    virtual ~DecimalPlaceValueType(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    DecimalPlaceValueType &operator = (const DecimalPlaceValueType &type)
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
    DecimalPlaceValueType &operator = (DecimalPlaceValueType &&type)
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
    virtual DecimalPlaceValueType &operator = (std::string type) override
    {
        static const std::map<std::string, Enum> typeMap =
        { { "tenth", Enum::Tenth },
          { "hundredth", Enum::Hundredth },
          { "thousandth", Enum::Thousandth },
          { "tenthousandth", Enum::TenThousandth },
          { "hundredthousandth", Enum::HundredThousandth },
          { "millionth", Enum::Millionth },
          { "tenmillionth", Enum::TenMillionth },
          { "hundredmillionth", Enum::HundredMillionth },
          { "billionth", Enum::Billionth },
          { "tenbillionth", Enum::TenBillionth },
          { "hundredbillionth", Enum::HundredBillionth },
          { "trillionth", Enum::Trillionth },
          { "tentrillionth", Enum::TenTrillionth },
          { "hundredtrillionth", Enum::HundredTrillionth },
          { "quadrillionth", Enum::Quadrillionth }
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
     * Conversion from DecimalPlaceValueType to double (returns 0.0 for unknown types)
     */
    virtual operator double (void) const final
    {
        switch (m_type)
        {
            case Enum::Tenth: return 1.0E-1;
            case Enum::Hundredth: return 1.0E-2;
            case Enum::Thousandth: return 1.0E-3;
            case Enum::TenThousandth: return 1.0E-4;
            case Enum::HundredThousandth: return 1.0E-5;
            case Enum::Millionth: return 1.0E-6;
            case Enum::TenMillionth: return 1.0E-7;
            case Enum::HundredMillionth: return 1.0E-8;
            case Enum::Billionth: return 1.0E-9;
            case Enum::TenBillionth: return 1.0E-10;
            case Enum::HundredBillionth: return 1.0E-11;
            case Enum::Trillionth: return 1.0E-12;
            case Enum::TenTrillionth: return 1.0E-13;
            case Enum::HundredTrillionth: return 1.0E-14;
            case Enum::Quadrillionth: return 1.0E-15;
            default: return 0.0;
        }
    }

    /**
     * Conversion to std::string operator
     */
    virtual operator std::string (void) const override
    {
        switch (m_type)
        {
            case Enum::Tenth: return "Tenth";
            case Enum::Hundredth: return "Hundredth";
            case Enum::Thousandth: return "Thousandth";
            case Enum::TenThousandth: return "TenThousandth";
            case Enum::HundredThousandth: return "HundredThousandth";
            case Enum::Millionth: return "Millionth";
            case Enum::TenMillionth: return "TenMillionth";
            case Enum::HundredMillionth: return "HundredMillions";
            case Enum::Billionth: return "Billionth";
            case Enum::TenBillionth: return "TenBillionth";
            case Enum::HundredBillionth: return "HundredBillionth";
            case Enum::Trillionth: return "Trillionth";
            case Enum::TenTrillionth: return "TenTrillionth";
            case Enum::HundredTrillionth: return "HundredTrillionth";
            case Enum::Quadrillionth: return "Quadrillionth";
            default: return "Unknown";
        }
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static std::vector<Enum> enumerations(void)
    {
        return { Enum::Tenth,
                 Enum::Hundredth,
                 Enum::Thousandth,
                 Enum::TenThousandth,
                 Enum::HundredThousandth,
                 Enum::Millionth,
                 Enum::TenMillionth,
                 Enum::HundredMillionth,
                 Enum::Billionth,
                 Enum::TenBillionth,
                 Enum::HundredBillionth,
                 Enum::Trillionth,
                 Enum::TenTrillionth,
                 Enum::HundredTrillionth,
                 Enum::Quadrillionth };
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
