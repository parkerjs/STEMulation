#ifndef DATE_FORMATTABLE_H
#define DATE_FORMATTABLE_H

#include "export_library.h"
#include <string>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for setting and retrieving the date format
 */
class DateFormattable
{
public:

    /**
     * Constructor
     */
    DateFormattable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~DateFormattable(void)
    {

    }

    /**
     * Get this object's date format
     */
    EXPORT_STEM virtual std::string getDateFormat(void) const = 0;

    /**
     * Set this object's date format
     */
    EXPORT_STEM virtual bool setDateFormat(const std::string &dateFormat) = 0;
};

}

}

#endif
