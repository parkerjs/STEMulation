#ifndef NONLINEARITY_H
#define NONLINEARITY_H

#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "reflective.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#endif

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

namespace control_systems
{

namespace blocks
{

/**
 * This class provides an abstract base for nonlinear control elements commonly encountered in control systems
 */
template<typename T>
class Nonlinearity
#ifdef RAPID_XML
: public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  virtual private attributes::abstract::Reflective
#else
: virtual private attributes::abstract::Reflective
#endif
{
public:

    /**
     * Constructor
     */
    Nonlinearity(void)
    {

    }

    /**
     * Copy constructor
     */
    Nonlinearity(const Nonlinearity<T> &nonlinearity)
    {
        operator = (nonlinearity);
    }

    /**
     * Move constructor
     */
    Nonlinearity(Nonlinearity<T> &&nonlinearity)
    {
        operator = (std::move(nonlinearity));
    }

    /**
     * Destructor
     */
    virtual ~Nonlinearity(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Nonlinearity<T> &operator = (const Nonlinearity<T> &nonlinearity)
    {
        if (&nonlinearity != this)
        {
            // TODO: add members here
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Nonlinearity<T> &operator = (Nonlinearity<T> &&nonlinearity)
    {
        if (&nonlinearity != this)
        {
            // TODO: move members here
        }

        return *this;
    }

    /**
     * Function to apply the nonlinearity represented by this object to the input signal
     * @param signal the input to which the nonlinearity is applied
     */
    virtual T apply(T signal) = 0;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Nonlinearity";
    }
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
        {
            // TODO: implement this
        }

        return bSuccess;
    }
#endif
};

}

}

}

#endif
