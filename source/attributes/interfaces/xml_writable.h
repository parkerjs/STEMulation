#ifndef XML_WRITABLE_H
#define XML_WRITABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * Classes inheriting this behavior acquire a basic interface for types that can write data to XML format
 */
template<typename XML_Node>
class XML_Writable
{
public:

    /**
     * Constructor
     */
    XML_Writable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~XML_Writable(void)
    {

    }

    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(XML_Node *pNode) = 0;
};

}

}

#endif
