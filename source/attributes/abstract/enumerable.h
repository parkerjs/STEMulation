#ifndef ENUMERABLE_H
#define ENUMERABLE_H

#include <map>
#include <string>
#include <vector>

#include "export_library.h"
#include "serializable.h"
#include "streamable.h"

namespace attributes
{

namespace abstract
{

/**
 * This class provides an abstract interface for types that can be enumerated
 */
template<typename T>
struct Enumerable
: public attributes::interfaces::Serializable,
  public attributes::concrete::Streamable<T>
{
    /**
     * Constructor
     */
    Enumerable(void)
    {

    }

    /**
     * Copy constructor
     */
    Enumerable(const Enumerable<T> &enumerable) = delete;

    /**
     * Move constructor
     */
    Enumerable(Enumerable<T> &&enumerable) = delete;

    /**
     * Destructor
     */
    virtual ~Enumerable(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Enumerable<T> &
    operator = (const Enumerable<T> &enumerable) = delete;

    /**
     * Move assignment operator
     */
    Enumerable<T> &
    operator = (Enumerable<T> &&enumerable) = delete;

    /**
     * Assignment operator
     */
    EXPORT_STEM virtual T &
    operator = (std::string type) = 0;

    /**
     * Conversion to std::string operator
     */
    EXPORT_STEM virtual operator std::string(void) const = 0;

    /**
     * Function to deserialize this object's data
     */
    virtual std::istream &
    deserialize(std::istream &stream) override
    {
        std::string type;

        std::getline(stream,
                     type,
                     '\0');
        operator = (type);

        return stream;
    }

    /**
     * Return a vector of enumerations supported by this class
     */
    inline static auto
    enumerations(void)
    {
        return T::enumerations();
    }

    /**
     * Input stream extraction function
     * @param stream a reference to an std::istream object
     */
    virtual std::istream &
    extract(std::istream &stream) override
    {
        std::string type;

        std::getline(stream,
                     type,
                     '\n');
        operator = (type);

        return stream;
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    virtual std::ostream &
    print(std::ostream &stream) const override
    {
        std::string type(*this);

        stream << type;

        return stream;
    }

    /**
     * Function to serialize this object's data
     */
    inline virtual std::ostream &
    serialize(std::ostream &stream) const override
    {
        return print(stream) << '\0';
    }

    /**
     * Function to return this object's type as a string
     */
    inline virtual std::string
    toString(void) const final
    {
        return operator std::string();
    }

    /**
     * Return a vector of encapsulated enumerations supported by this class
     */
    inline static std::vector<T>
    values(void)
    {
        std::vector<T> values;

        for (auto &&enumeration : enumerations())
        {
            values.push_back(enumeration);
        }

        return values;
    }
};

}

}

#endif

