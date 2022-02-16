#ifndef XML_READABLE_H
#define XML_READABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can read data from XML input
 */
template<typename XML_Node>
class XML_Readable
{
public:

    /**
     * Constructor
     */
    XML_Readable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~XML_Readable(void)
    {

    }

    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(XML_Node *pNode) = 0;
};

}

}

#endif
